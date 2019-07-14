#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"
#include <CLI/CLI.hpp>
#include <fmt/format.h>

bool parseLogLevel(std::vector<std::string> logLevelString,
                   spdlog::level::level_enum &logLevelResult) {
  std::map<std::string, spdlog::level::level_enum> levelMap{
      {"Critical", spdlog::level::critical}, {"Error", spdlog::level::err},
      {"Warning", spdlog::level::warn},      {"Info", spdlog::level::info},
      {"Debug", spdlog::level::debug},       {"Trace", spdlog::level::trace}};

  if (logLevelString.size() != 1) {
    return false;
  }

  try {
    logLevelResult = levelMap.at(logLevelString.at(0));
    return true;
  } catch (std::out_of_range &e) {
    // Do nothing, maybe user entered an integer
  }

  try {
    int TempLogMessageLevel = std::stoi(logLevelString.at(0));
    if (TempLogMessageLevel < 0 or TempLogMessageLevel > 5) {
      return false;
    }
    logLevelResult = spdlog::level::level_enum(TempLogMessageLevel);
  } catch (std::invalid_argument &e) {
    return false;
  }

  return true;
}

int main(int argc, char **argv) {
  std::string name = "World";
  spdlog::level::level_enum loggingLevel;

  CLI::App app{"Template project"};

  app.add_option("-n,--name", name, "Your name");
  std::string logLevelInfoStr =
      R"*(Set log message level. Set to 0 - 5 or one of
  `Trace`, `Debug`, `Info`, `Warning`, `Error`
  or `Critical`. Ex: "-v Debug". Default: `Info`)*";
  app.add_option(
      "-v,--verbosity",
      [&loggingLevel, logLevelInfoStr](std::vector<std::string> inputString) {
        return parseLogLevel(inputString, loggingLevel);
      },
      logLevelInfoStr, true);

  CLI11_PARSE(app, argc, argv)

  fmt::print("Hello {}", name);

  auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
  console_sink->set_level(loggingLevel);
  console_sink->set_pattern("[%H:%M:%S %z] [%n] [%^---%L---%$] [thread %t] %v");

  auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(
      "log.txt", true);
  file_sink->set_level(loggingLevel);

  spdlog::logger logger("logger", {console_sink, file_sink});
  logger.set_level(loggingLevel);

  logger.info("Log something");

  return 0;
}
