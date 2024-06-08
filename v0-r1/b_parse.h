#ifndef B_PARSE_H
#define B_PARSE_H

#include "b_lexer.h"
#include "ast.h"

void Parser_eat(Lexer *l, TokenType tp);

Expr *Parser_expr(Lexer *l);
Expr *Parser_factor(Lexer *l);

Stmt *Parser_stmt(Lexer *l);
Stmt *Parser_block(Lexer *l);

Stmt *Parser_program(Lexer *l);

#endif // B_PARSE_H
