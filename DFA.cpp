#include <vector>
#include <set>
#include <algorithm>
#include <iostream>
#include <string.h>

#include "AstNode.h"
#include "DFA.h"

namespace bert
{
namespace internal
{

// State
DFA::State::State(int id) : id(id), isFinal(false)
{
}

DFA::State::~State() 
{
}

std::string DFA::State::DebugString() const
{
    std::string res = "Final : ";
    if (isFinal)    
        res += "true";
    else   
        res += "false";
    
    res += ", translate size ";
    res += std::to_string(transition.size());
    res += "\n";

    return res;
}


void DFA::State::AddTranslation(const Input& input, DFA::State* dstState)
{
    transition[input] = dstState;
}
   
std::set<DFA::State*> DFA::State::NextState(int input) const
{
    std::set<DFA::State* > res;
    for (const auto& pair : transition)
    {
        if (pair.first.Accept(input))
        {
            res.insert(pair.second);
        }
    }

    return res;
}


bool DFA::Construct(ASTNode* root)
{
    dfa_.clear();
    root->CalcFollowPos();

    std::vector<ASTNode::PositionNodes > stateQueue{ root->FirstPos() };

    for (int src = 0; src < static_cast<int>(stateQueue.size()); ++src)
    {
        const auto& srcCurrentState = stateQueue[src];

        std::map<Input, ASTNode::PositionNodes > followSets;
        for (auto scs : srcCurrentState)
        {
            // add character node's follow pos
            auto& followset = followSets[scs->GetInput()];
            auto follow = scs->FollowPos();
            std::set_union(follow.begin(), follow.end(),
                followset.begin(), followset.end(), std::inserter(followset, followset.end()));
        }

        for (const auto& pair : followSets)
        {
            const auto& input = pair.first;
            const auto& followers = pair.second;
            // add new sets
            int dstIndex = 0;
            auto it = std::find(stateQueue.begin(), stateQueue.end(), followers);
            if (it == stateQueue.end())
            {
                stateQueue.push_back(followers);
                it = -- stateQueue.end();
                dstIndex = static_cast<int>(stateQueue.size() - 1);
            }
            else
            {
                dstIndex = static_cast<int>(std::distance(stateQueue.begin(), it));
            }

            // record transition
            DFA::State* srcState = _GetState(src);
            if (!srcState)
            {
                srcState = new DFA::State(src);
                dfa_[src].reset(srcState);
            }

            if (input.IsEnd())
            {
                srcState->isFinal = true;
                continue;
            }

            State* dstState = _GetState(dstIndex);
            if (!dstState)
            {
                dstState = new DFA::State(dstIndex);
                dfa_[dstIndex].reset(dstState);
            }

            srcState->AddTranslation(input, dstState);
        }
    }

    return true;
}

bool DFA::Match(const char* pattern, std::size_t len) const
{
    auto current = _GetState(0);
    if (!current)
        return true;

    return _Match(current, pattern, len);
}


bool DFA::Match(const char* pattern) const
{
    return Match(pattern, strlen(pattern));
}

bool DFA::Match(const std::string& pattern) const
{
    return Match(pattern.c_str(), pattern.size());
}


bool DFA::_Match(DFA::State* current, const char* pattern, std::size_t len)
{
    if (!current)
        return true;

    if (len == 0)
        return current->isFinal;

    auto nextlist = current->NextState(pattern[0]);

    for (auto state : nextlist)
    {
        if (_Match(state, pattern + 1, len - 1))
        {
            return true;
        }
    }

    return false;
}

DFA::State* DFA::_GetState(int id) const
{
    auto it = dfa_.find(id);
    if (it != dfa_.end())
        return it->second.get();

    return nullptr;
}

} // internal

} // bert
