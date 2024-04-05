#ifndef CSV_H
#define CSV_H 
#include "Parser.h"
#include <cstddef>
#include <cstdint>
#include <variant>
#include <vector>
#include <string>


/*
  * @brief This class represents the possible values in a CSV file.
  */
class CsvValues {
private:
  /// Type-safe union of possible values.
  std::variant<std::string, int64_t, double, std::nullptr_t> value;
public:
  /// Type that indicates which variant the class holds.
  enum var {
    String,
    Integer,
    Float,
    None,
    Separator,
  } variant;
  /// Constructor
  CsvValues(): value(nullptr), variant(None) {};
  

  std::optional<std::string> get_str();
  std::optional<int64_t> get_int();
  std::optional<double> get_flt();
  /// Helper function to display the contents of the CsvValue
  std::string display() {
    switch (variant) {
    case String:
      return "Str(" + std::get<std::string>(this->value) + ")"; 
    case Integer:
      return "Int(" + std::to_string(std::get<int64_t>(this->value)) + ")";
    case Float:
      return "Float(" + std::to_string(std::get<double>(this->value)) + ")";
    case None:
      return "Null";
    case Separator:
      return "Separator";
    }
    return "";
  }
  /// Static constructor for Int
  static CsvValues Int(int64_t i) {
    CsvValues r = CsvValues();
    r.variant = Integer;
    r.value = i;
    return r;
  }
  /// Static constructor for Str
  static CsvValues Str(std::string i) {
    CsvValues r = CsvValues();
    r.variant = String;
    r.value = i;
    return r;
  }
  /// Static constructor for Flt
  static CsvValues Flt(double i) {
    CsvValues r = CsvValues();
    r.variant = Float;
    r.value = i;
    return r;
  }
  /// Static constructor for Nil
  static CsvValues Nil() {
    CsvValues r = CsvValues();
    r.variant = None;
    r.value = nullptr;
    return r;
  }
  /// Static constructor for Sep
  static CsvValues Sep() {
    CsvValues r = CsvValues();
    r.variant = Separator;
    r.value = nullptr;
    return r;
  }
};

/*
  * @brief This class represents a line in a CSV file.
  */
struct CsvLine {
private:
  /// The line
  std::vector<CsvValues> line;
  /// Adds an item to the line
  public:
  void add_val(CsvValues v) {
    line.push_back(v);
  }
  /// Exposes the underlying data in the line
  const std::vector<CsvValues>& get_data() {
    return line;
  }
  /// Displays the contents of the line
  std::string display() {
    std::string res;
    for (auto v : line) {
      res += v.display() + ", ";
    }
    return res;
  }
};


/*
  * @brief Represents a CSV file that does not contain partially filled columns.
  */
class Csv {
private:
  /// The Header containing the title of each column
  CsvLine header;
  /// The actual data
  std::vector<CsvLine> data;
public:
  /// Default Constructor
  Csv(): header(CsvLine()), data(std::vector<CsvLine>()) {};
  /// Constructor 
  Csv(CsvLine head, std::vector<CsvLine> dat): header(head), data(dat) {};
  /// Returns the underlying data consuming the CSV.
  std::vector<CsvLine> to_data() {
    return std::move(data);
  }
  /// Displays the csv file
  std::string display() {
    std::string res;
    res += "Header: " + header.display() + "\n";
    for (int i = 0; i < data.size(); ++i) {
      res += "col: " + std::to_string(i) + ": " + data[i].display() + "\n"; 
    }
    return res;
  }
};
/*
  * @brief Parses a CsvLine.
  * @return Parser<CsvLine>
  * @note when called: O(1). 
  */
Parser<CsvLine> parse_line();

/*
  * @brief Parses a Csv file.
  * @return Parser<Csv>
  * @note when called: O(1). 
  */ 
Parser<Csv> parse_csv();

/*
  * @brief Parses a floating point number.
  * @return Parser<CsvValues>
  * @note when called: O(1). 
  */
Parser<CsvValues> parse_flt();

/*
  * @brief Parses a symbol in a Csv.
  * @return Parser<CsvValues>
  * @note when called: O(1). 
  */
Parser<CsvValues> parse_str();

/*
  * @brief Parses an integer.
  * @return Parser<CsvValues>
  * @note when called: O(1). 
  */
Parser<CsvValues> parse_int();

/*
  * @brief Parses the weird notation in one of the given files.
  * @return Parser<CsvValues>
  * @note when called: O(1). 
  */
Parser<CsvValues> parse_weird();



#endif // CSV_H
