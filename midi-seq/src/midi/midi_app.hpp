#pragma once

#include "../main/util.hpp"
#include "beats.hpp"
#include "chord_generator.hpp"
#include "midi_service.hpp"
#include "midi_queue.hpp"
#include "sequence.hpp"

class MidiApp {
public:
    int ticksPer64Note;
    Beats beats;
    MidiService midiService;
    MidiQueue<MidiService> midiQueue;
    ChordGenerator<MidiService> chordGenerator;
    Sequence<MidiService> sequence;
    int curTick = 0;

    MidiApp() :
        // ticksPer64Note(24),
        ticksPer64Note(60),
        beats(ticksPer64Note),
        midiService(1),                     // midi port
        midiQueue(midiService),
        chordGenerator(beats, midiQueue),
        sequence(beats, midiQueue, 8, B_16)
    {
        addRollEventOneShot(0, 50);
        addNoteEvent(2, 55);
        addNoteEvent(4, 60);
        addNoteEvent(6, 65);
    }

    // void tick() {
    //     if (sequence.isBeat(curTick)) {
    //         if (sequence.curStep == 0) {
    //         }
    //     }

    //     sequence.tick(curTick);

    //     // TODO: refactor code so that handleEvents can be first
    //     // this will make timing more stable
    //     midiQueue.handleEvents(curTick);

    //     ++curTick;
    // }

    void tick() {
        chordGenerator.tick(curTick);

        // TODO: refactor code so that handleEvents can be first
        // this will make timing more stable
        midiQueue.handleEvents(curTick);

        ++curTick;
    }

    void addRollEvent(int idx, int note) {
        sequence.addEvent(
            idx,
            Event{
                .on = true,
                .subEvent = RollEvent{
                    .note = note,
                    .velocity = 100,
                    .numRepeats = 4,
                    .totalDuration = beats.ticksPerBeat(B_64),
                    .restDuration = beats.ticksPerBeat(B_256)
                }
            }
        );
    }

    void addRollEventOneShot(int idx, int note) {
        sequence.addEvent(
            idx,
            Event{
                .on = true,
                .oneShot = true,
                .subEvent = RollEvent{
                    .note = note,
                    .velocity = 100,
                    .numRepeats = 4,
                    .totalDuration = beats.ticksPerBeat(B_64),
                    .restDuration = beats.ticksPerBeat(B_256)
                }
            }
        );
    }

    void addNoteEvent(int idx, int note) {
        sequence.addEvent(
            idx,
            Event{
                .on = true,
                .subEvent = NoteEvent{
                    .note = note,
                    .velocity = 100,
                    .duration = beats.ticksPerBeat(B_16)
                }
            }
        );
    }
};
