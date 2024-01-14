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
    int numChannels;
    std::vector<Sequence<MidiServiceType>> sequences;

public:
    MultiSequence(
        Beats& beats,
        MidiQueue<MidiServiceType>& midiQueue,
        int channel,
        int numSteps,
        BeatUnit stepSize,
        int numChannels
    ) :
        beats(beats),
        midiQueue(midiQueue),
        channel(channel),
        numSteps(numSteps),
        stepSize(stepSize),
        numChannels(numChannels)
    {}
};
