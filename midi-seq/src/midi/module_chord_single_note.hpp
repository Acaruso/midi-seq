#pragma once

#include "beats.hpp"
#include "generator_chord_single_note.hpp"
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
    GeneratorChordSingleNote<MidiServiceType> generatorChordSingleNote;

    ModuleChordSingleNote(
        MidiQueue<MidiServiceType>& midiQueue,
        RngService& rngService
    ) :
        midiQueue(midiQueue),
        rngService(rngService),
        ticksPer64Note(50),
        beats(ticksPer64Note),
        channel(1),
        generatorChordSingleNote(
            beats,
            midiQueue,
            rngService,
            channel
        )
    {}

    void tick(std::string& message, int curTick) {
        generatorChordSingleNote.tick(message, curTick);
    }
};
