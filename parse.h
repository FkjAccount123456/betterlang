#pragma once

#include "b_lex.h"
#include "b_ast.h"

extern char op_prio[2048];

typedef struct Parser {
  TokenList *tokens;
  Token *token;
} Parser;

Parser *Parser_new(TokenList *tokens);
Token *Parser_next(Parser *parser);
Token *Parser_eat(Parser *parser, TokenType type);
void Parser_free(Parser *parser);

ASTNode *parse_factor(Parser *parser);
ASTNode *parse_expr(Parser *parser);
ASTNode *parse_stmt(Parser *parser);
ASTNode *parse_block(Parser *parser);
ASTNode *parse_program(Parser *parser);
