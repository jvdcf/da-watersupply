#ifndef DA2324_PRJ1_G163_PARSER_H
#define DA2324_PRJ1_G163_PARSER_H
#include "Utils.h"

#include <cstddef>
#include <exception>
#include <functional>
#include <optional>
#include <ostream>
#include <string>
#include <tuple>
#include <vector>

/**
 * @brief The result of a parser after being unwrapped.
 */
template <typename O> using parsertup = std::tuple<std::string, O>;
/**
 * @brief The result of a parser.
 */
template <typename O> using POption = std::optional<parsertup<O>>;
/**
 * @brief Type delaration for functions that parse a string.
 * @details This is an helper declaration of a type used to enforce type-safety
 * of closures used to parse a given string for the Parser type.
 */
template <typename O> using parserfn = std::function<POption<O>(std::string)>;

/**
 * @brief This is a functor class that parses string.
 * @details This class is the basic center of the simple [Parser
 * Combinator](https://en.wikipedia.org/wiki/Parser_combinator) library
 * developed here. It provides simple building blocks to build custom parsers.
 */
template <typename O> class Parser {
private:
  /// Function that actually parses the input.
  parserfn<O> next;

public:
  /// Constructor for the class.
  Parser(parserfn<O> fn) : next(fn){};
  /// Operator overloading allowing the user to call a parser.
  POption<O> operator()(std::string input) const { return next(input); }

  /*
   * @brief  This function pairs two parsers together.
   * @return Parser<std::tuple<O, U>>>
   * @params Parser<U> snd
   * @details This Combinator, also called the 'sequence' Combinator, pairs two
   * Parsers and provides a new parser that when called, calls each of the two
   * in sequence.
   * @note O(1); when called: O(A+B) where A is the complexity of the first and
   * B the complexity of the second Parsers.
   */
  template <typename U> Parser<std::tuple<O, U>> pair(Parser<U> snd) {
    auto p = Parser(this->next);
    return Parser<std::tuple<O, U>>(
        [p, snd](auto input) -> POption<std::tuple<O, U>> {
          auto res1 = p(input);
          if (res1.has_value()) {
            auto [rest1, result1] = res1.value();
            auto res2 = snd(rest1);
            if (res2.has_value()) {
              auto [rest2, result2] = res2.value();
              return std::tuple(rest2, std::tuple(result1, result2));
            }
          }
          return {};
        });
  }
  /*
   * @brief  This function allows calling a parser once or multiple times,
   * equivalent to regex '+'.
   * @return Parser<std::vector<O>>
   * @details This Combinator, also called the 'repeat1' Combinator, calls the
   * parser once: if it fails, the whole parser fails. If it succeeds, it will
   * attempt to apply the underlying parser more times until it fails, returning
   * whatever was parsed up until that point. It is equivalent to the '+'
   * operator in Regular Expressions.
   * @note O(1). When called, O(N*A), where N is the number of times the
   * underlying parser is called and A is the complexity of the underlying
   * parser.
   */
  Parser<std::vector<O>> take_while() {
    auto p = Parser(this->next);
    return Parser<std::vector<O>>([p](auto s) -> POption<std::vector<O>> {
      auto status = p(s);
      if (!status.has_value())
        return {};
      std::vector<O> res;
      std::string rest = "";
      while (status.has_value()) {
        auto [s, val] = status.value();
        res.push_back(val);
        rest = s;
        if (rest.size() == 0)
          break;
        status = p(rest);
      }
      return std::tuple(rest, res);
    });
  }
  /*
   * @brief  This function provides a parser that eagerly finds a end delimeter.
   * @return Parser<O>
   * @params Parser<T> delimeter
   * @details This Combinator, also called the 'terminated' Combinator, eagerly
   * searches the input string for a provided delimeter and discards the result
   * when it finds it, attempting to parse the interior with the 'this' parser.
   * if you reach the end of the string before parsing the delimeter OR you do
   * not parse the entire captured input, the parser fails.
   * @note O(1); when called: O(N*B + A) where A is the complexity of the
   * underlying parser and B the complexity of the delimeter parser and N is the
   * number of characters until the delimeter is found.
   */
  template <typename T> Parser<O> ends_with_fst(Parser<T> delimiter) {
    auto p = Parser(this->next);
    return Parser<O>([p, delimiter](auto input) -> POption<O> {
      if (input.size() == 0)
        return {};
      for (std::string inp = input; 0 < inp.size(); inp = inp.substr(1)) {
        auto par = delimiter(inp);
        if (!par.has_value())
          continue;
        // std::cout << "Found delimiter \n";
        auto [rest, pars] = par.value();
        auto consumed = input.size() - rest.size();
        auto to_ev = input.size() - inp.size();
        std::string tgt = input.substr(0, to_ev);
        auto ppp = p(tgt);
        if (!ppp.has_value())
          return {};
        auto [r, parsed] = ppp.value();
        if (!r.empty())
          return {};
        return std::tuple(input.substr(consumed), parsed);
      }
      return {};
    });
  }
  /*
   * @brief  This function returns instead a portion of the input that is
   * recognized by the underlying parser.
   * @return Parser<std::string>
   * @details This Combinator simply applies the underlying parser, discards
   * the output, and returns the corresponding portion of the input that
   * is parsed by the underlying parser, keeping the return characteristics of
   * it.
   * @note O(1); when called: O(A) where A is the complexity of the
   * underlying parser.
   */
  Parser<std::string> recognize() {
    auto p = Parser(this->next);
    return Parser<std::string>([p](auto s) -> POption<std::string> {
      size_t lenght = s.size();
      auto res = p(s);
      if (!res.has_value())
        return {};
      auto [rest, parsed] = res.value();
      size_t consumed = lenght - rest.size();
      return std::tuple(rest, s.substr(0, consumed));
    });
  }
  /*
   * @brief  This function returns a new parser based on the results of the
   * previous parser; equivalent to Functor's fmap (<$>).
   * @return Parser<U>
   * @param  std::function<U(O)> f
   * @details This Combinator, which is essentially fmap from the Functor
   * category, penetrates the result of the parser and applies a function,
   * converting the overall result of the parsing into U.
   * @note O(1); when called: O(F) where F is the complexity of the function f.
   */
  template <typename U> Parser<U> pmap(std::function<U(O)> f) {
    auto p = Parser(this->next);
    return Parser<U>([p, f](auto inp) -> POption<U> {
      auto res = p(inp);
      if (!res.has_value())
        return {};
      auto [rest, resu] = res.value();
      try {
        return std::tuple(rest, f(resu));
      } catch(std::exception& e) {
        return {};
      }
    });
  };
};
/*
 * @brief  This function allows to alternate between multiple parsers.
 * @return Parser<T>
 * @param  std::vector<Parser<T>> alts
 * @details This Combinator will try to apply each parser in order, until it
 * finds one that parses successfully.
 * @note O(1); when called: O(N*A) where A is the complexity of each parser in
 * the list and N is the number of parsers.
 */
template <typename T> Parser<T> alt(std::vector<Parser<T>> alts) {
  return Parser<T>([alts](auto input) -> POption<T> {
    for (auto alt : alts) {
      auto res = alt(input);
      if (res.has_value())
        return res;
    }
    return {};
  });
}
/*
 * @brief  This function allows parse a character as long as it matches a
 * predicate.
 * @return Parser<char>
 * @param  std::function<bool (char)> f
 * @details This parser verifies if the first character on the input verifies a
 * predicate.
 * @note O(1); when called: O(1).
 */
Parser<char> verifies(std::function<bool(char)> f);

/*
 * @brief  This function allows to parse a specific char.
 * @return Parser<char>
 * @param  char c
 * @details This parser will parse the first character if it matches the given
 * char.
 * @note O(1); when called: O(1).
 */
Parser<char> char_p(char c);

/*
 * @brief  This function produces a parser that parses a specific string.
 * @return Parser<std::string>
 * @param  std::string s
 * @details This parser will create char_p parsers for each char in the string,
 * returning a parser that parses the same string.
 * @note O(N); when called: O(N) where N is the number of characters that are in
 * the input string.
 */
Parser<std::string> string_p(std::string s);

Parser<std::string> ws();

template <class T> Parser<T> null() {
  return Parser<T>([](auto s) -> POption<T> {
    return std::tuple(s, T());
  });
};
#endif // DA2324_PRJ1_G163_PARSER_H
