#include "common.h"
#include "syscall.h"
#include "am.h"

int fs_open(const char *pathname, int flags, int mode);
ssize_t fs_read(int fd, void *buf, size_t len);
ssize_t fs_write(int fd, const void *buf, size_t len);
off_t fs_lseek(int fd, off_t offset, int whence);
int fs_close(int fd);

static inline _RegSet* sys_none(_RegSet *r) {
  SYSCALL_ARG1(r) = 1;
  return NULL;
}

static inline _RegSet* sys_exit(_RegSet *r) {
  _halt(SYSCALL_ARG2(r));
  return NULL;
}

static inline _RegSet* sys_write(_RegSet *r) {
  int fd = (int)SYSCALL_ARG2(r);
  char* buf = (char*)SYSCALL_ARG3(r);
  size_t count = (size_t)SYSCALL_ARG4(r);
  if (fd == 1 || fd == 2) {
    for(int i = 0; i < count; i++){
      _putc(buf[i]);
    }
    SYSCALL_ARG1(r) = SYSCALL_ARG4(r);
  }
  else {
    panic("Unhandled fd=%d in sys_write.\n", fd);
    SYSCALL_ARG1(r) = -1;
  }
  return NULL; 
}

static inline _RegSet* sys_brk(_RegSet* r) {
  SYSCALL_ARG1(r) = 0;
  return NULL;
}

static inline _RegSet* sys_fopen(_RegSet* r) {
  char* pathname = (char*)SYSCALL_ARG2(r);
  int flags = (int)SYSCALL_ARG3(r);
  int mode = (int)SYSCALL_ARG4(r);
  int fd = fs_open(pathname, flags, mode);
  SYSCALL_ARG1(r) = fd;
  return NULL;
}

static inline _RegSet* sys_fclose(_RegSet* r) {
  int fd = (int)SYSCALL_ARG2(r);
  int info = fs_close(fd);
  SYSCALL_ARG1(r) = info;
  return NULL;
}

static inline _RegSet* sys_fread(_RegSet* r) {
  int fd = (int)SYSCALL_ARG2(r);
  void* buf = (void*)SYSCALL_ARG3(r);
  size_t len = (size_t)SYSCALL_ARG4(r);
  size_t res = fs_read(fd, buf, len);
  SYSCALL_ARG1(r) = res;
  return NULL;
}

static inline _RegSet* sys_fwrite(_RegSet* r) {
  int fd = (int)SYSCALL_ARG2(r);
  void* buf = (void*)SYSCALL_ARG3(r);
  size_t len = (size_t)SYSCALL_ARG4(r);
  size_t res = fs_write(fd, buf, len);
  SYSCALL_ARG1(r) = res;
  return NULL;
}

static inline _RegSet* sys_lseek(_RegSet* r) {
  int fd = (int)SYSCALL_ARG2(r);
  off_t offset = (off_t)SYSCALL_ARG3(r);
  int whence = (int)SYSCALL_ARG4(r);
  size_t res = fs_lseek(fd, offset, whence);
  SYSCALL_ARG1(r) = res;
  return NULL;
}

_RegSet* do_syscall(_RegSet *r) {
  uintptr_t a[4];
  a[0] = SYSCALL_ARG1(r);
  a[1] = SYSCALL_ARG2(r);
  a[2] = SYSCALL_ARG3(r);
  a[3] = SYSCALL_ARG4(r);

  switch (a[0]) {
    case SYS_none: return sys_none(r);
    case SYS_exit: return sys_exit(r);
    case SYS_brk: return sys_brk(r);
    case SYS_write: return sys_write(r);
    case SYS_read: return sys_fread(r);
    case SYS_open: return sys_fopen(r);
    case SYS_close: return sys_fclose(r);
    case SYS_lseek: return sys_lseek(r);
    
    default: panic("Unhandled syscall ID = %d", a[0]);
  }

  return NULL;
}
