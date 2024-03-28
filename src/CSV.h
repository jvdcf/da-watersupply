#ifndef CSV_H
#define CSV_H 
#include "Parser.h"
#include <cstddef>
#include <cstdint>
#include <variant>
#include <vector>
#include <string>

class CsvValues {
private:
  std::variant<std::string, int64_t, double, std::nullptr_t> value;
public:
  enum var {
    String,
    Integer,
    Float,
    None,
    Separator,
  } variant;
  CsvValues(): value(nullptr), variant(None) {};
  std::string get_str();
  int64_t get_int();
  double get_flt();
  std::string display() {
    switch (variant) {
    case String:
      return "Str: " + std::get<std::string>(this->value); 
    case Integer:
      return "Int: " + std::to_string(std::get<int64_t>(this->value));
    case Float:
      return "Float: " + std::to_string(std::get<double>(this->value));
    case None:
      return "Null";
    case Separator:
      return "Separator";
    }
    return "";
  }
  static CsvValues Int(int64_t i) {
    CsvValues r = CsvValues();
    r.variant = Integer;
    r.value = i;
    return r;
  }
  static CsvValues Str(std::string i) {
    CsvValues r = CsvValues();
    r.variant = String;
    r.value = i;
    return r;
  }
  static CsvValues Flt(double i) {
    CsvValues r = CsvValues();
    r.variant = Float;
    r.value = i;
    return r;
  }
  static CsvValues Nil() {
    CsvValues r = CsvValues();
    r.variant = None;
    r.value = nullptr;
    return r;
  }
  static CsvValues Sep() {
    CsvValues r = CsvValues();
    r.variant = Separator;
    r.value = nullptr;
    return r;
  }
};


struct CsvLine {
  std::vector<CsvValues> line;
  void add_val(CsvValues v) {
    line.push_back(v);
  }
  public:
  const std::vector<CsvValues>& get_data() {
    return line;
  }
  std::string display() {
    std::string res;
    for (auto v : line) {
      res += v.display() + ", ";
    }
    return res;
  }
};
Parser<CsvLine> parse_line();

struct Csv {
  private:
  CsvLine header;
  std::vector<CsvLine> data;
  public:
  Csv(): header(CsvLine()), data(std::vector<CsvLine>()) {};
  Csv(CsvLine head, std::vector<CsvLine> dat): header(head), data(dat) {};
  std::vector<CsvLine> get_data() {
    return data;
  }
  std::string display() {
    std::string res;
    res += "Header: " + header.display() + "\n";
    for (int i = 0; i < data.size(); ++i) {
      res += "col: " + std::to_string(i) + ": " + data[i].display() + "\n"; 
    }
    return res;
  }
};

Parser<Csv> parse_csv();


Parser<CsvValues> parse_flt();
Parser<CsvValues> parse_str();
Parser<CsvValues> parse_int();
Parser<CsvValues> parse_weird();



#endif // CSV_H
