#pragma once

#include "beats.hpp"
#include "interval_generator.hpp"
#include "midi_queue.hpp"

template <typename MidiServiceType>
class ModuleIntervals {
public:
    MidiServiceType& midiService;
    MidiQueue<MidiServiceType>& midiQueue;
    int ticksPer64Note;
    Beats beats;
    int channel;
    IntervalGenerator<MidiServiceType> intervalGenerator;

    ModuleIntervals(
        MidiServiceType& midiService,
        MidiQueue<MidiServiceType>& midiQueue
    ) :
        midiService(midiService),
        midiQueue(midiQueue),
        ticksPer64Note(60),
        beats(ticksPer64Note),
        channel(1),
        intervalGenerator(beats, midiQueue, channel)
    {}

    void tick(std::string& message, int curTick) {
        // singleNoteGenerator.tick(message, curTick);
    }
};
