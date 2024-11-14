#ifndef LIBKMEVENT_H
#define LIBKMEVENT_H

//#define DEBUG_INFO
//#define DEBUG_ECHO

#define	OFF 0
#define ON 1
#define	INF -1 // epol_wait() infinite wait flag
#define BUFLEN 16 // In case of double mouse event 32 (2*14 chars). Otherwise 16 is enough.

#ifdef __cplusplus
extern "C" {f
#endif

enum special_keys {
	UP_KEY = 'A', DOWN_KEY = 'B', RIGHT_KEY = 'C', LEFT_KEY = 'D', END_KEY = 'F', HOME_KEY = 'H', F1_KEY = 'P', F2_KEY = 'Q', F3_KEY = 'R', F4_KEY = 'S',
	INSERT_KEY = 50, DELETE_KEY = 51, PGUP= 53, PGDN_KEY = 54, F5_KEY = 15, F6_KEY = 17, F7_KEY = 18, F8_KEY = 19, F9_KEY = 20, F10_KEY = 21, F11_KEY = 23, F12_KEY = 24,
	NULL_KEY = 0, TAB_KEY = 9, ENTER_KEY = 10, ESC_KEY = 27, SPACE_KEY = 32, BACKSPACE_KEY = 127
};

enum control_chars {
	CTRL_2, CTRL_A, CTRL_B, CTRL_C, CTRL_D, CTRL_E, CTRL_F, CTRL_G, CTRL_H, CTRL_I, CTRL_J, CTRL_K, CTRL_L, CTRL_M, CTRL_N, CTRL_O,
	CTRL_P, CTRL_Q, CTRL_R, CTRL_S, CTRL_T, CTRL_U, CTRL_V, CTRL_W, CTRL_X, CTRL_Y, CTRL_Z, CTRL_3, CTRL_4, CTRL_5, CTRL_6, CTRL_7
};

enum mouse_traps					// At least two codes are required for correct mouse reporting: reporting + encoding.
{									//  Examples: 'set_mouse_trap(ON, REP_ALL_MOVE|ENC_DECIMAL1)', 'puts("\e[?1003h\e[?1006h")'
	REP_BTN_PUSH	= 0b0000001,	// reporting: '\e[?1000h' - X11 mouse reporting, reports on button press and release.
	REP_HIGHLIGH	= 0b0000010,	// reporting: '\e[?1001h' - highlight reporting, useful for reporting mouse highlights.
	REP_BTN_MOVE	= 0b0000100,	// reporting: '\e[?1002h' - button movement reporting, reports movement when a button is pressed.
	REP_ALL_MOVE	= 0b0001000,	// reporting: '\e[?1003h' - all movement reporting, reports all movements.
	ENC_UNI_UTF8	= 0b0010000,	//  encoding: '\e[?1005h' - report back encoded as utf-8 (xterm, urxvt, broken in several ways).
	ENC_DECIMAL1	= 0b0100000,	//  encoding: '\e[?1006h' - back as decimal values (xterm, many other terminal emulators, but not urxvt).
	ENC_DECIMAL2	= 0b1000000 	//  encoding: '\e[?1015h' - report back as decimal values (urxvt, xterm, other terminal emulators).
};

enum km_events
{ 	
	KE_NULL, KE_CHAR, KE_CTRL_CHAR, KE_WIN_CHAR, KE_ALT_CHAR, KE_ALT_CTRL_CHAR, KE_WIN_CTRL_CHAR, KE_WIN_ALT_CHAR, KE_WIN_ALT_CTRL_CHAR, KE_EXT_CHAR, KE_UNKNOWN,
	KE_FUNCTION, KE_CTRL_FUNCTION, KE_WIN_FUNCTION, KE_ALT_FUNCTION, KE_CTRL_ALT_FUNCTION, KE_CTRL_WIN_FUNCTION, KE_WIN_ALT_FUNCTION, KE_CTRL_WIN_ALT_FUNCTION,
	KE_NAVIGATION, KE_CTRL_NAVIGATION, KE_WIN_NAVIGATION, KE_ALT_NAVIGATION, KE_CTRL_ALT_NAVIGATION, KE_CTRL_WIN_NAVIGATION, KE_WIN_ALT_NAVIGATION, KE_CTRL_WIN_ALT_NAVIGATION,
	ME_BUTTON, ME_MOVE, ME_SCROLL
};

enum mouse_events {
	CTRL = 16, ALT = 8, DRAG = 32, // Modifiers
	LEFT_BUTTON_PRESSED = 0, LEFT_BUTTON_RELEASED = 100, WHEEL_BUTTON_PRESSED = 1,
	WHEEL_BUTTON_RELEASED = 101, RIGHT_BUTTON_PRESSED = 2, RIGHT_BUTTON_RELEASED = 102,
	NO_BUTTON_PRESSED = 3, WHEEL_SCROLL_DOWN = 65, WHEEL_SCROLL_UP = 64
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
