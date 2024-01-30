#pragma once

#include <string>

#include "beats.hpp"
#include "midi_queue.hpp"
#include "rng_service.hpp"
#include "single_note_generator.hpp"

template <typename MidiServiceType>
class ModuleSingleNote {
public:
    MidiServiceType& midiService;
    MidiQueue<MidiServiceType>& midiQueue;
    RngService& rngService;
    int ticksPer64Note;
    Beats beats;
    int channel;
    SingleNoteGenerator<MidiServiceType> singleNoteGenerator;

    ModuleSingleNote(
        MidiServiceType& midiService,
        MidiQueue<MidiServiceType>& midiQueue,
        RngService& rngService
    ) :
        midiService(midiService),
        midiQueue(midiQueue),
        rngService(rngService),
        ticksPer64Note(48),
        beats(ticksPer64Note),
        channel(1),
        singleNoteGenerator(beats, midiQueue, rngService, channel)
    {}

    void tick(std::string& message, int curTick) {
        singleNoteGenerator.tick(message, curTick);
    }
};
