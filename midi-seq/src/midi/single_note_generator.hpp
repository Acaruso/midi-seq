#pragma once

#include "../main/util.hpp"
#include "beats.hpp"
#include "midi_queue.hpp"

template <typename MidiServiceType>
class SingleNoteGenerator {
public:
    Beats& beats;
    MidiQueue<MidiServiceType>& midiQueue;
    int channel;
    int counter;
    int curNote;
    int prevNote;
    int lowNote;
    int highNote;

    SingleNoteGenerator(
        Beats& beats,
        MidiQueue<MidiServiceType>& queue,
        int channel
    ) :
        beats(beats),
        midiQueue(queue),
        channel(channel),
        counter(0),
        curNote(0),
        prevNote(0),
        lowNote(45),
        highNote(58)
    {
        generateNextNote();
    }

    void tick(int curTick) {
        if (beats.isBeat(curTick, B_4)) {
            if (((counter % 4) == 0)) {
                generateNextNote();
            }
            playNote(curTick, curNote, B_8);
            ++counter;
        }
    }

    void generateNextNote() {
        curNote = getRand(lowNote, highNote);
        while (checkNote(prevNote, curNote)) {
            curNote = getRand(lowNote, highNote);
        }
        prevNote = curNote;
    }

    bool checkNote(int prevNote, int curNote) {
        return (
            prevNote == curNote
            || prevNote == curNote + 1
            || prevNote == curNote - 1
            || prevNote == curNote + 2
            || prevNote == curNote - 2
        );
    }

    void playNote(int curTick, int note, BeatUnit duration) {
        midiQueue.noteOnOff(channel, note, 100, curTick, beats.ticksPerBeat(duration));
    }
};
