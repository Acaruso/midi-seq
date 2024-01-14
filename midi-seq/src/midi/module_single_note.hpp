#pragma once

#include "beats.hpp"
#include "midi_queue.hpp"
#include "single_note_generator.hpp"

template <typename MidiServiceType>
class ModuleSingleNote {
public:
    MidiServiceType& midiService;
    MidiQueue<MidiServiceType>& midiQueue;
    int ticksPer64Note;
    Beats beats;
    int channel;
    SingleNoteGenerator<MidiServiceType> singleNoteGenerator;

    ModuleSingleNote(
        MidiServiceType& midiService,
        MidiQueue<MidiServiceType>& midiQueue
    ) :
        midiService(midiService),
        midiQueue(midiQueue),
        ticksPer64Note(50),
        beats(ticksPer64Note),
        channel(1),
        singleNoteGenerator(beats, midiQueue, channel)
    {}

    void tick(std::string& message, int curTick) {
        singleNoteGenerator.tick(curTick);
    }
};
