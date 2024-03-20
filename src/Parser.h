#ifndef DA2324_PRJ1_G163_PARSER_H
#define DA2324_PRJ1_G163_PARSER_H
#include "Utils.h"

#include <functional>
#include <tuple>
#include <vector>
#include <string>

enum class ParseError {
  Recoverable,
  Irrecoverable,
};
template<typename O>
using parserfn = std::function<Option<std::tuple<std::string,O>>(std::string)>; 

template<typename O>
using parsertup = std::tuple<std::string,O>;

template<typename O> class Parser {
public:
  std::function<Option<std::tuple<std::string,O>>(std::string)> next;
  Parser(std::function<Option<std::tuple<std::string,O>>(std::string)>);
};

Parser<char> char_p(char c); 
Parser<std::string> string_p(std::string s);
template<typename T, typename U> Parser<std::tuple<T,U>> pair(Parser<T> fst, Parser<U> snd);
template<typename T, typename U, typename V> Parser<std::tuple<T,U,V>> sequence3_p(Parser<T> fst, Parser<U> tgt, Parser<V> snd);
template<typename A, typename I> Parser<std::vector<A>> take_while_p(Parser<I> f);
template<typename T, typename U> Parser<T> ends_with_fst(Parser<T> tgt, Parser<U> delimiter);

#endif //DA2324_PRJ1_G163_PARSER_H
