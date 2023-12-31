#pragma once

#include "chord_generator.hpp"
#include "midi_service.hpp"
#include "midi_queue.hpp"

class MidiApp {
public:
    MidiService midiService;
    MidiQueue<MidiService> midiQueue;
    ChordGenerator<MidiService> chordGenerator;

    int curTick = 0;

    MidiApp() :
        midiService(1),
        midiQueue(midiService),
        chordGenerator(midiQueue)
    {}

    void tick() {
        chordGenerator.tick(curTick);
        ++curTick;
    }
};
