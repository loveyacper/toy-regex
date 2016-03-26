#ifndef BERT_REGEX_H
#define BERT_REGEX_H

#include "DFA.h"

namespace bert
{

class Regex final
{
public:
    Regex() = default;
    Regex(const Regex&) = delete;
    void operator=(Regex r) = delete;

    Regex(const char* regex);
    Regex(const char* regex, std::size_t len);
    Regex(const std::string& regex);
    ~Regex();

    bool Compile(const char* regex);
    bool Compile(const char* regex, std::size_t len);
    bool Compile(const std::string& regex);

    bool Match(const char* pattern) const;
    bool Match(const std::string& pattern) const;

private:
    internal::DFA dfa_;
};

}

#endif
