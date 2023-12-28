#pragma once

#include <vector>

enum BeatUnit {
    B_64,
    B_32,
    B_16,
    B_8,
    B_4,
    B_2,
    B_1,
    SIZE,
};

class Beats {
public:
    std::vector<int> ticksPerBeatUnit = std::vector<int>(BeatUnit::SIZE, 0);

    Beats(int ticksPer64Note) {
        ticksPerBeatUnit[B_64] = ticksPer64Note * 1;
        ticksPerBeatUnit[B_32] = ticksPer64Note * 2;
        ticksPerBeatUnit[B_16] = ticksPer64Note * 4;
        ticksPerBeatUnit[B_8]  = ticksPer64Note * 8;
        ticksPerBeatUnit[B_4]  = ticksPer64Note * 16;
        ticksPerBeatUnit[B_2]  = ticksPer64Note * 32;
        ticksPerBeatUnit[B_1]  = ticksPer64Note * 64;
    }

    bool isBeat(int tick, BeatUnit beatUnit) {
        return ((tick % ticksPerBeatUnit[beatUnit]) == 0);
    }

    int getBeat(int tick, BeatUnit beatUnit) {
        return (tick / ticksPerBeatUnit[beatUnit]);
    }
};
