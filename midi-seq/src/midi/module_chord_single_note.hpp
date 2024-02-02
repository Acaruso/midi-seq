#pragma once

#include "beats.hpp"
#include "midi_queue.hpp"
#include "rng_service.hpp"

template <typename MidiServiceType>
class ModuleChordSingleNote {
public:
    MidiQueue<MidiServiceType>& midiQueue;
    RngService& rngService;
    int ticksPer64Note;
    Beats beats;
    int channel;
    // GeneratorChord<MidiServiceType> generatorChord;

    ModuleChordSingleNote(
        MidiQueue<MidiServiceType>& midiQueue,
        RngService& rngService
    ) :
        midiQueue(midiQueue),
        rngService(rngService),
        ticksPer64Note(50),
        beats(ticksPer64Note),
        channel(1)
        // generatorChord(beats, midiQueue, rngService, channel),
    {}

    void tick(std::string& message, int curTick) {
        // generatorChord.tick(message, curTick);
    }
};
