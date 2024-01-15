#pragma once

#include <iostream>
#include <string>

#include "../main/util.hpp"
#include "beats.hpp"
#include "chord_generator.hpp"
#include "midi_queue.hpp"
#include "midi_service.hpp"
#include "multi_sequence.hpp"

template <typename MidiServiceType>
class ModuleChordSeq {
public:
    MidiServiceType& midiService;
    MidiQueue<MidiServiceType>& midiQueue;

    int ticksPer64Note;
    Beats beats;

    int chordChannel;

    ChordGenerator<MidiServiceType> chordGenerator;
    int seqChannel;

    MultiSequence<MidiServiceType> multiSequence;

    ModuleChordSeq(
        MidiServiceType& midiService,
        MidiQueue<MidiServiceType>& midiQueue
    ) :
        midiService(midiService),
        midiQueue(midiQueue),
        ticksPer64Note(60),
        beats(ticksPer64Note),
        chordChannel(1),
        chordGenerator(beats, midiQueue, chordChannel),
        seqChannel(2),
        multiSequence(
            beats,
            midiQueue,
            seqChannel,
            8,                      // numSteps
            B_16,                   // step size
            4                       // numSequences
        )
    {
        int kickSeq      = 0;
        int snareSeq     = 1;
        int closedHatSeq = 2;
        int openHatSeq   = 3;

        int kickNote      = 36;
        int snareNote     = 37;
        int closedHatNote = 38;
        int openHatNote   = 39;

        // addRollEventOneShot(0, 50);
        addNoteEvent(kickSeq,  0, kickNote);
        addNoteEvent(kickSeq,  2, kickNote);
        addNoteEvent(kickSeq,  4, kickNote);
        addNoteEvent(kickSeq,  7, kickNote);

        addNoteEvent(snareSeq, 2, snareNote);
        addNoteEvent(snareSeq, 6, snareNote);

        addNoteEvent(closedHatSeq, 0, closedHatNote);
        addNoteEvent(closedHatSeq, 1, closedHatNote);
        addNoteEvent(closedHatSeq, 2, closedHatNote);
        addNoteEvent(closedHatSeq, 3, closedHatNote);
        addNoteEvent(closedHatSeq, 4, closedHatNote);
        addNoteEvent(closedHatSeq, 5, closedHatNote);
        addNoteEvent(closedHatSeq, 6, closedHatNote);
        addNoteEvent(closedHatSeq, 7, closedHatNote);
    }

    void tick(std::string& message, int curTick) {
        if (message == "s") {
            chordGenerator.generateNextChord();
        }

        chordGenerator.tick(curTick);
        multiSequence.tick(curTick);
    }

    // seq methods ////////////////////////////////////////////////////////////////////////////////

    void addRollEvent(int seqIdx, int idx, int note) {
        auto& sequence = multiSequence.get(seqIdx);
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

    void addRollEventOneShot(int seqIdx, int idx, int note) {
        auto& sequence = multiSequence.get(seqIdx);
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

    void addNoteEvent(int seqIdx, int idx, int note) {
        auto& sequence = multiSequence.get(seqIdx);
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
