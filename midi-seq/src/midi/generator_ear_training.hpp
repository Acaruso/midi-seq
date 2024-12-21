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
    int droneNote;
    int prevDroneNote;
    int note;
    int prevInterval;
    bool autoSwitch;
    bool changeDrone;
    std::vector<int> intervals;
    std::vector<int> droneChoices;

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
        droneNote(guitarToMidi(S_G, 5)),
        prevDroneNote(droneNote),
        note(0),
        prevInterval(0),
        autoSwitch(false),
        changeDrone(false),
        // intervals({2, 4, 5, 7, 9, 11}),  // major scale
        // intervals({2, 3, 5, 7, 8, 10}),  // minor scale
        intervals({1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11}),  // all notes
        // intervals({4, 5, 7}),
        // droneChoices({60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71})
        droneChoices({50, 51, 52, 53, 54, 55})
    {
        generateNextInterval();
    }

    void tick(std::string& message, int curTick) {
        if (message == "n") {
            generateNextInterval();
        }
        if (message == "u") {
            generateNextDrone();
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
        if (changeDrone) {
            droneNote = pickNext(droneChoices, prevDroneNote);
            prevDroneNote = droneNote;
        }

        int interval = pickNext(intervals, prevInterval);
        prevInterval = interval;
        note = droneNote + interval;

        midiToMainQueue->enqueue(std::to_string(interval));
    }

    void generateNextDrone() {
        droneNote = pickNext(droneChoices, prevDroneNote);
        prevDroneNote = droneNote;
    }

    void play(int curTick) {
        midiQueue.noteOnOff(channel, droneNote,  100, curTick, beats.ticksPerBeat(B_8));
        midiQueue.noteOnOff(channel, note,    100, curTick, beats.ticksPerBeat(B_8));
    }

    int pickNext(std::vector<int>& vec, int cur) {
        int res = vec[rngService.getRand(0, vec.size() - 1)];
        while (res == cur) {
            res = vec[rngService.getRand(0, vec.size() - 1)];
        }
        return res;
    }
};
