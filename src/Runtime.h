#ifndef DA2324_PRJ1_G163_RUNTIME_H
#define DA2324_PRJ1_G163_RUNTIME_H

#include "Parser.h"
#include "Utils.h"
#include "data/Data.h"
#include <array>
#include <cctype>
#include <cstdint>
#include <exception>
#include <optional>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

class CommandLineValue {
public:
  enum Kind {
    Command,
    Ident,
    Int,
    String,
    Sep,
    Code,
  };
  Kind kind;
  CommandLineValue(Kind k, auto v) : kind(k), value(v){};

  std::optional<std::string> getStr() {
    switch (this->kind) {
    case Ident:
      return std::get<std::string>(this->value);
    case Int:
      return {};
    case String:
      return std::get<std::string>(this->value);
    case Sep:
      return std::get<std::string>(this->value);
    case Command:
      return std::get<std::string>(this->value);
    case Code:
        {
          auto code = this->getCode().value();
          return Utils::parseId(code.first, code.second);
        }
    }
  }
  std::optional<uint32_t> getInt() {
    switch (this->kind) {
    case Command:
      return {};
    case Ident:
      return {};
    case Int:
      return std::get<uint32_t>(this->value);
    case String:
      return {};
    case Sep:
      return {};
    case Code:
        {
          auto code = this->getCode().value();
          return code.second;
        }
      break;
    }
  }
  std::optional<std::pair<Info::Kind, uint32_t>> getCode() {
    switch (this->kind) {
    case Command:
      return {};
    case Ident:
      return {};
    case Int:
      return {};
    case String:
      return {};
    case Sep:
      return {};
    case Code:
      return std::get<std::pair<Info::Kind, uint32_t>>(this->value);
      break;
    }
  }

  static Parser<CommandLineValue> parse_ident() {
    return alt(std::vector({
                   string_p("reservoir"),
                   string_p("pipe"),
                   string_p("pump"),
               }))
        .pmap<CommandLineValue>([](auto inp) {
          return CommandLineValue(CommandLineValue::Kind::Ident, inp);
        });
  }

  static Parser<CommandLineValue> parse_int() {
    return verifies(isdigit).take_while().recognize().pmap<CommandLineValue>(
        [](auto inp) {
          return CommandLineValue(Kind::Int, (uint32_t)std::stoul(inp));
        });
  }

  static Parser<CommandLineValue> parse_str() {
    return verifies(isalnum).take_while().recognize().pmap<CommandLineValue>(
        [](auto inp) { return CommandLineValue(Kind::String, inp); });
  }

  static Parser<CommandLineValue> parse_code() {
    return alt(std::vector({string_p("C_"), string_p("R_"), string_p("PS_")}))
        .pair(verifies(isdigit).take_while())
        .recognize()
        .pmap<CommandLineValue>([](auto inp) {
          return CommandLineValue(Kind::Code, Utils::parseCode(inp));
        });
  }

  static Parser<CommandLineValue> parse_sep(char c) {
    return verifies([c](auto b) { return c == b; })
        .pmap<CommandLineValue>([](auto inp) {
          return CommandLineValue(Kind::Sep, std::to_string(inp));
        });
  }

private:
  std::variant<uint32_t, std::string, std::pair<Info::Kind, uint32_t>> value;
};

class Command {
public:
  enum Cmd {
    Help,
    Quit,
    Count,
    MaxFlowCity,
    RmReservoir,
    RmPipe,
    RmPump,
    NeedsMet,
  } command;
  std::vector<CommandLineValue> args;
  Command(Cmd typ, std::vector<CommandLineValue> args)
      : command(typ), args(args){};
};

/**
 * @brief Interface and input handling.
 * @details This class is responsible for reading the user input and calling the
 * appropriate functions.
 */
class Runtime {
private:
  /// Pointer to a Data object. Used to call their methods.
  Data *data;

  /**
   * @brief From a list of arguments, process them and call the appropriate
   * function.
   * @param arg: A vector of strings with the arguments. The first string is the
   * command.
   */
  void processArgs(std::string input);

public:
  /**
   * @brief Constructor
   * @param d: Pointer to a Data object.
   */
  explicit Runtime(Data *d);

  /**
   * @brief Main loop.
   * @details Reads the user input, filters it and calls Runtime::processArgs().
   */
  [[noreturn]] void run();

  static Parser<Command> parse_quit() {
    return ws().pair(string_p("quit")).pair(ws()).pmap<Command>([](auto inp) {
      return Command(Command::Quit, {});
    });
  }

  static Parser<Command> parse_help() {
    return ws().pair(string_p("help")).pair(ws()).pmap<Command>([](auto inp) {
      return Command(Command::Help, {});
    });
  }

  static Parser<Command> parse_count() {
    return ws().pair(string_p("count")).pair(ws()).pmap<Command>([](auto inp) {
      return Command(Command::Count, {});
    });
  }

  static Parser<Command> parse_needsMet() {
    return ws().pair(string_p("needsMet")).pair(ws()).pmap<Command>([](auto inp) {
      return Command(Command::NeedsMet, {});
    });
  }

  static Parser<Command> parse_maxflowcity() {
    auto sole = ws().pair(string_p("maxFlowCity"))
                    .pair(ws())
                    .pmap<Command>([](auto inp) {
                      return Command(Command::MaxFlowCity, {});
                    });
    auto with_args = ws().pair(string_p("maxFlowCity"))
                         .pair(ws())
                         .pair(CommandLineValue::parse_int())
                         .pmap<Command>([](auto inp) {
                            auto [_, intt] = inp;
                            return Command(Command::MaxFlowCity, {intt});
                         });
    return alt(std::vector({with_args, sole}));
  }

  static Parser<Command> parse_rm() {
    auto reservoir = ws().pair(string_p("rm"))
                         .pair(ws())
                         .pair(string_p("reservoir"))
                         .pair(ws())
                         .pair(CommandLineValue::parse_int())
                         .pmap<Command>([](auto inp) { 
                            auto [rest, value] = inp;
                            return Command(Command::RmReservoir, {value});
                         });
    auto reservoir_sole = ws().pair(string_p("rm"))
                    .pair(ws())
                    .pair(string_p("reservoir"))
                    .pair(ws())
                    .pmap<Command>([](auto inp) {
                      return Command(Command::RmReservoir, {});
                    });
    auto pump = ws().pair(string_p("rm"))
                    .pair(ws())
                    .pair(string_p("pump"))
                    .pair(ws())
                    .pair(CommandLineValue::parse_int())
                    .pmap<Command>([](auto inp) {
                      auto [rest, value] = inp;
                      return Command(Command::RmPump, {value});
                    });
    auto pump_sole = ws().pair(string_p("rm"))
                    .pair(ws())
                    .pair(string_p("pump"))
                    .pair(ws())
                    .pmap<Command>([](auto inp) {
                      return Command(Command::RmPump, {});
                    });
 
    auto pipe = ws().pair(string_p("rm"))
                    .pair(ws())
                    .pair(string_p("pipe"))
                    .pair(ws())
                    .pair(CommandLineValue::parse_code())
                    .pair(ws())
                    .pair(CommandLineValue::parse_code())
                    .pmap<Command>([](auto inp) {
                      auto [r1, snd] = inp;
                      auto [r2, _] = r1;
                      auto [__, fst] = r2;
                      return Command(Command::RmPipe, {fst, snd});
                    });
    auto pipe_sole = ws().pair(string_p("rm"))
                    .pair(ws())
                    .pair(string_p("pipe"))
                    .pair(ws())
                    .pmap<Command>([](auto inp) {
                      return Command(Command::RmPipe, {});
                    });

    return alt(std::vector({reservoir, reservoir_sole, pump, pump_sole, pipe, pipe_sole}));
  }

  static Parser<Command> parse_cmd() {
    return alt(std::vector({
      parse_help(),
      parse_quit(),
      parse_count(),
      parse_needsMet(),
      parse_maxflowcity(),
      parse_rm(),
    }));
  }

  void printHelp();
  void handleQuit();
  void handleCount();
  void handleNeedsMet();
  void handleMaxFlowCity(std::vector<CommandLineValue> args);
  void handleRmReservoir(std::vector<CommandLineValue> args);
  void handleRmPump(std::vector<CommandLineValue> args);
  void handleRmPipe(std::vector<CommandLineValue> args);
};

#endif // DA2324_PRJ1_G163_RUNTIME_H
