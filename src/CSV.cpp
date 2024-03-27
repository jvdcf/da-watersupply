#include "CSV.h"
#include "Parser.h"
#include <cctype>
#include <cstdint>
#include <exception>
#include <iostream>
#include <string>

Parser<CsvValues> parse_int() {
  return verifies(isdigit).take_while().recognize().pmap<CsvValues>(
      [](auto inp) {
        try {
          int64_t d = std::stoll(inp);
          return CsvValues::Int(d);
        } catch (std::exception &e) {
          std::cout << "Failed to parse int: " << inp << std::endl;
          return CsvValues::Nil();
        }
      });
}

Parser<CsvValues> parse_flt() {
  auto fst = verifies(isdigit).take_while();
  auto final =
      fst.pair(char_p('.')).pair(fst).recognize().pmap<CsvValues>([](auto inp) {
        try {
          double d = std::stod(inp);
          return CsvValues::Flt(d);
        } catch (std::exception &e) {
          std::cout << "Failed to parse flt: " << inp << std::endl;
          return CsvValues::Nil();
        }
      });
  return final;
}

Parser<CsvValues> parse_str() {
  auto fst = verifies(isalpha);
  auto snd =
      verifies([](auto c) { return c != ',' && c != '\n'; }).take_while();
  auto final = fst.pair(snd).recognize().pmap<CsvValues>(
      [](auto inp) { return CsvValues::Str(inp); });
  return final;
}

Parser<CsvValues> parse_weird() {
  auto parse_quot = char_p('"');
  auto parse_nums = verifies(isdigit).take_while();
  auto parse_comm = char_p(',');
  return parse_quot.pair(parse_nums)
      .pair(parse_comm)
      .pair(parse_nums)
      .pair(parse_quot)
      .pmap<CsvValues>([](auto p) {
        auto [rest0, qu] = p;
        auto [rest1, num2] = rest0;
        auto [rest2, comm] = rest1;
        auto [rest3, num1] = rest2;
        std::string s1(num1.begin(), num1.end());
        std::string s2(num2.begin(), num2.end());
        try {
          int64_t val = std::stoll(s1 + s2);
          return CsvValues::Int(val);
        } catch (std::exception &e) {
          std::cout << "Failed to parse weird: " << s1 + s2 << std::endl;
          return CsvValues::Nil();
        }
      });
}

Parser<CsvLine> parse_line() {
  auto parse_BOM = string_p("\xEF\xBB\xBF").pmap<CsvValues>([](auto c) {
    return CsvValues::Sep();
  });
  auto parse_value = alt(std::vector<Parser<CsvValues>>(
      {parse_BOM, parse_flt(), parse_int(), parse_weird(), parse_str()}));
  auto parse_sep =
      char_p(',').pmap<CsvValues>([](auto p) { return CsvValues::Sep(); });
  return alt(std::vector<Parser<CsvValues>>({parse_sep, parse_value}))
      .take_while()
      .ends_with_fst(alt(std::vector(
          {string_p("\r\n"), string_p("\n\r"),
           string_p("\n"), string_p("\r")}))) // Windows, RISCOS, Unix, Legacy MacOs
      .pmap<CsvLine>([](auto p) {
        CsvLine res;
        for (CsvValues r : p) {
          if (r.variant == CsvValues::None)
            return CsvLine();
          if (r.variant == CsvValues::Separator)
            continue;
          res.add_val(r);
        }
        return res;
      });
}

Parser<Csv> parse_csv() {
  return parse_line().pair(parse_line().take_while()).pmap<Csv>([](auto p) {
    auto [head, data] = p;
    return Csv(head, data);
  });
}
