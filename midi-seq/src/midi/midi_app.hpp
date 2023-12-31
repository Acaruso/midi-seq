#pragma once

#include "beats.hpp"
#include "chord_generator.hpp"
#include "midi_service.hpp"
#include "midi_queue.hpp"
#include "sequence.hpp"

class MidiApp {
public:
    Beats beats;
    MidiService midiService;
    MidiQueue<MidiService> midiQueue;
    ChordGenerator<MidiService> chordGenerator;
    Sequence<MidiService> sequence;

    int curTick = 0;

    MidiApp() :
        beats(24),                          // ticks per 64 note
        midiService(1),                     // midi port
        midiQueue(midiService),
        chordGenerator(beats, midiQueue),
        sequence(beats, midiQueue, 8, B_16)
    {
        // sequence.addNoteEvent(0, 50, 100, beats.ticksPerBeat(B_16));
        sequence.addNoteRollEvent(0, 50, 100, 2, beats.ticksPerBeat(B_16), beats.ticksPerBeat(B_64));
        sequence.addNoteEvent(2, 55, 100, beats.ticksPerBeat(B_16));
        sequence.addNoteEvent(4, 60, 100, beats.ticksPerBeat(B_16));
        sequence.addNoteEvent(6, 65, 100, beats.ticksPerBeat(B_16));
    }

    void tick() {
        // chordGenerator.tick(curTick);
        sequence.tick(curTick);

        // TODO: refactor code so that handleEvents can be first
        // this will make timing more stable
        midiQueue.handleEvents(curTick);

        ++curTick;
    }
};
