#pragma once

#include <string>

#include "beats.hpp"
#include "midi_queue.hpp"
#include "rng_service.hpp"
#include "generator_single_note.hpp"

template <typename MidiServiceType>
class ModuleSingleNote {
public:
    MidiQueue<MidiServiceType>& midiQueue;
    RngService& rngService;
    int ticksPer64Note;
    Beats beats;
    int channel;
    GeneratorSingleNote<MidiServiceType> generatorSingleNote;

    ModuleSingleNote(
        MidiQueue<MidiServiceType>& midiQueue,
        RngService& rngService
    ) :
        midiQueue(midiQueue),
        rngService(rngService),
        ticksPer64Note(48),
        beats(ticksPer64Note),
        channel(1),
        generatorSingleNote(beats, midiQueue, rngService, channel)
    {}

    void tick(std::string& message, int curTick) {
        generatorSingleNote.tick(message, curTick);
    }
};
