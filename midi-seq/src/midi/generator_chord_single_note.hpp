#pragma once

#include <algorithm>
#include <vector>

#include "beats.hpp"
#include "midi_queue.hpp"
#include "random_chord_service.hpp"
#include "rng_service.hpp"
#include "util.hpp"

enum GeneratorChordSingleNoteMode {
    NORMAL,

};

template <typename MidiServiceType>
class GeneratorChordSingleNote {
public:
    enum Mode {
        NORMAL,
        DEGREE,
    };

    Beats& beats;
    MidiQueue<MidiServiceType>& midiQueue;
    RngService& rngService;
    int root;
    int scaleMode;
    bool autoSwitch;
    Mode mode;
    bool muteSingleNoteLine;
    RandomChordService randomChordService;
    int channel;
    int counter;
    Scale scale;

    std::vector<int> curChord;

    int singleNoteLineLength;
    int singleNoteLineOffset;
    int singleNoteLineNumNotes;

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
        root(guitarToMidi(S_LOW_E, 8)),
        scaleMode(0),
        autoSwitch(false),
        mode(NORMAL),
        muteSingleNoteLine(false),
        randomChordService(rngService, root, scaleMode),
        channel(channel),
        counter(0),
        scale(root, scaleMode),

        // singleNoteLineLength(8),
        // singleNoteLineNumNotes(5),

        singleNoteLineLength(16),
        singleNoteLineNumNotes(7),

        singleNoteLineOffset(0),
        singleNoteLine(singleNoteLineLength, 0),
        singleNoteLineOnOff(singleNoteLineLength, false)
    {
        if (mode == NORMAL) {
            for (int i = 0; i <= singleNoteLineNumNotes; ++i) {
                singleNoteLineOnOff[i] = true;
            }
        } else if (mode == DEGREE) {
            int counter = 0;
            for (int i = 0; i <= singleNoteLineLength; ++i) {
                if (counter != 2 && counter != 3) {
                    singleNoteLineOnOff[i] = true;
                }
                counter = (counter == 3) ? 0 : (counter + 1);
            }
        }

        generateRandChord();
        generateRandSingleNoteLine();
    }

    void tick(std::string& message, int curTick) {
        if (message == "n") {
            generateRandChord();
            generateRandSingleNoteLine();
        } else if (message == "s") {
            generateRandSingleNoteLine();
        } else if (message == "a") {
            autoSwitch = !autoSwitch;
        } else if (message == "u") {
            muteSingleNoteLine = !muteSingleNoteLine;
        }

        if (beats.isBeat(curTick, B_8)) {
            if (autoSwitch && ((counter % 32) == 0)) {
                generateRandChord();
                generateRandSingleNoteLine();
            }

            if (!muteSingleNoteLine) {
                playSingleNote(curTick);
            }

            ++counter;
        }

        if (beats.isBeat(curTick, B_4)) {
            playChord(curTick, curChord, B_8);
        }
    }

    void generateRandChord() {
        curChord = randomChordService.getChord();
    }

    void generateRandSingleNoteLine() {
        if (mode == NORMAL) {
            shuffle(singleNoteLineOnOff);
            int direction = (rngService.getRand(0, 1) == 0) ? -1 : 1;
            int note = rngService.getRand(0, 13);

            for (int i = 0; i < singleNoteLineLength; ++i) {
                if (singleNoteLineOnOff[i]) {
                    singleNoteLine[i] = scale.getNote(note) + singleNoteLineOffset;
                    getNextNote(note, direction);
                }
            }
        } else if (mode == DEGREE) {
            int note = rngService.getRand(0, 13);
            for (int i = 0; i < singleNoteLineLength; ++i) {
                singleNoteLine[i] = scale.getNote(note) + singleNoteLineOffset;
            }
        }
    }

    void getNextNote(int& note, int& direction) {
        int newNote = note + (direction * rngService.getRand(1, 2));

        if (newNote < 0 || newNote > 13) {
            direction = direction * -1;
            newNote = note + (direction * rngService.getRand(1, 2));
        }

        note = newNote;
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
