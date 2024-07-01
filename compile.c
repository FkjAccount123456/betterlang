#include "compile.h"
#include "lex.h"

char op_prio[128];

void op_prio_init() {
  op_prio[MUL] = op_prio[DIV] = 100;
  op_prio[ADD] = op_prio[SUB] = 99;
  op_prio[EQ] = op_prio[NE] = 97;
  op_prio[GT] = op_prio[LT] = op_prio[GE] = op_prio[LE] = 96;
  op_prio[AND] = 92;
  op_prio[XOR] = 91;
  op_prio[OR] = 90;
}

ByteCode optoken2vmop(TokenType op) { return op - ADD_TOKEN + ADD; }

Parser *Parser_new(TokenList *tokens) {
  Parser *parser = malloc(sizeof(Parser));
  parser->gc_base = GC_objs_add(GC_Object_new(parser, (GC_Dstcor)Parser_free));
  parser->tokens = tokens->tokens;
  parser->cur = parser->tokens;
  parser->size = 0;
  parser->max = 8;
  parser->ps = ParserScope_new(NULL);
  parser->output = malloc(sizeof(VMCode) * parser->max);
  parser->while_beginposs = SizeList_new();
  parser->while_jmpends = SizeList_new();
  return parser;
}

Token Parser_next(Parser *p) { return *p->cur++; }

Token Parser_eat(Parser *p, TokenType tp) {
  if (p->cur && p->cur->tp == tp)
    return *p->cur++;
  printf("Unexpected token");
  exit(-1);
}

void Parser_add_output(Parser *p, VMCode code) {
  if (p->size == p->max) {
    p->max *= 2;
    p->output = realloc(p->output, sizeof(VMCode) * p->max);
  }
  p->output[p->size++] = code;
}

void Parser_expr(Parser *p);

void Parser_factor(Parser *p) {
  if (p->cur->tp == EOF_TOKEN) {
    printf("Unexpected EOF");
    exit(-1);
  } else if (p->cur->tp == INT_TOKEN) {
    VMCode code = VMCode_new(PUSH_I);
    code.i = Parser_next(p).int_token;
    Parser_add_output(p, code);
  } else if (p->cur->tp == FLOAT_TOKEN) {
    VMCode code = VMCode_new(PUSH_F);
    code.f = Parser_next(p).float_token;
    Parser_add_output(p, code);
  } else if (p->cur->tp == STR_TOKEN) {
    VMCode code = VMCode_new(PUSH_S);
    code.s = String_new(Parser_next(p).str_token->val);
    Parser_add_output(p, code);
    GC_obj_add_ch(p->gc_base, code.s->gc_base);
  } else if (p->cur->tp == ID_TOKEN) {
    String *id = Parser_next(p).str_token;
    unsigned int fcnt = 0, vcnt = 0;
    ParserScope *ps;
    unsigned int *res = NULL;
    for (ps = p->ps; ps; ps = ps->parent, fcnt++) {
      if ((res = IDict_find(ps->dict, id->val)))
        break;
    }
    if (res == NULL) {
      printf("Undefined variable '%s'", id->val);
      exit(-1);
    }
    vcnt = *IDict_find(ps->dict, id->val);
    VMCode code = VMCode_new(LOAD_V);
    code.l = ((size_t)fcnt << 32) + vcnt;
    Parser_add_output(p, code);
  } else if (p->cur->tp == LSQBR) {
    Parser_next(p);
    size_t len = 0;
    if (p->cur->tp != RSQBR) {
      Parser_expr(p);
      len++;
      while (p->cur->tp == COMMA) {
        Parser_next(p);
        Parser_expr(p);
        len++;
      }
    }
    Parser_eat(p, RSQBR);
    VMCode code = VMCode_new(BUILD_LIST);
    code.l = len;
    Parser_add_output(p, code);
  } else if (p->cur->tp == LPAREN) {
    Parser_next(p);
    Parser_expr(p);
    Parser_eat(p, RPAREN);
  } else {
    // printf("%d\n", p->cur->tp);
    printf("Unexpected token");
    exit(-1);
  }

  while (p->cur->tp == LPAREN || p->cur->tp == LSQBR) {
    if (p->cur->tp == LPAREN) {
      Parser_next(p);
      size_t nargs = 0;
      if (p->cur->tp != RPAREN) {
        Parser_expr(p);
        nargs++;
        while (p->cur->tp == COMMA) {
          Parser_next(p);
          Parser_expr(p);
          nargs++;
        }
      }
      Parser_eat(p, RPAREN);
      Parser_add_output(p, VMCode_new(CALL));
      p->output[p->size - 1].l = nargs;
    } else if (p->cur->tp == LSQBR) {
      Parser_next(p);
      Parser_expr(p);
      Parser_eat(p, RPAREN);
      Parser_add_output(p, VMCode_new(NTH));
    }
  }
}

void Parser_expr(Parser *p) {
  Parser_factor(p);
  NewSeq(TokenType, op_stack);
  while (op_prio[p->cur->tp]) {
    TokenType op = Parser_next(p).tp;
    while (SeqSize(op_stack) &&
           op_prio[op_stack_val[op_stack_size - 1]] >= op_prio[op]) {
      Parser_add_output(
          p, VMCode_new(optoken2vmop(op_stack_val[--op_stack_size])));
    }
    SeqAppend(TokenType, op_stack, op);
    Parser_factor(p);
  }
  while (SeqSize(op_stack)) {
    Parser_add_output(p,
                      VMCode_new(optoken2vmop(op_stack_val[--op_stack_size])));
  }
  FreeSeq(op_stack);
}

void Parser_block(Parser *p);

void Parser_stmt(Parser *p) {
  if (p->cur->tp == EOF_TOKEN) {
    printf("Unexpected EOF");
    exit(-1);
  } else if (p->cur->tp == SEMICOLON) {
    Parser_next(p);
  } else if (p->cur->tp == VAR_TOKEN) {
    Parser_next(p);
    String *name = Parser_eat(p, ID_TOKEN).str_token;
    if (p->cur->tp == ASSIGN) {
      Parser_next(p);
      Parser_expr(p);
    } else {
      VMCode nullcode = VMCode_new(PUSH_N);
      Parser_add_output(p, nullcode);
    }
    IDict_insert(p->ps->dict, name->val, p->ps->cnt++);
    VMCode code = VMCode_new(ADD_V);
    Parser_add_output(p, code);
    while (p->cur->tp == COMMA) {
      Parser_next(p);
      name = Parser_eat(p, ID_TOKEN).str_token;
      if (p->cur->tp == ASSIGN) {
        Parser_next(p);
        Parser_expr(p);
      } else {
        VMCode nullcode = VMCode_new(PUSH_N);
        Parser_add_output(p, nullcode);
      }
      IDict_insert(p->ps->dict, name->val, p->ps->cnt++);
      Parser_add_output(p, code);
    }
    Parser_eat(p, SEMICOLON);
  } else if (p->cur->tp == IF_TOKEN) {
    Parser_next(p);
    Parser_expr(p);
    NewSeq(size_t *, jmps);
    Parser_add_output(p, VMCode_new(JNZ));
    size_t *jnz = &p->output[p->size - 1].l;
    Parser_block(p);
    Parser_add_output(p, VMCode_new(JMP));
    SeqAppend(size_t *, jmps, &p->output[p->size - 1].l);
    *jnz = p->size - 1;
    while (p->cur->tp == ELSE_TOKEN) {
      Parser_next(p);
      if (p->cur->tp == IF_TOKEN) {
        Parser_next(p);
        Parser_expr(p);
        Parser_add_output(p, VMCode_new(JNZ));
        jnz = &p->output[p->size - 1].l;
        SeqAppend(size_t *, jmps, &p->output[p->size - 1].l);
        *jnz = p->size - 1;
      } else {
        Parser_block(p);
      }
    }
    for (size_t i = 0; i < jmps_size; i++) {
      *jmps_val[i] = p->size - 1;
    }
    FreeSeq(jmps);
  } else if (p->cur->tp == WHILE_TOKEN) {
    Parser_next(p);
    SizeList_append(p->while_jmpends, 0);
    SizeList_append(p->while_beginposs, p->size - 1);
    Parser_expr(p);
    Parser_add_output(p, VMCode_new(JNZ));
    SizeList_append(p->while_jmpends, p->size - 1);
    Parser_block(p);
    Parser_add_output(p, VMCode_new(JMP));
    p->output[p->size - 1].l =
        p->while_beginposs->items[p->while_beginposs->size - 1];
    while (p->while_jmpends->size) {
      p->while_beginposs->size--;
      if (p->while_jmpends->items[p->while_beginposs->size] == 0) {
        break;
      }
      p->output[p->while_beginposs->size].l = p->size - 1;
    }
    p->while_beginposs->size--;
  } else if (p->cur->tp == BREAK_TOKEN) {
    Parser_next(p);
    Parser_eat(p, SEMICOLON);
    Parser_add_output(p, VMCode_new(JMP));
    p->output[p->size - 1].l =
        p->while_beginposs->items[p->while_beginposs->size - 1];
  } else if (p->cur->tp == CONTINUE_TOKEN) {
    Parser_next(p);
    Parser_eat(p, SEMICOLON);
    Parser_add_output(p, VMCode_new(JMP));
    SizeList_append(p->while_jmpends, p->size - 1);
  } else if (p->cur->tp == RETURN_TOKEN) {
    Parser_next(p);
    Parser_expr(p);
    Parser_eat(p, SEMICOLON);
    Parser_add_output(p, VMCode_new(RET));
  } else if (p->cur->tp == FUNC_TOKEN) {
    Parser_next(p);
    String *name = Parser_eat(p, ID_TOKEN).str_token;
    Parser_add_output(p, VMCode_new(PUSH_FN));
    Parser_add_output(p, VMCode_new(ADD_V));
    IDict_insert(p->ps->dict, name->val, p->ps->cnt++);
    size_t pos = p->size;
    Parser_add_output(p, VMCode_new(JMP));
    Parser_eat(p, LPAREN);
    p->ps = ParserScope_new(p->ps);
    if (p->cur->tp != RPAREN) {
      IDict_insert(p->ps->dict, Parser_eat(p, ID_TOKEN).str_token->val,
                   p->ps->cnt++);
      Parser_add_output(p, VMCode_new(ADD_V));
      while (p->cur->tp == COMMA) {
        Parser_next(p);
        IDict_insert(p->ps->dict, Parser_eat(p, ID_TOKEN).str_token->val,
                     p->ps->cnt++);
        Parser_add_output(p, VMCode_new(ADD_V));
      }
    }
    Parser_eat(p, RPAREN);
    Parser_block(p);
    p->output[pos].l = p->size - 1;
    ParserScope_free(&p->ps);
  } else {
    Parser_expr(p);
    if (p->cur->tp == ASSIGN) {
      Parser_next(p);
      if (p->output[p->size - 1].head == LOAD_V) {
        p->size--;
        Parser_expr(p);
        Parser_add_output(p, VMCode_new(SET_V));
      } else if (p->output[p->size - 1].head == NTH) {
        p->size--;
        Parser_expr(p);
        Parser_add_output(p, VMCode_new(SET_NTH));
      } else {
        printf("Expect an l-value");
        exit(-1);
      }
    } else {
      Parser_add_output(p, VMCode_new(POP));
    }
    Parser_eat(p, SEMICOLON);
  }
}

void Parser_block(Parser *p) {
  Parser_eat(p, BEGIN);
  while (p->cur->tp != EOF_TOKEN && p->cur->tp != END) {
    Parser_stmt(p);
  }
  Parser_eat(p, END);
}

void Parser_program(Parser *p) {
  while (p->cur->tp != EOF_TOKEN) {
    Parser_stmt(p);
  }
  Parser_add_output(p, VMCode_new(EXIT));
}

void Parser_free(Parser *parser) {
  printf("Parser_free %llx output %llx\n", parser, parser->output);
  free(parser->output);
  puts("Parser_free 1");
  SizeList_free(parser->while_beginposs);
  SizeList_free(parser->while_jmpends);
  ParserScope_free(&parser->ps);
  free(parser);
}

ParserScope *ParserScope_new(ParserScope *parent) {
  ParserScope *ps = malloc(sizeof(ParserScope));
  ps->parent = parent;
  ps->dict = IDict_new();
  ps->cnt = 0;
  return ps;
}

void ParserScope_free(ParserScope **ps) {
  puts("ParserScope_free");
  if (*ps) {
    IDict_free((*ps)->dict);
    ParserScope *parent = (*ps)->parent;
    free(*ps);
    *ps = parent;
    return;
  }
}

SizeList *SizeList_new() {
  SizeList *list = malloc(sizeof(SizeList));
  list->size = 0;
  list->max = 8;
  list->items = malloc(sizeof(size_t) * list->max);
  return list;
}

void SizeList_append(SizeList *list, size_t val) {
  if (list->size == list->max) {
    list->max *= 2;
    list->items = realloc(list->items, sizeof(size_t) * list->max);
  }
  list->items[list->size++] = val;
}

void SizeList_free(SizeList *list) {
  free(list->items);
  free(list);
}
