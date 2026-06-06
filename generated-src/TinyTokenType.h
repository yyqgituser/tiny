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
  EQEQ = 16,
  NE = 17,
  LT = 18,
  GT = 19,
  LE = 20,
  GE = 21,
  PLUS = 22,
  MINUS = 23,
  MULT = 24,
  DIV = 25,
  MOD = 26,
  DECIMAL_INT = 27,
  NEW = 28
};

}

#endif
