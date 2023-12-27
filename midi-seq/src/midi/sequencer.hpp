#pragma once

#include <cstdlib>
#include <iostream>
#include <vector>

#include "../main/util.hpp"
#include "chords.hpp"
#include "midi_service.hpp"

struct SeqEvent_ {
    int note;
    int offset;
    int duration;
    bool shouldTrigger = false;
};

enum Duration {
    N_64 = 0,
    N_32 = 1,
    N_16 = 2,
    N_8  = 3,
    N_4  = 4,
    N_2  = 5,
    N_1  = 6,
};

class Sequencer {
public:
    MidiService& midiService;
    int tick = 0;

    std::vector<int> ticksPerDur = std::vector<int>(7, 0);

    // int ticksPer64Note = 24;
    int ticksPer64Note = 60;
    int ticksPerBar = ticksPer64Note * 64;

    int bdNote = 60;
    int sdNote = 61;
    int hhNote = 62;

    std::vector<SeqEvent_> events;
    std::vector<int> eventsToRemove;

    std::vector<int> curChord;
    int chordCounter = 0;

    // MIDI note 45 == A string on guitar == A1
    int chordLow = 48;
    int chordHigh = 54;

    int curRoot = 0;
    int prevRoot = 0;

    int midiCCCounter = 0;

    Sequencer(MidiService& midiService) : midiService(midiService) {
        ticksPerDur[N_64] = ticksPer64Note * 1;
        ticksPerDur[N_32] = ticksPer64Note * 2;
        ticksPerDur[N_16] = ticksPer64Note * 4;
        ticksPerDur[N_8]  = ticksPer64Note * 8;
        ticksPerDur[N_4]  = ticksPer64Note * 16;
        ticksPerDur[N_2]  = ticksPer64Note * 32;
        ticksPerDur[N_1]  = ticksPer64Note * 64;
    }

    void doTick() {
        handleEvents();

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
            midiService.cc(1, 70, 126);
            midiCCCounter = (midiCCCounter + 1) % 16;
            ++chordCounter;
        }

        // tick = (tick + 1) % ticksPerBar;
        ++tick;
    }

    // void doTick() {
    //     handleEvents();

    //     if (isNote(N_4)) {
    //         addEvent(bdNote, 100, N_8);
    //         if (getNote(N_4) % 2 != 0) {
    //             addEvent(sdNote, 100, N_8);
    //         }
    //     }

    //     if (isNote(N_8)) {
    //         addEvent(hhNote, 100, N_8);
    //         if (getNote(N_8) == 7) {
    //             addEvent(bdNote, 100, N_8);
    //         }
    //     }

    //     if (isNote(N_16)) {
    //         if (getNote(N_16) % 3 == 0) {
    //             addEvent(hhNote, 100, N_8);
    //         }
    //     }

    //     tick = (tick + 1) % ticksPerBar;
    // }

    // void doTick() {
    //     handleEvents();

    //     if (isNote(N_4)) {
    //         if (getNote(N_4) == 0) {
    //             addChord(
    //                 createChordByRoot(50, MAJOR, ROOT),
    //                 N_8
    //             );
    //         }
    //         if (getNote(N_4) == 1) {
    //             addChord(
    //                 createChordByRoot(52, MINOR, SECOND_INV),
    //                 N_8
    //             );
    //         }
    //         if (getNote(N_4) == 2) {
    //             addChord(
    //                 createChordByRoot(55, MAJOR, FIRST_INV),
    //                 N_8
    //             );
    //         }
    //         if (getNote(N_4) == 3) {
    //             addChord(
    //                 createChordByRoot(57, MAJOR, FIRST_INV),
    //                 N_8
    //             );
    //         }
    //     }

    //     tick = (tick + 1) % ticksPerBar;
    // }

    bool isNote(Duration dur) {
        return ((tick % ticksPerDur[dur]) == 0);
    }

    int getNote(Duration dur) {
        return (tick / ticksPerDur[dur]);
    }

    void addChord(std::vector<int> chord, Duration dur) {
        for (auto note : chord) {
            addEvent(note, 100, dur);
        }
    }

    void addEvent(int note, int velocity, int offset, Duration dur) {
        events.push_back(
            SeqEvent_{
                note,               // note
                offset,             // offset
                ticksPerDur[dur],   // duration
                true                // shouldTrigger
            }
        );
    }

    void addEvent(int note, int velocity, Duration dur) {
        midiService.noteOn(note, 100);
        events.push_back(
            SeqEvent_{
                note,               // note
                0,                  // offset
                ticksPerDur[dur],   // duration
                false               // shouldTrigger
            }
        );
    }

    void handleEvents() {
        for (int i = events.size() - 1; i >= 0; --i) {
            auto& event = events[i];

            if (event.offset > 0) {
                --event.offset;
            } else if (event.offset == 0 && event.shouldTrigger) {
                midiService.noteOn(event.note, 100);
                event.shouldTrigger = false;
            } else {
                --event.duration;
                if (event.duration == 0) {
                    midiService.noteOff(event.note);
                    eventsToRemove.push_back(i);
                }
            }
        }

        for (auto i : eventsToRemove) {
            events.erase(events.begin() + i);
        }

        eventsToRemove.clear();
    }
};
