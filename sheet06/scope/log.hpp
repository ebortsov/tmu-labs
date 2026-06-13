#pragma once
#include <cstdint>
#include <string>
#include <vector>

namespace scope {
/// Alias for a complete list of logs.
using LogVec = std::vector<std::string>;

/// Log severity level.
enum LogLevel : uint8_t {
    /// The lowest level for debug messages.
    Debug,
    /// Default level for informational messages.
    Info,
    /// Recoverable issues that the user should know about.
    Warning,
    /// Irrecoverable errors.
    Error
};

/// Stream of logs that saves its messages in a LogVec in sequential order.
class LogStream {
public:
    /// Default constructor for LogLevel::Info.
    LogStream() : LogStream(LogLevel::Info) {}
    /// Creates a stream filtering all messages below a certain level.
    /// @param logLevel Minimum allowed level for log messages.
    explicit LogStream(LogLevel logLevel) : logLevel(logLevel) {}

    LogStream(LogStream &) = delete;
    LogStream(LogStream &&) = default;
    LogStream &operator=(LogStream &) = delete;
    LogStream &operator=(LogStream &&) = default;

    /// Consume a stream and extract all its messages.
    /// @param logStream LogStream to consume.
    /// @return LogVec of all messages from the stream.
    static LogVec extractLogs(LogStream logStream) { return logStream.logs; }

private:
    /// Minimum level of logs for filtering.
    LogLevel logLevel;
    /// Sequence of emitted log messages.
    LogVec logs;

    friend class Logger;
};

/// Static logger accessible only to the Scope.
class Logger {
    /// Log a message to the given stream.
    /// @param logStream LogStream to log into.
    /// @param logLevel LogLevel of the message.
    /// @param message Message string.
    static void log(LogStream &logStream, LogLevel logLevel, std::string message) {
        if (logLevel < logStream.logLevel) { return; }
        logStream.logs.push_back(std::move(message));
    }

    friend class Scope;
};
} // namespace scope