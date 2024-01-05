#pragma once

#include <vector>

enum BeatUnit {
    B_256,
    B_128,
    B_64,
    B_32,
    B_16,
    B_8,
    B_4,
    B_2,
    B_1,
    COUNT,
};

class Beats {
public:
    std::vector<int> ticksPerBeatUnit = std::vector<int>(BeatUnit::COUNT, 0);

    Beats(int ticksPer64Note) {
        ticksPerBeatUnit[B_256] = ticksPer64Note / 4;      // TODO: test if (ticksPer64Note % 4) == 0
        ticksPerBeatUnit[B_128] = ticksPer64Note / 2;
        ticksPerBeatUnit[B_64]  = ticksPer64Note * 1;
        ticksPerBeatUnit[B_32]  = ticksPer64Note * 2;
        ticksPerBeatUnit[B_16]  = ticksPer64Note * 4;
        ticksPerBeatUnit[B_8]   = ticksPer64Note * 8;
        ticksPerBeatUnit[B_4]   = ticksPer64Note * 16;
        ticksPerBeatUnit[B_2]   = ticksPer64Note * 32;
        ticksPerBeatUnit[B_1]   = ticksPer64Note * 64;
    }

    int ticksPerBeat(BeatUnit beatUnit) {
        return ticksPerBeatUnit[beatUnit];
    }

    bool isBeat(int tick, BeatUnit beatUnit) {
        return ((tick % ticksPerBeatUnit[beatUnit]) == 0);
    }

    int tickToBeat(int tick, BeatUnit beatUnit) {
        return (tick / ticksPerBeatUnit[beatUnit]);
    }
};
