#pragma once
#include <compare>
#include <cstdint>

namespace scope {
/// Duration between two timestamps measured in nanoseconds.
class Duration {
public:
    /// Create a new Duration representing the given number of nanoseconds.
    /// @param nanos Nanosecond count.
    explicit Duration(uint64_t nanos);

    /// Duration as a rounded number of seconds.
    ///
    /// Following standard mathematical rounding, e.g. 1.5s is 2, 1.49s is 1.
    /// @return Number of seconds rounded to the nearest integer.
    uint64_t seconds() const;
    /// Duration as the number of fully passed seconds.
    ///
    /// Compared to `seconds()` this function always rounds down.
    /// @return Number of full seconds (rounded down).
    uint64_t fullSeconds() const;
    /// Duration as a rounded number of milliseconds.
    ///
    /// Following standard mathematical rounding, e.g. 300.5ms is 301, 300.49 is 300.
    /// @return Number of milliseconds rounded to the nearest integer.
    uint64_t milliseconds() const;
    /// Duration as the number of fully passed milliseconds.
    ///
    /// Compared to `milliseconds()` this function always rounds down.
    /// @return Number of full milliseconds (rounded down).
    uint64_t fullMilliseconds() const;
    /// Duration as the number of nanoseconds.
    /// @return Number of nanoseconds.
    uint64_t nanoseconds() const;

    /// Sum two Durations.
    /// @param other Duration to add.
    /// @return Sum of this Duration and the other Duration.
    Duration operator+(const Duration &other) const;
    /// Add-and-assign another Duration.
    /// @param other Duration to add to this.
    /// @return A reference to this.
    Duration &operator+=(const Duration &other);

    /// Compare another Duration for equality.
    /// @param other Duration to compare to.
    /// @return True if the Durations are equal; false otherwise.
    bool operator==(const Duration &other) const;
    /// Three-way-compare to another Duration.
    /// @param other Duration to compare to.
    /// @return The strong_ordering result of three-way-comparison of this Duration to other.
    std::strong_ordering operator<=>(const Duration &other) const;

private:
};

/// Stopwatch tuned to the global `get_clock_time()` function.
class Stopwatch {
public:
    /// Create a new Stopwatch and immediately start it.
    Stopwatch();

    /// Start the Stopwatch.
    ///
    /// Does nothing if already running.
    void start();
    /// Stop the Stopwatch.
    ///
    /// Does nothing if already stopped.
    void stop();
    /// Restart the Stopwatch.
    ///
    /// Zeroes the timer and starts the Stopwatch again.
    void restart();
    /// Check if the Stopwatch is currently running.
    /// @return True if Stopwatch is started, false if stopped.
    bool isRunning() const;
    /// Currently elapsed time.
    /// @return Duration in which this Stopwatch was running since first started or restarted.
    Duration elapsed() const;

private:
};
} // namespace scope