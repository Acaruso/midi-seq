#pragma once

#include <cstdlib>
#include <iostream>
#include <vector>

#include "../main/util.hpp"
#include "beats.hpp"
#include "chords.hpp"
#include "midi_queue.hpp"

struct MChord {
    int degree;
    int lowestNote;
    ChordType chordType;
    ChordInversion chordInversion;
};

static std::vector<MChord> voiceLeadingChords = {
    MChord{1, 0, MAJOR, ROOT},          // 0
    MChord{6, 0, MINOR, ROOT},          // 1
    MChord{4, 0, MAJOR, SECOND_INV},    // 2
    MChord{2, 2, MINOR, ROOT},          // 3
    MChord{5, 2, MAJOR, SECOND_INV},    // 4
    MChord{3, 4, MINOR, ROOT},          // 5
    MChord{1, 4, MAJOR, FIRST_INV},     // 6
    MChord{6, 4, MINOR, SECOND_INV},    // 7
    MChord{4, 5, MAJOR, ROOT},          // 8
    MChord{2, 5, MINOR, FIRST_INV},     // 9
    MChord{5, 7, MAJOR, ROOT},          // 10
    MChord{3, 7, MINOR, FIRST_INV},     // 11
    MChord{1, 7, MAJOR, SECOND_INV},    // 12
    MChord{6, 9, MINOR, ROOT},          // 13
    MChord{4, 9, MAJOR, FIRST_INV},     // 14
    MChord{2, 9, MINOR, SECOND_INV},    // 15
    MChord{5, 11, MAJOR, FIRST_INV},     // 16
    MChord{3, 11, MINOR, SECOND_INV},    // 17
};

enum ChordGeneratorMode {
    RANDOM,
    VOICE_LEADING,
};

template <typename MidiServiceType>
class ChordGenerator {
public:
    Beats& beats;
    MidiQueue<MidiServiceType>& midiQueue;
    int channel;

    std::vector<int> curChord;
    int chordCounter = 0;

    // int lowLimit = 48;
    // int highLimit = 54;

    int lowLimit = 58;
    int highLimit = 64;

    int curLowestNote = 0;
    int prevLowestNote = 0;

    bool autoSwitch = true;                     // automatically go to next chord
    int numRepeats = 4;
    ChordGeneratorMode mode = VOICE_LEADING;

    int voiceLeadingTarget;
    int voiceLeadingIdx;
    int voiceLeadingLowLimit = 57;              // key is A. G string == 55. A == 57 == 55 + 2

    ChordGenerator(
        Beats& beats,
        MidiQueue<MidiServiceType>& queue,
        int channel
    ) :
        beats(beats),
        midiQueue(queue),
        channel(channel),
        voiceLeadingTarget(0),
        voiceLeadingIdx(0)
    {
        generateNextChord();
    }

    void tick(int curTick) {
        if (beats.isBeat(curTick, B_4)) {
            if (autoSwitch && ((chordCounter % numRepeats) == 0)) {
                generateNextChord();
            }
            playChord(curTick, curChord, B_8);
            ++chordCounter;
        }
    }

    void generateNextChord() {
        switch (mode) {
            case RANDOM: generateNextChordRandom(); break;
            case VOICE_LEADING: generateNextChordVoiceLeading(); break;
        }
    }

    void generateNextChordRandom() {
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

    void generateNextChordVoiceLeading() {
        generateNextVoiceLeadingIdx();
        std::cout << "idx: " << voiceLeadingIdx << std::endl;
        MChord& c = voiceLeadingChords[voiceLeadingIdx];
        curChord = createChordByLowestNote(
            c.lowestNote + voiceLeadingLowLimit,
            c.chordType,
            c.chordInversion
        );
    }

    void generateNextVoiceLeadingIdx() {
        while (voiceLeadingIdx == voiceLeadingTarget) {
            voiceLeadingTarget = getRand(0, voiceLeadingChords.size() - 1);
        }
        int direction = voiceLeadingTarget > voiceLeadingIdx ? 1 : (-1);
        voiceLeadingIdx += direction;
    }

    // void generateNextVoiceLeadingIdx() {
    //     int direction = getRand(0, 1);
    //     voiceLeadingIdx += direction == 0 ? (-1) : 1;

    //     while (voiceLeadingIdx < 0 || voiceLeadingIdx >= voiceLeadingChords.size()) {
    //         direction = getRand(0, 1);
    //         voiceLeadingIdx += direction == 0 ? (-1) : 1;
    //     }
    // }

    void playChord(int curTick, std::vector<int>& chord, BeatUnit duration) {
        for (auto note : chord) {
            midiQueue.noteOnOff(channel, note, 100, curTick, beats.ticksPerBeat(duration));
        }
    }
};
