#include "common.h"

#define NAME(key) \
  [_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [_KEY_NONE] = "NONE",
  _KEYS(NAME)
};

// extern _Screen _screen;

size_t events_read(void *buf, size_t len) {
  return 0;
}

static char dispinfo[128] __attribute__((used));

void dispinfo_read(void *buf, off_t offset, size_t len) {
  memcpy(buf, dispinfo+offset, len);
}

void fb_write(const void *buf, off_t offset, size_t len) {
  int size = sizeof(uint32_t);
  int index = offset / size;
  int line = len / size;
  int start_x = index % _screen.width;
  int start_y = index / _screen.width;
  if (line + start_x <= _screen.width) {
    _draw_rect(buf, start_x, start_y, line, 1);
  }
  else if (line + start_x <= 2 * _screen.width) {
    int tmp = _screen.width - start_x;
    _draw_rect(buf, start_x, start_y, tmp, 1);
    _draw_rect(buf + tmp * size, 0, start_y + 1, line - tmp, 1);
  }
  else {
    int line1 = _screen.width - start_x;
    int lines = (line - line1) / _screen.width;
    int lline = line - line - lines * _screen.width;
    _draw_rect(buf, start_x, start_y, line1, 1);
    _draw_rect(buf + line1 * size, 0, start_y + 1, _screen.width, lines);
    _draw_rect(buf + (line1 + lines * _screen.width) * size, 0, start_y + lines, lline, 1);
  }
  
}

void init_device() {
  _ioe_init();

  // TODO: print the string to array `dispinfo` with the format
  // described in the Navy-apps convention
  sprintf(dispinfo, "WIDTH:%d\tHEIGHT:%d\t", _screen.width, _screen.height);
}
