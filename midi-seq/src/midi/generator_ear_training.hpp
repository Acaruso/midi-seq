#pragma once

#include <vector>

#include "beats.hpp"
#include "midi_queue.hpp"
#include "rng_service.hpp"
#include "util.hpp"

template <typename MidiServiceType>
class GeneratorEarTraining {
public:
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
    std::vector<int> choices;

    GeneratorEarTraining(
        Beats& beats,
        MidiQueue<MidiServiceType>& midiQueue,
        RngService& rngService,
        int channel
    ):
        beats(beats),
        midiQueue(midiQueue),
        rngService(rngService),
        channel(channel),
        numRepeats(4),
        counter(0),
        curNote(0),
        prevNote(0),
        autoSwitch(false),
        choices({2, 4, 5, 7, 9, 11})
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
        curNote = droneNote + choices[rngService.getRand(0, choices.size() - 1)];
        while (curNote == prevNote) {
            curNote = droneNote + choices[rngService.getRand(0, choices.size() - 1)];
        }
        prevNote = curNote;
    }

    void play(int curTick) {
        midiQueue.noteOnOff(channel, droneNote,  100, curTick, beats.ticksPerBeat(B_8));
        midiQueue.noteOnOff(channel, curNote,    100, curTick, beats.ticksPerBeat(B_8));
    }
};
