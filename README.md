# toy-regex
A toy-regex use follow set and DFA, support () [^] | . * ? + concatenation.
## example
```c++
Regex  r("a\\+[bc](.*)e?f+");

assert (r.Match("a+cbf"));
assert (r.Match("a+bceff"));

///
assert (r.Compile("x(a(b|c))*(d|e)"));

assert (r.Match("xabacd"));
assert (r.Match("xd"));

assert (!r.Match("xde"));
```
