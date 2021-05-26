#include "common.h"

#define DEFAULT_ENTRY ((void *)0x4000000)

void ramdisk_read(void* buf, off_t offset, size_t len);
void ramdisk_write(const void* buf, off_t offset, size_t len);
size_t get_ramdisk_size();

int fs_open(const char *pathname, int flags, int mode);
size_t fs_filesz(int fd);
size_t fs_read(int fd, void *buf, size_t len);
int fs_close(int fd);

const char filename[] = "/bin/text";

uintptr_t loader(_Protect *as, const char *filename) {
  //TODO();
  // raw program loader
  // size_t len = get_ramdisk_size();
  // ramdisk_read(DEFAULT_ENTRY, 0, len);
  // return (uintptr_t)DEFAULT_ENTRY;
  // raw filesystem loader
  int fd = fs_open(filename, 0, 0);
  size_t size = fs_filesz(fd);
  fs_read(fd, DEFAULT_ENTRY, size);
  fs_close(fd);
  return (uintptr_t)DEFAULT_ENTRY;
}
