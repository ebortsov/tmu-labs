# Sheet 10 - Compile-time Programming

## `config_eval`

Given a compile-time constant string describing configuration settings in a `key=value` format we want to parse it
&ndash; at compile time &ndash; into a structure that allows lookup by key.

For example, given the string:

```ini
level=debug dir=/var/log/cppprog timeout=60 format=json
```

we want a structure that will allow querying like:

```cpp
structure["level"]; // returns "debug"
```

The string can be assumed to be valid, in particular:

- pairs are separated by one or more space or newline characters (`' '` or `'\n'`)
- key-value pairs are always of the form `k=v` where `k` and `v` are _non-empty_ strings that contain no
  whitespace or the `=` sign; the only whitespace in the string is that separating the pairs.

Your task is to implement the template function

```cpp
template <const char *S>
consteval [some type] parse_config();
```

The particular type returned by `parse_config()` has to match the `Config` concept defined in `config_concept.hpp`.
All the requirements are listed in that concept.

There are no additional requirements on the size of the returned structure or the time complexity of its operations,
however all operations _must_ be `constexpr`.

Your solution should be header-only and placed in `config_eval.hpp`. You are not allowed to edit `config_concept.hpp`.

## Conceptual Serializer

We will implement a generic serializer using concepts and template meta-programming.
In principle we could serialize to any format, or even make the solution generic and able to handle
an arbitrary format, but for simplicity we will use [JSON](https://www.rfc-editor.org/rfc/rfc7159) as the target.

In short, a JSON document is a tree whose nodes are either primitive types, lists, or maps, and any
node can also be `null`.
Your task is to create concepts that will match C++ types serializable as one of those node types,
and then to implement the actual serializer.

The main concept you need to define is `Serializable`, and the serializer function
required is `std::string conceptual::serialize(const T& t)` for any `Serializable` `T`,
yielding the JSON representation of the passed value.

Importantly, all JSON values returned from your serializer should be compressed, i.e. contain no
extraneous whitespace characters. For example, `{"a":[1,2,3]}` is a compressed JSON, while
`{"a": [1, 2, 3]}` or

```json
{
  "a": [
    1,
    2,
    3
  ]
}
```

are not.

### `Primitive` and `StringLike`

Primitive types are the standard integer types (`int8_t`, `int16_t`, `int32_t`, `int64_t` and their unsigned variants),
`float`, `double`, `bool`, `char`, `std::string`, and other `StringLike` types.

Some primitives are `StringLike` which means they can be treated as strings.
These include all types implicitly convertible to `std::string` as well as `char`.

All `Primitive` types are `Serializable`. Serializing a primitive should yield:

- the converted string value surrounded in double quotes for `StringLike`.
- the string `"false"` for `false` `bool` values, `"true"` for `true` values.
- for other primitives the result of calling [
  `std::to_string`](https://en.cppreference.com/w/cpp/string/basic_string/to_string) on their value.

For example, the following are true:

```cpp
"42" == conceptual::serialize(42)
"true" == conceptual::serialize(true)
"42.000000" == conceptual::serialize(42.0)
"\"string value\"" == conceptual::serialize("string value")
"\"a\"" == conceptual::serialize('a')
```

#### String escapes

For simplicity, you can assume all characters and all strings we are concerned with are ASCII and don't
contain any special characters like newlines, null terminators, escapes, or the double quotes char.
In other words, you don't have to concern yourself with escaping anything,
just surrounding the string in quotes is fine.

### Lists: `SeqSerializable`, `SeqLike` and `ArrayLike`

Anything that looks like a sequence should be serialized as a JSON list.
We distinguish two concept:

- `SeqLike` for types that provide a `begin()` and an `end()` function returning at least
  an `std::forward_iterator` whose `value_type` is also `Serializable`.
- `ArrayLike` for types that provide a `size()` function and an `operator[]` that takes in
  unsigned-integer indices and returns something `Serializable`.

`ArrayLike` types in the standard library are: `std::vector`, `std::deque`, `std::array`.
`SeqLike` types in the standard library are: `std::forward_list`, `std::list`, `std::multiset`,
`std::set`, `std::unordered_multiset`, `std::unordered_set`.

Serialization of `SeqLike` and `ArrayLike` types should create a valid JSON list, delimited in `[` and `]`
characters, containing the comma-separated list of recursively serialized elements in the sequence.

For example, the following are true:

```cpp
"[true]" == conceptual::serialize(std::vector<bool>{true})
"[\"a\",\"b\",\"c\"]" == conceptual::serialize(std::list<char>{'a','b','c'})
"[]" == conceptual::serialize(std::set<uint32_t>{});
"[[1,2],[],[1]]" == conceptual::serialize(std::vector<std::vector<uint32_t>>{{1,2},{},{1}});
```

There are types that are both `SeqLike` and `ArrayLike`, e.g. `std::vector<T>` for any `Serializable T`.
In those cases, when serializing, your implementation should *always* prefer the `ArrayLike` interface
and not call the `begin()` or `end()` functions.

Additionally, define a `ListSerializable` concept that encompasses all types that are either
`SeqLike` or `ArrayLike`.

### Maps: `MapLike`

Types that look like maps are serialized as JSON objects.

The `MapLike` concept requires two member types, `key_type` and `mapped_type`, and two functions
`begin()` and `end()` returning a key-value pair iterator. The `key_type` *must be `Primitive`*
and `mapped_type` must be `Serializable`.
The iterator must be at least an `std::forward_iterator`.
For an iterator value `it` the expression `it->first` should return
a `const key_type &` and `it->second` should return a `const mapped_type &`.
As a reference, see the `std::map` and `std::unordered_map` iterators.

Keys are serialized directly even if they are `StringLike`, i.e. without the additional double quotes.
If the type returns multiple values for the same key (for example as in `std::multimap`),
the serializer's behaviour is not guaranteed -- it can serialize any one of the values.
Moreover, order in JSON objects in unspecified, therefore the implementation can put the keys in any order
within the returned string.

For example, the following are true:

```cpp
"{}" == conceptual::serialize(std::map<bool, bool>{})
"{\"a\":42}" == conceptual::serialize(std::map<char, uint32_t>{{'a', 42}}
```

On the other hand, for the following:

```cpp
conceptual::serialize(std::multimap<uint32_t, uint32_t>{
  {0, 1},
  {1, 2},
  {1, 3},
})
```

there are four possible valid string results:

```json
{"0":1,"1":2}
{"0":1,"1":3}
{"1":2,"0":1}
{"1":3,"0":1}
```

### Nulls and `std::optional`

For any `Serializable` `T`, `std::optional<T>` is also `Serializable`.
The behaviour is straightforward: if the optional contains a value, the value is serialized;
an empty optional is serialized as the literal string `null`.

For example, the following are true:

```
"42" == conceptual::serialize(std::optional<uint32_t>{42})
"null" == conceptual::serialize(std::optional<uint32_t>{})
```

Note that `std::optional<T>` is not `Primitive` even if `T` is `Primitive`.

### File organization

The declared `concepts` should be located in `serializer_traits.hpp`.
The `serialize` function should be located in `serializer.hpp`.
Your solution must be header-only.

### Summary

To gather all the requirements in a single place, here are all the entities you need to define.
Of course, you can define more helper concepts and functions, but these are required.

#### Concepts

- `Serializable`
- `Primitive`
- `StringLike`
- `ListSerializable`
- `SeqLike`
- `ArrayLike`
- `MapLike`

#### Functions

`std::string serialize(const T& t)` for any `T` that is `Serializable`.
