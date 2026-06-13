#include "../TempDirectory.hpp"
#include "../TempFile.hpp"
#include <type_traits>
#include <gtest/gtest.h>

TEST(TestTraits, DirectoryIsNotCopy) {
    ASSERT_FALSE(std::is_copy_constructible<tempfs::TempDirectory>());
    ASSERT_FALSE(std::is_copy_assignable<tempfs::TempDirectory>());
}

TEST(TestTraits, DirectoryIsNotMove) {
    ASSERT_FALSE(std::is_move_constructible<tempfs::TempDirectory>());
    ASSERT_FALSE(std::is_move_assignable<tempfs::TempDirectory>());
}

TEST(TestTraits, FileIsNotCopy) {
    ASSERT_FALSE(std::is_copy_constructible<tempfs::TempFile>());
    ASSERT_FALSE(std::is_copy_assignable<tempfs::TempFile>());
}

TEST(TestTraits, FileIsMoveButNotMoveAssign) {
    ASSERT_TRUE(std::is_move_constructible<tempfs::TempFile>());
    ASSERT_FALSE(std::is_move_assignable<tempfs::TempFile>());
}