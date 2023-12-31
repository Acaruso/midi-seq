#include "pch.h"
#include <vector>
#include "../midi-seq/src/midi/chords.hpp"

TEST(TestChords, CreateChordByRoot1) {
    std::vector<int> res = createChordByRoot(10, MAJOR, ROOT);
    std::vector<int> expected = {10, 14, 17};
    EXPECT_EQ(res, expected);
}

TEST(TestChords, CreateChordByRoot2) {
    std::vector<int> res = createChordByRoot(10, MINOR, ROOT);
    std::vector<int> expected = {10, 13, 17};
    EXPECT_EQ(res, expected);
}

TEST(TestChords, CreateChordByRoot3) {
    std::vector<int> res = createChordByRoot(10, MAJOR, FIRST_INV);
    std::vector<int> expected = {2, 5, 10};
    EXPECT_EQ(res, expected);
}

TEST(TestChords, CreateChordByRoot4) {
    std::vector<int> res = createChordByRoot(10, MAJOR, SECOND_INV);
    std::vector<int> expected = {5, 10, 14};
    EXPECT_EQ(res, expected);
}

TEST(TestChords, CreateChordByLowestNote1) {
    std::vector<int> res = createChordByLowestNote(10, MAJOR, ROOT);
    std::vector<int> expected = {10, 14, 17};
    EXPECT_EQ(res, expected);
}

TEST(TestChords, CreateChordByLowestNote2) {
    std::vector<int> res = createChordByLowestNote(10, MAJOR, FIRST_INV);
    std::vector<int> expected = {10, 13, 18};
    EXPECT_EQ(res, expected);
}

TEST(TestChords, CreateChordByLowestNote3) {
    std::vector<int> res = createChordByLowestNote(10, MAJOR, SECOND_INV);
    std::vector<int> expected = {10, 15, 19};
    EXPECT_EQ(res, expected);
}
