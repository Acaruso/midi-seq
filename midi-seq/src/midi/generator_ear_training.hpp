#pragma once

#include <string>
#include <vector>

#include "../lib/readerwriterqueue.h"

#include "beats.hpp"
#include "midi_queue.hpp"
#include "rng_service.hpp"
#include "util.hpp"

template <typename MidiServiceType>
class GeneratorEarTraining {
public:
    moodycamel::ReaderWriterQueue<std::string>* midiToMainQueue;
    Beats& beats;
    MidiQueue<MidiServiceType>& midiQueue;
    RngService& rngService;
    int channel;
    int numRepeats;
    int counter;
    int droneNote = guitarToMidi(S_D, 5);
    int curNote;
    int prevNote;
    bool autoSwitch;
    std::vector<int> intervals;

    GeneratorEarTraining(
        moodycamel::ReaderWriterQueue<std::string>* midiToMainQueue,
        Beats& beats,
        MidiQueue<MidiServiceType>& midiQueue,
        RngService& rngService,
        int channel
    ):
        midiToMainQueue(midiToMainQueue),
        beats(beats),
        midiQueue(midiQueue),
        rngService(rngService),
        channel(channel),
        numRepeats(4),
        counter(0),
        curNote(0),
        prevNote(0),
        autoSwitch(false),
        intervals({2, 4, 5, 7, 9, 11})
    {
        generateNextInterval();
    }

    void tick(std::string& message, int curTick) {
        if (message == "n") {
            generateNextInterval();
        }

        if (beats.isBeat(curTick, B_4)) {
            if (autoSwitch && (counter % numRepeats) == 0) {
                generateNextInterval();
            }
            play(curTick);
            ++counter;
        }
    }

    void generateNextInterval() {
        int interval = intervals[rngService.getRand(0, intervals.size() - 1)];
        curNote = droneNote + interval;
        while (curNote == prevNote) {
            interval = intervals[rngService.getRand(0, intervals.size() - 1)];
            curNote = droneNote + interval;
        }
        prevNote = curNote;

        midiToMainQueue->enqueue(std::to_string(interval));
    }

    void play(int curTick) {
        midiQueue.noteOnOff(channel, droneNote,  100, curTick, beats.ticksPerBeat(B_8));
        midiQueue.noteOnOff(channel, curNote,    100, curTick, beats.ticksPerBeat(B_8));
    }
};
