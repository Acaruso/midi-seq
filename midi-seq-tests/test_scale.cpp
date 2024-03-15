#include "pch.h"

#include "../midi-seq/src/midi/scale.hpp"

TEST(TestScale, TestScale1) {
    Scale scale(50, 0);
    int res = scale.getNote(0);
    int expected = 50;
    EXPECT_EQ(res, expected);
}

TEST(TestScale, TestScale2) {
    Scale scale(50, 0);
    int res = scale.getNote(1);
    int expected = 52;
    EXPECT_EQ(res, expected);
}

TEST(TestScale, TestScale3) {
    Scale scale(50, 0);
    int res = scale.getNote(-2);
    int expected = 0;
    EXPECT_EQ(res, expected);
}

TEST(TestScale, TestScale4) {
    Scale scale(50, 0);
    int res = scale.getNote(9);
    int expected = 66;
    EXPECT_EQ(res, expected);
}

TEST(TestScale, TestScale5) {
    Scale scale(50, 1);             // dorian mode
    int res = scale.getNote(2);
    int expected = 53;
    EXPECT_EQ(res, expected);
}

TEST(TestScale, TestScale6) {
    Scale scale(50, 1);             // dorian mode
    int res = scale.getNote(10);
    int expected = 67;
    EXPECT_EQ(res, expected);
}
