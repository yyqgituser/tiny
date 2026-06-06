#ifndef _TINY_TINYTOKEN_H
#define _TINY_TINYTOKEN_H

#include <string>

namespace tiny {

class TinyToken {
public:
  int id;

  std::u32string text;

  int line;

  int column;

public:
  TinyToken() : TinyToken(-1, std::u32string(), -1, -1) {
  }

  TinyToken(int id) : TinyToken(id, std::u32string(), -1, -1) {
  }

  TinyToken(int id, std::u32string text) : TinyToken(id, text, -1, -1) {
  }

  TinyToken(int id, std::u32string text, int line, int column) : id(id), text(text), line(line), column(column) {
  }

  TinyToken(int id, int line, int column) : TinyToken(id, std::u32string(), line, column) {
  }

  ~TinyToken() {
  }
};

}

#endif
