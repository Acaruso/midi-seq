#include "pch.h"
#include <vector>
#include "../midi-seq/src/midi/chords.hpp"

TEST(ChordTests, CreateChordByRoot1) {
    std::vector<int> res = createChordByRoot(10, MAJOR, ROOT);
    std::vector<int> expected = {10, 14, 17};
    EXPECT_EQ(res, expected);
}

TEST(ChordTests, CreateChordByRoot2) {
    std::vector<int> res = createChordByRoot(10, MINOR, ROOT);
    std::vector<int> expected = {10, 13, 17};
    EXPECT_EQ(res, expected);
}

TEST(ChordTests, CreateChordByRoot3) {
    std::vector<int> res = createChordByRoot(10, MAJOR, FIRST_INV);
    std::vector<int> expected = {2, 5, 10};
    EXPECT_EQ(res, expected);
}

TEST(ChordTests, CreateChordByRoot4) {
    std::vector<int> res = createChordByRoot(10, MAJOR, SECOND_INV);
    std::vector<int> expected = {5, 10, 14};
    EXPECT_EQ(res, expected);
}

TEST(ChordTests, CreateChordByLowestNote1) {
    std::vector<int> res = createChordByLowestNote(10, MAJOR, ROOT);
    std::vector<int> expected = {10, 14, 17};
    EXPECT_EQ(res, expected);
}

TEST(ChordTests, CreateChordByLowestNote2) {
    std::vector<int> res = createChordByLowestNote(10, MAJOR, FIRST_INV);
    std::vector<int> expected = {10, 13, 18};
    EXPECT_EQ(res, expected);
}

TEST(ChordTests, CreateChordByLowestNote3) {
    std::vector<int> res = createChordByLowestNote(10, MAJOR, SECOND_INV);
    std::vector<int> expected = {10, 15, 19};
    EXPECT_EQ(res, expected);
}
