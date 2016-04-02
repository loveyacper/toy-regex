
#include <iostream> 

#include "UnitTest.h"
#include "Regex.h"

using bert::Regex;


// Toy regex, support [^] () | * . + ? concat

TEST_CASE(hello)
{
    Regex r("(a|b)*abb");

    EXPECT_TRUE(r.Match("abb"));
    EXPECT_TRUE(r.Match("aabb"));
    EXPECT_TRUE(r.Match("ababb")) << "why ababb not match (a|b)*abb?";

    EXPECT_FALSE(r.Match("ab"));
    EXPECT_FALSE(r.Match(""));
}

TEST_CASE(hello22)
{
    Regex r("x(a(b|c))*(d|e)");

    EXPECT_TRUE(r.Match("xacabe"));
    EXPECT_TRUE(!r.Match("xbcbe"));
    EXPECT_TRUE(r.Match("xe"));

    EXPECT_FALSE(r.Match(""));
    EXPECT_FALSE(r.Match("xbd"));
}

TEST_CASE(hello3)
{
    Regex r("a|b");

    EXPECT_TRUE(r.Match("a"));
    EXPECT_TRUE(r.Match("b"));

    EXPECT_FALSE(r.Match("ab"));
    EXPECT_FALSE(r.Match("ba"));
    EXPECT_FALSE(r.Match(""));
}

TEST_CASE(hello4)
{
    Regex r("(ab|bc|cd)*");

    EXPECT_TRUE(r.Match("ab"));
    EXPECT_TRUE(r.Match("bc"));
    EXPECT_TRUE(r.Match("abbccd"));
    EXPECT_TRUE(r.Match(""));

    EXPECT_FALSE(r.Match("ac"));
}


TEST_CASE(hello5)
{
    Regex r("(a|b|c)*b*");

    EXPECT_TRUE(r.Match("b"));
    EXPECT_TRUE(r.Match("bbb"));
    EXPECT_TRUE(r.Match("bbbabc"));
    EXPECT_TRUE(r.Match(""));
}

TEST_CASE(hello6)
{
    Regex r("(a*)*");

    EXPECT_TRUE(r.Match("a"));
    EXPECT_TRUE(r.Match("aa"));
    EXPECT_TRUE(r.Match(""));
}

TEST_CASE(hello7)
{
    Regex r("a*a*");

    EXPECT_TRUE(r.Match("a"));
    EXPECT_TRUE(r.Match("aaaa"));
    EXPECT_TRUE(r.Match(""));
}

TEST_CASE(hello8)
{
    Regex r("((ab*|ac*)*|ad*)*");

    EXPECT_TRUE(r.Match("a"));
    EXPECT_TRUE(r.Match(""));
    EXPECT_TRUE(r.Match("abad"));

    EXPECT_FALSE(r.Match("abcd"));
}

TEST_CASE(hello9)
{
    Regex r("((ab*|ac*)*|ad*|ef)*");

    EXPECT_TRUE(r.Match("abbacadaaefa"));
    EXPECT_TRUE(r.Match(""));

    //
    EXPECT_TRUE(r.Compile("x(a(b|c))*(d|e)"));

    EXPECT_TRUE(r.Match("xabacd"));
    EXPECT_TRUE(r.Match("xd"));

    EXPECT_FALSE(r.Match("xde"));
}

TEST_CASE(hello10)
{
    Regex t("a.*b");
    
    EXPECT_TRUE(t.Match("ab"));
    EXPECT_TRUE(t.Match("abb"));
    EXPECT_TRUE(t.Match("abbb"));

    EXPECT_FALSE(t.Match("a"));
    EXPECT_FALSE(t.Match("b"));
    EXPECT_FALSE(t.Match("bb"));
    EXPECT_FALSE(t.Match(""));
}

TEST_CASE(hello11)
{
    Regex t("[^abc]*");
    EXPECT_TRUE(t.Match("xyz"));
    EXPECT_FALSE(t.Match("a"));
    EXPECT_FALSE(t.Match("b"));
    EXPECT_FALSE(t.Match("abc"));
    EXPECT_FALSE(t.Match("fuckabc"));
}

TEST_CASE(hello12)
{
    Regex t("12[^a-ch-ij]*");

    EXPECT_TRUE(t.Match("12xyz"));
    EXPECT_TRUE(t.Match("12"));
    EXPECT_FALSE(t.Match("12abcd"));
    EXPECT_FALSE(t.Match(""));
}

TEST_CASE(hello13)
{
    Regex t("([^ab]|a)f");

    EXPECT_TRUE(t.Match("af"));
    EXPECT_TRUE(t.Match("cf"));
    EXPECT_FALSE(t.Match(""));
    EXPECT_FALSE(t.Match("bf"));
}

TEST_CASE(hello14)
{
    Regex t("(ab)+");

    EXPECT_TRUE(t.Match("ab"));
    EXPECT_TRUE(t.Match("abab"));

    EXPECT_FALSE(t.Match(""));
    EXPECT_FALSE(t.Match("a"));
}


TEST_CASE(hello15)
{
    Regex t("(ab)?");

    EXPECT_TRUE(t.Match(""));
    EXPECT_TRUE(t.Match("ab"));

    EXPECT_FALSE(t.Match("abab"));
}

TEST_CASE(hello16)
{
    Regex t("a\\+");
    EXPECT_TRUE(t.Match("a+"));
    EXPECT_TRUE(!t.Match(""));
    EXPECT_FALSE(t.Match("a"));
}

TEST_CASE(hello17)
{
    Regex t("a\\+[bc].*e?f+");
    EXPECT_TRUE(t.Match("a+cbf"));
    EXPECT_TRUE(t.Match("a+bceff"));
}
