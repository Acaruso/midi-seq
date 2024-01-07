#pragma once

#include <string>

#include "../main/util.hpp"
#include "beats.hpp"
#include "chord_generator.hpp"
#include "midi_service.hpp"
#include "midi_queue.hpp"
#include "sequence.hpp"

class MidiAppSequence {
public:
    int channel;
    int ticksPer64Note;
    Beats beats;
    MidiService midiService;
    MidiQueue<MidiService> midiQueue;
    Sequence<MidiService> sequence;
    int curTick = 0;

    MidiAppSequence() :
        channel(2),
        ticksPer64Note(24),
        beats(ticksPer64Note),
        midiService(1),             // midi port
        midiQueue(midiService),
        sequence(
            beats,
            midiQueue,
            channel,
            8,                      // numSteps
            B_16                    // step size
        )
    {
        // addRollEventOneShot(0, 50);
        addRollEvent(0, 50);
        addNoteEvent(2, 55);
        addNoteEvent(4, 60);
        addNoteEvent(6, 65);
    }

    void tick(std::string& message) {
        if (sequence.isBeat(curTick)) {
            if (sequence.curStep == 0) {
            }
        }

        sequence.tick(curTick);

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
                .channel = channel,
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
                .channel = channel,
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
                .channel = channel,
                .subEvent = NoteEvent{
                    .note = note,
                    .velocity = 100,
                    .duration = beats.ticksPerBeat(B_16)
                }
            }
        );
    }
};
