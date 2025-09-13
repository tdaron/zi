#ifndef TG_H
#define TG_H

#include <stdbool.h>
#include <termios.h>

extern int operations;

// --- Event types ---
typedef enum {
    TG_EV_NONE = 0,
    TG_EV_KEY,
    TG_EV_MOUSE
} tg_event_type;

typedef enum {
    TG_KEY_UNKNOWN = 0,
    TG_KEY_UP,
    TG_KEY_DOWN,
    TG_KEY_LEFT,
    TG_KEY_RIGHT,
    TG_KEY_ENTER,
    TG_KEY_ESC,
    TG_KEY_BACKSPACE,
    TG_KEY_CHAR
} tg_key;

typedef struct {
    int x;
    int y;
    int button;
} tg_mouse;

typedef struct {
    tg_event_type type;
    union {
        tg_key key;
        tg_mouse mouse;
    } data;
    char fullChar[4]; // valid if key == TG_KEY_CHAR - UTF8
    int n_bytes;
    char ch;
} tg_event;

typedef struct {
    int r;
    int g;
    int b;
} Color;

typedef struct {
    Color fg;
    Color bg;
    char value[4]; // UTF-8 bytes
    int n_bytes;
} Cell;


// This hardcoded color means TRANSPARENT
// I expect that no one will actually use this color
// as something else than TRANSPARENT
// in a theme or something. (just go full white lmao)
#define TRANSPARENT &(Color){255, 254, 253}
#define CLAY_TO_TERM_COLOR(color) &(Color){color.r, color.g, color.b}

void tg_cursor_hide(void);
void tg_cursor_show(void);

// --- Screen helpers ---
void tg_clear(void);
void tg_alt_screen_enable(void);
void tg_alt_screen_disable(void);

// --- Drawing ---
void tg_set_fg(Color *c);
void tg_set_bg(Color* c);
void tg_draw_box(int x, int y, int w, int h, char c);

// Those functions returns the number of cells used in x axis
// Useful to know where to put the cursor at
int tg_print_text(int x, int y, const char* s);
int tg_print_text_with_length(int x, int y, const char* s, int length);



void tg_flush();

// --- Raw mode ---
void tg_enable_raw(void);
void tg_disable_raw(void);

// --- Terminal size ---
void tg_update_size(void);
int tg_width(void);
int tg_height(void);
void tg_install_winch_handler(void);

// --- Input ---
int tg_getch(void);
void tg_set_nonblocking(int enable);
void tg_wait_for_keypress(int timeout_ms);
tg_event tg_get_event(void);

// --- Mouse ---
void tg_mouse_enable(void);
void tg_mouse_disable(void);

#endif // TG_H

