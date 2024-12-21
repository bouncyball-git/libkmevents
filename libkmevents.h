#ifndef LIBKMEVENT_H
#define LIBKMEVENT_H

#define DEBUG_INFO
#define DEBUG_ECHO

#define	OFF 0
#define ON 1
#define	INF -1 // epoll wait: infinite wait flag
#define	STDIN_READY 0 // Standard input ready for read
#define	TIMEOUT -1 // epoll timeout
#define	ERROR -2 // any epoll or signal error
#define	CLOSE -3 // epoll close: close descriptor flag
#define BUFLEN 16 // In case of double mouse event 32 (2*14 chars). Otherwise 16 is enough.
#define MAX_EVENTS 2  // Epoll is handling two types of events: stdin and signals

#ifdef __cplusplus
extern "C" {f
#endif

enum special_keys {
	UP_KEY = 'A', DOWN_KEY = 'B', RIGHT_KEY = 'C', LEFT_KEY = 'D', END_KEY = 'F', HOME_KEY = 'H', F1_KEY = 'P', F2_KEY = 'Q', F3_KEY = 'R', F4_KEY = 'S',
	INSERT_KEY = 50, DELETE_KEY = 51, PGUP_KEY = 53, PGDN_KEY = 54, F5_KEY = 15, F6_KEY = 17, F7_KEY = 18, F8_KEY = 19, F9_KEY = 20, F10_KEY = 21, F11_KEY = 23, F12_KEY = 24,
	NULL_KEY = 0, TAB_KEY = 9, ENTER_KEY = 13, ESC_KEY = 27, SPACE_KEY = 32, BACKSPACE_KEY = 127
};

enum control_chars {
	CTRL_2, CTRL_A, CTRL_B, CTRL_C, CTRL_D, CTRL_E, CTRL_F, CTRL_G, CTRL_H, CTRL_I, CTRL_J, CTRL_K, CTRL_L, CTRL_M, CTRL_N, CTRL_O,
	CTRL_P, CTRL_Q, CTRL_R, CTRL_S, CTRL_T, CTRL_U, CTRL_V, CTRL_W, CTRL_X, CTRL_Y, CTRL_Z, CTRL_3, CTRL_4, CTRL_5, CTRL_6, CTRL_7
};

enum mouse_traps
{													// Examples: 'set_mouse_trap(ON, SET_ANY_EVENT_MOUSE | SET_EXT_MODE_MOUSE)' is same as 'puts("\e[?1003h\e[?1006h")'
	SET_VT200_MOUSE           = 0b0000000000000001,	// Esc code: '\e[?1000h' - X11 mouse reporting, reports on button press and release.
	SET_VT200_HIGHLIGHT_MOUSE = 0b0000000000000010,	// Esc code: '\e[?1001h' - highlight reporting, useful for reporting mouse highlights.
	SET_BTN_EVENT_MOUSE       = 0b0000000000000100,	// Esc code: '\e[?1002h' - button movement reporting, reports movement when a button is pressed.
	SET_ANY_EVENT_MOUSE       = 0b0000000000001000,	// Esc code: '\e[?1003h' - all movement reporting, reports all movements.
	SET_FOCUS_EVENT_MOUSE     = 0b0000000000010000,	// Esc code: '\e[?1004h' - it causes xterm to send CSI I when the terminal gains focus, and CSI O when it loses focus.
	SET_ALTERNATE_SCROLL      = 0b0000000000100000,	// Esc code: '\e[?1005h' - report back encoded as utf-8 (xterm, urxvt, broken in several ways).
	SET_EXT_MODE_MOUSE        = 0b0000000001000000,	// Esc code: '\e[?1006h' - back as decimal values (xterm, many other terminal emulators, but not urxvt).
	SET_SGR_EXT_MODE_MOUSE    = 0b0000000010000000,	// Esc code: '\e[?1007h' - wheel mouse may send cursor-keys.
	SET_URXVT_EXT_MODE_MOUSE  = 0b0000000100000000,	// Esc code: '\e[?1015h' - report back as decimal values (urxvt, xterm, other terminal emulators).
	SET_PIXEL_POSITION_MOUSE  = 0b0000001000000000,	// Esc code: '\e[?1016h' - like 1006, but reports pixels not chars.
	SET_BUTTON1_MOVE_POINT    = 0b0000010000000000,	// Esc code: '\e[?2001h' - click1 emit Esc seq to move point.
	SET_BUTTON2_MOVE_POINT    = 0b0000100000000000,	// Esc code: '\e[?2002h' - press2 emit Esc seq to move point.
	SET_DBUTTON3_DELETE       = 0b0001000000000000,	// Esc code: '\e[?2003h' - double click-3 deletes.
	SET_PASTE_IN_BRACKET      = 0b0010000000000000,	// Esc code: '\e[?2004h' - surround paste by escapes.
	SET_PASTE_QUOTE           = 0b0100000000000000,	// Esc code: '\e[?2005h' - quote each char during paste.
	SET_PASTE_LITERAL_NL      = 0b1000000000000000,	// Esc code: '\e[?2006h' - paste "\n" as C-j.
};

enum km_events
{ 	
	KE_NULL, KE_CHAR, KE_CTRL_CHAR, KE_WIN_CHAR, KE_ALT_CHAR, KE_ALT_CTRL_CHAR,
	KE_WIN_CTRL_CHAR, KE_WIN_ALT_CHAR, KE_WIN_ALT_CTRL_CHAR, KE_EXT_CHAR, KE_UNKNOWN,
	
	KE_FUNCTION, KE_SHIFT_FUNCTION, KE_SHIFT_CTRL_FUNCTION, KE_CTRL_FUNCTION, KE_WIN_FUNCTION,
	KE_ALT_FUNCTION, KE_CTRL_ALT_FUNCTION, KE_CTRL_WIN_FUNCTION, KE_WIN_ALT_FUNCTION, KE_CTRL_WIN_ALT_FUNCTION,
	
	KE_NAVIGATION, KE_SHIFT_NAVIGATION, KE_SHIFT_CTRL_NAVIGATION, KE_CTRL_NAVIGATION, KE_WIN_NAVIGATION,
	KE_ALT_NAVIGATION, KE_CTRL_ALT_NAVIGATION, KE_CTRL_WIN_NAVIGATION, KE_WIN_ALT_NAVIGATION, KE_CTRL_WIN_ALT_NAVIGATION,
	
	ME_BUTTON, ME_MOVE, ME_SCROLL, ME_FOCUS, SE_SIGNAL
};

enum mouse_events {
	SHIFT = 4, ALT = 8, CTRL = 16, DRAG = 32, // Modifiers
	LEFT_BUTTON_PRESSED = 0, LEFT_BUTTON_RELEASED = 100, WHEEL_BUTTON_PRESSED = 1,
	WHEEL_BUTTON_RELEASED = 101, RIGHT_BUTTON_PRESSED = 2, RIGHT_BUTTON_RELEASED = 102,
	NO_BUTTON_PRESSED = 3, WHEEL_SCROLL_DOWN = 65, WHEEL_SCROLL_UP = 64, FOCUS_IN = 73, FOCUS_OUT = 79
};

typedef struct {
	int event;
	unsigned char ch;
	int x;
	int y;
} km_event;

// get_event() returns 0 on success, 1 on timeout, -1 on error
int get_event(km_event *kme, int timeout);
void set_term_attr(int mode);
void set_mouse_trap(int mode, int traps);
void bale_out(const char *msg);

#ifdef __cplusplus
}
#endif

#endif
