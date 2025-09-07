// termgfx.h - POSIX only
// ================================================================
// TermGFX: Minimal terminal graphics / input helpers for POSIX
//
// Features:
//
// 1. **Raw Mode Input**
//    - `tg_enable_raw()` switches terminal to raw mode:
//        * disables canonical input and echo (ICANON, ECHO flags)
//        * sets minimum input characters = 1 (VMIN) and timeout = 0 (VTIME)
//        * uses `tcgetattr()` to get current termios and `tcsetattr()` to apply changes
//    - `tg_disable_raw()` restores terminal to previous state
//    - POSIX references:
//        * man 3 termios: https://man7.org/linux/man-pages/man3/termios.3.html
//
// 2. **Non-blocking Input**
//    - `tg_set_nonblocking(1)` sets stdin to non-blocking mode using `fcntl()`
//    - `tg_set_nonblocking(0)` restores blocking mode
//    - POSIX references:
//        * man 2 fcntl: https://man7.org/linux/man-pages/man2/fcntl.2.html
//
// 3. **Terminal Resize Handling**
//    - `tg_install_winch_handler()` installs SIGWINCH handler to automatically
//      update terminal width/height
//    - `tg_update_size()` queries terminal size via `ioctl(TIOCGWINSZ)`
//    - POSIX references:
//        * man 2 ioctl: https://man7.org/linux/man-pages/man2/ioctl.2.html
//        * man 7 signal: https://man7.org/linux/man-pages/man7/signal.7.html
//
// 4. **Cursor Control**
//    - `tg_cursor_hide()` / `tg_cursor_show()` toggle the terminal cursor
//    - `tg_move(x,y)` moves cursor to zero-based coordinates
//
// 5. **Screen Control**
//    - `tg_clear()` clears the screen and moves cursor home
//    - `tg_alt_screen_enable()` / `tg_alt_screen_disable()` switch to alternate screen buffer
//
// 6. **Drawing Helpers**
//    - `tg_draw_box(x,y,w,h,c)` draws rectangle using character `c`
//    - `tg_print_text(x,y,s)` prints string at coordinates
//    - `tg_set_fg(r,g,b)` / `tg_set_bg(r,g,b)` set 24-bit colors
//    - `tg_reset()` resets colors/attributes
//
// 7. **Mouse Support**
//    - `tg_mouse_enable()` enables X10/VT200 mouse reporting (SGR mode)
//    - `tg_mouse_disable()` disables mouse reporting
//    - Mouse events available via `tg_get_event()`
//
// 8. **Event Handling**
//    - `tg_get_event()` returns a `tg_event` struct:
//        * key events: arrow keys, enter, escape, backspace, printable chars
//        * mouse events: click positions and button state
//        * returns `TG_EV_NONE` if no event in non-blocking mode
//
// 9. **Terminal Size Queries**
//    - `tg_width()` / `tg_height()` return current terminal size in cells
//
// Notes:
// - Designed for POSIX terminals (Linux, macOS, BSD)
// - Intended for simple terminal UIs, text editors, games, and demos
// - Does not handle UTF-8 multi-byte input
// - Does not require external libraries (ncurses, terminfo, etc.)
//
// References:
// - termios, tcgetattr, tcsetattr: man 3 termios
//   https://man7.org/linux/man-pages/man3/termios.3.html
// - fcntl for non-blocking mode: man 2 fcntl
//   https://man7.org/linux/man-pages/man2/fcntl.2.html
// - ioctl for terminal size: man 2 ioctl
//   https://man7.org/linux/man-pages/man2/ioctl.2.html
// - signal handling (SIGWINCH): man 7 signal
//   https://man7.org/linux/man-pages/man7/signal.7.html
// - xterm control sequences: https://invisible-island.net/xterm/ctlseqs/ctlseqs.pdf
// ================================================================

#ifndef TERMGFX_H
#define TERMGFX_H

#include <ctype.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>
#include <sys/select.h>

// --- Saved terminal state ---
static struct termios tg_old_termios;
static int tg_term_w = 80, tg_term_h = 24;

// --- Color helpers ---
static inline void tg_set_fg(int r, int g, int b) { printf("\x1b[38;2;%d;%d;%dm", r, g, b); }
static inline void tg_set_bg(int r, int g, int b) { printf("\x1b[48;2;%d;%d;%dm", r, g, b); }
static inline void tg_reset(void) { printf("\x1b[0m"); }

// --- Cursor helpers ---
static inline void tg_move(int x, int y) { printf("\x1b[%d;%dH", y + 1, x + 1); }
static inline void tg_cursor_hide(void) { printf("\x1b[?25l"); }
static inline void tg_cursor_show(void) { printf("\x1b[?25h"); }

// --- Screen helpers ---
static inline void tg_clear(void) { printf("\x1b[2J\x1b[H"); }
static inline void tg_alt_screen_enable(void) { printf("\x1b[?1049h"); }
static inline void tg_alt_screen_disable(void) { printf("\x1b[?1049l"); }

// --- Drawing ---
static inline void tg_draw_box(int x, int y, int w, int h, char c)
{
    for (int j = 0; j < h; j++) {
        tg_move(x, y + j);
        for (int i = 0; i < w; i++)
            putchar(c);
    }
}
static inline void tg_print_text(int x, int y, const char* s)
{
    tg_move(x, y);
    fputs(s, stdout);
}
static inline void tg_print_text_with_length(int x, int y, const char* s, int length)
{
    tg_move(x, y);
    fwrite(s, sizeof(char), length, stdout);
}

// --- Raw mode ---
static inline void tg_enable_raw(void)
{
    struct termios raw;
    tcgetattr(STDIN_FILENO, &tg_old_termios);
    raw = tg_old_termios;
    raw.c_lflag &= ~(ICANON | ECHO);
    raw.c_cc[VMIN] = 1;
    raw.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSANOW, &raw);
}
static inline void tg_disable_raw(void) { tcsetattr(STDIN_FILENO, TCSANOW, &tg_old_termios); }

// --- Terminal size ---
static inline void tg_update_size(void)
{
    struct winsize ws;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == 0) {
        tg_term_w = ws.ws_col;
        tg_term_h = ws.ws_row;
    }
}
static inline int tg_width(void) { return tg_term_w; }
static inline int tg_height(void) { return tg_term_h; }

// optional: call in main to auto-update on SIGWINCH
static void tg_handle_winch(int sig)
{
    (void)sig;
    tg_update_size();
}
static inline void tg_install_winch_handler(void)
{
    signal(SIGWINCH, tg_handle_winch);
    tg_update_size();
}

// --- Input ---
static inline int tg_getch(void) { return getchar(); }
static inline void tg_set_nonblocking(int enable)
{
    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    if (flags == -1)
        return;
    if (enable)
        fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);
    else
        fcntl(STDIN_FILENO, F_SETFL, flags & ~O_NONBLOCK);
}

// X10 / VT200 style mouse reporting
static inline void tg_mouse_enable(void)
{
    printf("\x1b[?1000h"); // send mouse click events
    printf("\x1b[?1006h"); // enable SGR extended mode
    fflush(stdout);
}
static inline void tg_mouse_disable(void)
{
    printf("\x1b[?1000l");
    printf("\x1b[?1006l");
    fflush(stdout);
}


void tg_wait_for_keypress(int timeout_ms) {
    fd_set fds;
    struct timeval tv;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);

    tv.tv_sec  = timeout_ms / 1000;
    tv.tv_usec = (timeout_ms % 1000) * 1000;

    select(STDIN_FILENO + 1, &fds, NULL, NULL,
                     timeout_ms >= 0 ? &tv : NULL);

    return; // timeout or no data
}


// --- Event types ---
typedef enum { TG_EV_NONE = 0,
    TG_EV_KEY,
    TG_EV_MOUSE } tg_event_type;

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
    int x, y;
    int button;
} tg_mouse;

typedef struct {
    tg_event_type type;
    union {
        tg_key key;
        tg_mouse mouse;
    } data;
    char ch; // valid if key == TG_KEY_CHAR
} tg_event;

// returns TG_EV_NONE if no event
static inline tg_event tg_get_event(void)
{
    tg_event ev = { 0 };
    int c = getchar();
    if (c == EOF)
        return ev;

    if (c == 0x1b) { // ESC sequence
        int c1 = getchar();
        if (c1 == '[') { // CSI
            int c2 = getchar();
            switch (c2) {
            case 'A':
                ev.type = TG_EV_KEY;
                ev.data.key = TG_KEY_UP;
                return ev;
            case 'B':
                ev.type = TG_EV_KEY;
                ev.data.key = TG_KEY_DOWN;
                return ev;
            case 'C':
                ev.type = TG_EV_KEY;
                ev.data.key = TG_KEY_RIGHT;
                return ev;
            case 'D':
                ev.type = TG_EV_KEY;
                ev.data.key = TG_KEY_LEFT;
                return ev;
            case '<': { // mouse SGR
                int b = 0, x = 0, y = 0;
                char m;
                if (scanf("%d;%d;%d%c", &b, &x, &y, &m) == 4) {
                    ev.type = TG_EV_MOUSE;
                    ev.data.mouse.x = x - 1;
                    ev.data.mouse.y = y - 1;
                    ev.data.mouse.button = b & 3;
                    return ev;
                }
            }
            }
        } else if (c1 == EOF) { // single ESC key
            ev.type = TG_EV_KEY;
            ev.data.key = TG_KEY_ESC;
            return ev;
        }
    }

    // normal keys
    ev.type = TG_EV_KEY;
    if (c == '\r' || c == '\n')
        ev.data.key = TG_KEY_ENTER;
    else if (c == 127 || c == 8)
        ev.data.key = TG_KEY_BACKSPACE;
    else if (isprint(c)) {
        ev.data.key = TG_KEY_CHAR;
        ev.ch = (char)c;
    } else
        ev.data.key = TG_KEY_UNKNOWN;
    return ev;
}

#endif // TERMGFX_H
