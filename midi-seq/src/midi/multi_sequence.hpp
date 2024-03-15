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
    int numTracks;
    std::vector<Sequence<MidiServiceType>> tracks;

public:
    MultiSequence(
        Beats& beats,
        MidiQueue<MidiServiceType>& midiQueue,
        int channel,
        int numSteps,
        BeatUnit stepSize,
        int numTracks
    ) :
        beats(beats),
        midiQueue(midiQueue),
        channel(channel),
        numSteps(numSteps),
        stepSize(stepSize),
        numTracks(numTracks)
    {
        for (int i = 0; i < numTracks; i++) {
            tracks.push_back(
                Sequence<MidiServiceType>(beats, midiQueue, channel, numSteps, stepSize)
            );
        }
    }

    Sequence<MidiServiceType>& get(int i) {
        return tracks[i];
    }

    void tick(int curTick) {
        for (auto& track : tracks) {
            track.tick(curTick);
        }
    }
};
