# toy-regex
A toy-regex use follow set and DFA, support () [^] | . * ? + concatenation.
## example
```c++
Regex  r("a\\+[bc](.*)e?f+");

assert (t.Match("a+cbf"));
assert (t.Match("a+bceff"));
```
