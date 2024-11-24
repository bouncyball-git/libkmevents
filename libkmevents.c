#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <sys/epoll.h>
#include "libkmevents.h"

static int stdin_ready(int timeout)
{
    int retval;
	static int epoll_fd;
    static struct epoll_event event;

    if(epoll_fd == 0) {
		// Create an epoll instance
    	epoll_fd = epoll_create(1);
    	if (epoll_fd == -1) {
        	bale_out("Error: epoll_create\r\n");
    	}

    	// Initialize the epoll event structure
    	event.events = EPOLLIN; // Wait for input events
    	event.data.fd = 0; // Standard input

    	// Add the file descriptor to the epoll instance
    	retval = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, 0, &event);
    	if (retval == -1) {
        	bale_out("Error: epoll_ctl\r\n");
    	}
	}
    // Wait for one events 'timeout' amount of time
	retval = epoll_wait(epoll_fd, &event, 1, timeout);
    if (retval == -1) {
        bale_out("Error: epoll_wait\r\n");
    }

	return retval;
}

void bale_out(const char *msg)
{
	set_mouse_trap(OFF, 
		SET_VT200_MOUSE |
		SET_VT200_HIGHLIGHT_MOUSE |
		SET_BTN_EVENT_MOUSE |
		SET_ANY_EVENT_MOUSE |
		SET_FOCUS_EVENT_MOUSE |
		SET_ALTERNATE_SCROLL |
		SET_EXT_MODE_MOUSE |
		SET_SGR_EXT_MODE_MOUSE |
		SET_URXVT_EXT_MODE_MOUSE |
		SET_PIXEL_POSITION_MOUSE |
		SET_BUTTON1_MOVE_POINT |
		SET_BUTTON2_MOVE_POINT |
		SET_DBUTTON3_DELETE |
		SET_PASTE_IN_BRACKET |
		SET_PASTE_QUOTE |
		SET_PASTE_LITERAL_NL
	);

	set_term_attr(OFF);
	if(msg) { perror(msg); exit(EXIT_FAILURE); }
	exit(EXIT_SUCCESS);
}

void set_term_attr(int mode)
{
	int echo_on = 0;
#ifdef DEBUG_ECHO
	echo_on = 1;
#endif
	static struct termios attr_new, attr_old;

	if(mode) { // Set terminal to 'raw' mode
		tcgetattr(0, &attr_new);
		attr_old = attr_new;
		// Input modes - clear indicated ones giving: no break, no CR to NL, no parity check, no strip char, no start/stop output (sic) control.
		attr_new.c_iflag &= ~(BRKINT | INPCK | ISTRIP | IXON | ICRNL);
  		// Output modes - clear giving: no post processing such as NL to CR+NL
		attr_new.c_oflag &= ~(OPOST);
		// control modes - set 8 bit chars.
		attr_new.c_cflag |= (CS8);
		// Local modes - clear giving: echoing off, canonical off (no erase with backspace, ^U,...), no extended functions, no signal chars (^Z,^C, etc).
		attr_new.c_lflag &= ( (echo_on) ? ~(ICANON | IEXTEN | ISIG) : ~(ECHO | ICANON | IEXTEN | ISIG) );
		// The VMIN value sets the minimum number of bytes of input needed before read() can return. Using default (1).
		//attr_new.c_cc[VMIN] = 0;
		// The VTIME value sets the maximum amount of time to wait before read() returns. 'epoll_wait()' does the same thing with 'timeout' argument.
		//attr_new.c_cc[VTIME] = 5;
		tcsetattr(0, TCSANOW, &attr_new);
	} else { // Restore terminal to normal mode
		tcsetattr(0, TCSANOW, &attr_old);
	}
}

void set_mouse_trap(int mode, int mt_flag)
{
	char ts[9] = { 0 };
	static int tc[16] = { 1000, 1001, 1002, 1003, 1004, 1005, 1006, 1007, 1015, 1016, 2001, 2002, 2003, 2004,2005, 2006 };
	switch(mode) {
		case ON: // Activate mouse traps
			for (int i = 0; i < 16; i++) {
				if(mt_flag & (1<<i)) {
					if(sprintf(ts, "\e[?%dh", tc[i]) < 8) {
						bale_out("Error: sprintf\r\n");
					};
					write(fileno(stdout), ts, 8);
				}
			}
			break;
	 	case OFF: // Deactivate mouse traps
			for (int i = 0; i < 16; i++) {
				if(mt_flag & (1<<i)) {
					if(sprintf(ts, "\e[?%dl", tc[i]) < 8) {
						bale_out("Error: sprintf\r\n");
					};
					write(fileno(stdout), ts, 8);
				}
			}
			break;
		default:
			break;
	}
}

int get_event(km_event *kme, int timeout)
{
	char mbtn_state = 0, chtmp = 0;
	int ret = 0, chars_read = 0, mouse_event = 0;
	unsigned int chint = 0;
	unsigned char buf[BUFLEN] = { 0 };

	// Zero kme fields
	kme->event = 0; kme->ch = 0; kme->x = 0; kme->y = 0;

	// Check if stdin has data
	ret = stdin_ready(timeout);
	if(ret > 0) { // Data available
		chars_read = read(fileno(stdin), buf, sizeof(buf));
		if (!chars_read) { 
			perror("Error: failed reading from stdin\r\n");
			return -1;
		}
#ifdef DEBUG_INFO
		for(int i = 0; i < chars_read; i++)
			if(buf[i] == '\e')
				printf("%s buf[%d]  esc  %d   0x%x\n", (i == 0) ? "\r\n" : "\r", i, buf[i], buf[i]);
			else
				printf("%s buf[%d]   %c   %d   0x%x\n", (i == 0) ? "\r\n" : "\r", i, buf[i], buf[i], buf[i]);
#endif
		switch(buf[0]) {
			case ESC_KEY: // If escape sequence
				switch(chars_read) {
					case 1: // ESC char exception
						kme->ch = buf[0];
						kme->event = KE_CTRL_CHAR;
						break;
					case 2: // ALT + char
						kme->ch = buf[1];
						kme->event = (kme->ch < 32) ? KE_ALT_CTRL_CHAR : KE_ALT_CHAR;
						break;
					case 3: // UP, DOWN, RIGHT, LEFT, HOME, END, F1..F4
						ret = sscanf((const char*)buf, "\e%c%c", &chtmp, &kme->ch); // Navigation
						if(ret == 2) { 
							if(chtmp == '[') { 
								if(kme->ch == 'I' || kme->ch == 'O') { kme->event = ME_FOCUS; break; }
								kme->event = KE_NAVIGATION; break; 
							}
							if(chtmp == 'O') { kme->event = KE_FUNCTION; break; }
						}
						kme->event = KE_UNKNOWN;
						break;
					case 4: // INSERT, DELETE, PGUP, PGDN, CTRL or ALT or WIN + F1..F4
						ret = sscanf((const char*)buf, "\e[%c~", &kme->ch);
						if(ret == 1) { kme->event = KE_NAVIGATION; break; }
						ret = sscanf((const char*)buf, "\eO%c%c", &chtmp, &kme->ch);
						if(ret == 2) { 
							if(chtmp == '1') { kme->event = KE_WIN_FUNCTION; break; }
							if(chtmp == '3') { kme->event = KE_ALT_FUNCTION; break; }
							if(chtmp == '5') { kme->event = KE_CTRL_FUNCTION; break; }
							if(chtmp == '7') { kme->event = KE_CTRL_ALT_FUNCTION; break; }
						}
						kme->event = KE_UNKNOWN;
						break;
					case 5: // F5..F12
						ret = sscanf((const char*)buf, "\e[%u~", &chint);
						if(ret == 1) { kme->event = KE_FUNCTION; kme->ch = chint; break; }
						kme->event = KE_UNKNOWN;
						break;
					case 6: // CTRL or ALT or WIN + NAVIGATION
						ret = sscanf((const char*)buf, "\e[1;%c%c",  &chtmp, &kme->ch);
						if(ret == 2) { // UP, DOWN, RIGHT, LEFT, HOME, END
							if(chtmp == '1') { kme->event = KE_WIN_NAVIGATION; break; }
							if(chtmp == '3') { kme->event = KE_ALT_NAVIGATION; break; }
							if(chtmp == '5') { kme->event = KE_CTRL_NAVIGATION; break; }
							if(chtmp == '7') { kme->event = KE_CTRL_ALT_NAVIGATION; break; }
						}
						ret = sscanf((const char*)buf, "\e[%c;%c~", &kme->ch, &chtmp);
						if(ret == 2) { // INSERT, DELETE, PGUP, PGDN
							if(chtmp == '1') { kme->event = KE_WIN_NAVIGATION; break; }
							if(chtmp == '3') { kme->event = KE_ALT_NAVIGATION; break; }
							if(chtmp == '5') { kme->event = KE_CTRL_NAVIGATION; break; }
							if(chtmp == '7') { kme->event = KE_CTRL_ALT_NAVIGATION; break; }
						}	
						kme->event = KE_UNKNOWN;
						break;
					case 7: // CTRL F5..F12
						ret = sscanf((const char*)buf, "\e[%u;%c~", &chint, &chtmp);
						if(ret == 2) {
							kme->ch = chint;
							if(chtmp == '1') { kme->event = KE_WIN_FUNCTION; break; }
							if(chtmp == '3') { kme->event = KE_ALT_FUNCTION; break; }
							if(chtmp == '5') { kme->event = KE_CTRL_FUNCTION; break; }
							if(chtmp == '7') { kme->event = KE_CTRL_ALT_FUNCTION; break; }
						}	
						kme->event = KE_UNKNOWN;
						break;
					default: // Mouse events
						kme->ch = mbtn_state;
						ret = sscanf((const char*)buf, "\e[<%d;%d;%d%c", &mouse_event, &kme->x, &kme->y, &mbtn_state);
						if(ret == 4) { 
							if(mouse_event < 30) {
								kme->event = ME_BUTTON; kme->ch = mouse_event + ((mbtn_state == 'm') ? 100 : 0); break;
							} else if(mouse_event > 30 && mouse_event < 60) {
								kme->event = ME_MOVE; kme->ch = mouse_event; break;
							} else if (mouse_event > 60 || mouse_event < 90) {
								kme->event = ME_SCROLL; kme->ch = mouse_event; break;
							}
						}
						kme->event = KE_UNKNOWN;
						break;
					}
					break;
			case CTRL_X:
				switch(chars_read) {
					case 1: // CTRL + X exception
						kme->ch = buf[0];
						kme->event = KE_CTRL_CHAR;
						break;
					case 4: // WIN + char, WIN + control char
						ret = sscanf((const char*)buf, "\x18@s%c", &kme->ch);
						if(ret == 1) { kme->event = (kme->ch < 32) ? KE_WIN_CTRL_CHAR : (kme->ch > 127) ? KE_UNKNOWN : KE_WIN_CHAR; break;}
						kme->event = KE_UNKNOWN;
						break;
					case 5: // WIN + ALT + char, WIN + ALT + control char
						ret = sscanf((const char*)buf, "\x18@s\e%c", &kme->ch);
						if(ret == 1) { kme->event = (kme->ch < 32) ? KE_WIN_ALT_CTRL_CHAR : (kme->ch > 127) ? KE_UNKNOWN : KE_WIN_ALT_CHAR; break;}
						kme->event = KE_UNKNOWN;
						break;
				}
				break;
			default: // Alphanumeric, extended or control character
				if(chars_read == 1) { // Make sure only one character was read
					kme->ch = buf[0];
					kme->event = (kme->ch < 32) ? KE_CTRL_CHAR : (kme->ch > 127) ? KE_EXT_CHAR : KE_CHAR;
				}
				break;
		}
		ret = 0; // Event read
	} else if (!ret) { // Buffer is empty
		kme->ch = buf[0];
		kme->event = KE_NULL;
		ret = 1; // Timeout
	}
#ifdef DEBUG_INFO
	printf("\r kme_event: %d, kme_ch: %d, x: %d, y: %d, mbtn_state: %c\r\n", kme->event, kme->ch, kme->x, kme->y, mbtn_state);
#endif
	return ret;
}
