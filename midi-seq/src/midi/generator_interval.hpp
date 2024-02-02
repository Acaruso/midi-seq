#pragma once

#include <vector>

#include "../main/util.hpp"
#include "beats.hpp"
#include "midi_queue.hpp"
#include "rng_service.hpp"
#include "util.hpp"

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
    // 6,
    7,
    // 8,
    // 9,
    // 10,
    // 11,
};

template <typename MidiServiceType>
class GeneratorInterval {
public:
    Beats& beats;
    MidiQueue<MidiServiceType>& midiQueue;
    RngService& rngService;

    int channel;

    int lowLimit = guitarToMidi(S_D, 0);
    int highLimit = guitarToMidi(S_D, 5);

    int curIntervalLow = 0;
    int prevIntervalLow = 0;
    int curIntervalHigh = 0;
    bool playing;
    int numRepeats;
    int counter;

    GeneratorInterval(
        Beats& beats,
        MidiQueue<MidiServiceType>& midiQueue,
        RngService& rngService,
        int channel
    ) :
        beats(beats),
        midiQueue(midiQueue),
        rngService(rngService),
        channel(channel),
        playing(false),
        numRepeats(8),
        counter(0)
    {
        generateNextInterval();
    }

    void tick(std::string& message, int curTick) {
        if (message == " ") {
            playing = !playing;
        }
        if (!playing) {
            return;
        }

        if (beats.isBeat(curTick, B_4)) {
            if ((counter % numRepeats) == 0) {
                generateNextInterval();
            }
            play(curTick);
            ++counter;
        }
    }

    void generateNextInterval() {
        curIntervalLow = rngService.getRand(lowLimit, highLimit);
        while (curIntervalLow == prevIntervalLow) {
            curIntervalLow = rngService.getRand(lowLimit, highLimit);
        }
        prevIntervalLow = curIntervalLow;
        int offsetIdx = rngService.getRand(0, intervalOffsets.size() - 1);
        curIntervalHigh = curIntervalLow + intervalOffsets[offsetIdx];
    }

    void play(int curTick) {
        midiQueue.noteOnOff(channel, curIntervalLow,  100, curTick, beats.ticksPerBeat(B_8));
        midiQueue.noteOnOff(channel, curIntervalHigh, 100, curTick, beats.ticksPerBeat(B_8));
    }
};
