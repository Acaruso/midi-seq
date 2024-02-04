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
    // MChord2{11, DIM},
};

static std::vector<int> majorScale = {
    0,      // 0
    2,      // 1
    4,      // 2
    5,      // 3
    7,      // 4
    9,      // 5
    11,     // 6
};

class RandomChordService {
public:
    RngService& rngService;
    int scaleRoot;
    int curChordRootDegree;
    int prevChordRootDegree;

    RandomChordService(
        RngService& rngService,
        int scaleRoot
    ) :
        rngService(rngService),
        scaleRoot(scaleRoot),
        curChordRootDegree(0),
        prevChordRootDegree(0)
    {}

    // generate a random (potentially inverted) chord in range [root, root + 13]
    // where root is the root note of a major scale
    std::vector<int> getChord() {
        curChordRootDegree = rngService.getRand(0, 6);
        while (curChordRootDegree == prevChordRootDegree) {
            curChordRootDegree = rngService.getRand(0, 6);
        }
        prevChordRootDegree = curChordRootDegree;

        int inv;

        if (curChordRootDegree == 6 || curChordRootDegree == 5) {
            inv = rngService.getRand(0, 1);
        } else {
            inv = rngService.getRand(0, 2);
        }

        std::vector<int> chord = createChordByRoot(
            majorScale[curChordRootDegree],
            chordTypes[curChordRootDegree],
            ROOT
        );

        switch (inv) {
            case 0:
                break;
            case 1:
                chord[0] += 12;
                break;
            case 2:
                chord[0] += 12;
                chord[1] += 12;
                break;
        }

        for (auto& note : chord) {
            note += scaleRoot;
        }

        std::sort(chord.begin(), chord.end());

        return chord;
    }
};

enum GeneratorChordMode {
    RANDOM,
    VOICE_LEADING,
    IN_KEY,
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

    int voiceLeadingTarget;
    int voiceLeadingIdx;
    int voiceLeadingLowLimit;

    bool playing;

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
        randomChordService(rngService, guitarToMidi(S_LOW_E, 5)),
        channel(channel),
        chordCounter(0),

        // RANDOM
        lowLimit(guitarToMidi(S_G, 3)),
        highLimit(guitarToMidi(S_G, 9)),

        // lowLimit(guitarToMidi(S_A, 5)),
        // highLimit(guitarToMidi(S_G, 7)),

        // IN_KEY
        // lowLimit(guitarToMidi(S_D, 7)),
        // highLimit(guitarToMidi(S_B, 8)),

        curLowestNote(0),
        prevLowestNote(0),
        autoSwitch(true),
        numRepeats(8),

        mode(IN_KEY),

        voiceLeadingTarget(0),
        voiceLeadingIdx(0),
        voiceLeadingLowLimit(guitarToMidi(S_G, 2)),
        playing(false),
        highNoteOnly(false)
    {
        generateNextChord();
    }

    void tick(std::string& message, int curTick) {
        if (message == " ") {
            playing = !playing;
        } else if (message == "n") {
            generateNextChord();
        } else if (message == "a") {
            autoSwitch = !autoSwitch;
        } else if (message == "h") {
            highNoteOnly = !highNoteOnly;
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

    // void generateNextChordInKey() {
    //     curLowestNote = rngService.getRand(0, chordsInKey.size() - 1);
    //     while (curLowestNote == prevLowestNote) {
    //         curLowestNote = rngService.getRand(0, chordsInKey.size() - 1);
    //     }
    //     prevLowestNote = curLowestNote;

    //     MChord2& chord = chordsInKey[curLowestNote];

    //     curChord = createChordByRoot(
    //         lowLimit + chord.root,
    //         chord.chordType,
    //         // ROOT,
    //         getRandChordInversion()
    //     );
    // }

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
        return (ChordType)(rngService.getRand(0, 1));
    }

    ChordInversion getRandChordInversion() {
        return (ChordInversion)(rngService.getRand(0, 2));
    }
};
