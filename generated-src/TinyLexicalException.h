#ifndef _TINY_TINYLEXICALEXCEPTION_H
#define _TINY_TINYLEXICALEXCEPTION_H
#include <string>
#include <stdexcept>

namespace tiny {

class TinyLexicalException : public std::runtime_error {
private:
  int line;

  int column;

public:
  TinyLexicalException(int line, int column, std::string message): std::runtime_error(message), line(line), column(column) {
  }

  int getLine() const {
    return line;
  }

  int getColumn() const {
    return column;
  }

  std::string getMessage() const {
    return what();
  }
};

}

#endif
