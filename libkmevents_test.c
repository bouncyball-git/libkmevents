#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "libkmevents.h"

static struct skeys_t {
	char ch;
	const unsigned char name[10];
} skeys[] = {
	{ 'A', "UP" }, { 'B', "DOWN" }, { 'C', "RIGHT" }, { 'D', "LEFT" }, { 'F', "END" }, { 'H', "HOME" }, { 'P', "F1" }, { 'Q', "F2" }, { 'R', "F3" }, { 'S', "F4" },
	{ 50, "INSERT" }, { 51, "DELETE" }, { 53, "PGUP" }, { 54, "PGDN" }, { 15, "F5" }, { 17, "F6" }, { 18, "F7" }, { 19, "F8" }, { 20, "F9" }, { 21, "F10" }, { 23, "F11" }, { 24, "F12" },
	{ 0, "NULL" }, { 9, "TAB" }, { 10, "ENTER" }, { 27, "ESC" }, { 32, "SPACE" }, { 127, "BACKSPACE" }, { -1, "NONE" }
};

static const unsigned char ctrl_chars[32][7] = {
	"NULL", "^A", "^B", "^C", "^D", "^E", "^F", "^G",
	"^H", "TAB", "^J", "^K", "^L", "ENTER", "^N", "^O",
	"^P", "^Q", "^R", "^S", "^T", "^U", "^V", "^W",
	"^X", "^Y", "^Z", "ESC", "^4", "^5", "^6", "^7" };

static int get_index(char ch) {
	int i = 0;
	for(i = 0; skeys[i].ch != -1; i++)
		if(skeys[i].ch == ch) return i;
	return i;
}

int main()
{
	km_event kme = { 0 };
	int ret = 0;
	char tab = ' ';
#ifdef DEBUG_ECHO
	tab = '\t';
#endif
	set_term_attr(ON);
	set_mouse_trap(ON, SET_ANY_EVENT_MOUSE | SET_FOCUS_EVENT_MOUSE | SET_EXT_MODE_MOUSE);

	while(1) {    
		ret = get_event(&kme, INF); // 2nd param: wait 'timeout' in miliseconds. 'INF' for infinity.
		if(ret == -1) {
			printf(" Error: get_event() failure. ret = %d\r\n", ret);
			break;
		}
#ifdef DEBUG_INFO
		else if (ret == 1)
			printf(" Debug: get_event() timeout. ret = %d\r\n", ret);
		else
			printf(" Debug: get_event() success. ret = %d\r\n", ret);
#endif
		switch (kme.event) {
			case KE_NULL:
				printf("%c%s\tKEY:%d (0x%x)\r\n", tab, "NO EVENTS RECEIVED", kme.ch, kme.ch);
				break;	
			case KE_CHAR:
				if(kme.ch == '\x20') { printf("%c%s\tKEY:%s (0x%x)\r\n", tab, "NORMAL CHAR", "SPACE", kme.ch); break; }
				if(kme.ch == '\x7f') { printf("%c%s\tKEY:%s (0x%x)\r\n", tab, "NORMAL CHAR", "BACKSPACE", kme.ch); break; }
				printf("%c%s\tKEY:%c (0x%x)\r\n", tab, "NORMAL CHAR", kme.ch, kme.ch);
				break;
			case KE_CTRL_CHAR:
				printf("%c%s\tKEY:%s (0x%x)\r\n", tab, "CONTROL CHAR", ctrl_chars[kme.ch], kme.ch);
				if(kme.ch == CTRL_Q) bale_out(NULL);
				break;
			case KE_ALT_CHAR:
				printf("%c%s\tKEY:%c (0x%x)\r\n", tab, "ALT + NORMAL CHAR", kme.ch, kme.ch);
				break;
			case KE_WIN_CHAR:
				printf("%c%s\tKEY:%c (0x%x)\r\n", tab, "WIN + NORMAL CHAR", kme.ch, kme.ch);
				break;
			case KE_ALT_CTRL_CHAR:
				printf("%c%s\tKEY:%s (0x%x)\r\n", tab, "ALT + CONTROL CHAR", ctrl_chars[kme.ch], kme.ch);
				break;
			case KE_WIN_CTRL_CHAR:
				printf("%c%s\tKEY:%s (0x%x)\r\n", tab, "WIN + CONTROL CHAR", ctrl_chars[kme.ch], kme.ch);
				break;
			case KE_WIN_ALT_CHAR:
				printf("%c%s\tKEY:%c (0x%x)\r\n", tab, "WIN + ALT + CHAR", kme.ch, kme.ch);
				break;
			case KE_WIN_ALT_CTRL_CHAR:
				printf("%c%s\tKEY:%s (0x%x)\r\n", tab, "WIN + ALT + CONTROL CHAR", ctrl_chars[kme.ch], kme.ch);
				break;
			case KE_FUNCTION:
				printf("%c%s\tKEY:%s (0x%x)\r\n", tab, "FUNCTION KEY", skeys[get_index(kme.ch)].name, kme.ch);
				break;
			case KE_CTRL_FUNCTION:
				printf("%c%s\tKEY:%s (0x%x)\r\n", tab, "CTRL + FUNCTION KEY", skeys[get_index(kme.ch)].name, kme.ch);
				break;
			case KE_ALT_FUNCTION:
				printf("%c%s\tKEY:%s (0x%x)\r\n", tab, "ALT + FUNCTION KEY", skeys[get_index(kme.ch)].name, kme.ch);
				break;
			case KE_WIN_FUNCTION:
				printf("%c%s\tKEY:%s (0x%x)\r\n", tab, "WIN + FUNCTION KEY", skeys[get_index(kme.ch)].name, kme.ch);
				break;
			case KE_CTRL_ALT_FUNCTION:
				printf("%c%s\tKEY:%s (0x%x)\r\n", tab, "CTRL + ALT + FUNCTION KEY", skeys[get_index(kme.ch)].name, kme.ch);
				break;
			case KE_NAVIGATION:
				printf("%c%s\tKEY:%s (0x%x)\r\n", tab, "NAVIGATION KEY", skeys[get_index(kme.ch)].name, kme.ch);
				break;
			case KE_CTRL_NAVIGATION:
				printf("%c%s\tKEY:%s (0x%x)\r\n", tab, "CTRL + NAVIGATION KEY", skeys[get_index(kme.ch)].name, kme.ch);
				break;
			case KE_ALT_NAVIGATION:
				printf("%c%s\tKEY:%s (0x%x)\r\n", tab, "ALT + NAVIGATION KEY", skeys[get_index(kme.ch)].name, kme.ch);
				break;
			case KE_WIN_NAVIGATION:
				printf("%c%s\tKEY:%s (0x%x)\r\n", tab, "WIN + NAVIGATION KEY", skeys[get_index(kme.ch)].name, kme.ch);
				break;
			case KE_CTRL_ALT_NAVIGATION:
				printf("%c%s\tKEY:%s (0x%x)\r\n", tab, "CTRL + ALT + NAVIGATION KEY", skeys[get_index(kme.ch)].name, kme.ch);
				break;
			case KE_UNKNOWN:
				printf("%c%s\tKEY:%d (0x%x)\r\n", tab, "UNKNOWN EVENT", kme.ch, kme.ch);
				break;
			case ME_BUTTON: 
				switch(kme.ch) {
					case LEFT_BUTTON_PRESSED:
						printf("%c%s\tROW:%d\tCOL:%d\r\n", tab, "MOUSE LEFT BUTTON PRESSED", kme.x, kme.y);
						break;
					case LEFT_BUTTON_RELEASED:
						printf("%c%s\tROW:%d\tCOL:%d\r\n", tab, "MOUSE LEFT BUTTON RELEASED", kme.x, kme.y);
						break;
					case CTRL + LEFT_BUTTON_PRESSED:
						printf("%c%s\tROW:%d\tCOL:%d\r\n", tab, "CTRL + MOUSE LEFT BUTTON PRESSED", kme.x, kme.y);
						break;
					case CTRL + LEFT_BUTTON_RELEASED:
						printf("%c%s\tROW:%d\tCOL:%d\r\n", tab, "CTRL + MOUSE LEFT BUTTON RELEASED", kme.x, kme.y);
						break;
					case ALT + LEFT_BUTTON_PRESSED:
						printf("%c%s\tROW:%d\tCOL:%d\r\n", tab, "ALT + MOUSE LEFT BUTTON PRESSED", kme.x, kme.y);
						break;
					case ALT + LEFT_BUTTON_RELEASED:
						printf("%c%s\tROW:%d\tCOL:%d\r\n", tab, "ALT + MOUSE LEFT BUTTON RELEASED", kme.x, kme.y);
						break;
					case CTRL + ALT + LEFT_BUTTON_PRESSED:
						printf("%c%s\tROW:%d\tCOL:%d\r\n", tab, "CTRL + ALT + MOUSE LEFT BUTTON PRESSED", kme.x, kme.y);
						break;
					case CTRL + ALT + LEFT_BUTTON_RELEASED:
						printf("%c%s\tROW:%d\tCOL:%d\r\n", tab, "CTRL + ALT + MOUSE LEFT BUTTON RELEASED", kme.x, kme.y);
						break;
					case WHEEL_BUTTON_PRESSED:
						printf("%c%s\tROW:%d\tCOL:%d\r\n", tab, "MOUSE WHEEL BUTTON PRESSED", kme.x, kme.y);
						break;
					case WHEEL_BUTTON_RELEASED:
						printf("%c%s\tROW:%d\tCOL:%d\r\n", tab, "MOUSE WHEEL BUTTON RELEASED", kme.x, kme.y);
						break;
					case CTRL + WHEEL_BUTTON_PRESSED:
						printf("%c%s\tROW:%d\tCOL:%d\r\n", tab, "CTRL + MOUSE WHEEL BUTTON PRESSED", kme.x, kme.y);
						break;
					case CTRL + WHEEL_BUTTON_RELEASED:
						printf("%c%s\tROW:%d\tCOL:%d\r\n", tab, "CTRL + MOUSE WHEEL BUTTON RELEASED", kme.x, kme.y);
						break;
					case ALT + WHEEL_BUTTON_PRESSED:
						printf("%c%s\tROW:%d\tCOL:%d\r\n", tab, "ALT + MOUSE WHEEL BUTTON PRESSED", kme.x, kme.y);
						break;
					case ALT + WHEEL_BUTTON_RELEASED:
						printf("%c%s\tROW:%d\tCOL:%d\r\n", tab, "ALT + MOUSE WHEEL BUTTON RELEASED", kme.x, kme.y);
						break;
					case CTRL + ALT + WHEEL_BUTTON_PRESSED:
						printf("%c%s\tROW:%d\tCOL:%d\r\n", tab, "CTRL + ALT + MOUSE WHEEL BUTTON PRESSED", kme.x, kme.y);
						break;
					case CTRL + ALT + WHEEL_BUTTON_RELEASED:
						printf("%c%s\tROW:%d\tCOL:%d\r\n", tab, "CTRL + ALT + MOUSE WHEEL BUTTON RELEASED", kme.x, kme.y);
						break;
					case RIGHT_BUTTON_PRESSED:
						printf("%c%s\tROW:%d\tCOL:%d\r\n", tab, "MOUSE RIGHT BUTTON PRESSED", kme.x, kme.y);
						break;
					case RIGHT_BUTTON_RELEASED:
						printf("%c%s\tROW:%d\tCOL:%d\r\n", tab, "MOUSE RIGHT BUTTON RELEASED", kme.x, kme.y);
						break;
					case CTRL + RIGHT_BUTTON_PRESSED:
						printf("%c%s\tROW:%d\tCOL:%d\r\n", tab, "CTRL + MOUSE RIGHT BUTTON PRESSED", kme.x, kme.y);
						break;
					case CTRL + RIGHT_BUTTON_RELEASED:
						printf("%c%s\tROW:%d\tCOL:%d\r\n", tab, "CTRL + MOUSE RIGHT BUTTON RELEASED", kme.x, kme.y);
						break;
					case ALT + RIGHT_BUTTON_PRESSED:
						printf("%c%s\tROW:%d\tCOL:%d\r\n", tab, "ALT + MOUSE RIGHT BUTTON PRESSED", kme.x, kme.y);
						break;
					case ALT + RIGHT_BUTTON_RELEASED:
						printf("%c%s\tROW:%d\tCOL:%d\r\n", tab, "ALT + MOUSE RIGHT BUTTON RELEASED", kme.x, kme.y);
						break;
					case CTRL + ALT + RIGHT_BUTTON_PRESSED:
						printf("%c%s\tROW:%d\tCOL:%d\r\n", tab, "CTRL + ALT + MOUSE RIGHT BUTTON PRESSED", kme.x, kme.y);
						break;
					case CTRL + ALT + RIGHT_BUTTON_RELEASED:
						printf("%c%s\tROW:%d\tCOL:%d\r\n", tab, "CTRL + ALT + MOUSE RIGHT BUTTON RELEASED", kme.x, kme.y);
						break;
				}
				break;
			case ME_MOVE:
				switch(kme.ch) {
					case NO_BUTTON_PRESSED + DRAG:
						printf("%c%s\tROW:%d\tCOL:%d\r\n", tab, "MOUSE DRAGGED", kme.x, kme.y);
						break;
					case CTRL + NO_BUTTON_PRESSED + DRAG:
						printf("%c%s\tROW:%d\tCOL:%d\r\n", tab, "CTRL + MOUSE DRAGGED", kme.x, kme.y);
						break;
					case ALT + NO_BUTTON_PRESSED + DRAG:
						printf("%c%s\tROW:%d\tCOL:%d\r\n", tab, "ALT + MOUSE DRAGGED", kme.x, kme.y);
						break;
					case CTRL + ALT + NO_BUTTON_PRESSED + DRAG:
						printf("%c%s\tROW:%d\tCOL:%d\r\n", tab, "CTRL + ALT + MOUSE DRAGGED", kme.x, kme.y);
						break;
					case LEFT_BUTTON_PRESSED + DRAG:
						printf("%c%s\tROW:%d\tCOL:%d\r\n", tab, "LEFT BUTTON PRESSED + MOUSE DRAGGED", kme.x, kme.y);
						break;
					case CTRL + LEFT_BUTTON_PRESSED + DRAG:
						printf("%c%s\tROW:%d\tCOL:%d\r\n", tab, "CTRL + LEFT BUTTON PRESSED + MOUSE DRAGGED", kme.x, kme.y);
						break;
					case ALT + LEFT_BUTTON_PRESSED + DRAG:
						printf("%c%s\tROW:%d\tCOL:%d\r\n", tab, "ALT + LEFT BUTTON PRESSED + MOUSE DRAGGED", kme.x, kme.y);
						break;
					case CTRL + ALT + LEFT_BUTTON_PRESSED + DRAG:
						printf("%c%s\tROW:%d\tCOL:%d\r\n", tab, "CTRL + ALT + LEFT BUTTON PRESSED + MOUSE DRAGGED", kme.x, kme.y);
						break;				
					case WHEEL_BUTTON_PRESSED + DRAG:
						printf("%c%s\tROW:%d\tCOL:%d\r\n", tab, "WHEEL BUTTON PRESSED + MOUSE DRAGGED", kme.x, kme.y);
						break;
					case CTRL + WHEEL_BUTTON_PRESSED + DRAG:
						printf("%c%s\tROW:%d\tCOL:%d\r\n", tab, "CTRL + WHEEL BUTTON PRESSED + MOUSE DRAGGED", kme.x, kme.y);
						break;
					case ALT + WHEEL_BUTTON_PRESSED + DRAG:
						printf("%c%s\tROW:%d\tCOL:%d\r\n", tab, "ALT + WHEEL BUTTON PRESSED + MOUSE DRAGGED", kme.x, kme.y);
						break;
					case CTRL + ALT + WHEEL_BUTTON_PRESSED + DRAG:
						printf("%c%s\tROW:%d\tCOL:%d\r\n", tab, "CTRL + ALT + WHEEL BUTTON PRESSED + MOUSE DRAGGED", kme.x, kme.y);
						break;						
					case RIGHT_BUTTON_PRESSED + DRAG:
						printf("%c%s\tROW:%d\tCOL:%d\r\n", tab, "RIGHT BUTTON PRESSED + MOUSE DRAGGED", kme.x, kme.y);
						break;
					case CTRL + RIGHT_BUTTON_PRESSED + DRAG:
						printf("%c%s\tROW:%d\tCOL:%d\r\n", tab, "CTRL + RIGHT BUTTON PRESSED + MOUSE DRAGGED", kme.x, kme.y);
						break;
					case ALT + RIGHT_BUTTON_PRESSED + DRAG:
						printf("%c%s\tROW:%d\tCOL:%d\r\n", tab, "ALT + RIGHT BUTTON PRESSED + MOUSE DRAGGED", kme.x, kme.y);
						break;
					case CTRL + ALT + RIGHT_BUTTON_PRESSED + DRAG:
						printf("%c%s\tROW:%d\tCOL:%d\r\n", tab, "CTRL + ALT + RIGHT BUTTON PRESSED + MOUSE DRAGGED", kme.x, kme.y);
						break;						
				}
				break;
			case ME_SCROLL:
				switch(kme.ch) {
					case WHEEL_SCROLL_UP:
						printf("%c%s\r\n", tab, "MOUSE WHEEL SCROLL UP");
						break;
					case CTRL + WHEEL_SCROLL_UP:
						printf("%c%s\r\n", tab, "CTRL + MOUSE WHEEL SCROLL UP");
						break;
					case ALT + WHEEL_SCROLL_UP:
						printf("%c%s\r\n", tab, "ALT + MOUSE WHEEL SCROLL UP");
						break;
					case CTRL + ALT + WHEEL_SCROLL_UP:
						printf("%c%s\r\n", tab, "CTRL + ALT + MOUSE WHEEL SCROLL UP");
						break;
					case WHEEL_SCROLL_DOWN:
						printf("%c%s\r\n", tab, "MOUSE WHEEL SCROLL DOWN");
						break;
					case CTRL + WHEEL_SCROLL_DOWN:
						printf("%c%s\r\n", tab, "CTRL + MOUSE WHEEL SCROLL DOWN");
						break;
					case ALT + WHEEL_SCROLL_DOWN:
						printf("%c%s\r\n", tab, "ALT + MOUSE WHEEL SCROLL DOWN");
						break;
					case CTRL + ALT + WHEEL_SCROLL_DOWN:
						printf("%c%s\r\n", tab, "CTRL + ALT + MOUSE WHEEL SCROLL DOWN");
						break;
				}
				break;
			case ME_FOCUS:
				switch(kme.ch) {
					case FOCUS_IN:
						printf("%c%s\r\n", tab, "MOUSE FOCUS IN");
						break;
					case FOCUS_OUT:
						printf("%c%s\r\n", tab, "MOUSE FOCUS OUT");
						break;
				}
				break;
			default:
				printf("%c%s\r\n", tab, "UNKNOWN EVENT RECEIVED");
				break;
		}
	}

	bale_out(NULL);
}
