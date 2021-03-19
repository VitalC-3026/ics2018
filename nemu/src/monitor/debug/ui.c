#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint64_t);

/* We use the `readline' library to provide more flexibility to read from stdin. */
char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}

static int cmd_q(char *args) {
  return -1;
}

static int cmd_help(char *args);

static int cmd_si(char *args); // single step to debug

static int cmd_info(char *args); // print info, r: registers, w: watchpoints

static int cmd_p(char *args); // calculate for the expression

static int cmd_x(char *args); // scan the internal storage

static int cmd_w(char *args); // set watchpoints

static int cmd_d(char *args); // delete watchpoints

static struct {
  char *name;
  char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },

  /* TODO: Add more commands */
  {"si", "Single-step execution", cmd_si },
  {"info", "Print information of registers or watchpoints", cmd_info },
  {"p", "Compute the result of an expression", cmd_p},
  {"x", "Scan the memory", cmd_x },
  {"w", "Set watchpoints", cmd_w},
  {"d", "Delete watchpoints", cmd_d}
};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

static int cmd_si(char* args) {
  char* arg = strtok(args, " ");
  if (arg == NULL) {
    cpu_exec(1);
    // printf("si 1 OK!\n");
  }
  else {
    int num = atoi(arg);
    if (strtok(NULL, " ") != NULL) {
      printf("Too many arguments!\n");
      return 0;
    }
    cpu_exec(num);
    // printf("si %d OK!\n", num);
  }
  return 0;
}

static int cmd_info(char* args) {
  char* arg = strtok(args, " ");
  if (arg == NULL) {
    printf("Too few arguments.\n");
  }
  else {
    if (strtok(NULL, " ") != NULL) {
      printf("Too many arguments.\n");
      return 0;
    }
    if (strcmp(arg, "r") == 0) {
      printf("eax: 0x%08x.\n", cpu.eax);
      printf("ecx: 0x%08x.\n", cpu.ecx);
      printf("edx: 0x%08x.\n", cpu.edx);
      printf("ebx: 0x%08x.\n", cpu.ebx);
      printf("esp: 0x%08x.\n", cpu.esp);
      printf("ebp: 0x%08x.\n", cpu.ebp);
      printf("esi: 0x%08x.\n", cpu.esi);
      printf("edi: 0x%08x.\n", cpu.edi);
    }
    else if (strcmp(arg, "w") == 0) {
      show_wp();
    }
  }
  return 0;
}

static int cmd_x(char* args) {
  char* arg = strtok(args, " ");
  if (arg == NULL) {
    printf("Too few arguments!\n");
  }
  int num = atoi(arg);
  arg = strtok(NULL, " ");
  if (arg == NULL) {
    printf("Too few arguments!\n");
    return 0;
  }
  if (strtok(NULL, " ") != NULL) {
    printf("Too many arguments!\n");
    return 0;
  }
  if (arg[0] != '0' || arg[1] != 'x') {
    printf("Unknown address!\n");
    return 0;
  }
  char* str;
  vaddr_t vaddr;
  vaddr = strtol(arg, &str, 16); // get the virtual address from the command line
  for(int i = 0; i < num; i++) {
    uint32_t data = vaddr_read(vaddr + 4 * i, 4);
    printf("0x%08x ", vaddr + 4 * i);
    for(int j = 0; j < 3; j++) {
      printf("0x%02x ", data & 0xff);
      data = data >> 8;
    }
    printf("0x%02x\n", data & 0xff);
  }
  return 0;
}

static int cmd_p(char* args) {
  char* arg = strtok(args, " ");
  if (arg == NULL) {
    printf("Too few arguments!\n");
    return 0;
  }
  char* sub = strtok(NULL, " ");
  while (sub != NULL) {
    strcat(arg, sub);
    sub = strtok(NULL, " ");
  }
  bool success;
  int res = expr(arg, &success);
  if(success) {
    printf("res = %d\n", res);
  }
  return 0;
}

static int cmd_w(char* args) {
  char* arg = strtok(args, " ");
  if (arg == NULL) {
    printf("Too few arguments!\n");
    return 0;
  }
  char* sub = strtok(NULL, " ");
  while (sub != NULL) {
    strcat(arg, sub);
    sub = strtok(NULL, " ");
  }
  bool success;
  WP* wp = new_wp();
  printf("%d\n", wp->NO);
  wp->expr = (char*)malloc(strlen(arg)*sizeof(char));
  memset(wp->expr, 0, strlen(arg));
  strcpy(wp->expr, arg);
  printf("%s\n", wp->expr);
  wp->value = expr(arg, &success);
  printf("Set a watchpoint %d on %s", wp->NO, wp->expr);
  return 0;
}

static int cmd_d(char* args) {
  char* arg = strtok(args, " ");
  if(arg == NULL) {
    printf("Too few arguments.\n");
    return 0;
  }
  if(strtok(NULL, " ") != NULL) {
    printf("Too many arguments.\n");
    return 0;
  }
  for(int i = 0; i < strlen(arg); i++) {
    if (arg[i] > '9' || arg[i] < '0') {
      printf("Invalid expression.\n");
      return 0;
    }
  }
  int n = atoi(arg);
  if (n >= 32) {
    printf("Only 32 watchpoints(No: 0-31) provided.\n");
    return 0;
  }
  free_wp(n);
  printf("Successfully delete watchpoint %d.\n", n);
  return 0;
}

void ui_mainloop(int is_batch_mode) {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  while (1) {
    char *str = rl_gets();
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef HAS_IOE
    extern void sdl_clear_event_queue(void);
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}
