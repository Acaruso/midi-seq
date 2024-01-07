#pragma once

#include <iostream>
#include <string>

#include "../main/util.hpp"
#include "beats.hpp"
#include "chord_generator.hpp"
#include "midi_queue.hpp"
#include "midi_service.hpp"
#include "sequence.hpp"

template <typename MidiServiceType>
class ModuleChordSeq {
public:
    MidiServiceType& midiService;
    MidiQueue<MidiServiceType>& midiQueue;

    int chordChannel;
    int seqChannel;

    int ticksPer64Note;
    Beats beats;

    ChordGenerator<MidiServiceType> chordGenerator;
    Sequence<MidiServiceType> sequence;

    ModuleChordSeq(
        MidiServiceType& midiService,
        MidiQueue<MidiServiceType>& midiQueue
    ) :
        midiService(midiService),
        midiQueue(midiQueue),

        chordChannel(1),
        seqChannel(2),

        ticksPer64Note(60),
        beats(ticksPer64Note),

        chordGenerator(beats, midiQueue, chordChannel),
        sequence(
            beats,
            midiQueue,
            seqChannel,
            8,                      // numSteps
            B_16                    // step size
        )
    {
        int kick  = 36;
        int snare = 37;
        int ch    = 38;
        int oh    = 39;

        // addRollEventOneShot(0, 50);
        addNoteEvent(0, kick);
        addNoteEvent(1, ch);
        addNoteEvent(2, snare);
        addNoteEvent(3, ch);
        addNoteEvent(4, kick);
        addNoteEvent(5, ch);
        addNoteEvent(6, snare);
        addNoteEvent(7, ch);
    }

    void tick(std::string& message, int curTick) {
        if (message == "s") {
            chordGenerator.generateNextChord();
        }

        chordGenerator.tick(curTick);
        sequence.tick(curTick);
    }

    // seq methods

    void addRollEvent(int idx, int note) {
        sequence.addEvent(
            idx,
            Event{
                .on = true,
                .channel = seqChannel,
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
                .channel = seqChannel,
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
                .channel = seqChannel,
                .subEvent = NoteEvent{
                    .note = note,
                    .velocity = 100,
                    .duration = beats.ticksPerBeat(B_16)
                }
            }
        );
    }
};
