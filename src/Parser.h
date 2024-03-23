#ifndef DA2324_PRJ1_G163_PARSER_H
#define DA2324_PRJ1_G163_PARSER_H
#include "Utils.h"

#include <cstddef>
#include <functional>
#include <optional>
#include <ostream>
#include <string>
#include <tuple>
#include <vector>

template <typename O>
using parserfn =
    std::function<std::optional<std::tuple<std::string, O>>(std::string)>;

template <typename O> using parsertup = std::tuple<std::string, O>;

template <typename O> using POption = std::optional<parsertup<O>>;

template <typename O> class Parser {
public:
  parserfn<O> next;
  Parser(parserfn<O> fn) : next(fn){};

  template <typename U> Parser<std::tuple<O, U>> pair(Parser<U> snd) {
    auto p = Parser(this->next);
    return Parser<std::tuple<O, U>>(
        [p, snd](auto input) -> POption<std::tuple<O, U>> {
          auto res1 = p.next(input);
          if (res1.has_value()) {
            auto [rest1, result1] = res1.value();
            auto res2 = snd.next(rest1);
            if (res2.has_value()) {
              auto [rest2, result2] = res2.value();
              return std::tuple(rest2, std::tuple(result1, result2));
            }
          }
          return {};
        });
  }
  Parser<std::vector<O>> take_while() {
    auto p = Parser(this->next);
    return Parser<std::vector<O>>([p](auto s) -> POption<std::vector<O>> {
      auto status = p.next(s);
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
        status = p.next(rest);
      }
      return std::tuple(rest, res);
    });
  }
  // template <typename T> Parser<O> ends_with_fst(Parser<T> delimiter) {
  //   auto p = Parser(this->next);
  //   return Parser<O>([p, delimiter](auto input) -> POption<O> {
  //     auto rest = input;
  //     auto fnd_del = delimiter.next(rest);
  //     std::string collected;
  //     while (!fnd_del.has_value()) {
  //       if (input.size() <= 1)
  //         return {};
  //       collected.push_back(input[0]);
  //       rest = rest.substr(1);
  //       fnd_del = delimiter.next(rest);
  //     }
  //     auto collected_parsed = p.next(collected);
  //     if (!collected_parsed.has_value())
  //       return {};
  //     auto [empty, parsed] = collected_parsed.value();
  //     if (empty != "")
  //       return {};
  //     return std::tuple(rest, parsed);
  //   });
  // }
  template <typename T> Parser<O> ends_with_fst(Parser<T> delimiter) {
    auto p = Parser(this->next);
    return Parser<O>([p, delimiter](auto input) -> POption<O> {
      if (input.size() == 0) return {};
      for (std::string inp = input; 0 < inp.size(); inp = inp.substr(1)) {
        auto par = delimiter.next(inp);
        if (!par.has_value()) continue;
        //std::cout << "Found delimiter \n";
        auto [rest,pars] = par.value();
        auto consumed = input.size() - rest.size();
        auto to_ev = input.size() - inp.size();
        std::string tgt = input.substr(0,to_ev);
        auto ppp = p.next(tgt);
        if (!ppp.has_value()) return {};
        auto [r, parsed] = ppp.value();
        if (!r.empty()) return {};
        return std::tuple(input.substr(consumed), parsed);
      } 
      return {};
    });
  }

  Parser<std::string> recognize() {
    auto p = Parser(this->next);
    return Parser<std::string>([p](auto s) -> POption<std::string> {
      size_t lenght = s.size();
      auto res = p.next(s);
      if (!res.has_value())
        return {};
      auto [rest, parsed] = res.value();
      size_t consumed = lenght - rest.size();
      return std::tuple(rest, s.substr(0, consumed));
    });
  }

  template <typename U> Parser<U> pmap(std::function<U(O)> f) {
    auto p = Parser(this->next);
    return Parser<U>([p, f](auto inp) -> POption<U> {
      auto res = p.next(inp);
      if (!res.has_value())
        return {};
      auto [rest, resu] = res.value();
      return std::tuple(rest, f(resu));
    });
  };
};

template <typename T> Parser<T> alt(std::vector<Parser<T>> alts) {
  return Parser<T>([alts](auto input) -> POption<T> {
    for (auto alt : alts) {
      auto res = alt.next(input);
      if (res.has_value())
        return res;
    }
    return {};
  });
}

Parser<char> verifies(std::function<bool(char)> f);
Parser<char> char_p(char c);
Parser<std::string> string_p(std::string s);
#endif // DA2324_PRJ1_G163_PARSER_H
