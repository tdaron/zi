#include <termgfx.h>
#include <buffer.h>

Buffer* b;

void draw() {
  tg_reset();
  tg_clear();
  tg_update_size();
  tg_set_fg(255, 255, 255);
  tg_print_text(0, 0, "Zi Editor");
  tg_print_text(0, 5, get_raw_content(b));
  fflush(stdout);
}

void render() {
  b = new_buffer(NULL, 0);
  tg_alt_screen_enable();
  tg_enable_raw();
  tg_set_nonblocking(1);
  tg_cursor_hide();
  tg_install_winch_handler();
  tg_mouse_enable();

  tg_event ev;
  while (1) {
    ev = tg_get_event();
    if (ev.type == TG_EV_KEY) {
      if (ev.data.key == TG_KEY_ESC)
        break;
      if (ev.data.key == TG_KEY_BACKSPACE) delete_chars(b, 1);
      if (ev.data.key == TG_KEY_CHAR) insert_char(b, ev.ch);
    }
    draw();

    // optional: throttle loop
    usleep(16000);
  }

  tg_mouse_disable();
  tg_set_nonblocking(0);
  tg_disable_raw();
  tg_alt_screen_disable();
  tg_cursor_show();
}
