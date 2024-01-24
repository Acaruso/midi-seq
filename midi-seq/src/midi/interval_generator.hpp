#pragma once

#include <vector>

#include "../main/util.hpp"
#include "beats.hpp"
#include "midi_queue.hpp"

enum Intervals {
    MINOR_3,
    MAJOR_3,
    PERFECT_4,
    FLAT_5,
    PERFECT_5,
    MINOR_6,
    MAJOR_6,
    MINOR_7,
    MAJOR_7
};

static std::vector<int> intervalOffsets = {
    3,
    4,
    5,
    6,
    7,
    8,
    9,
    10,
    11,
};

template <typename MidiServiceType>
class IntervalGenerator {
public:
    Beats& beats;
    MidiQueue<MidiServiceType>& midiQueue;
    int channel;

    int lowLimit = 50;
    int highLimit = 60;

    int curIntervalLow = 0;
    int prevIntervalLow = 0;
    int curIntervalHigh = 0;

    IntervalGenerator(
        Beats& beats,
        MidiQueue<MidiServiceType>& midiQueue,
        int channel
    ) :
        beats(beats),
        midiQueue(midiQueue),
        channel(channel)
    {
        generateNextInterval();
    }

    void tick(std::string& message, int curTick) {
    }

    void generateNextInterval() {
        curIntervalLow = getRand(lowLimit, highLimit);
        while (curIntervalLow == prevIntervalLow) {
            curIntervalLow = getRand(lowLimit, highLimit);
        }
        prevIntervalLow = curIntervalLow;
        int offsetIdx = getRand(0, intervalOffsets.size() - 1);
        curIntervalHigh = curIntervalLow + intervalOffsets[offsetIdx];
    }
};
