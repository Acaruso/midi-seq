#pragma once

#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

#include "../main/util.hpp"
#include "beats.hpp"
#include "chords.hpp"
#include "midi_queue.hpp"
#include "rng_service.hpp"
#include "util.hpp"

struct MChord {
    int degree;
    int lowestNote;
    ChordType chordType;
    ChordInversion chordInversion;
};

static std::vector<MChord> chordsVoiceLeading = {
    MChord{1, 0, MAJOR, ROOT},           // 0
    MChord{6, 0, MINOR, FIRST_INV},      // 1
    MChord{4, 0, MAJOR, SECOND_INV},     // 2
    MChord{2, 2, MINOR, ROOT},           // 3
    MChord{5, 2, MAJOR, SECOND_INV},     // 4
    MChord{3, 4, MINOR, ROOT},           // 5
    MChord{1, 4, MAJOR, FIRST_INV},      // 6
    MChord{6, 4, MINOR, SECOND_INV},     // 7
    MChord{4, 5, MAJOR, ROOT},           // 8
    MChord{2, 5, MINOR, FIRST_INV},      // 9
    MChord{5, 7, MAJOR, ROOT},           // 10
    MChord{3, 7, MINOR, FIRST_INV},      // 11
    MChord{1, 7, MAJOR, SECOND_INV},     // 12
    MChord{6, 9, MINOR, ROOT},           // 13
    MChord{4, 9, MAJOR, FIRST_INV},      // 14
    MChord{2, 9, MINOR, SECOND_INV},     // 15
    MChord{5, 11, MAJOR, FIRST_INV},     // 16
    MChord{3, 11, MINOR, SECOND_INV},    // 17
};

struct MChord2 {
    int root;
    ChordType chordType;
};

static std::vector<MChord2> chordsInKey = {
    MChord2{0,  MAJOR},
    MChord2{2,  MINOR},
    MChord2{4,  MINOR},
    MChord2{5,  MAJOR},
    MChord2{7,  MAJOR},
    MChord2{9,  MINOR},
    MChord2{11, DIM},
};

enum ChordGeneratorMode {
    RANDOM,
    VOICE_LEADING,
    IN_KEY,
};

template <typename MidiServiceType>
class ChordGenerator {
public:
    Beats& beats;
    MidiQueue<MidiServiceType>& midiQueue;
    RngService& rngService;

    int channel;

    std::vector<int> curChord;
    int chordCounter = 0;

    int lowLimit  = guitarToMidi(S_G, 3);
    int highLimit = guitarToMidi(S_G, 9);

    int curLowestNote = 0;
    int prevLowestNote = 0;

    bool autoSwitch = true;                     // automatically go to next chord
    int numRepeats;

    ChordGeneratorMode mode = RANDOM;

    int voiceLeadingTarget;
    int voiceLeadingIdx;
    int voiceLeadingLowLimit = guitarToMidi(S_G, 2);

    bool playing;

    ChordGenerator(
        Beats& beats,
        MidiQueue<MidiServiceType>& queue,
        RngService& rngService,
        int channel
    ) :
        beats(beats),
        midiQueue(queue),
        rngService(rngService),
        channel(channel),
        voiceLeadingTarget(0),
        voiceLeadingIdx(0),
        playing(false)
    {
        generateNextChord();
    }

    void tick(std::string& message, int curTick) {
        if (message == "s") {
            playing = !playing;
        }
        if (!playing) {
            return;
        }

        if (mode == VOICE_LEADING) {
            numRepeats = 4;
        } else {
            numRepeats = 8;
        }

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
            case RANDOM:
                generateNextChordRandom();
                break;
            case VOICE_LEADING:
                generateNextChordVoiceLeading();
                break;
            case IN_KEY:
                generateNextChordInKey();
                break;
        }
    }

    void generateNextChordRandom() {
        curLowestNote = rngService.getRand(lowLimit, highLimit);
        while (curLowestNote == prevLowestNote) {
            curLowestNote = rngService.getRand(lowLimit, highLimit);
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
        MChord& c = chordsVoiceLeading[voiceLeadingIdx];
        curChord = createChordByLowestNote(
            c.lowestNote + voiceLeadingLowLimit,
            c.chordType,
            c.chordInversion
        );
    }

    void generateNextVoiceLeadingIdx() {
        while (voiceLeadingIdx == voiceLeadingTarget) {
            voiceLeadingTarget = rngService.getRand(0, chordsVoiceLeading.size() - 1);
        }
        int direction = voiceLeadingTarget > voiceLeadingIdx ? 1 : (-1);
        voiceLeadingIdx += direction;
    }

    void generateNextChordInKey() {
        curLowestNote = rngService.getRand(0, chordsInKey.size() - 1);
        while (curLowestNote == prevLowestNote) {
            curLowestNote = rngService.getRand(0, chordsInKey.size() - 1);
        }
        prevLowestNote = curLowestNote;

        MChord2& chord = chordsInKey[curLowestNote];

        curChord = createChordByRoot(
            lowLimit + chord.root,
            chord.chordType,
            ROOT
        );
    }

    void playChord(int curTick, std::vector<int>& chord, BeatUnit duration) {
        for (auto note : chord) {
            midiQueue.noteOnOff(channel, note, 100, curTick, beats.ticksPerBeat(duration));
        }
    }

    ChordType getRandChordType() {
        return (ChordType)(rngService.getRand(0, 1));
    }

    ChordInversion getRandChordInversion() {
        return (ChordInversion)(rngService.getRand(0, 2));
    }
};
