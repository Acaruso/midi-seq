#pragma once

#include <cstdlib>
#include <iostream>
#include <vector>

#include "../main/util.hpp"
#include "chords.hpp"
#include "sequencer_event_queue.hpp"

enum Duration {
    N_64 = 0,
    N_32 = 1,
    N_16 = 2,
    N_8  = 3,
    N_4  = 4,
    N_2  = 5,
    N_1  = 6,
};

template <typename MidiServiceType>
class Sequencer {
public:
    SequencerEventQueue<MidiServiceType>& queue;
    int tick = 0;

    std::vector<int> ticksPerDur = std::vector<int>(7, 0);

    int ticksPer64Note = 60;
    int ticksPerBar = ticksPer64Note * 64;

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

    Sequencer(SequencerEventQueue<MidiServiceType>& queue) 
        : queue(queue) 
    {
        ticksPerDur[N_64] = ticksPer64Note * 1;
        ticksPerDur[N_32] = ticksPer64Note * 2;
        ticksPerDur[N_16] = ticksPer64Note * 4;
        ticksPerDur[N_8]  = ticksPer64Note * 8;
        ticksPerDur[N_4]  = ticksPer64Note * 16;
        ticksPerDur[N_2]  = ticksPer64Note * 32;
        ticksPerDur[N_1]  = ticksPer64Note * 64;
    }

    void doTick() {
        if (isNote(N_4)) {
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
            addChord(curChord, N_8);
            ++chordCounter;
        }

        queue.handleEvents(tick);

        ++tick;
    }

    bool isNote(Duration dur) {
        return ((tick % ticksPerDur[dur]) == 0);
    }

    int getNote(Duration dur) {
        return (tick / ticksPerDur[dur]);
    }

    void addChord(std::vector<int>& chord, Duration dur) {
        for (auto note : chord) {
            queue.addNoteOnEvent(note, 100, tick);
            queue.addNoteOffEvent(note, tick + ticksPerDur[dur]);
        }
    }
};
