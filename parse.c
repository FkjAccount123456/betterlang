#include "parse.h"
#include "b_seq.h"
#include "b_stdlib.h"
#include <stdio.h>
#include <stdlib.h>

char op_prio[2048] = {
    [MulToken] = 100, [DivToken] = 100,   [ModToken] = 100, [AddToken] = 99,
    [SubToken] = 99,  [LshToken] = 98,    [RshToken] = 98,  [EqToken] = 97,
    [NeToken] = 97,   [GtToken] = 96,     [GeToken] = 96,   [LtToken] = 96,
    [LeToken] = 96,   [BitAndToken] = 95, [XorToken] = 94,  [BitOrToken] = 93,
    [AndToken] = 92,  [OrToken] = 91,
};

Parser *Parser_new(TokenList *tokens) {
  Parser *parser = b_malloc(sizeof(Parser));
  parser->tokens = tokens;
  parser->token = tokens->tokens;
  return parser;
}

Token *Parser_next(Parser *parser) {
  if (parser->token->tp != EofToken) {
    return parser->token++;
  }
  return parser->token;
}

Token *Parser_eat(Parser *parser, TokenType tp) {
  if (parser->token->tp == tp) {
    return parser->token++;
  }
  printf("ParseError: unexpected token %d %d", parser->token->tp, tp);
  exit(-1);
}

void Parser_free(Parser *parser) {
  TokenList_free(parser->tokens);
  b_free(parser);
}

ASTNode *parse_factor(Parser *p) {
  ASTNode *node;
  if (p->token->tp == EofToken) {
    printf("ParseError: unexpected EOF");
    exit(-1);
  } else if (p->token->tp == IntToken) {
    node = ASTNode_new(IntExpr);
    node->intAST = Parser_next(p)->intToken;
  } else if (p->token->tp == FloatToken) {
    node = ASTNode_new(FloatExpr);
    node->floatAST = Parser_next(p)->floatToken;
  } else if (p->token->tp == StringToken) {
    node = ASTNode_new(StringExpr);
    node->stringAST = Parser_next(p)->strToken;
  } else if (p->token->tp == IdToken) {
    node = ASTNode_new(VarExpr);
    node->stringAST = Parser_next(p)->strToken;
  } else if (p->token->tp == LParen) {
    Parser_next(p);
    node = parse_expr(p);
    Parser_eat(p, RParen);
  } else if (p->token->tp == LSqBr) {
    Parser_next(p);
    ASTList list = SeqNew(ASTList);
    if (p->token->tp != RSqBr) {
      SeqAppend(list, parse_expr(p));
      while (p->token->tp == Comma) {
        Parser_next(p);
        SeqAppend(list, parse_expr(p));
      }
    }
    Parser_eat(p, RSqBr);
    node = ASTNode_new(BuildListExpr);
    node->listAST = list;
  } else if (p->token->tp == Begin) {
    Parser_next(p);
    ASTList list = SeqNew(ASTList);
    while (p->token->tp != End) {
      SeqAppend(list, ASTNode_init(VarExpr, stringAST,
                                   Parser_eat(p, IdToken)->strToken));
      Parser_eat(p, Colon);
      SeqAppend(list, parse_expr(p));
      if (p->token->tp != End) {
        Parser_eat(p, Comma);
      }
    }
    Parser_eat(p, End);
    node = ASTNode_new(BuildDictExpr);
    node->listAST = list;
  } else if (p->token->tp == FuncToken) {
    Parser_next(p);
    ASTList lambda = SeqNew(ASTList);
    Parser_eat(p, LParen);
    ASTList args = SeqNew(ASTList);
    if (p->token->tp != RParen) {
      SeqAppend(args, ASTNode_init(VarExpr, stringAST, Parser_eat(p, IdToken)->strToken));
      while (p->token->tp == Comma) {
        Parser_next(p);
        SeqAppend(args, ASTNode_init(VarExpr, stringAST, Parser_eat(p, IdToken)->strToken));
      }
    }
    SeqAppend(lambda, ASTNode_init(BuildListExpr, listAST, args));
    Parser_eat(p, RParen);
    SeqAppend(lambda, parse_block(p));
    return ASTNode_init(LambdaExpr, listAST, lambda);
  } else {
    printf("ParseError: unexpected token %d", p->token->tp);
    exit(-1);
  }

  while (p->token->tp == LParen || p->token->tp == LSqBr ||
         p->token->tp == Dot) {
    if (p->token->tp == LParen) {
      Parser_next(p);
      ASTList args = SeqNew(ASTList);
      SeqAppend(args, node);
      if (p->token->tp != RParen) {
        SeqAppend(args, parse_expr(p));
        while (p->token->tp == Comma) {
          Parser_next(p);
          SeqAppend(args, parse_expr(p));
        }
      }
      Parser_eat(p, RParen);
      node = ASTNode_new(CallExpr);
      node->listAST = args;
    } else if (p->token->tp == LSqBr) {
      Parser_next(p);
      ASTList args = SeqNew(ASTList);
      SeqAppend(args, node);
      SeqAppend(args, parse_expr(p));
      Parser_eat(p, RSqBr);
      node = ASTNode_new(IndexExpr);
      node->listAST = args;
    } else if (p->token->tp == Dot) {
      Parser_next(p);
      ASTList args = SeqNew(ASTList);
      SeqAppend(args, node);
      SeqAppend(args, ASTNode_init(VarExpr, stringAST,
                                   Parser_eat(p, IdToken)->strToken));
      node = ASTNode_new(AttrExpr);
      node->listAST = args;
    }
  }

  return node;
}

ASTNode *parse_expr(Parser *p) {
  ASTList expr = SeqNew(ASTList);
  SeqAppend(expr, parse_factor(p));
  typedef struct Seq(TokenType) OpStack;
  OpStack stack = SeqNew(OpStack);
  while (p->token->tp >= AddToken && p->token->tp <= OrToken) {
    TokenType op = Parser_next(p)->tp;
    while (stack.len && op_prio[stack.v[stack.len - 1]] >= op_prio[op]) {
      SeqAppend(expr, ASTNode_init(OpTerminal, opAST, SeqPop(stack)));
    }
    SeqAppend(stack, op);
    SeqAppend(expr, parse_factor(p));
  }
  while (stack.len) {
    SeqAppend(expr, ASTNode_init(OpTerminal, opAST, SeqPop(stack)));
  }
  SeqFree(stack);
  if (expr.len == 1) {
    ASTNode *res = expr.v[0];
    b_free(expr.v);
    return res;
  }
  ASTNode *res = ASTNode_new(ExprExpr);
  res->listAST = expr;
  return res;
}

ASTNode *parse_stmt(Parser *p) {
  if (p->token->tp == Semi) {
    Parser_next(p);
    return ASTNode_new(NoOpStmt);
  } else if (p->token->tp == IfToken) {
    Parser_next(p);
    ASTList cases = SeqNew(ASTList);
    SeqAppend(cases, parse_expr(p));
    SeqAppend(cases, parse_block(p));
    while (p->token->tp == ElseToken) {
      Parser_next(p);
      if (p->token->tp == IfToken) {
        Parser_next(p);
        SeqAppend(cases, parse_expr(p));
        SeqAppend(cases, parse_block(p));
      } else {
        SeqAppend(cases, parse_block(p));
        return ASTNode_init(IfStmt, listAST, cases);
      }
    }
    return ASTNode_init(IfStmt, listAST, cases);
  } else if (p->token->tp == WhileToken) {
    Parser_next(p);
    ASTList pair = SeqNew(ASTList);
    SeqAppend(pair, parse_expr(p));
    SeqAppend(pair, parse_block(p));
    return ASTNode_init(WhileStmt, listAST, pair);
  } else if (p->token->tp == VarToken) {
    Parser_next(p);
    ASTList vars = SeqNew(ASTList);
    SeqAppend(vars, ASTNode_init(VarExpr, stringAST, Parser_eat(p, IdToken)->strToken));
    if (p->token->tp == Assign) {
      Parser_next(p);
      SeqAppend(vars, parse_expr(p));
    } else {
      SeqAppend(vars, ASTNode_new(NoneExpr));
    }
    while (p->token->tp == Comma) {
      Parser_next(p);
      SeqAppend(vars, ASTNode_init(VarExpr, stringAST, Parser_eat(p, IdToken)->strToken));
      if (p->token->tp == Assign) {
        Parser_next(p);
        SeqAppend(vars, parse_expr(p));
      } else {
        SeqAppend(vars, ASTNode_new(NoneExpr));
      }
    }
    Parser_eat(p, Semi);
    return ASTNode_init(VarDeclStmt, listAST, vars);
  } else if (p->token->tp == FuncToken) {
    Parser_next(p);
    ASTList fndef = SeqNew(ASTList);
    SeqAppend(fndef, ASTNode_init(VarExpr, stringAST, Parser_eat(p, IdToken)->strToken));
    Parser_eat(p, LParen);
    ASTList args = SeqNew(ASTList);
    if (p->token->tp != RParen) {
      SeqAppend(args, ASTNode_init(VarExpr, stringAST, Parser_eat(p, IdToken)->strToken));
      while (p->token->tp == Comma) {
        Parser_next(p);
        SeqAppend(args, ASTNode_init(VarExpr, stringAST, Parser_eat(p, IdToken)->strToken));
      }
    }
    SeqAppend(fndef, ASTNode_init(BuildListExpr, listAST, args));
    Parser_eat(p, RParen);
    SeqAppend(fndef, parse_block(p));
    return ASTNode_init(FuncDefStmt, listAST, fndef);
  } else if (p->token->tp == ReturnToken) {
    Parser_next(p);
    ASTNode *res = ASTNode_init(ReturnStmt, nodeAST, parse_expr(p));
    Parser_eat(p, Semi);
    return res;
  } else if (p->token->tp == BreakToken) {
    Parser_next(p);
    Parser_eat(p, Semi);
    return ASTNode_new(BreakStmt);
  } else if (p->token->tp == ContinueToken) {
    Parser_next(p);
    Parser_eat(p, Semi);
    return ASTNode_new(ContinueStmt);
  } else {
    ASTNode *left = parse_expr(p);
    if (p->token->tp == Assign) {
      Parser_next(p);
      ASTList assign = SeqNew(ASTList);
      SeqAppend(assign, left);
      SeqAppend(assign, parse_expr(p));
      Parser_eat(p, Semi);
      return ASTNode_init(AssignStmt, listAST, assign);
    }
    Parser_eat(p, Semi);
    return ASTNode_init(ExprStmt, nodeAST, left);
  }
}

ASTNode *parse_block(Parser *p) {
  Parser_eat(p, Begin);
  ASTList stmts = SeqNew(ASTList);
  while (p->token->tp != EofToken && p->token->tp != End) {
    SeqAppend(stmts, parse_stmt(p));
  }
  Parser_eat(p, End);
  return ASTNode_init(BlockStmt, listAST, stmts);
}

ASTNode *parse_program(Parser *p) {
  ASTList stmts = SeqNew(ASTList);
  while (p->token->tp != EofToken) {
    SeqAppend(stmts, parse_stmt(p));
  }
  return ASTNode_init(BlockStmt, listAST, stmts);
}
