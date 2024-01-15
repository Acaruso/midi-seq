#pragma once

#include <cmath>
#include <vector>

#include "../main/util.hpp"
#include "beats.hpp"
#include "midi_queue.hpp"
#include "util.hpp"

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
    std::vector<int> intervalBlacklist;

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
        // lowNote(45),
        // highNote(58),
        lowNote(guitarToMidi(S_A, 0)),
        highNote(guitarToMidi(S_G, 3)),
        intervalBlacklist({0, 1, 2, 12})
        // intervalBlacklist({0, 1, 2, 3, 4, 5, 6, 7, 12})
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
        int absDiff = abs(prevNote - curNote);
        for (auto interval : intervalBlacklist) {
            if (absDiff == interval) {
                return true;
            }
        }
        return false;
    }

    void playNote(int curTick, int note, BeatUnit duration) {
        midiQueue.noteOnOff(channel, note, 100, curTick, beats.ticksPerBeat(duration));
    }
};
