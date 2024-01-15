#pragma once

#include "beats.hpp"
#include "chord_generator.hpp"
#include "midi_queue.hpp"
#include "midi_service.hpp"

template <typename MidiServiceType>
class ModuleChord {
public:
    MidiServiceType& midiService;
    MidiQueue<MidiServiceType>& midiQueue;
    int ticksPer64Note;
    Beats beats;
    int channel;
    ChordGenerator<MidiServiceType> chordGenerator;
    int curTick;

    ModuleChord(
        MidiServiceType& midiService,
        MidiQueue<MidiServiceType>& midiQueue
    ) :
        midiService(midiService),
        midiQueue(midiQueue),
        ticksPer64Note(44),
        beats(ticksPer64Note),
        channel(1),
        chordGenerator(beats, midiQueue, channel),
        curTick(0)
    {}

    void tick(std::string& message, int curTick) {
        if (message == "s") {
            chordGenerator.generateNextChord();
        }

        chordGenerator.tick(curTick);
    }
};
