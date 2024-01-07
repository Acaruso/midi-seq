#pragma once

#include <cstdlib>
#include <iostream>
#include <vector>

#include "../main/util.hpp"
#include "beats.hpp"
#include "chords.hpp"
#include "midi_queue.hpp"

template <typename MidiServiceType>
class ChordGenerator {
public:
    Beats& beats;
    MidiQueue<MidiServiceType>& midiQueue;
    int channel;

    std::vector<int> curChord;
    int chordCounter = 0;

    // MIDI note 45 == A string on guitar == A1
    int lowLimit = 48;
    int highLimit = 54;

    int curLowestNote = 0;
    int prevLowestNote = 0;

    bool autoSwitch = false;         // automatically go to next chord after 8 repeats

    ChordGenerator(
        Beats& beats,
        MidiQueue<MidiServiceType>& queue,
        int channel
    ) :
        beats(beats),
        midiQueue(queue),
        channel(channel)
    {
        generateNextChord();
    }

    void tick(int curTick) {
        if (beats.isBeat(curTick, B_4)) {
            if (autoSwitch) {
                if ((chordCounter % 8) == 0) {
                    generateNextChord();
                }
            }
            addChord(curTick, curChord, B_8);
            ++chordCounter;
        }
    }

    void generateNextChord() {
        curLowestNote = getRand(lowLimit, highLimit);
        while (curLowestNote == prevLowestNote) {
            curLowestNote = getRand(lowLimit, highLimit);
        }
        prevLowestNote = curLowestNote;
        curChord = createChordByLowestNote(
            curLowestNote,
            getRandChordType(),
            getRandChordInversion()
        );
    }

    void addChord(int curTick, std::vector<int>& chord, BeatUnit duration) {
        for (auto note : chord) {
            midiQueue.noteOnOff(channel, note, 100, curTick, beats.ticksPerBeat(duration));
        }
    }
};
