#pragma once

#include <algorithm>
#include <vector>

#include "beats.hpp"
#include "midi_queue.hpp"
#include "random_chord_service.hpp"
#include "rng_service.hpp"
#include "util.hpp"

template <typename MidiServiceType>
class GeneratorChordSingleNote {
public:
    Beats& beats;
    MidiQueue<MidiServiceType>& midiQueue;
    RngService& rngService;
    int root;
    int mode;
    bool autoSwitch;
    RandomChordService randomChordService;
    int channel;
    int counter;
    Scale scale;

    std::vector<int> curChord;

    int singleNoteLineLength;
    int singleNoteLineOffset;

    std::vector<int> singleNoteLine;
    std::vector<bool> singleNoteLineOnOff;

    GeneratorChordSingleNote(
        Beats& beats,
        MidiQueue<MidiServiceType>& midiQueue,
        RngService& rngService,
        int channel
    ) :
        beats(beats),
        midiQueue(midiQueue),
        rngService(rngService),
        root(guitarToMidi(S_LOW_E, 5)),
        mode(0),
        autoSwitch(true),
        randomChordService(rngService, root, mode),
        channel(channel),
        counter(0),
        scale(root, mode),
        singleNoteLineLength(8),
        singleNoteLineOffset(0),
        singleNoteLine(singleNoteLineLength, 0),
        singleNoteLineOnOff({true, true, true, true, true, false, false, false})
    {
        generateRandChord();
        generateRandSingleNoteLine();
    }

    void tick(std::string& message, int curTick) {
        if (message == "n") {
            generateRandChord();
        } else if (message == "s") {
            generateRandSingleNoteLine();
        }

        if (beats.isBeat(curTick, B_8)) {
            if (autoSwitch && ((counter % 32) == 0)) {
                generateRandChord();
                generateRandSingleNoteLine();
            }

            playSingleNote(curTick);
            ++counter;
        }

        if (beats.isBeat(curTick, B_4)) {
            playChord(curTick, curChord, B_8);
        }
    }

    void generateRandChord() {
        curChord = randomChordService.getChord();
    }

    // void generateRandSingleNoteLine() {
    //     int r = rngService.getRand(0, 14);

    //     for (int i = 0; i < singleNoteLineLength; ++i) {
    //         int inc = rngService.getRand(0, 4);

    //         r += inc + getSingleNoteRandOffset(r);

    //         int note = scale.getNote(r);

    //         singleNoteLine[i] = singleNoteLineOffset + note;

    //         shuffle(singleNoteLineOnOff);
    //     }
    // }

    // int getSingleNoteRandOffset(int r) {
    //     switch (r) {
    //         case 14: return -4;
    //         case 13: return -3;
    //         case 12: return -2;
    //         case 11: return -1;
    //         default: return 0;
    //     }
    // }

    void generateRandSingleNoteLine() {
        int r = rngService.getRand(0, 14);
        int inc = 0;

        for (int i = 0; i < singleNoteLineLength; ++i) {
            if (r == 0) {
                inc = 1;
            } else if (r == 14) {
                inc = -1;
            } else {
                int r2 = rngService.getRand(0, 1);
                inc = (r2 == 0) ? -1 : 1;
            }

            r += inc;

            int note = scale.getNote(r);

            singleNoteLine[i] = singleNoteLineOffset + note;

            shuffle(singleNoteLineOnOff);
        }
    }

    void playChord(int curTick, std::vector<int>& chord, BeatUnit duration) {
        for (auto note : chord) {
            midiQueue.noteOnOff(channel, note, 70, curTick, beats.ticksPerBeat(duration));
        }
    }

    void playSingleNote(int curTick) {
        int i = counter % singleNoteLineLength;

        if (singleNoteLineOnOff[i]) {
            midiQueue.noteOnOff(channel, singleNoteLine[i], 100, curTick, beats.ticksPerBeat(B_8));
        }
    }

    bool getRandBool(int probability) {
        int r = rngService.getRand(0, 9);
        return (r < probability);
    }

    void shuffle(std::vector<bool>& v) {
        bool t;
        for (int i = v.size() - 1; i > 0; --i) {
            int r = rngService.getRand(0, i);
            t = v[i];
            v[i] = v[r];
            v[r] = t;
        }
    }
};

