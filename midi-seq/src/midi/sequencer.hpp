#pragma once

#include <cstdlib>
#include <iostream>
#include <vector>

#include "../main/util.hpp"
#include "beats.hpp"
#include "chords.hpp"
#include "sequencer_event_queue.hpp"

template <typename MidiServiceType>
class Sequencer {
public:
    SequencerEventQueue<MidiServiceType>& queue;
    Beats beats{60};

    int curTick = 0;

    int bdNote = 60;
    int sdNote = 61;
    int hhNote = 62;

    std::vector<int> curChord;
    int chordCounter = 0;

    // MIDI note 45 == A string on guitar == A1
    int chordLow = 48;
    int chordHigh = 54;

    int curRoot = 0;
    int prevRoot = 0;

    Sequencer(SequencerEventQueue<MidiServiceType>& queue) : queue(queue) {}

    void tick() {
        if (beats.isBeat(curTick, B_4)) {
            if ((chordCounter % 8) == 0) {
                curRoot = getRand(chordLow, chordHigh);
                while (curRoot == prevRoot) {
                    curRoot = getRand(chordLow, chordHigh);
                }
                prevRoot = curRoot;
                curChord = createChordByLowestNote(
                    curRoot,
                    getRandChordType(),
                    getRandChordInversion()
                );
            }
            addChord(curChord, B_8);
            ++chordCounter;
        }

        queue.handleEvents(curTick);

        ++curTick;
    }

    void addChord(std::vector<int>& chord, BeatUnit beat) {
        for (auto note : chord) {
            queue.addNoteOnEvent(note, 100, curTick);
            queue.addNoteOffEvent(note, curTick + beats.ticksPerBeat(beat));
        }
    }
};
