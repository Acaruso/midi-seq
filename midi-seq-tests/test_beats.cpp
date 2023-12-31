#include "pch.h"
#include "../midi-seq/src/midi/beats.hpp"

TEST(TestBeats, T1) {
    Beats b(24);

    EXPECT_EQ(b.ticksPerBeat(B_128), 12);
    EXPECT_EQ(b.ticksPerBeat(B_64),  24);
    EXPECT_EQ(b.ticksPerBeat(B_32),  48);
    EXPECT_EQ(b.ticksPerBeat(B_16),  96);
    EXPECT_EQ(b.ticksPerBeat(B_8),   192);
    EXPECT_EQ(b.ticksPerBeat(B_4),   384);
    EXPECT_EQ(b.ticksPerBeat(B_2),   768);
    EXPECT_EQ(b.ticksPerBeat(B_1),   1536);

    EXPECT_EQ(b.isBeat(0,   B_64), true);
    EXPECT_EQ(b.isBeat(24,  B_64), true);
    EXPECT_EQ(b.isBeat(192, B_16), true);

    EXPECT_EQ(b.tickToBeat(0,  B_64), 0);
    EXPECT_EQ(b.tickToBeat(23, B_64), 0);
    EXPECT_EQ(b.tickToBeat(24, B_64), 1);
    EXPECT_EQ(b.tickToBeat(25, B_64), 1);

    EXPECT_EQ(b.tickToBeat(0,  B_128), 0);
    EXPECT_EQ(b.tickToBeat(11, B_128), 0);
    EXPECT_EQ(b.tickToBeat(12, B_128), 1);
    EXPECT_EQ(b.tickToBeat(23, B_128), 1);
}
