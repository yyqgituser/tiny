#ifndef _TINY_TINYPARSER_H
#define _TINY_TINYPARSER_H

#include <memory>
#include <stack>
#include <vector>

#include "TinyTokenScanner.h"
#include "TinySyntaxNode.h"
#include "csio/MemoryInputStream.h"

namespace tiny {
class TinyParser {
public:
    static int MAX_LOOK;

protected:
    static int** PROD_INFOS;

    static int ACTION_NUM;

    static int** ACTIONS;

    static int* STATE_ACTIONS;

    static int* PARSE_BASES;

    static int PARSE_CAPACITY;

    static int* PARSE_CHECK;

    static int* PARSE_NEXTS;

    static int* GOTO_BASES;

    static int GOTO_CAPACITY;

    static int* GOTO_CHECK;

    static int* GOTO_NEXTS;

    static int INIT_STATE;

    static int ACCEPT_STATE;

    static int* parseNext(int state, int input);

    static int gotoNext(int state, int input);

private:
    static void staticInit();

protected:
    std::shared_ptr<TinyTokenScanner> scanner;

private:
    static class Init {
    public:
        Init() {
            TinyParser::staticInit();
        }
    } initializer;

    static int* readProductionInfo(csio::MemoryInputStream* ins);

    static int* readAction(csio::MemoryInputStream* ins);

    static void loadProductions(csio::MemoryInputStream* ins);

    static void loadStates(csio::MemoryInputStream* ins);

    static void loadActions(csio::MemoryInputStream* ins);

    static void loadParseData(csio::MemoryInputStream* ins);

    static void loadGotoData(csio::MemoryInputStream* ins);

public:
    TinyParser(std::shared_ptr<TinyTokenScanner> scanner) : scanner(scanner) {
    }

    virtual ~TinyParser() = default;

protected:
    std::stack<int> stateStack;

    std::stack<std::shared_ptr<TinySyntaxNode>> nodeStack;

    virtual int reduce(int pid);

public:
    virtual std::shared_ptr<TinySyntaxNode> parse();
};

}

#endif
