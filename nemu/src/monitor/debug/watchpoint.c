#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP];
static WP *head, *free_;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].used = false;
    wp_pool[i].next = &wp_pool[i + 1];
  }
  wp_pool[NR_WP - 1].next = NULL;

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */
WP* new_wp() {
  WP* current = head;
  if (free_ == NULL) {
    return NULL;
  }
  if (current == NULL) {
    printf("head null\n");
    current = free_;
    free_ = free_->next;
    current->next = NULL;
    current->used = true;
    return current;
  }
  else {
    while(current->next != NULL) {
      current = current->next;
    }
    current->next = free_;
    free_ = free_->next;
    current->next->next = NULL;
    current->next->used = true;
    return current->next;
  }
}

void free_wp(int no) {
  WP* curr_head = head;
  WP* curr_free = free_;
  if (curr_head == NULL || (curr_head->next == NULL && curr_head->NO != no)) {
    printf("Error: try to free a nonexistent watchpoint.\n");
    assert(0);
  }
  while(curr_head->next->NO != no) {
    curr_head = curr_head->next;
  }
  WP* tmp = curr_head->next;
  curr_head->next = curr_head->next->next;
  if(free_ == NULL) {
    tmp->next = NULL;
    tmp->used = false;
    free_ = tmp;
  }
  else {
    while(curr_free->next != NULL && curr_free->next->NO < no) {
      curr_free = curr_free->next;
    }
    if (curr_free->next == NULL && curr_free->NO < no) {
      tmp->used = false;
      tmp->next = NULL;
      curr_free->next = tmp;
    }
    else if (curr_free->next == NULL && curr_free->NO > no) {
      tmp->next = curr_free;
      tmp->used = false;
      free_ = tmp;
    }
    else {
      tmp->next = curr_free->next;
      tmp->used = false;
      curr_free->next = tmp;
    }
  }
}

void show_wp() {
  WP* curr = head;
  printf("               expr\tvalue\n");
  while (curr != NULL) {
    printf("watchpoint %d: %s\t%d\n", curr->NO, curr->expr, curr->value);
    curr = curr->next;
  }
}