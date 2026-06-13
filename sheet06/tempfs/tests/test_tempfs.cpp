#include "../TempDirectory.hpp"
#include "../TempFile.hpp"
#include "test_utils.hpp"

using namespace tempfs;

void assertChildrenEq(std::initializer_list<std::string> expected, const TempDirectory &dir) {
    std::vector<std::string> expectedVec{expected};
    auto actual = dir.files();
    std::sort(actual.begin(), actual.end());
    std::sort(expectedVec.begin(), expectedVec.end());
    ASSERT_EQ(expectedVec, actual);
}
void assertChildrenEq(std::vector<std::string> &expected, const TempDirectory &dir) {
    auto actual = dir.files();
    std::sort(actual.begin(), actual.end());
    std::sort(expected.begin(), expected.end());
    ASSERT_EQ(expected, actual);
}

//---------------------------------------------------------------------------
TEST(TestTempfs, CreateEmptyDir) {
    CleanTestDirectory clean("/tmp/raii_test");
    DirectoryWatch watch("/tmp");
    {
        TempDirectory dir{"/tmp/raii_test"};

        ASSERT_TRUE(watch.expectDirectoryCreated("raii_test"));
        ASSERT_TRUE(dir.isEmpty());
        ASSERT_TRUE(dir.files().empty());
        ASSERT_EQ("/tmp/raii_test", dir.getPath());
    }

    ASSERT_TRUE(watch.expectDeleted("raii_test"));
}
//---------------------------------------------------------------------------
TEST(TestTempfs, CreateTwoFiles) {
    CleanTestDirectory clean("/tmp/raii_test");
    InotifyContext context;

    DirectoryWatch watch("/tmp");
    {
        TempDirectory dir{"/tmp/raii_test"};

        ASSERT_TRUE(watch.expectDirectoryCreated("raii_test"));
        ASSERT_TRUE(dir.isEmpty());
        ASSERT_TRUE(dir.files().empty());
        ASSERT_EQ("/tmp/raii_test", dir.getPath());
        DirectoryWatch innerWatch("/tmp/raii_test");

        {
            TempFile child1 = dir.createChildFile("file0.txt");

            ASSERT_TRUE(innerWatch.expectFileCreated("file0.txt"));
            ASSERT_FALSE(dir.isEmpty());
            assertChildrenEq({"file0.txt"}, dir);
            ASSERT_EQ("/tmp/raii_test/file0.txt", child1.getPath());
            ASSERT_EQ("file0.txt", child1.getName());

            {
                TempFile child2 = dir.createChildFile("file1.bin");

                ASSERT_TRUE(innerWatch.expectFileCreated("file1.bin"));
                ASSERT_FALSE(dir.isEmpty());
                ASSERT_EQ(2, dir.files().size());
                assertChildrenEq({"file0.txt", "file1.bin"}, dir);
                ASSERT_EQ("/tmp/raii_test/file1.bin", child2.getPath());
                ASSERT_EQ("file1.bin", child2.getName());
            }

            ASSERT_TRUE(innerWatch.expectDeleted("file1.bin"));
            ASSERT_FALSE(dir.isEmpty());
            assertChildrenEq({"file0.txt"}, dir);
        }

        ASSERT_TRUE(innerWatch.expectDeleted("file0.txt"));
        ASSERT_TRUE(dir.isEmpty());
        ASSERT_TRUE(dir.files().empty());
    }

    ASSERT_TRUE(watch.expectDeleted("raii_test"));
}
//---------------------------------------------------------------------------
TEST(TestTempfs, MultipleDirectories) {
    CleanTestDirectory clean1("/tmp/raii_test1");
    CleanTestDirectory clean2("/tmp/raii_test2");
    CleanTestDirectory clean3("/tmp/raii_test3");
    DirectoryWatch watch("/tmp");
    std::vector<std::string> expectedDir1;
    std::vector<std::string> expectedDir2;
    std::vector<std::string> expectedDir3;
    std::vector<DirectoryWatch> dirWatch;
    {
        TempDirectory dir1{"/tmp/raii_test1"};
        ASSERT_TRUE(watch.expectDirectoryCreated("raii_test1"));
        TempDirectory dir2{"/tmp/raii_test2"};
        ASSERT_TRUE(watch.expectDirectoryCreated("raii_test2"));
        TempDirectory dir3{"/tmp/raii_test3"};
        ASSERT_TRUE(watch.expectDirectoryCreated("raii_test3"));

        ASSERT_TRUE(dir1.isEmpty());
        ASSERT_TRUE(dir2.isEmpty());
        ASSERT_TRUE(dir3.isEmpty());

        dirWatch.emplace_back("/tmp/raii_test1");
        dirWatch.emplace_back("/tmp/raii_test2");
        dirWatch.emplace_back("/tmp/raii_test3");

        std::vector<TempFile> filesDir1;
        std::vector<TempFile> filesDir2;
        std::vector<TempFile> filesDir3;

        for (unsigned i = 0; i < 10; i += 1) {
            std::string name = std::to_string(i);
            filesDir1.push_back(dir1.createChildFile(name));
            expectedDir1.push_back(name);

            ASSERT_TRUE(dirWatch[0].expectFileCreated(name));
        }
        assertChildrenEq(expectedDir1, dir1);

        auto delete1At = [&](unsigned i) {
            { TempFile trash{std::move(filesDir1[i])}; }
            expectedDir1.erase(std::find(expectedDir1.begin(), expectedDir1.end(), std::to_string(i)));
            ASSERT_TRUE(dirWatch[0].expectDeleted(std::to_string(i)));
            assertChildrenEq(expectedDir1, dir1);
        };

        delete1At(1);
        delete1At(7);
        delete1At(3);
        delete1At(4);
        delete1At(9);
        delete1At(0);

        for (unsigned i = 0; i < 5; i += 1) {
            std::string name = std::to_string(i);
            filesDir2.push_back(dir2.createChildFile(name));
            expectedDir2.push_back(name);

            ASSERT_TRUE(dirWatch[1].expectFileCreated(name));
        }
        assertChildrenEq(expectedDir1, dir1);
        assertChildrenEq(expectedDir2, dir2);

        for (unsigned i = 0; i < 7; i += 1) {
            std::string name = std::to_string(i);
            filesDir3.push_back(dir3.createChildFile(name));
            expectedDir3.push_back(name);

            ASSERT_TRUE(dirWatch[2].expectFileCreated(name));
        }
        assertChildrenEq(expectedDir1, dir1);
        assertChildrenEq(expectedDir2, dir2);
        assertChildrenEq(expectedDir3, dir3);

        auto delete2At = [&](unsigned i) {
            { TempFile trash{std::move(filesDir2[i])}; }
            expectedDir2.erase(std::find(expectedDir2.begin(), expectedDir2.end(), std::to_string(i)));
            ASSERT_TRUE(dirWatch[1].expectDeleted(std::to_string(i)));
            assertChildrenEq(expectedDir1, dir1);
            assertChildrenEq(expectedDir2, dir2);
            assertChildrenEq(expectedDir3, dir3);
        };
        auto delete3At = [&](unsigned i) {
            { TempFile trash{std::move(filesDir3[i])}; }
            expectedDir3.erase(std::find(expectedDir3.begin(), expectedDir3.end(), std::to_string(i)));
            ASSERT_TRUE(dirWatch[2].expectDeleted(std::to_string(i)));
            assertChildrenEq(expectedDir1, dir1);
            assertChildrenEq(expectedDir2, dir2);
            assertChildrenEq(expectedDir3, dir3);
        };

        delete2At(0);
        delete2At(2);
        delete3At(2);
        delete2At(4);
        delete3At(3);
        delete3At(4);
    }

    ASSERT_TRUE(dirWatch[0].expectDeleted(expectedDir1));
    ASSERT_TRUE(dirWatch[1].expectDeleted(expectedDir2));
    ASSERT_TRUE(dirWatch[2].expectDeleted(expectedDir3));
}
//---------------------------------------------------------------------------
TEST(TestTempfs, DeleteNonEmptyDirectory) {
    CleanTestDirectory clean("/tmp/raii_test");
    std::unique_ptr<TempDirectory> td;
    std::vector<TempFile> fs;
    DirectoryWatch watch("/tmp");

    {
        td = std::make_unique<TempDirectory>("/tmp/raii_test");
        ASSERT_TRUE(watch.expectDirectoryCreated("raii_test"));

        DirectoryWatch innerWatch("/tmp/raii_test");
        fs.push_back(td->createChildFile("file.txt"));
        ASSERT_TRUE(innerWatch.expectFileCreated("file.txt"));
    }

    ASSERT_DEATH(td = nullptr, "tempfs::TempDirectory::~TempDirectory\\(\\): Assertion .* failed");
}
//---------------------------------------------------------------------------

int main(int argc, char *argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}