#ifndef BERT_ASTNODE_H
#define BERT_ASTNODE_H

#include <set>
#include <vector>
#include <memory>

namespace bert
{

namespace internal
{


using Range = std::pair<int, int>;

struct Input
{
    Input();

    bool IsEnd() const;
    bool IsAny() const;
    bool Accept(int ch) const;

    friend bool operator==(const Input& a, const Input& b);
    friend bool operator<(const Input& a, const Input& b);

    bool exclude;
    std::vector<Range> ranges;
    std::set<int> chars;
};

class ASTNode
{
public:
    using PositionNodes = std::set<const ASTNode* >;

    virtual ~ASTNode();

    // for debug print ast tree
    virtual void Print(int indent) const  { }

    virtual bool Nullable() const = 0;
    virtual PositionNodes FirstPos() const = 0;
    virtual PositionNodes LastPos() const = 0;
    virtual void CalcFollowPos() const = 0;

    const PositionNodes& FollowPos() const;
    void AddFollowPos(const PositionNodes& follows) const;

    virtual Input GetInput() const { return Input();  }

    static const int kEnd;
    static const int kAny;

private:
    mutable PositionNodes  followpos_;
};



class CharNode : public ASTNode
{
public:
    explicit CharNode(int c);

    void Print(int indent) const override;

    bool Nullable() const override;
    PositionNodes FirstPos() const override;
    PositionNodes LastPos() const override;
    void CalcFollowPos() const override;

    Input GetInput() const override;
private:
    int char_;
};


class RepeateNode : public ASTNode
{
public:
    explicit RepeateNode(std::unique_ptr<ASTNode> node);

    void Print(int indent) const override;

    bool Nullable() const override;
    PositionNodes FirstPos() const override;
    PositionNodes LastPos() const override;
    void CalcFollowPos() const override;

    void SetRepeatCount(int low, int high);

private:
    std::unique_ptr<ASTNode> node_;
    std::pair<int, int>  repeat_;
};

class RangeNode : public ASTNode
{
public:
    explicit RangeNode(bool exclude);

    void Print(int indent) const override;

    bool Nullable() const override;
    PositionNodes FirstPos() const override;
    PositionNodes LastPos() const override;
    void CalcFollowPos() const override;

    void AddChar(int c);
    void AddRange(int low, int high);

    Input GetInput() const override;
private:
    const bool exclude_;
    std::vector<Range> ranges_;
    std::set<int> chars_;
};


class ConcatNode : public ASTNode
{
public:
    ConcatNode();

    void Print(int indent) const override;
    void AddNode(std::unique_ptr<ASTNode> node);

    bool Nullable() const override;
    PositionNodes FirstPos() const override;
    PositionNodes LastPos() const override;
    void CalcFollowPos() const override;

private:
    std::unique_ptr<ASTNode> left_;
    std::unique_ptr<ASTNode> right_;
};



class AlterNode : public ASTNode
{
public:
    AlterNode();

    void Print(int indent) const override;
    void AddNode(std::unique_ptr<ASTNode> node);

    bool Nullable() const override;
    PositionNodes FirstPos() const override;
    PositionNodes LastPos() const override;
    void CalcFollowPos() const override;

private:
    std::unique_ptr<ASTNode> left_;
    std::unique_ptr<ASTNode> right_;
};



class KleeneNode : public ASTNode
{
public:
    explicit KleeneNode(std::unique_ptr<ASTNode> node);

    void Print(int indent) const override;

    bool Nullable() const override;
    PositionNodes FirstPos() const override;
    PositionNodes LastPos() const override;
    void CalcFollowPos() const override;

private:
    std::unique_ptr<ASTNode> kleene_;
};

} // internal

} // bert

#endif
