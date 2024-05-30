#include "b_lexer.h"

size_t op_prio[128];

void init_op_prio() {
  memset(op_prio, 0, sizeof(op_prio));
  op_prio[MUL_TOKEN] = op_prio[DIV_TOKEN] = op_prio[MOD_TOKEN] = 100;
  op_prio[ADD_TOKEN] = op_prio[SUB_TOKEN] = 99;
  op_prio[EQ_TOKEN] = op_prio[NE_TOKEN] = 98;
  op_prio[LT_TOKEN] = op_prio[GT_TOKEN] = op_prio[LE_TOKEN] =
      op_prio[GE_TOKEN] = 97;
  op_prio[AND_TOKEN] = 96;
  op_prio[XOR_TOKEN] = 95;
  op_prio[OR_TOKEN] = 94;
}

Token *Token_new(TokenType tp) {
  Token *token = (Token *)malloc(sizeof(Token));
  if (token == NULL) {
    printf("Failed to malloc.");
    exit(-1);
  }
  token->tp = tp;
  return token;
}

Token *Token_int(long long intToken) {
  Token *token = Token_new(INT_TOKEN);
  token->intToken = intToken;
  return token;
}

Token *Token_str(TokenType tp, String *strToken) {
  Token *token = Token_new(tp);
  token->strToken = strToken;
  return token;
}

Lexer *Lexer_new(char *code) {
  Lexer *lexer = (Lexer *)malloc(sizeof(Lexer));
  if (lexer == NULL) {
    printf("Failed to malloc.");
    exit(-1);
  }
  lexer->code = lexer->cur = code;
  lexer->token = NULL;
  Lexer_next(lexer);
  return lexer;
}

void Lexer_free(Lexer *lexer) {
  free(lexer->token);
  free(lexer);
}

char Lexer_escape(Lexer *l) {
  if (!*l->cur) {
    printf("Unexpected EOF.");
    exit(-1);
  } else if (*l->cur == 'r') {
    l->cur++;
    return '\r';
  } else if (*l->cur == 't') {
    l->cur++;
    return '\t';
  } else if (*l->cur == 'a') {
    l->cur++;
    return '\a';
  } else if (*l->cur == 'f') {
    l->cur++;
    return '\f';
  } else if (*l->cur == 'v') {
    l->cur++;
    return '\v';
  } else if (*l->cur == 'b') {
    l->cur++;
    return '\b';
  } else if (*l->cur == 'n') {
    l->cur++;
    return '\n';
  } else if (*l->cur == 'x') {
    l->cur++;
    char ch = 0;
    for (int i = 0; i < 2; i++) {
      if (!*l->cur) {
        printf("Unexpected EOF.");
        exit(-1);
      }
      ch *= 16;
      if ('0' <= *l->cur && *l->cur <= '9') {
        ch += *l->cur++ - '0';
      } else if ('a' <= *l->cur && *l->cur <= 'f') {
        ch += *l->cur++ - 'a' + 10;
      } else if ('A' <= *l->cur && *l->cur <= 'F') {
        ch += *l->cur++ - 'A' + 10;
      } else {
        printf("Invalid escape sequence.");
        exit(-1);
      }
    }
    return ch;
  } else {
    printf("Invalid escape sequence.");
    exit(-1);
  }
}

void Lexer_skip(Lexer *l) {
  while (*l->cur && (isspace(*l->cur) || *l->cur == '#')) {
    if (*l->cur == '#') {
      while (*l->cur && *l->cur != '\n')
        l->cur++;
    } else {
      l->cur++;
    }
  }
}

void Lexer_next(Lexer *l) {
  Lexer_skip(l);

  if (l->token)
    free(l->token), l->token = NULL;
  if (*l->cur == '\0') {
    l->token = NULL;
  } else if (isdigit(*l->cur)) {
    long long num = *l->cur++ - '0';
    while (*l->cur && isdigit(*l->cur)) {
      num *= 10;
      num += *l->cur++ - '0';
    }
    l->token = Token_int(num);
  } else if (isalpha(*l->cur) || *l->cur == '_') {
    String *id = String_new("");
    while (*l->cur && (isalnum(*l->cur) || *l->cur == '_')) {
      String_append(id, *l->cur++);
    }
    if (!strcmp(id->val, "if"))
      l->token = Token_new(IF_TOKEN);
    else if (!strcmp(id->val, "else"))
      l->token = Token_new(ELSE_TOKEN);
    else if (!strcmp(id->val, "while"))
      l->token = Token_new(WHILE_TOKEN);
    else if (!strcmp(id->val, "return"))
      l->token = Token_new(RETURN_TOKEN);
    else if (!strcmp(id->val, "break"))
      l->token = Token_new(BREAK_TOKEN);
    else if (!strcmp(id->val, "continue"))
      l->token = Token_new(CONTINUE_TOKEN);
    else if (!strcmp(id->val, "var"))
      l->token = Token_new(VAR_TOKEN);
    else if (!strcmp(id->val, "func"))
      l->token = Token_new(FUNC_TOKEN);
    else
      l->token = Token_str(ID_TOKEN, id);
  } else if (*l->cur == '"') {
    l->cur++;
    String *str = String_new("");
    while (*l->cur && *l->cur != '"') {
      if (*l->cur == '\\') {
        l->cur++;
        String_append(str, Lexer_escape(l));
      } else {
        String_append(str, *l->cur++);
      }
    }
    if (!*l->cur) {
      printf("Unexpected EOF.");
      exit(-1);
    }
    l->cur++;
    l->token = Token_str(STR_TOKEN, str);
  } else if (*l->cur == '+') {
    l->cur++;
    l->token = Token_new(ADD_TOKEN);
  } else if (*l->cur == '-') {
    l->cur++;
    l->token = Token_new(SUB_TOKEN);
  } else if (*l->cur == '*') {
    l->cur++;
    l->token = Token_new(MUL_TOKEN);
  } else if (*l->cur == '/') {
    l->cur++;
    l->token = Token_new(DIV_TOKEN);
  } else if (*l->cur == '%') {
    l->cur++;
    l->token = Token_new(MOD_TOKEN);
  } else if (*l->cur == '=') {
    l->cur++;
    if (*l->cur == '=')
      l->cur++, l->token = Token_new(EQ_TOKEN);
    else
      l->token = Token_new(ASSIGN_TOKEN);
  } else if (*l->cur == '!') {
    l->cur++;
    if (*l->cur == '=')
      l->cur++, l->token = Token_new(NE_TOKEN);
    else
      l->token = Token_new(NOT_TOKEN);
  } else if (*l->cur == '>') {
    l->cur++;
    if (*l->cur == '=')
      l->cur++, l->token = Token_new(GE_TOKEN);
    else
      l->token = Token_new(GT_TOKEN);
  } else if (*l->cur == '<') {
    l->cur++;
    if (*l->cur == '=')
      l->cur++, l->token = Token_new(LE_TOKEN);
    else
      l->token = Token_new(LT_TOKEN);
  } else if (*l->cur == '&') {
    l->cur++;
    l->token = Token_new(AND_TOKEN);
  } else if (*l->cur == '|') {
    l->cur++;
    l->token = Token_new(OR_TOKEN);
  } else if (*l->cur == '^') {
    l->cur++;
    l->token = Token_new(XOR_TOKEN);
  } else if (*l->cur == '(') {
    l->cur++;
    l->token = Token_new(LPAREN_TOKEN);
  } else if (*l->cur == ')') {
    l->cur++;
    l->token = Token_new(RPAREN_TOKEN);
  } else if (*l->cur == '[') {
    l->cur++;
    l->token = Token_new(LSQBR_TOKEN);
  } else if (*l->cur == ']') {
    l->cur++;
    l->token = Token_new(RSQBR_TOKEN);
  } else if (*l->cur == '{') {
    l->cur++;
    l->token = Token_new(BEGIN_TOKEN);
  } else if (*l->cur == '}') {
    l->cur++;
    l->token = Token_new(END_TOKEN);
  } else if (*l->cur == ',') {
    l->cur++;
    l->token = Token_new(COMMA_TOKEN);
  } else if (*l->cur == ';') {
    l->cur++;
    l->token = Token_new(SEMI_TOKEN);
  } else if (*l->cur == ':') {
    l->cur++;
    l->token = Token_new(COLON_TOKEN);
  } else if (*l->cur == '.') {
    l->cur++;
    l->token = Token_new(DOT_TOKEN);
  } else {
    printf("Unexpected character: %c.", *l->cur);
    exit(-1);
  }
}
