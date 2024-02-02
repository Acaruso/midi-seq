#pragma once

#include <iostream>
#include <string>

#include "../main/util.hpp"
#include "beats.hpp"
#include "generator_chord.hpp"
#include "midi_queue.hpp"
#include "midi_service.hpp"
#include "multi_sequence.hpp"
#include "rng_service.hpp"

template <typename MidiServiceType>
class ModuleChordSeq {
public:
    MidiServiceType& midiService;
    MidiQueue<MidiServiceType>& midiQueue;
    RngService& rngService;

    int ticksPer64Note;
    Beats beats;

    int chordChannel;

    GeneratorChord<MidiServiceType> generatorChord;
    int seqChannel;

    MultiSequence<MidiServiceType> multiSequence;

    ModuleChordSeq(
        MidiServiceType& midiService,
        MidiQueue<MidiServiceType>& midiQueue,
        RngService& rngService
    ) :
        midiService(midiService),
        midiQueue(midiQueue),
        rngService(rngService),
        ticksPer64Note(40),
        beats(ticksPer64Note),
        chordChannel(1),
        generatorChord(beats, midiQueue, rngService, chordChannel),
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

        // addNoteEvent(closedHatTrack, 0, closedHatNote);
        // addNoteEvent(closedHatTrack, 1, closedHatNote);
        // addNoteEvent(closedHatTrack, 2, closedHatNote);
        // addNoteEvent(closedHatTrack, 3, closedHatNote);
        // addNoteEvent(closedHatTrack, 4, closedHatNote);
        // addNoteEvent(closedHatTrack, 5, closedHatNote);
        // addNoteEvent(closedHatTrack, 6, closedHatNote);
        // addNoteEvent(closedHatTrack, 7, closedHatNote);

        addNoteEvent(closedHatTrack, 0, closedHatNote);
        addNoteProbEvent(closedHatTrack, 1, 50, closedHatNote);
        addNoteEvent(closedHatTrack, 2, closedHatNote);
        addNoteProbEvent(closedHatTrack, 3, 30, closedHatNote);
        addNoteEvent(closedHatTrack, 4, closedHatNote);
        addNoteProbEvent(closedHatTrack, 5, 10, closedHatNote);
        addNoteEvent(closedHatTrack, 6, closedHatNote);
        addNoteProbEvent(closedHatTrack, 7, 5, closedHatNote);
    }

    void tick(std::string& message, int curTick) {
        if (message == "s") {
            generatorChord.generateNextChord();
        }

        generatorChord.tick(curTick);
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

    void addNoteOffsetEvent(int seqIdx, int idx, int offset, int note) {
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

    void addNoteProbEvent(int seqIdx, int idx, int probability, int note) {
        auto& sequence = multiSequence.get(seqIdx);
        sequence.addEvent(
            idx,
            Event{
                .on = true,
                .probability = probability,
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
