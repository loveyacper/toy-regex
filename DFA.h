#ifndef BERT_DFA_H
#define BERT_DFA_H
 
#include <map>
#include <memory>
#include <string>

namespace bert
{
namespace internal
{

class ASTNode;
struct Input;

class DFA
{
public:
    bool Construct(ASTNode* root);
    bool Match(const std::string& pattern) const;
    bool Match(const char* pattern) const;
    bool Match(const char* pattern, std::size_t len) const;

    struct State
    {
        explicit State(int id = -1);
        ~State();

        std::string DebugString() const;

        void AddTranslation(const Input& input, State* dstState);
        State* NextState(int input) const;

        int id;
        bool isFinal;
        std::map<Input, State* > transition;
    };

private:
    typedef std::map<int, std::unique_ptr<State> > States;

    States dfa_;
    State* _GetState(int id) const;
};

} // internal

} // bert
#endif
