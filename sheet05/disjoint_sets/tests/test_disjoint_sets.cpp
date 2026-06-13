#include "../disjoint_sets.hpp"
#include <gtest/gtest.h>
//---------------------------------------------------------------------------
TEST(TestDisjointSets, initial) {
    disjoint_sets::DisjointSets disjointSets(16);

    for (unsigned i = 0; i < 16; ++i) { ASSERT_EQ(disjointSets.find(i), i); }
}
//---------------------------------------------------------------------------
TEST(TestDisjointSets, add) {
    disjoint_sets::DisjointSets disjointSets;

    for (unsigned i = 0; i < 16; ++i) { ASSERT_EQ(disjointSets.add(), i); }
    for (unsigned i = 0; i < 16; ++i) { ASSERT_EQ(disjointSets.find(i), i); }
}
//---------------------------------------------------------------------------
TEST(TestDisjointSets, unionAll) {
    disjoint_sets::DisjointSets disjointSets(16);

    unsigned rep = 0;
    for (unsigned i = 1; i < 16; ++i) { rep = disjointSets.unionSets(rep, i); }

    for (unsigned i = 0; i < 16; ++i) { ASSERT_EQ(disjointSets.find(i), rep); }
}
//---------------------------------------------------------------------------
TEST(TestDisjointSets, unionHalves) {
    disjoint_sets::DisjointSets disjointSets(16);

    unsigned rep1 = 0;
    unsigned rep2 = 8;
    for (unsigned i = 1; i < 8; ++i) {
        rep1 = disjointSets.unionSets(rep1, i);
        rep2 = disjointSets.unionSets(rep2, i + 8);
    }

    for (unsigned i = 0; i < 8; ++i) {
        ASSERT_EQ(disjointSets.find(i), rep1);
        ASSERT_EQ(disjointSets.find(i + 8), rep2);
    }
}
//---------------------------------------------------------------------------
TEST(TestDisjointSets, check) {
    disjoint_sets::DisjointSets disjointSets(16);

    unsigned rep = 0;
    for (unsigned i = 1; i < 16; ++i) {
        rep = disjointSets.unionSets(rep, i);

        for (unsigned j = 0; j <= i; ++j) {
            for (unsigned k = 0; k <= i; ++k) { ASSERT_FALSE(disjointSets.disjoint(j, k)); }
        }
    }
}
//---------------------------------------------------------------------------
TEST(TestDisjointSets, performance1) {
    const unsigned Size = 1'000'000;
    disjoint_sets::DisjointSets disjointSets(Size);

    for (unsigned step = 1; step < Size; step *= 2) {
        for (unsigned i = 0, j = i + step; j < Size; i += step * 2, j += step * 2) { disjointSets.unionSets(i, j); }
    }
    unsigned rep = disjointSets.unionSets(0, Size - 1);

    for (unsigned i = 0; i < Size; i += 1) {
        unsigned rep2 = disjointSets.find(i);
        ASSERT_EQ(rep, rep2);
    }
}
//---------------------------------------------------------------------------
TEST(TestDisjointSets, performance2) {
    const unsigned Size = 1'000'000;
    disjoint_sets::DisjointSets disjointSets(Size);

    for (unsigned i = 0; i < Size; i += 1) {
        unsigned new_rep = disjointSets.unionSets(0, i);
        unsigned expected = disjointSets.find(0);
        ASSERT_EQ(new_rep, expected);
    }
}
//---------------------------------------------------------------------------
TEST(TestDisjointSets, performance3) {
    const unsigned Size = 1'000'000;
    disjoint_sets::DisjointSets disjointSets(Size);

    for (unsigned i = 0; i < Size; i += 1) {
        unsigned new_rep = disjointSets.unionSets(i, 0);
        unsigned expected = disjointSets.find(0);
        ASSERT_EQ(new_rep, expected);
    }
}
//---------------------------------------------------------------------------

int main(int argc, char *argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
