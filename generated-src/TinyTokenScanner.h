#ifndef _TINY_TINYTOKENSCANNER_H
#define _TINY_TINYTOKENSCANNER_H

#include <memory>
#include <string>
#include <deque>

#include "TinyToken.h"
#include "TinyScanner.h"

namespace tiny {

class TinyTokenScanner {
protected:
  std::shared_ptr<TinyScanner> scanner;
  
  std::deque<std::shared_ptr<TinyToken>> queue;

public:
  TinyTokenScanner(std::shared_ptr<TinyScanner> scanner) : scanner(scanner) {
  }
  
  virtual ~TinyTokenScanner() {
  }

protected:
  void scanTo(std::size_t k) {
    for (std::size_t i = queue.size(); i < k; i++) {
      int id = (int)scanner->next();
      queue.push_back(make_shared<TinyToken>(id, scanner->text(), scanner->line(), scanner->column()));
    }
  }

public:
  virtual std::shared_ptr<TinyToken> look(std::size_t i) {
    scanTo(i + 1);
    return queue[i];
  }

  virtual void pop() {
    if (!queue.empty()) {
      queue.pop_front();
    } else {
      scanner->next();
    }
  }
};

}

#endif
