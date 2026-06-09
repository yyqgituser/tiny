#ifndef _TINY_TINYTOKENTYPE_H
#define _TINY_TINYTOKENTYPE_H

namespace tiny {

enum TinyTokenType {
  EOI = 0,
  IDENTIFIER = 1,
  LPAREN = 2,
  RPAREN = 3,
  INT = 4,
  LBRACK = 5,
  RBRACK = 6,
  COMMA = 7,
  LBRACE = 8,
  RBRACE = 9,
  IF = 10,
  ELSE = 11,
  WHILE = 12,
  RETURN = 13,
  SEMI = 14,
  EQ = 15,
  PIPEPIPE = 16,
  AMPAMP = 17,
  EQEQ = 18,
  NE = 19,
  LT = 20,
  GT = 21,
  LE = 22,
  GE = 23,
  PLUS = 24,
  MINUS = 25,
  MULT = 26,
  DIV = 27,
  MOD = 28,
  DECIMAL_INT = 29,
  NEW = 30,
  BANG = 31
};

}

#endif
