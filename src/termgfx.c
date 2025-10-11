#include <fcntl.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <termgfx.h>
#include <termios.h>
#include <unistd.h>
#include <utf8.h>

Cell* framebuffer = NULL;
Cell* backbuffer = NULL;

int operations;

// --- Saved terminal state ---
static struct termios tg_old_termios;
static int tg_term_w = 80, tg_term_h = 24;
Clay_Color current_fg;
Clay_Color current_bg;

void tg_move(int x, int y) { printf("\x1b[%d;%dH", y + 1, x + 1); }

// --- Clay_Color helpers ---
void _term_tg_set_fg(Clay_Color* c) { printf("\x1b[38;2;%d;%d;%dm", (int)c->r, (int)c->g, (int)c->b); }
void _term_tg_set_bg(Clay_Color* c) { printf("\x1b[48;2;%d;%d;%dm", (int)c->r, (int)c->g, (int)c->b); }

bool same_color(Clay_Color* a, Clay_Color* b) { return a->b == b->b && a->g == b->g && a->r == b->r; }
void copy_color(Clay_Color* dest, Clay_Color* src)
{
    dest->r = src->r;
    dest->g = src->g;
    dest->b = src->b;
}

void tg_set_bg(Clay_Color* c) { copy_color(&current_bg, c); }
void tg_set_fg(Clay_Color* c) { copy_color(&current_fg, c); }
void tg_reset(void)
{
    printf("\x1b[0m");
    current_fg.r = 255;
    current_fg.g = 255;
    current_fg.b = 255;
    current_bg.r = 0;
    current_bg.g = 0;
    current_bg.b = 0;
    tg_move(0, 0);
}

// --- Cursor helpers ---
void tg_cursor_hide(void) { printf("\x1b[?25l"); }
void tg_cursor_show(void) { printf("\x1b[?25h"); }

// --- Screen helpers ---
void tg_clear(void) { printf("\x1b[2J\x1b[H"); }
void tg_alt_screen_enable(void) { printf("\x1b[?1049h"); }
void tg_alt_screen_disable(void) { printf("\x1b[?1049l"); }

void paint_color(int x, int y)
{
    if (!(same_color(&current_fg, TRANSPARENT))) {
        backbuffer[x + tg_term_w * y].fg = current_fg;
    }
    if (!(same_color(&current_bg, TRANSPARENT))) {
        backbuffer[x + tg_term_w * y].bg = current_bg;
    }
}

// --- Drawing ---
void tg_draw_box(int x, int y, int w, int h, char c)
{
    for (int j = 0; j < h; j++) {
        for (int i = 0; i < w; i++) {
            backbuffer[(x + i) + tg_term_w * (y + j)].value[0] = c;
            paint_color(x + i, y + j);
        }
    }
}
int tg_print_text(int x, int y, const char* s)
{
    return tg_print_text_with_length(x, y, s, strlen(s));
}
int tg_print_text_with_length(int x, int y, const char* s, int length)
{
    bufferProviderData data = {s, 0};
    char buf[4];
    int i = 0;
    while (data.pos < length) {
        read_utf8_char(buf, buffer_provider, &data);
        if (buf[0] == '\n') {
            i = 0;
            y++;
            continue;
        }
        memcpy(backbuffer[x + i + tg_term_w * y].value, buf, CHAR_LEN(buf[0]));
        paint_color(x + i, y);
        i++;
    }
    return i;
}

void tg_flush()
{
    tg_reset();
    // _term_tg_set_bg(current_bg.r, current_bg.g, current_bg.b);
    // _term_tg_set_fg(current_fg.r, current_fg.g, current_fg.b);
    operations = 0;
    // The dirty flags is set when position must be reset using tg_move
    // This is necessary when skipping portions not to redraw
    // But when drawing continuously, its ok to not tg_move each time
    // as putchar already moves us.
    bool dirty = false;
    for (int y = 0; y < tg_term_h; y++) {
        for (int x = 0; x < tg_term_w; x++) {
            Cell* c = &backbuffer[x + y * tg_term_w];
            Cell* cf = &framebuffer[x + y * tg_term_w];
            // If the backbuffer is the same as frontbuffer, not need to update anything
            bool sameBg = same_color(&c->bg, &cf->bg);
            bool sameFg = same_color(&c->fg, &cf->fg);
            bool sameValue = c->value == cf->value;
            if (sameValue && sameBg && sameFg) {

                dirty = true;
                continue;
            }
            if (!same_color(&current_fg, &c->fg)) {
                copy_color(&current_fg, &c->fg);
                _term_tg_set_fg(&c->fg);
                operations++;
            }
            if (!same_color(&current_bg, &c->bg)) {
                copy_color(&current_bg, &c->bg);
                _term_tg_set_bg(&c->bg);
                operations++;
            }
            if (dirty) {
                dirty = false;
                operations++;
                tg_move(x, y);
            }
            for (int i = 0; i < CHAR_LEN(c->value[0]); i++)
                putchar(c->value[i]);
            operations++;
        }
    }
    memcpy(framebuffer, backbuffer, tg_term_h * tg_term_w * sizeof(Cell));
    fflush(stdout);
}

// --- Raw mode ---
void tg_enable_raw(void)
{
    struct termios raw;
    tcgetattr(STDIN_FILENO, &tg_old_termios);
    raw = tg_old_termios;
    raw.c_lflag &= ~(ICANON | ECHO);
    raw.c_cc[VMIN] = 1;
    raw.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSANOW, &raw);
}
void tg_disable_raw(void) { tcsetattr(STDIN_FILENO, TCSANOW, &tg_old_termios); }

// --- Terminal size ---
void tg_update_size(void)
{
    struct winsize ws;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == 0) {
        if (ws.ws_col != tg_term_w || ws.ws_row != tg_term_h) {
            if (framebuffer)
                free(framebuffer);
            if (backbuffer)
                free(backbuffer);
            backbuffer = calloc(ws.ws_col * ws.ws_row, sizeof(Cell));
            framebuffer = calloc(ws.ws_col * ws.ws_row, sizeof(Cell));
        }
        tg_term_w = ws.ws_col;
        tg_term_h = ws.ws_row;
    }
}
int tg_width(void) { return tg_term_w; }
int tg_height(void) { return tg_term_h; }

// optional: call in main to auto-update on SIGWINCH
static void tg_handle_winch(int sig)
{
    (void)sig;
    tg_update_size();
}
void tg_install_winch_handler(void)
{
    signal(SIGWINCH, tg_handle_winch);
    tg_update_size();
}

// --- Input ---
int tg_getch(void) { return getchar(); }
void tg_set_nonblocking(int enable)
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
void tg_mouse_enable(void)
{
    printf("\x1b[?1000h"); // send mouse click events
    printf("\x1b[?1006h"); // enable SGR extended mode
    fflush(stdout);
}
void tg_mouse_disable(void)
{
    printf("\x1b[?1000l");
    printf("\x1b[?1006l");
    fflush(stdout);
}

void tg_wait_for_keypress(int timeout_ms)
{
    fd_set fds;
    struct timeval tv;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);

    tv.tv_sec = timeout_ms / 1000;
    tv.tv_usec = (timeout_ms % 1000) * 1000;

    select(STDIN_FILENO + 1, &fds, NULL, NULL, timeout_ms >= 0 ? &tv : NULL);

    return; // timeout or no data
}

// returns TG_EV_NONE if no event
tg_event tg_get_event(void)
{
    tg_event ev = { 0 };
    char fullChar[4];
    if (read_utf8_char(fullChar, getchar_provider, NULL) == EOF) {
        return ev;
    }
    char c = fullChar[0];

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

    // TODO: Check if UTF8 is valid.
    // else if (iswprint(c)) {
    else {
        ev.data.key = TG_KEY_CHAR;
        memcpy(ev.fullChar, fullChar, 4 * sizeof(char));
        ev.ch = c;
    }
    return ev;
}
