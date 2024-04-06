#include "Parser.h"
#include <cctype>
#include <optional>
#include <tuple>

Parser<char> char_p(char c) {
  return Parser<char>([c](std::string s) -> POption<char> {
    if (s.size() != 0) {
      if (c == s[0]) {
        if (s.size() > 1) {
          return std::tuple(s.substr(1), c);
        } else {
          return std::tuple(std::string(""), c);
        }
      }
    }
    return {};
  });
}
Parser<std::string> string_p(std::string s) {
  std::vector<Parser<char>> pp;
  for (auto c : s) {
    pp.push_back(char_p(c));
  }
  return Parser<std::string>([s, pp](auto other) -> POption<std::string> {
    std::string o = other;
    for (auto p : pp) {
      auto resu = p(o);
      if (!resu.has_value()) {
        return {};
      } else {
        auto [in, out] = resu.value();
        o = in;
      }
    }
    return std::tuple(o, s);
  });
}

Parser<char> verifies(std::function<bool(char)> f) {
  return Parser<char>([f](auto input) -> POption<char> {
    if (input.size() == 0) return {};
    char fst = input[0];
    if (f(fst)) {
      return input.size() == 1 ? std::tuple("",fst) : std::tuple(input.substr(1), fst);
    }
    return {};
  });
}

Parser<std::string> ws() {
  return alt(std::vector({verifies(isspace).take_while().recognize(), null<std::string>()}));
}

