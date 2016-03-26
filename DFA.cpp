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
   
DFA::State* DFA::State::NextState(int input) const
{
    for (const auto& pair : transition)
    {
        if (pair.first.Accept(input))
            return pair.second;
    }

    return nullptr;
}


bool DFA::Construct(ASTNode* root)
{
    dfa_.clear();
    root->CalcFollowPos();

    std::vector<ASTNode::PositionNodes > stateQueue{ root->FirstPos() };

    for (std::size_t src = 0; src < stateQueue.size(); ++src)
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
            std::size_t dstIndex = 0;
            auto it = std::find(stateQueue.begin(), stateQueue.end(), followers);
            if (it == stateQueue.end())
            {
                stateQueue.push_back(followers);
                it = -- stateQueue.end();
                dstIndex = stateQueue.size() - 1;
            }
            else
            {
                dstIndex = std::distance(stateQueue.begin(), it);
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

    std::size_t offset = 0;
    while (offset < len)
    {
        auto next = current->NextState(pattern[offset]);
        if (!next)
            return false;

        current = next;
        ++ offset;
    }

    if (current && current->isFinal)
        return true;

    return false;
}


bool DFA::Match(const char* pattern) const
{
    return Match(pattern, strlen(pattern));
}

bool DFA::Match(const std::string& pattern) const
{
    return Match(pattern.c_str(), pattern.size());
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
