#pragma once

#include "beats.hpp"
#include "chord_generator.hpp"
#include "midi_queue.hpp"
#include "midi_service.hpp"
#include "rng_service.hpp"

template <typename MidiServiceType>
class ModuleChord {
public:
    MidiServiceType& midiService;
    MidiQueue<MidiServiceType>& midiQueue;
    RngService& rngService;
    int ticksPer64Note;
    Beats beats;
    int channel;
    ChordGenerator<MidiServiceType> chordGenerator;
    int curTick;

    ModuleChord(
        MidiServiceType& midiService,
        MidiQueue<MidiServiceType>& midiQueue,
        RngService& rngService
    ) :
        midiService(midiService),
        midiQueue(midiQueue),
        rngService(rngService),
        ticksPer64Note(50),
        beats(ticksPer64Note),
        channel(1),
        chordGenerator(beats, midiQueue, rngService, channel),
        curTick(0)
    {}

    void tick(std::string& message, int curTick) {
        chordGenerator.tick(message, curTick);
    }
};
