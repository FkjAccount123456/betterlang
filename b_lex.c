#include "b_lex.h"
#include "b_stdlib.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 你说得对，但是O(1)
TokenType opstr_table[65536];

void init_opstr_table() {
  memset(opstr_table, 0, sizeof(opstr_table));

  opstr_table['+'] = AddToken;
  opstr_table['-'] = SubToken;
  opstr_table['*'] = MulToken;
  opstr_table['/'] = DivToken;
  opstr_table['%'] = ModToken;
  opstr_table['&'] = BitAndToken;
  opstr_table['|'] = BitOrToken;
  opstr_table['^'] = XorToken;
  opstr_table['~'] = InvToken;
  opstr_table['!'] = NotToken;
  opstr_table['>'] = GtToken;
  opstr_table['<'] = LtToken;
  opstr_table['='] = Assign;
  opstr_table['('] = LParen;
  opstr_table[')'] = RParen;
  opstr_table['['] = LSqBr;
  opstr_table[']'] = RSqBr;
  opstr_table['{'] = Begin;
  opstr_table['}'] = End;
  opstr_table[','] = Comma;
  opstr_table['.'] = Dot;
  opstr_table[';'] = Semi;
  opstr_table[':'] = Colon;

  opstr_table['=' * 128 + '='] = EqToken;
  opstr_table['!' * 128 + '='] = NeToken;
  opstr_table['>' * 128 + '='] = GeToken;
  opstr_table['<' * 128 + '='] = LeToken;
  opstr_table['&' * 128 + '&'] = AndToken;
  opstr_table['|' * 128 + '|'] = OrToken;
  opstr_table['<' * 128 + '<'] = LshToken;
  opstr_table['>' * 128 + '>'] = RshToken;

  opstr_table['+' * 128 + '='] = AddEqToken;
  opstr_table['-' * 128 + '='] = SubEqToken;
  opstr_table['*' * 128 + '='] = MulEqToken;
  opstr_table['/' * 128 + '='] = DivEqToken;
  opstr_table['%' * 128 + '='] = ModEqToken;
  opstr_table['&' * 128 + '='] = BitAndEqToken;
  opstr_table['|' * 128 + '='] = BitOrEqToken;
  opstr_table['^' * 128 + '='] = XorEqToken;

  // <<= >>=两个放tokenize里面，太长了存不下
}

Token Token_new(TokenType tp) {
  Token t;
  t.tp = tp;
  return t;
}

Token Token_int(long long intToken) {
  Token t = Token_new(IntToken);
  t.intToken = intToken;
  return t;
}

Token Token_float(double floatToken) {
  Token t = Token_new(FloatToken);
  t.floatToken = floatToken;
  return t;
}

Token Token_string(String *strToken) {
  Token t = Token_new(StringToken);
  t.strToken = strToken;
  return t;
}

Token Token_id(String *strToken) {
  Token t = Token_new(IdToken);
  t.strToken = strToken;
  return t;
}

TokenList *TokenList_new() {
  TokenList *tl = b_malloc(sizeof(TokenList));
  tl->len = 0;
  tl->max = 8;
  tl->tokens = b_malloc(sizeof(Token) * tl->max);
  return tl;
}

void TokenList_add(TokenList *tl, Token t) {
  if (tl->len == tl->max) {
    tl->max *= 2;
    tl->tokens = b_realloc(tl->tokens, sizeof(Token) * tl->max);
  }
  tl->tokens[tl->len++] = t;
}

void TokenList_free(TokenList *tl) {
  for (size_t i = 0; i < tl->len; i++) {
    if (tl->tokens[i].tp == StringToken || tl->tokens[i].tp == IdToken) {
      String_free(tl->tokens[i].strToken);
    }
  }
  b_free(tl->tokens);
  b_free(tl);
}

TokenList *tokenize(char *code) {
  TokenList *res = TokenList_new();
  char *cur = code;
  while (*cur) {
    while (*cur && (isspace(*cur) || *cur == '/' && cur[1] == '/' ||
                    *cur == '/' && cur[1] == '*')) {
      if (*cur == '/' && cur[1] == '/') {
        while (*cur && *cur != '\n')
          cur++;
      } else if (*cur == '/' && cur[1] == '*') {
        cur += 2;
        while (*cur && !(*cur == '*' && cur[1] == '/'))
          cur++;
        if (!*cur) {
          printf("LexError: unexpected EOF");
          exit(-1);
        }
        cur += 2;
      } else {
        cur++;
      }
    }

    if (!*cur) {
      break;
    } else if (isdigit(*cur)) {
      String *num = String_new("");
      bool isFloat = false;
      while (*cur && (isdigit(*cur) || *cur == '.')) {
        if (*cur == '.')
          isFloat = true;
        String_append(num, *cur++);
      }
      if (isFloat) {
        TokenList_add(res, Token_float(atof(num->v)));
      } else {
        TokenList_add(res, Token_int(atoll(num->v)));
      }
      String_free(num);
    } else if (isalpha(*cur) || *cur == '_') {
      String *id = String_new("");
      while (*cur && (isalnum(*cur) || *cur == '_')) {
        String_append(id, *cur++);
      }
      TokenList_add(res, Token_id(id));
    } else if (*cur == '\'' || *cur == '\"') {
      char x = *cur++;
      String *str = String_new("");
      while (*cur && *cur != x) {
        if (*cur == '\\') {
          cur++;
          if (!*cur) {
            printf("LexError: unexpected EOF");
            exit(-1);
          }
          switch (*cur) {
          case 'r':
            String_append(str, '\r');
            break;
          case 't':
            String_append(str, '\t');
            break;
          case 'a':
            String_append(str, '\a');
            break;
          case 'f':
            String_append(str, '\f');
            break;
          case 'v':
            String_append(str, '\v');
            break;
          case 'b':
            String_append(str, '\b');
            break;
          case 'n':
            String_append(str, '\n');
            break;
          case '\\':
            String_append(str, '\\');
            break;
          case '\'':
            String_append(str, '\'');
            break;
          case '\"':
            String_append(str, '\"');
            break;
          case 'x': {
            char ch = 0;
            for (char i = 0; i < 2; i++) {
              cur++;
              if (!*cur) {
                printf("LexError: unexpected EOF");
                exit(-1);
              }
              ch *= 16;
              if (*cur >= '0' && *cur <= '9') {
                ch += *cur - '0';
              } else if (*cur >= 'a' && *cur <= 'f') {
                ch += *cur - 'a' + 10;
              } else if (*cur >= 'A' && *cur <= 'F') {
                ch += *cur - 'A' + 10;
              } else {
                printf("LexError: unknown escape char '%c'", *cur);
                exit(-1);
              }
            }
            String_append(str, ch);
            break;
          }
          default:
            printf("LexError: unknown escape char '%c'", *cur);
            exit(-1);
          }
          cur++;
        } else {
          String_append(str, *cur++);
        }
      }
      if (!*cur) {
        printf("LexError: unexpected EOF");
        exit(-1);
      }
      cur++;
      TokenList_add(res, Token_string(str));
    } else if (cur[1] && cur[2] && cur[0] == '<' && cur[1] == '<' && cur[2] == '=') {
      TokenList_add(res, Token_new(LshEqToken));
      cur += 3;
    } else if (cur[1] && cur[2] && cur[0] == '>' && cur[1] == '>' && cur[2] == '=') {
      TokenList_add(res, Token_new(RshEqToken));
      cur += 3;
    } else if (cur[1] && opstr_table[cur[0] * 128 + cur[1]]) {
      TokenList_add(res, Token_new(opstr_table[cur[0] * 128 + cur[1]]));
      cur += 2;
    } else if (opstr_table[*cur]) {
      TokenList_add(res, Token_new(opstr_table[*cur]));
      cur++;
    } else {
      printf("LexError: unknown char '%c'", *cur);
      exit(-1);
    }
  }

  TokenList_add(res, Token_new(EofToken));
  return res;
}
