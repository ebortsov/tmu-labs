#include "../Scope.hpp"
#include <type_traits>
#include <gtest/gtest.h>

TEST(TestTraits, ScopeIsNotCopy) {
    ASSERT_FALSE(std::is_copy_constructible<scope::Scope>());
    ASSERT_FALSE(std::is_copy_assignable<scope::Scope>());
}

TEST(TestTraits, ScopeIsMoveButNotMoveAssignable) {
    ASSERT_TRUE(std::is_move_constructible<scope::Scope>());
    ASSERT_FALSE(std::is_move_assignable<scope::Scope>());
}

TEST(TestTraits, DurationIsTrivialCopy) {
    ASSERT_TRUE(std::is_trivially_copy_constructible<scope::Duration>());
    ASSERT_TRUE(std::is_trivially_copy_assignable<scope::Duration>());
}

TEST(TestTraits, DurationIsTrivialMove) {
    ASSERT_TRUE(std::is_trivially_move_constructible<scope::Duration>());
    ASSERT_TRUE(std::is_trivially_move_assignable<scope::Duration>());
}

TEST(TestTraits, StopwatchIsCopy) {
    ASSERT_TRUE(std::is_copy_constructible<scope::Stopwatch>());
    ASSERT_TRUE(std::is_copy_assignable<scope::Stopwatch>());
}

TEST(TestTraits, StopwatchIsMove) {
    ASSERT_TRUE(std::is_move_constructible<scope::Stopwatch>());
    ASSERT_TRUE(std::is_move_assignable<scope::Stopwatch>());
}