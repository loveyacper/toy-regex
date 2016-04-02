#ifndef BERT_ASTBUILDER_H
#define BERT_ASTBUILDER_H

#include <string>
#include <memory>
#include <stack>

#include "AstNode.h"

namespace bert
{
namespace internal
{

class ASTBuilder final
{
    static const int kLeft = '(';
    static const int kRight = ')';
    static const int kAlter = '|';
    static const int kConcat = '&';
    static const int kKleene = '*';
    static const int kLeftRange = '[';
    static const int kRightRange = ']';
    static const int kExclude = '^';
    static const int kPlus= '+';
    static const int kQuestion = '?';
    static const int kSlash = '\\';
    static const int kDot = '.';

public:
    void Print() const;
    bool Build(const char* regex);
    bool Build(const char* regex, std::size_t len);
    bool Build(const std::string& regex);
    ASTNode* GetASTRoot() const;

private:
    int _Priority(int ch) const;
    void _Reset();
    std::unique_ptr<RangeNode > _ProcessRange(const char* regex, std::size_t len, std::size_t& offset);
    bool _ProcessNode(int symbol);
    bool _AddSymbol(std::stack<int>& symbols, int newSymbol);
    bool _AddEndNode();

    std::stack<std::unique_ptr<ASTNode> > ast_;
};

} // internal

} // bert

#endif
