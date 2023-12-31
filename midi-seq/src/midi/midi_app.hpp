#pragma once

#include "beats.hpp"
#include "chord_generator.hpp"
#include "midi_service.hpp"
#include "midi_queue.hpp"

class MidiApp {
public:
    Beats beats;
    MidiService midiService;
    MidiQueue<MidiService> midiQueue;
    ChordGenerator<MidiService> chordGenerator;

    int curTick = 0;

    MidiApp() :
        beats(60),                          // ticks per 64 note
        midiService(1),                     // midi port
        midiQueue(midiService),
        chordGenerator(beats, midiQueue)
    {}

    void tick() {
        chordGenerator.tick(curTick);

        // TODO: refactor code so that handleEvents can be first
        // this will make timing more stable
        midiQueue.handleEvents(curTick);

        ++curTick;
    }
};
