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
        ticksPer64Note(40),
        beats(ticksPer64Note),
        chordChannel(1),
        chordGenerator(beats, midiQueue, chordChannel),
        seqChannel(2),
        multiSequence(
            beats,
            midiQueue,
            seqChannel,
            8,                      // numSteps
            B_16,                   // stepSize
            4                       // numTracks
        )
    {
        int kickTrack      = 0;
        int snareTrack     = 1;
        int closedHatTrack = 2;
        int openHatTrack   = 3;

        int kickNote      = 36;
        int snareNote     = 37;
        int closedHatNote = 38;
        int openHatNote   = 39;

        int offsetAmnt = 20;

        // addRollEventOneShot(0, 50);
        addNoteEvent(kickTrack, 0, kickNote);
        addNoteEvent(kickTrack, 4, kickNote);

        addNoteEvent(snareTrack, 4, snareNote);

        addNoteEvent(closedHatTrack, 0, closedHatNote);
        addNoteEvent(closedHatTrack, 1, closedHatNote);
        addNoteEvent(closedHatTrack, 2, closedHatNote);
        addNoteEvent(closedHatTrack, 3, closedHatNote);
        addNoteEvent(closedHatTrack, 4, closedHatNote);
        addNoteEvent(closedHatTrack, 5, closedHatNote);
        addNoteEvent(closedHatTrack, 6, closedHatNote);
        addNoteEvent(closedHatTrack, 7, closedHatNote);
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

    void addNoteEvent(int seqIdx, int idx, int offset, int note) {
        auto& sequence = multiSequence.get(seqIdx);
        sequence.addEvent(
            idx,
            Event{
                .on = true,
                .offset = offset,
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
