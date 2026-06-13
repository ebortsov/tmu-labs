#include "../Scope.hpp"
#include <format>
#include <gtest/gtest.h>

namespace {
uint64_t currentTick = 0;

void tickMs() { currentTick += 1'000'000; }

void setTick(uint64_t value) { currentTick = value; }

void addTicks(uint64_t value) { currentTick += value; }

uint64_t defaultClockImpl() {
    uint64_t result = currentTick;
    return result;
}

std::function clockImpl = defaultClockImpl;
} // namespace

uint64_t get_clock_time() { return clockImpl(); }

using namespace scope;

TEST(TestDuration, RoundTicks) {
    Duration d{1'100'000'000};

    ASSERT_EQ(1, d.seconds());
    ASSERT_EQ(1, d.fullSeconds());
    ASSERT_EQ(1'100, d.milliseconds());
    ASSERT_EQ(1'100, d.fullMilliseconds());
    ASSERT_EQ(1'100'000'000, d.nanoseconds());
}
//---------------------------------------------------------------------------
TEST(TestDuration, SecondsRoundingUp) {
    Duration d{3'500'000'000};

    ASSERT_EQ(4, d.seconds());
    ASSERT_EQ(3, d.fullSeconds());
    ASSERT_EQ(3'500, d.milliseconds());
    ASSERT_EQ(3'500, d.fullMilliseconds());
    ASSERT_EQ(3'500'000'000, d.nanoseconds());
}
//---------------------------------------------------------------------------
TEST(TestDuration, SecondsRoundingDown) {
    Duration d{3'499'999'999};

    ASSERT_EQ(3, d.seconds());
    ASSERT_EQ(3, d.fullSeconds());
    ASSERT_EQ(3'500, d.milliseconds());
    ASSERT_EQ(3'499, d.fullMilliseconds());
    ASSERT_EQ(3'499'999'999, d.nanoseconds());
}
//---------------------------------------------------------------------------
TEST(TestDuration, MillisecondsRoundingUp) {
    Duration d{3'100'500'000};

    ASSERT_EQ(3, d.seconds());
    ASSERT_EQ(3, d.fullSeconds());
    ASSERT_EQ(3'101, d.milliseconds());
    ASSERT_EQ(3'100, d.fullMilliseconds());
    ASSERT_EQ(3'100'500'000, d.nanoseconds());
}
//---------------------------------------------------------------------------
TEST(TestDuration, MillisecondsRoundingDown) {
    Duration d{3'100'499'999};

    ASSERT_EQ(3, d.seconds());
    ASSERT_EQ(3, d.fullSeconds());
    ASSERT_EQ(3'100, d.milliseconds());
    ASSERT_EQ(3'100, d.fullMilliseconds());
    ASSERT_EQ(3'100'499'999, d.nanoseconds());
}
//---------------------------------------------------------------------------
TEST(TestStopwatch, SingleRun) {
    setTick(1'000'000'000'000);
    Stopwatch stopwatch{};
    addTicks(2'137);
    Duration d = stopwatch.elapsed();

    ASSERT_TRUE(stopwatch.isRunning());
    ASSERT_EQ(2'137, d.nanoseconds());
}
//---------------------------------------------------------------------------
TEST(TestStopwatch, StopStart) {
    setTick(1'000'000'000'000);
    Stopwatch stopwatch{};
    bool shouldBeRunning1 = stopwatch.isRunning();
    addTicks(2'137);
    stopwatch.stop();
    bool shouldBeStopped1 = stopwatch.isRunning();
    Duration d1 = stopwatch.elapsed();
    addTicks(5'000);
    Duration d2 = stopwatch.elapsed();
    stopwatch.start();
    bool shouldBeRunning2 = stopwatch.isRunning();
    addTicks(10'000);
    Duration d3 = stopwatch.elapsed();
    stopwatch.stop();
    bool shouldBeStopped2 = stopwatch.isRunning();
    addTicks(10'000);
    Duration d4 = stopwatch.elapsed();

    ASSERT_TRUE(shouldBeRunning1);
    ASSERT_FALSE(shouldBeStopped1);
    ASSERT_TRUE(shouldBeRunning2);
    ASSERT_FALSE(shouldBeStopped2);

    ASSERT_EQ(2'137, d1.nanoseconds());
    ASSERT_EQ(2'137, d2.nanoseconds());
    ASSERT_EQ(12'137, d3.nanoseconds());
    ASSERT_EQ(12'137, d4.nanoseconds());
}
//---------------------------------------------------------------------------
TEST(TestStopwatch, Restart) {
    setTick(1'000'000'000'000);
    Stopwatch stopwatch{};
    addTicks(2'137);
    Duration d1 = stopwatch.elapsed();
    stopwatch.restart();
    addTicks(10'000);
    Duration d2 = stopwatch.elapsed();

    ASSERT_TRUE(stopwatch.isRunning());
    ASSERT_EQ(2'137, d1.nanoseconds());
    ASSERT_EQ(10'000, d2.nanoseconds());
}
//---------------------------------------------------------------------------
TEST(TestScope, ReadmeExample) {
    setTick(0);
    Scope::resetIds();
    LogStream logs{};
    {
        Scope sc1{"process", logs};
        bool needsBlock2;
        {
            Scope sc2{"block1", logs};
            needsBlock2 = true;
            addTicks(500'000'000);
            sc2.log(std::format("Result of calculate is {}", needsBlock2));
        }
        sc1.log("Continuing...");
        if (needsBlock2) {
            Scope sc3{"block2", logs};
            sc3.log("Starting computation...");
            addTicks(2'000'000'000);
            sc3.log(std::format("Final result is {}", 42));
        }
    }
    LogVec expected{"[Info] Enter scope process (#1)",
                    "[Info] Enter scope block1 (#2)",
                    "[Info] block1 (#2) [+0.500s] Result of calculate is true",
                    "[Info] Exit block1 (#2), elapsed 0.500s",
                    "[Info] process (#1) [+0.500s] Continuing...",
                    "[Info] Enter scope block2 (#3)",
                    "[Info] block2 (#3) [+0.000s] Starting computation...",
                    "[Info] block2 (#3) [+2.000s] Final result is 42",
                    "[Info] Exit block2 (#3), elapsed 2.000s",
                    "[Info] Exit process (#1), elapsed 2.500s"};

    LogVec actual = LogStream::extractLogs(std::move(logs));

    ASSERT_EQ(expected, actual);
}
//---------------------------------------------------------------------------
TEST(TestScope, BasicUsage) {
    setTick(1'000'000'000'000);
    Scope::resetIds();
    LogStream logs{LogLevel::Debug};
    {
        Scope scope{logs};
        tickMs();
        scope.log("Start processing");
        {
            tickMs();
            Scope inner{LogLevel::Debug, "loop", logs};
            tickMs();
            for (int i = 0; i < 8; i += 1) {
                inner.log(std::format("iter {}", i));
                tickMs();
            }
        }
        tickMs();
    }

    LogVec expected = {"[Info] Enter anonymous scope (#1)",
                       "[Info] (#1) [+0.001s] Start processing",
                       "[Debug] Enter scope loop (#2)",
                       "[Debug] loop (#2) [+0.001s] iter 0",
                       "[Debug] loop (#2) [+0.002s] iter 1",
                       "[Debug] loop (#2) [+0.003s] iter 2",
                       "[Debug] loop (#2) [+0.004s] iter 3",
                       "[Debug] loop (#2) [+0.005s] iter 4",
                       "[Debug] loop (#2) [+0.006s] iter 5",
                       "[Debug] loop (#2) [+0.007s] iter 6",
                       "[Debug] loop (#2) [+0.008s] iter 7",
                       "[Debug] Exit loop (#2), elapsed 0.009s",
                       "[Info] Exit (#1), elapsed 0.012s"};
    LogVec actual = LogStream::extractLogs(std::move(logs));

    ASSERT_EQ(expected, actual);
}
//---------------------------------------------------------------------------
TEST(TestScope, DisabledLogLevel) {
    setTick(1'000'000'000'000);
    Scope::resetIds();
    LogStream logs{};
    {
        Scope scope{logs};
        tickMs();
        scope.log("Start processing");
        {
            tickMs();
            Scope inner{LogLevel::Debug, "loop", logs};
            tickMs();
            for (int i = 0; i < 8; i += 1) {
                inner.log(std::format("iter {}", i));
                tickMs();
            }
        }
        tickMs();
    }

    LogVec expected = {"[Info] Enter anonymous scope (#1)",
                       "[Info] (#1) [+0.001s] Start processing",
                       "[Info] Exit (#1), elapsed 0.012s"};
    LogVec actual = LogStream::extractLogs(std::move(logs));

    ASSERT_EQ(expected, actual);
}
//---------------------------------------------------------------------------
TEST(TestScope, MixedScopeLogs) {
    setTick(1'000'000'000'000);
    Scope::resetIds();
    LogStream logs{LogLevel::Debug};
    {
        Scope scope{LogLevel::Warning, logs};
        tickMs();
        scope.log("Start processing");
        {
            tickMs();
            Scope inner{"loop", logs};
            tickMs();
            for (int i = 0; i < 8; i += 1) {
                Scope innerMost{LogLevel::Debug, std::format("iter {}", i), logs};
                tickMs();
                scope.log("abc");
                inner.log("def");
                innerMost.log("ghi");
                tickMs();
            }
        }
        tickMs();
    }

    LogVec expected = {"[Warning] Enter anonymous scope (#1)",
                       "[Warning] (#1) [+0.001s] Start processing",
                       "[Info] Enter scope loop (#2)",
                       "[Debug] Enter scope iter 0 (#3)",
                       "[Warning] (#1) [+0.004s] abc",
                       "[Info] loop (#2) [+0.002s] def",
                       "[Debug] iter 0 (#3) [+0.001s] ghi",
                       "[Debug] Exit iter 0 (#3), elapsed 0.002s",
                       "[Debug] Enter scope iter 1 (#4)",
                       "[Warning] (#1) [+0.006s] abc",
                       "[Info] loop (#2) [+0.004s] def",
                       "[Debug] iter 1 (#4) [+0.001s] ghi",
                       "[Debug] Exit iter 1 (#4), elapsed 0.002s",
                       "[Debug] Enter scope iter 2 (#5)",
                       "[Warning] (#1) [+0.008s] abc",
                       "[Info] loop (#2) [+0.006s] def",
                       "[Debug] iter 2 (#5) [+0.001s] ghi",
                       "[Debug] Exit iter 2 (#5), elapsed 0.002s",
                       "[Debug] Enter scope iter 3 (#6)",
                       "[Warning] (#1) [+0.010s] abc",
                       "[Info] loop (#2) [+0.008s] def",
                       "[Debug] iter 3 (#6) [+0.001s] ghi",
                       "[Debug] Exit iter 3 (#6), elapsed 0.002s",
                       "[Debug] Enter scope iter 4 (#7)",
                       "[Warning] (#1) [+0.012s] abc",
                       "[Info] loop (#2) [+0.010s] def",
                       "[Debug] iter 4 (#7) [+0.001s] ghi",
                       "[Debug] Exit iter 4 (#7), elapsed 0.002s",
                       "[Debug] Enter scope iter 5 (#8)",
                       "[Warning] (#1) [+0.014s] abc",
                       "[Info] loop (#2) [+0.012s] def",
                       "[Debug] iter 5 (#8) [+0.001s] ghi",
                       "[Debug] Exit iter 5 (#8), elapsed 0.002s",
                       "[Debug] Enter scope iter 6 (#9)",
                       "[Warning] (#1) [+0.016s] abc",
                       "[Info] loop (#2) [+0.014s] def",
                       "[Debug] iter 6 (#9) [+0.001s] ghi",
                       "[Debug] Exit iter 6 (#9), elapsed 0.002s",
                       "[Debug] Enter scope iter 7 (#10)",
                       "[Warning] (#1) [+0.018s] abc",
                       "[Info] loop (#2) [+0.016s] def",
                       "[Debug] iter 7 (#10) [+0.001s] ghi",
                       "[Debug] Exit iter 7 (#10), elapsed 0.002s",
                       "[Info] Exit loop (#2), elapsed 0.017s",
                       "[Warning] Exit (#1), elapsed 0.020s"};
    LogVec actual = LogStream::extractLogs(std::move(logs));

    ASSERT_EQ(expected, actual);
}
//---------------------------------------------------------------------------
TEST(TestScope, ThatSpecificVolatilityBug) {
    struct Guard {
        ~Guard() { clockImpl = defaultClockImpl; }
    };
    setTick(0);
    Guard g{};
    LogStream logs{};
    Scope::resetIds();
    {
        // This should start the stopwatch at tick 0.
        Scope scope{"foo", logs};
        setTick(1'499'000'000);
        clockImpl = [] {
            uint64_t result = currentTick;
            currentTick = 2'499'999'999;
            return result;
        };
        // The correct timestamp for this message is either +1.499 or +2.500.s
        scope.log("msg");
        clockImpl = defaultClockImpl;
    }

    LogVec actual = LogStream::extractLogs(std::move(logs));

    ASSERT_EQ(3, actual.size());
    ASSERT_EQ("[Info] Enter scope foo (#1)", actual[0]);
    bool eq1499 = "[Info] foo (#1) [+1.499s] msg" == actual[1];
    bool eq2500 = "[Info] foo (#1) [+2.500s] msg" == actual[1];
    ASSERT_TRUE(eq1499 || eq2500) << "actual message " << actual[1];
    ASSERT_EQ("[Info] Exit foo (#1), elapsed 2.500s", actual[2]);
}
//---------------------------------------------------------------------------

int main(int argc, char *argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}