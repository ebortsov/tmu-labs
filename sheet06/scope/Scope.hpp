#pragma once
#include "Stopwatch.hpp"
#include "log.hpp"
#include <string>

namespace scope {
/// Scoped logger.
class Scope {
public:
    /// Create a new anonymous scope writing to the given LogStream at Info level.
    /// @param logStream LogStream to write to. Has to outlive the scope.
    explicit Scope(LogStream &logStream);
    /// Create a new anonymous scope writing to the given LogStream at a given LogLevel.
    /// @param logLevel LogLevel of messages of this scope.
    /// @param logStream LogStream to write to. Has to outlive the scope.
    Scope(LogLevel logLevel, LogStream &logStream);
    /// Create a new named scope writing to the given LogStream
    /// @param name Name of the scope.
    /// @param logStream LogStream to write to. Has to outlive the scope.
    Scope(std::string name, LogStream &logStream);
    /// Create a new named scope writing to the given LogStream at a given LogLevel.
    /// @param logLevel LogLevel of messages of this scope.
    /// @param name Name of the scope.
    /// @param logStream LogStream to write to. Has to outlive the scope.
    Scope(LogLevel logLevel, std::string name, LogStream &logStream);

    /*
        PUT COPY/MOVE CONSTRUCTORS/ASSIGNMENT HERE
    */

    /// Close the scope.
    ~Scope();

    /// Log a message to the underlying LogStream.
    /// @param message Message string.
    void log(std::string message);

    /// Reset the IDs of the scopes so that the next created Scope will have ID 1.
    static void resetIds();

private:
};
} // namespace scope