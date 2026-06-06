#ifndef _TINY_TINYSCANNER_H
#define _TINY_TINYSCANNER_H

#include <stdexcept>
#include <memory>
#include <fstream>
#include <filesystem>
#include "csio/MemoryInputStream.h"
#include "csio/TextReader.h"
#include "TinyTokenType.h"

namespace tiny {

class TinyScanner {
private:
  static int STATE_NUM;
  
  static int INPUT_NUM;
  
  static int DEAD_STATE;

  static int DEFAULT_LEX_STATE;

  static bool* ACCEPTS;

  static int* FA_TABLE;

  static int* BASES;

  static int CAPACITY;

  static int* CHECK;

  static int* NEXTS;

  static int OPT_TOP;

  static int* OPT_CMAP;

  static int* LEX_STATES;

  static bool* SKIP;

  static tiny::TinyTokenType* TOKENS;

  const static int DEFAULT_BUFFER_SIZE = 1024;

  std::shared_ptr<csio::TextReader> reader;

  char32_t *buffer;

  int bufferSize;

  int initialState;

  int limit;

  int currentPosition;

  int lastAcceptedPosition;

  int currentLineState;

  int currentLine;

  int currentColumn;

  int tabWidth;

  int tokenStartPosition;

  int tokenLine;

  int tokenColumn;

  int tokenLineState;

  static class Init {
    public:
      Init() {
        TinyScanner::staticInit();
      }
  } initializer;

  bool readChars();

  int readCodePoint();

public:
  TinyScanner(std::shared_ptr<csio::TextReader> reader): TinyScanner(reader, DEFAULT_BUFFER_SIZE) {
  }

  TinyScanner(std::shared_ptr<csio::TextReader> reader, int bufferSize): TinyScanner(reader, bufferSize, 1) {
  }

  TinyScanner(std::shared_ptr<csio::TextReader> reader, int bufferSize, int tabWidth);

  TinyScanner(std::shared_ptr<std::istream> in, const std::string& charsetName, int bufferSize) : TinyScanner(std::make_shared<csio::TextReader>(in, charsetName), bufferSize) {
  }

  TinyScanner(std::shared_ptr<std::istream> in, const std::string& charsetName) : TinyScanner(in, charsetName, DEFAULT_BUFFER_SIZE) {
  }

  TinyScanner(std::shared_ptr<std::istream> in) : TinyScanner(in, "UTF-8") {
  }

  TinyScanner(const std::string& fileName, const std::string& charsetName, int bufferSize) :
      TinyScanner(std::make_shared<std::fstream>(fileName, std::ios::in | std::ios::binary), charsetName, bufferSize) {
  }

  TinyScanner(const std::string& fileName, const std::string& charsetName) : TinyScanner(fileName, charsetName, DEFAULT_BUFFER_SIZE) {
  }

  TinyScanner(const std::string& fileName) : TinyScanner(fileName, "UTF-8") {
  }

  TinyScanner(const std::filesystem::path& path, const std::string& charsetName, int bufferSize) :
      TinyScanner(std::make_shared<std::fstream>(path, std::ios::in | std::ios::binary), charsetName, bufferSize) {
  }

  TinyScanner(const std::filesystem::path& path, const std::string& charsetName) : TinyScanner(path, charsetName, DEFAULT_BUFFER_SIZE) {
  }

  TinyScanner(const std::filesystem::path& path) : TinyScanner(path, "UTF-8") {
  }

  TinyScanner(const char* fileName) : TinyScanner(std::string(fileName)) {
  }

  ~TinyScanner() {
    delete[] buffer;
  }

  TinyScanner(const TinyScanner&) = delete;
  TinyScanner& operator=(const TinyScanner&) = delete;

  std::u32string text() const {
    if (lastAcceptedPosition == -1) {
        throw std::runtime_error("invalid state");
    }
    return std::u32string(&buffer[tokenStartPosition], &buffer[lastAcceptedPosition]);
  }

  int line() const {
    return tokenLine;
  }

  int column() const {
    return tokenColumn;
  }

  void setLexState(int lexId) {
    initialState = LEX_STATES[lexId];
  }

  void setTabWidth(int tabWidth) {
    this->tabWidth = tabWidth;
  }

  tiny::TinyTokenType next();

private:
  static void staticInit();

  static void staticUninit();
};

}

#endif
