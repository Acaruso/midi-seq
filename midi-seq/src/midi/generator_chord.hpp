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
#include "scale.hpp"
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

class RandomChordService {
public:
    RngService& rngService;

    int root;
    int mode;
    Scale scale;

    int curLowDegree;
    int prevLowDegree;

    // how many notes are in the mode above the 2nd octave
    std::vector<int> modeUpperNotes;

    RandomChordService(
        RngService& rngService,
        int root,
        int mode
    ) :
        rngService(rngService),
        root(root),
        mode(mode),
        scale(root, mode),
        curLowDegree(0),
        prevLowDegree(0),
        modeUpperNotes({
            1,
            2,
            2,
            1,
            1,
            2,
            2,
        })
    {}

    // // generate a random (potentially inverted) chord in range [root, root + 13]
    // // where root is the root note of a major scale
    // std::vector<int> getChord() {
    //     curChordRootDegree = rngService.getRand(0, 9);
    //     while (curChordRootDegree == prevChordRootDegree) {
    //         curChordRootDegree = rngService.getRand(0, 9);
    //     }
    //     prevChordRootDegree = curChordRootDegree;

    //     int inv;
    //     if (curChordRootDegree > 6) {
    //         inv = 0;
    //     } else if (curChordRootDegree == 5 || curChordRootDegree == 6) {
    //         inv = rngService.getRand(0, 1);
    //     } else {
    //         inv = rngService.getRand(0, 2);
    //     }

    //     std::vector<int> chord = createChordByRoot(
    //         scale.getNote(curChordRootDegree),
    //         scale.getChordType(curChordRootDegree),
    //         ROOT
    //     );

    //     switch (inv) {
    //         case 0:
    //             break;
    //         case 1:
    //             chord[0] += 12;
    //             break;
    //         case 2:
    //             chord[0] += 12;
    //             chord[1] += 12;
    //             break;
    //     }

    //     std::sort(chord.begin(), chord.end());

    //     return chord;
    // }

    // generate a random (potentially inverted) chord in range [root, root + 13]
    // where root is the root note of a major scale
    std::vector<int> getChord() {
        ChordInversion inv = (ChordInversion)rngService.getRand(0, 2);

        int topNote = 13 + modeUpperNotes[mode];

        int lowDegreeLimit = 0;
        if (inv == ROOT) {
            lowDegreeLimit = topNote - 4;
        } else {
            lowDegreeLimit = topNote - 5;
        }

        getCurLowDegree(lowDegreeLimit);

        std::vector<int> chord(3, 0);

        switch (inv) {
            case ROOT:
                chord[0] = scale.getNote(curLowDegree);
                chord[1] = scale.getNote(curLowDegree + 2);
                chord[2] = scale.getNote(curLowDegree + 4);
                break;
            case FIRST_INV:
                chord[0] = scale.getNote(curLowDegree);
                chord[1] = scale.getNote(curLowDegree + 2);
                chord[2] = scale.getNote(curLowDegree + 5);
                break;
            case SECOND_INV:
                chord[0] = scale.getNote(curLowDegree);
                chord[1] = scale.getNote(curLowDegree + 3);
                chord[2] = scale.getNote(curLowDegree + 5);
                break;
        }

        return chord;
    }

    void getCurLowDegree(int lowDegreeLimit) {
        getRandIntNonRepeating(
            curLowDegree,
            prevLowDegree,
            lowDegreeLimit
        );
    }

    void getRandIntNonRepeating(int& cur, int& prev, int highLimitInclusive) {
        cur = rngService.getRand(0, highLimitInclusive);
        while (cur == prev) {
            cur = rngService.getRand(0, highLimitInclusive);
        }
        prev = cur;
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
        randomChordService(
            rngService,
            guitarToMidi(S_LOW_E, 5),
            0
        ),
        channel(channel),
        chordCounter(0),

        lowLimit(guitarToMidi(S_G, 3)),
        highLimit(guitarToMidi(S_G, 9)),

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
