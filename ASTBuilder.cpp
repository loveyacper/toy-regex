
#include <assert.h>
#include <iostream>
#include <string.h>
#include <limits>

#include "ASTBuilder.h"
#include "AstNode.h"

namespace bert
{
namespace internal
{

void ASTBuilder::Print() const
{
    assert(ast_.size() == 1);
    ast_.top()->Print(0);
}


ASTNode* ASTBuilder::GetASTRoot() const
{
    if (ast_.size() != 1)
        return nullptr;

    return ast_.top().get();
}

bool ASTBuilder::Build(const std::string& regex)
{
    return Build(regex.c_str(), regex.size());
}


bool ASTBuilder::Build(const char* regex)
{
    return Build(regex, strlen(regex));
}

std::unique_ptr<RangeNode > ASTBuilder::_ProcessRange(const char* regex, std::size_t len, std::size_t& offset)
{
    const auto pLen_ = len;
    auto& pOff_ = offset;

    assert(regex[pOff_] == '[');

    if (pOff_ + 1 >= pLen_)
    {
        std::cerr << "expect ] at end\n";
        return  nullptr;
    }

    ++ pOff_;

    bool  exclude = false;
    if (regex[pOff_] == '^')
    {
        exclude = true;
        ++ pOff_;
    }

    std::unique_ptr<RangeNode> ptr(new RangeNode(exclude));
    std::set<char>  chars;
    
    if (pOff_ < pLen_ && regex[pOff_] == ']')
    {
        ptr->AddChar(']'); // No allowed empty brackets.
        ++ pOff_;
    }

    while (pOff_ < pLen_ && regex[pOff_] != ']')
    {
        if ((pOff_ + 3) < pLen_ && regex[pOff_ + 1] == '-')
        {
            int start = regex[pOff_];
            int end   = regex[pOff_ + 2];

            if (start == end)
            {
                ptr->AddChar(start);
            }
            else
            {
                if (start > end)
                    std::swap(start, end);

                ptr->AddRange(start, end);
            }

            pOff_ += 3;
        }
        else 
        {
            ptr->AddChar(regex[pOff_]);
            ++ pOff_;
        }
    }

    if (pOff_ == pLen_)
    {
        std::cerr << "expect ]\n";
        ptr.reset();
        return ptr;
    }
    else
    {
        assert (regex[pOff_] == ']');
        ++ pOff_;
    }

    return std::move(ptr);
}

bool ASTBuilder::Build(const char* regex, std::size_t len)
{
    _Reset();

    bool prevHasSign = true;
    std::stack<int> symbols;

    for (std::size_t i = 0; i < len; ++ i)
    {
        int ch = regex[i];
        switch (ch)
        {
        case kLeft:
            if (!prevHasSign)
            {
                if (!_AddSymbol(symbols, kConcat))
                    return false;
    
                prevHasSign = true;
            }
            
            symbols.push(ch);
            break;
    
        case kRight:
            while (symbols.top() != kLeft)
            {
                if (!_ProcessNode(symbols.top()))
                    return false;

                symbols.pop();
                prevHasSign = false;    
            }
    
            symbols.pop(); // pop '(', if () is empty, prevHasSign should not change
            break;
    
        case kPlus:
        case kQuestion:
        case kKleene:
        case kAlter:
            if (!_AddSymbol(symbols, ch))
                return false;
    
            prevHasSign = (ch != kKleene && ch != kPlus && ch != kQuestion); // for example, "a*b"
            break;
    
        case kLeftRange:
            {
                auto node = _ProcessRange(regex, len, i);
                if (!node)
                    return false;

                if (!prevHasSign)
                {
                    if (!_AddSymbol(symbols, kConcat))
                        return false;
        
                    prevHasSign = true;
                }
                
                ast_.push(std::move(node));
                prevHasSign = false;
                --i; // fuck, so ugly, to be fixed
            }
            break;
        case kSlash:
            ch = regex[++i];
            // fall through
        default:
            if (!prevHasSign)
            {
                if (!_AddSymbol(symbols, kConcat))
                    return false;
    
                prevHasSign = true;
            }
            
            ast_.push(std::unique_ptr<CharNode>(new CharNode(ch == kDot ? ASTNode::kAny : ch)));
            prevHasSign = false;
            break;
        }
    }

    while (!symbols.empty())
    {
        if (!_ProcessNode(symbols.top()))
            return false;
    
        symbols.pop();
    }
    
    if (!_AddEndNode())
        return false;

    return true;
}

bool ASTBuilder::_AddEndNode()
{
    if (ast_.size() != 1)
        return false;

    auto leftnode = std::move(ast_.top());
    ast_.pop();

    std::unique_ptr<CharNode> endnode(new CharNode(ASTNode::kEnd));

    std::unique_ptr<ConcatNode> root(new ConcatNode);
    root->AddNode(std::move(leftnode));
    root->AddNode(std::move(endnode));

    ast_.push(std::move(root));

    return true;
}

int ASTBuilder::_Priority(int ch) const
{
    switch (ch)
    {
    case kPlus:
    case kQuestion:
    case kKleene:
        return 3;
    case kConcat:
        return 2;
    case kAlter:
        return 1;
        
    default:
        return 0;    
    }
    
    return 0;
}

void ASTBuilder::_Reset()
{
    while (!ast_.empty())
        ast_.pop();
}

    
bool ASTBuilder::_ProcessNode(int symbol)
{
    switch (symbol)
    {
    case kKleene:
    {
        auto node = std::move(ast_.top());
        ast_.pop();
           
        std::unique_ptr<ASTNode> kl(new KleeneNode(std::move(node)));
        ast_.push(std::move(kl));
    }
        break;

    case kPlus:
    case kQuestion:
    {
        auto node = std::move(ast_.top());
        ast_.pop();
           
        std::unique_ptr<RepeateNode> rp(new RepeateNode(std::move(node)));
        if (kQuestion == symbol)
            rp->SetRepeatCount(0, 1);
        else
            rp->SetRepeatCount(1, std::numeric_limits<int>::max());

        ast_.push(std::move(rp));
    }
        break;

       
    case kAlter:
    {
         auto right = std::move(ast_.top());
         ast_.pop();
         auto left = std::move(ast_.top());
         ast_.pop();

         std::unique_ptr<AlterNode> node(new AlterNode);
         node->AddNode(std::move(left));
         node->AddNode(std::move(right));

         ast_.push(std::move(node));     
    }
        break;

    case kConcat:
    {
         auto right = std::move(ast_.top());
         ast_.pop();
         auto left = std::move(ast_.top());
         ast_.pop();


         std::unique_ptr<ConcatNode> node(new ConcatNode);
         node->AddNode(std::move(left));
         node->AddNode(std::move(right));
         
         ast_.push(std::move(node));    
    }
        break;

    default:
        std::cerr << "false symbol " << symbol << std::endl;
        return false;
    }
    
    return true;    
}

bool ASTBuilder::_AddSymbol(std::stack<int>& symbols, int newSymbol)
{
    while (!symbols.empty() && _Priority(newSymbol) <= _Priority(symbols.top()))
    {
        auto sym(symbols.top());
        symbols.pop();

        if (!_ProcessNode(sym))
            return false;
    }
    
    symbols.push(newSymbol);
    return true;
}

} // internal

} // bert
