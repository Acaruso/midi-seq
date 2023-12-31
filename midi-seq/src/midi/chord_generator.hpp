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

    std::vector<int> curChord;
    int chordCounter = 0;

    // MIDI note 45 == A string on guitar == A1
    int lowLimit = 48;
    int highLimit = 54;

    int curLowestNote = 0;
    int prevLowestNote = 0;

    ChordGenerator(Beats& beats, MidiQueue<MidiServiceType>& queue) :
        beats(beats),
        midiQueue(queue)
    {}

    void tick(int curTick) {
        if (beats.isBeat(curTick, B_4)) {
            if ((chordCounter % 8) == 0) {
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
            addChord(curTick, curChord, B_8);
            ++chordCounter;
        }
    }

    void addChord(int curTick, std::vector<int>& chord, BeatUnit beat) {
        for (auto note : chord) {
            midiQueue.addNoteOnEvent(note, 100, curTick);
            midiQueue.addNoteOffEvent(note, curTick + beats.ticksPerBeat(beat));
        }
    }
};
