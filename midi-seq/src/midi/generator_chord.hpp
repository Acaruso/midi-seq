#pragma once

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

#include "../main/util.hpp"
#include "beats.hpp"
#include "chords.hpp"
#include "midi_queue.hpp"
#include "random_chord_service.hpp"
#include "rng_service.hpp"
#include "scale.hpp"
#include "util.hpp"

struct MChord {
    int degree;
    int lowestNote;
    ChordType chordType;
    ChordInversion chordInversion;
};

static std::vector<MChord> chordsVoiceLeading = {
    MChord{1, 0,  MAJOR, ROOT},           // 0
    MChord{6, 0,  MINOR, FIRST_INV},      // 1
    MChord{4, 0,  MAJOR, SECOND_INV},     // 2
    MChord{2, 2,  MINOR, ROOT},           // 3
    MChord{5, 2,  MAJOR, SECOND_INV},     // 4
    MChord{3, 4,  MINOR, ROOT},           // 5
    MChord{1, 4,  MAJOR, FIRST_INV},      // 6
    MChord{6, 4,  MINOR, SECOND_INV},     // 7
    MChord{4, 5,  MAJOR, ROOT},           // 8
    MChord{2, 5,  MINOR, FIRST_INV},      // 9
    MChord{5, 7,  MAJOR, ROOT},           // 10
    MChord{3, 7,  MINOR, FIRST_INV},      // 11
    MChord{1, 7,  MAJOR, SECOND_INV},     // 12
    MChord{6, 9,  MINOR, ROOT},           // 13
    MChord{4, 9,  MAJOR, FIRST_INV},      // 14
    MChord{2, 9,  MINOR, SECOND_INV},     // 15
    MChord{5, 11, MAJOR, FIRST_INV},      // 16
    MChord{3, 11, MINOR, SECOND_INV},     // 17
};

enum GeneratorChordMode {
    RANDOM,
    VOICE_LEADING,
    IN_KEY,
};

enum GeneratorChordMajMinMode {
    MAJOR_MODE,
    MINOR_MODE,
    MAJOR_MINOR_MODE,
};

template <typename MidiServiceType>
class GeneratorChord {
public:
    Beats& beats;
    MidiQueue<MidiServiceType>& midiQueue;
    RngService& rngService;
    RandomChordService randomChordService;

    int channel;

    std::vector<int> curChord;
    int chordCounter;

    int lowLimit;
    int highLimit;

    int curLowestNote;
    int prevLowestNote;

    bool autoSwitch;                       // automatically go to next chord
    int numRepeats;

    GeneratorChordMode mode;
    GeneratorChordMajMinMode majMinMode;

    int voiceLeadingTarget;
    int voiceLeadingIdx;
    int voiceLeadingLowLimit;

    bool highNoteOnly;

    GeneratorChord(
        Beats& beats,
        MidiQueue<MidiServiceType>& queue,
        RngService& rngService,
        int channel
    ) :
        beats(beats),
        midiQueue(queue),
        rngService(rngService),

        // randomChordService(
        //     rngService,
        //     guitarToMidi(S_LOW_E, 5),   // root
        //     0                           // mode
        // ),

        randomChordService(
            rngService,
            guitarToMidi(S_LOW_E, 7),   // root
            5                           // mode
        ),

        channel(channel),
        chordCounter(0),

        lowLimit(guitarToMidi(S_G, 3)),
        highLimit(guitarToMidi(S_G, 9)),

        curLowestNote(0),
        prevLowestNote(0),
        autoSwitch(true),
        // numRepeats(8),
        numRepeats(4),

        mode(RANDOM),
        // mode(IN_KEY),

        majMinMode(MAJOR_MODE),

        voiceLeadingTarget(0),
        voiceLeadingIdx(0),
        voiceLeadingLowLimit(guitarToMidi(S_G, 2)),
        highNoteOnly(false)
    {
        generateNextChord();
    }

    void tick(std::string& message, int curTick) {
        if (message == "n") {
            generateNextChord();
        } else if (message == "a") {
            autoSwitch = !autoSwitch;
        } else if (message == "h") {
            highNoteOnly = !highNoteOnly;
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
        curChord = randomChordService.getChord();
    }

    void playChord(int curTick, std::vector<int>& chord, BeatUnit duration) {
        if (highNoteOnly) {
            int note = chord[chord.size() - 1];
            midiQueue.noteOnOff(channel, note, 100, curTick, beats.ticksPerBeat(duration));
        } else {
            for (auto note : chord) {
                midiQueue.noteOnOff(channel, note, 100, curTick, beats.ticksPerBeat(duration));
            }
        }
    }

    ChordType getRandChordType() {
        switch (majMinMode) {
            case MAJOR_MODE:
                return MAJOR;
            case MINOR_MODE:
                return MINOR;
            case MAJOR_MINOR_MODE:
                return (ChordType)(rngService.getRand(0, 1));
        }
    }

    ChordInversion getRandChordInversion() {
        // return (ChordInversion)(rngService.getRand(0, 2));
        return (ChordInversion)(rngService.getRand(0, 1));
    }
};
