#pragma once

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
        randomChordService(rngService, root, mode),
        channel(channel),
        counter(0),
        scale(root, mode),
        singleNoteLineLength(8),
        singleNoteLineOffset(12),
        singleNoteLine(singleNoteLineLength, 0),
        singleNoteLineOnOff(singleNoteLineLength, false)
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
            playSingleNote(curTick);
            counter = (counter + 1) % singleNoteLineLength;
        }

        if (beats.isBeat(curTick, B_4)) {
            playChord(curTick, curChord, B_8);
        }
    }

    void generateRandChord() {
        curChord = randomChordService.getChord();
    }

    void generateRandSingleNoteLine() {
        for (int i = 0; i < singleNoteLineLength; ++i) {
            int r = rngService.getRand(0, 14);

            int note = scale.getNote(r);

            singleNoteLine[i] = singleNoteLineOffset + note;

            if (i % 2 == 0) {
                singleNoteLineOnOff[i] = getRandBool(7);
            } else {
                singleNoteLineOnOff[i] = getRandBool(2);
            }
        }
    }

    void playChord(int curTick, std::vector<int>& chord, BeatUnit duration) {
        for (auto note : chord) {
            midiQueue.noteOnOff(channel, note, 60, curTick, beats.ticksPerBeat(duration));
        }
    }

    void playSingleNote(int curTick) {
        if (singleNoteLineOnOff[counter]) {
            midiQueue.noteOnOff(channel, singleNoteLine[counter], 100, curTick, beats.ticksPerBeat(B_8));
        }
    }

    bool getRandBool(int probability) {
        int r = rngService.getRand(0, 9);
        return (r < probability);
    }
};
