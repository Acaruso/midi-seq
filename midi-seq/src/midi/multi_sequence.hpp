#pragma once

#include <vector>

#include "beats.hpp"
#include "midi_queue.hpp"
#include "sequence.hpp"

template <typename MidiServiceType>
class MultiSequence {
    Beats& beats;
    MidiQueue<MidiServiceType>& midiQueue;
    int channel;
    int numSteps;
    BeatUnit stepSize;
    int numSequences;
    std::vector<Sequence<MidiServiceType>> sequences;

public:
    MultiSequence(
        Beats& beats,
        MidiQueue<MidiServiceType>& midiQueue,
        int channel,
        int numSteps,
        BeatUnit stepSize,
        int numSequences
    ) :
        beats(beats),
        midiQueue(midiQueue),
        channel(channel),
        numSteps(numSteps),
        stepSize(stepSize),
        numSequences(numSequences)
    {
        for (int i = 0; i < numSequences; i++) {
            sequences.push_back(
                Sequence<MidiServiceType>(beats, midiQueue, channel, numSteps, stepSize)
            );
        }
    }

    Sequence<MidiServiceType>& get(int i) {
        return sequences[i];
    }

    void tick(int curTick) {
        for (auto& s : sequences) {
            s.tick(curTick);
        }
    }
};
