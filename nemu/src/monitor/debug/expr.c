#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>
#include <stdlib.h>

enum {
  TK_NOTYPE = 256, TK_EQ, 
  /* TODO: Add more token types */
  TK_ADD, TK_MIN, TK_MUL, TK_DIV,
  TK_LP, TK_RP, TK_NUM
};

static struct rule {
  char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},    // spaces
  {"\\+", TK_ADD},         // plus
  {"-", TK_MIN},           // minus
  {"\\*", TK_MUL},         // multiply
  {"/", TK_DIV},           // divide
  {"(", TK_LP},         // (
  {")", TK_RP},         // )
  {"[1-9][0-9]* | 0", TK_NUM}, // decimal
  {"==", TK_EQ}         // equal
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX];

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

Token tokens[32];
int nr_token;

// identify token types of each part of the string e
static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      // TOQ: pmatch.rm_so must be 0? who initialize it?
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);
        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        switch (rules[i].token_type) {
          case TK_ADD: {
            Token t;
            t.type = TK_ADD;
            t.str[0] = '+';
            t.str[1] = '\0';
            tokens[nr_token] = t;
            nr_token += 1;
            break;
          }
          case TK_MIN: {
            Token t;
            t.type = TK_MIN;
            t.str[0] = '-';
            t.str[1] = '\0';
            tokens[nr_token] = t;
            nr_token += 1;
            break;
          }
          case TK_MUL: {
            Token t;
            t.type = TK_MUL;
            t.str[0] = '*';
            t.str[1] = '\0';
            tokens[nr_token] = t;
            nr_token += 1;
            break;
          }
          case TK_DIV: {
            Token t;
            t.type = TK_DIV;
            t.str[0] = '/';
            t.str[1] = '\0';
            tokens[nr_token] = t;
            nr_token += 1;
            break;
          }
          case TK_LP: {
            Token t;
            t.type = TK_LP;
            t.str[0] = '(';
            t.str[1] = '\0';
            tokens[nr_token] = t;
            nr_token += 1;
            break;
          }
          case TK_RP: {
            Token t;
            t.type = TK_RP;
            t.str[0] = ')';
            t.str[1] = '\0';
            tokens[nr_token] = t;
            nr_token += 1;
            break;
          }
          case TK_NUM: {
            Token t;
            t.type = TK_NUM;
            int j;
            for (j = 0; j < strlen(rules[i].regex); j++) {
              t.str[j] = rules[i].regex[j];
            }
            t.str[j] = '\0';
            tokens[nr_token] = t;
            nr_token++;
            break;
          }
          default: TODO();
        }
        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}

// TODO: distinguish (2*3))+4 false / (2+3)*(5-9) false => can be computed
bool check_parentheses(int p, int q) {
  if (tokens[p].type != TK_LP || tokens[q].type != TK_RP) {
    return false;
  }
  int t = p + 1;
  int count = 0;
  while(t < q) {
    if (tokens[t].type == TK_LP) {
      count++;
    }
    else if (tokens[t].type == TK_RP) {
      count--;
    }
  }
  if (count != 0) {
    return false;
  }
  return true;
}

int find_operator(int p, int q) {
  int len = (q - p) + 1;
  int * stack = malloc(sizeof(int) * len);
  memset(stack, 0, len);
  int t = p;
  int count = 0;
  int loc = p;
  // restriction: count < len
  while(t <= q) {
    if(tokens[t].type == TK_ADD || tokens[t].type == TK_MIN) {
      if(count == 0) {
        stack[count] = tokens[t].type;
        loc = t;
        count++;
      }
      else {
        switch (stack[count - 1]){
          case TK_ADD:{
            stack[count - 1] = tokens[t].type;
            loc = t;
            break;
          }
          case TK_MIN:{
            stack[count - 1] = tokens[t].type;
            loc = t;
            break;
          }
          case TK_MUL: {
            stack[count - 1] = tokens[t].type;
            loc = t;
            break;
          }
          case TK_DIV: {
            stack[count - 1] = tokens[t].type;
            loc = t;
            break;
          }
          case TK_LP: {
            stack[count] = tokens[t].type;
            count++;
            break;
          }
          case TK_RP: {
            int tmp = count - 1;
            while(stack[tmp] != TK_LP || tmp >= 0) {
              stack[tmp] = 0;
              tmp--;
            }
            if (tmp < 0) {
              printf("impossible to reach here!\n");
            }
            if (tmp != 0) {
              stack[tmp - 1] = tokens[t].type;
              loc = t;
              count = tmp;
            }
            break;
          }
        }
      }
    }
    else if (tokens[t].type == TK_MUL || tokens[t].type == TK_DIV){
      if (count == 0) {
        stack[count] = tokens[t].type;
        loc = t;
        count++;
      }
      else {
        switch (stack[count - 1]) {
          case TK_ADD: {
            stack[count] = tokens[t].type;
            count++;
            break;
          }
          case TK_MIN: {
            stack[count] = tokens[t].type;
            count++;
            break;
          }
          case TK_MUL: {
            stack[count - 1] = tokens[t].type;
            loc = t;
            break;
          }
          case TK_DIV: {
            stack[count - 1] = tokens[t].type;
            loc = t;
            break;
          }
          case TK_LP: {
            stack[count] = tokens[t].type;
            count++;
            break;
          }
          case TK_RP: { break; }
        }
      }
    }
    else if (tokens[t].type == TK_LP) {
      stack[count] = TK_LP;
      count++;
    }
    else if (tokens[t].type == TK_RP) {
      if(count == 0) {
        printf("impossible to reach here! THe first character of the string is RP.\n");
      }
      int tmp = count;
      while(stack[tmp] != TK_LP || tmp >= 0) {
        stack[tmp] = 0;
        tmp--;
      }
      if (tmp < 0) {
        printf("impossible to reach here! Incompatible parentheses.\n");
      }
      count = tmp - 1;
    }
  }
  free(stack);
  return loc;
}

int evaluate(int p, int q) {
  if (p > q) {
    printf("Error: bad expression!\n");
    return -1;
  }
  else if (p == q) {
    /*
      Single token, must be a number.
    */
    return atoi(tokens[p].str);
  }
  else if (check_parentheses(p, q)) {
    return evaluate(p + 1, q - 1);
  }
  else {
    int op = find_operator(p, q);
    int val1 = evaluate(p, op - 1);
    int val2 = evaluate(op + 1, q);
    switch (tokens[op].type) {
      case TK_ADD: return val1 + val2;
      case TK_MIN: return val1 - val2;
      case TK_MUL: return val1 * val2;
      case TK_DIV: return val1 / val2;
      default: assert(0);
    }

  }
}

uint32_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  /* TODO: Insert codes to evaluate the expression. */
  int p = 0, q = nr_token - 1;
  int value = evaluate(p, q);
  return value;
}
