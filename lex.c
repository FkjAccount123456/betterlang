#include "b_lex.h"

Token Token_new(TokenType tp) {
  Token token;
  token.tp = tp;
  return token;
}

Token Token_int(long long int_token) {
  Token token = Token_new(INT_TOKEN);
  token.int_token = int_token;
  return token;
}

Token Token_float(double float_token) {
  Token token = Token_new(FLOAT_TOKEN);
  token.float_token = float_token;
  return token;
}

Token Token_id(GC_Node *str) {
  Token token = Token_new(ID_TOKEN);
  token.str_token = str;
  return token;
}

Token Token_str(GC_Node *str) {
  Token token = Token_new(STR_TOKEN);
  token.str_token = str;
  return token;
}

void TokenList_append(TokenList *tl, Token token) {
  if (tl->size == tl->max) {
    tl->max *= 2;
    tl->tokens = malloc(sizeof(Token) * tl->max);
  }
  tl->tokens[tl->size++] = token;
}

void TokenList_free(TokenList *tl) {
  free(tl->tokens);
  free(tl);
}

TokenList *TokenList_new() {
  TokenList *tl = malloc(sizeof(TokenList));
  tl->max = 8;
  tl->size = 0;
  tl->tokens = malloc(sizeof(Token) * tl->max);
  return tl;
}

bool contains(char *base, char ch) {
  for (size_t i = 0;  base[i]; i++)
    if (base[i] == ch)
      return true;
  return false;
}

EscapeRes _tokenize_escape(char *cur) {
  char res = 0;
  if (!*cur) {
    printf("Unexpected EOF.");
    exit(-1);
  } else if (*cur == 'r') {
    cur++;
    res = '\r';
  } else if (*cur == 't') {
    cur++;
    res = '\t';
  } else if (*cur == 'a') {
    cur++;
    res = '\a';
  } else if (*cur == 'f') {
    cur++;
    res = '\f';
  } else if (*cur == 'v') {
    cur++;
    res = '\v';
  } else if (*cur == 'b') {
    cur++;
    res = '\b';
  } else if (*cur == 'n') {
    cur++;
    res = '\n';
  } else if (*cur == '\\') {
    cur++;
    res = '\\';
  } else if (*cur == '\'') {
    cur++;
    res = '\'';
  } else if (*cur == '"') {
    cur++;
    res = '"';
  } else if (*cur == 'x') {
    cur++;
    for (char i = 0; i < 2; i++) {
      res *= 16;
      if (!*cur) {
        printf("Unexpected EOF.");
        exit(-1);
      }
      if ('0' <= res && res <= '9') {
        res += *cur - '0';
      } else if ('a' <= res <= 'f') {
        res += *cur - 'a' + 10;
      } else if ('A' <= res <= 'F') {
        res += *cur - 'A' + 10;
      } else {
        printf("Wrong escape sequence.");
        exit(-1);
      }
    }
  } else {
    printf("Wrong escape sequence.");
    exit(-1);
  }
  EscapeRes res_data;
  res_data.res = res;
  res_data.cur = cur;
  return res_data;
}

TokenList *tokenize(char *code, GC_Root *gc) {
  char *cur = code;
  TokenList *res = TokenList_new();

  while (*cur) {
    while (*cur && contains(" \n\t#", *cur)) {
      if (*cur == '#') {
        while (*cur && *cur != '\n')
          cur++;
      } else
        cur++;
    }

    if (!*cur)
      break;
    else if (isdigit(*cur)) {
      long long base_num = 0;
      while (*cur && isdigit(*cur)) {
        base_num *= 10;
        base_num += *cur++ - '0';
      }
      if (*cur == '.') {
        cur++;
        double num = base_num;
        double shift = 0.1;
        while (*cur && isdigit(*cur)) {
          num += (*cur++ - '0') * shift;
          shift /= 10;
        }
        TokenList_append(res, Token_float(num));
      } else
        TokenList_append(res, Token_int(base_num));
    } else if (isalpha(*cur) || *cur == '_') {
      GC_Node *id = String_new("", gc);
      while (*cur && (isalnum(*cur) || *cur == '_')) {
        String_append(id->ptr, *cur++, gc);
      }
      char *iv = ((String *)id->ptr)->val->ptr;
      if (!strcmp(iv, "if"))
        TokenList_append(res, Token_new(IF_TOKEN));
      else if (!strcmp(iv, "else"))
        TokenList_append(res, Token_new(ELSE_TOKEN));
      else if (!strcmp(iv, "while"))
        TokenList_append(res, Token_new(WHILE_TOKEN));
      else if (!strcmp(iv, "var"))
        TokenList_append(res, Token_new(VAR_TOKEN));
      else if (!strcmp(iv, "func"))
        TokenList_append(res, Token_new(FUNC_TOKEN));
      else if (!strcmp(iv, "return"))
        TokenList_append(res, Token_new(RETURN_TOKEN));
      else if (!strcmp(iv, "break"))
        TokenList_append(res, Token_new(BREAK_TOKEN));
      else if (!strcmp(iv, "continue"))
        TokenList_append(res, Token_new(CONTINUE_TOKEN));
      else if (!strcmp(iv, "True"))
        TokenList_append(res, Token_new(TRUE_TOKEN));
      else if (!strcmp(iv, "False"))
        TokenList_append(res, Token_new(FALSE_TOKEN));
      else if (!strcmp(iv, "Null"))
        TokenList_append(res, Token_new(NULL_TOKEN));
      else if (!strcmp(iv, "and"))
        TokenList_append(res, Token_new(CONTINUE_TOKEN));
      else if (!strcmp(iv, "or"))
        TokenList_append(res, Token_new(OR_TOKEN));
      else if (!strcmp(iv, "xor"))
        TokenList_append(res, Token_new(XOR_TOKEN));
      else
        TokenList_append(res, Token_id(id));
    } else if (*cur == '"') {
      cur++;
      GC_Node *str = String_new("", gc);
      while (*cur && *cur != '"') {
        if (*cur == '\\') {
          cur++;
          EscapeRes data = _tokenize_escape(cur);
          cur = data.cur;
          String_append(str->ptr, data.res, gc);
        } else {
          String_append(str->ptr, *cur++, gc);
        }
      }
      if (!*cur) {
        printf("Unexpected EOF.");
        exit(-1);
      }
      cur++;
      TokenList_append(res, Token_str(str));
    } else if (*cur == '+') {
      cur++;
      TokenList_append(res, Token_new(ADD_TOKEN));
    } else if (*cur == '-') {
      cur++;
      TokenList_append(res, Token_new(SUB_TOKEN));
    } else if (*cur == '*') {
      cur++;
      TokenList_append(res, Token_new(MUL_TOKEN));
    } else if (*cur == '/') {
      cur++;
      TokenList_append(res, Token_new(DIV_TOKEN));
    } else if (*cur == '=') {
      cur++;
      if (*cur == '=') {
        cur++;
        TokenList_append(res, Token_new(EQ_TOKEN));
      } else {
        TokenList_append(res, Token_new(ASSIGN));
      }
    } else if (*cur == '!') {
      cur++;
      if (*cur == '=') {
        cur++;
        TokenList_append(res, Token_new(NE_TOKEN));
      } else {
        TokenList_append(res, Token_new(NOT_TOKEN));
      }
    } else if (*cur == '>') {
      cur++;
      if (*cur == '=') {
        cur++;
        TokenList_append(res, Token_new(GE_TOKEN));
      } else {
        TokenList_append(res, Token_new(GT_TOKEN));
      }
    } else if (*cur == '<') {
      cur++;
      if (*cur == '=') {
        cur++;
        TokenList_append(res, Token_new(LE_TOKEN));
      } else {
        TokenList_append(res, Token_new(LT_TOKEN));
      }
    } else if (*cur == '(') {
      cur++;
      TokenList_append(res, Token_new(LPAREN));
    } else if (*cur == ')') {
      cur++;
      TokenList_append(res, Token_new(RPAREN));
    } else if (*cur == '[') {
      cur++;
      TokenList_append(res, Token_new(LSQBR));
    } else if (*cur == ']') {
      cur++;
      TokenList_append(res, Token_new(RSQBR));
    } else if (*cur == '{') {
      cur++;
      TokenList_append(res, Token_new(BEGIN));
    } else if (*cur == '}') {
      cur++;
      TokenList_append(res, Token_new(END));
    } else if (*cur == ',') {
      cur++;
      TokenList_append(res, Token_new(COMMA));
    } else if (*cur == '.') {
      cur++;
      TokenList_append(res, Token_new(DOT));
    } else if (*cur == ':') {
      cur++;
      TokenList_append(res, Token_new(COLON));
    } else if (*cur == ';') {
      cur++;
      TokenList_append(res, Token_new(SEMICOLON));
    } else {
      printf("Unexpected character '%c'.\n", *cur);
      exit(-1);
    }
  }

  TokenList_append(res, Token_new(EOF_TOKEN));
  return res;
}
