#ifndef _TINY_TINYPARSEEXCEPTION_H
#define _TINY_TINYPARSEEXCEPTION_H

#include <string>

namespace tiny {

class TinyParseException {
private:
    std::string message;

    std::shared_ptr<TinyToken> token;

public:
  TinyParseException() : token(0) {
  }

  TinyParseException(std::string message) : TinyParseException(message, 0) {
  }

  TinyParseException(std::string message, std::shared_ptr<TinyToken> token) : message(message), token(token) {
  }

  std::string getMessage() {
    return message;
  }

  std::shared_ptr<TinyToken> getToken() {
    return token;
  }
};

}

#endif
