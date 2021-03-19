#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;

  /* TODO: Add more members if necessary */
  bool used;
  char* value;
  char* expr;
} WP;

void init_wp_pool();
WP* new_wp();
void free_wp(int);
void show_wp();

#endif
