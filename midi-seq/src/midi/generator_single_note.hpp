#pragma once

#include <cmath>
#include <string>
#include <vector>

#include "../main/util.hpp"
#include "beats.hpp"
#include "midi_queue.hpp"
#include "rng_service.hpp"
#include "util.hpp"

template <typename MidiServiceType>
class GeneratorSingleNote {
public:
    Beats& beats;
    MidiQueue<MidiServiceType>& midiQueue;
    RngService& rngService;

    int channel;
    int counter;
    int curNote;
    int prevNote;
    int lowNote;
    int highNote;
    std::vector<int> intervalBlacklist;
    bool playing;

    GeneratorSingleNote(
        Beats& beats,
        MidiQueue<MidiServiceType>& queue,
        RngService& rngService,
        int channel
    ) :
        beats(beats),
        midiQueue(queue),
        rngService(rngService),
        channel(channel),
        counter(0),
        curNote(0),
        prevNote(0),

        lowNote(guitarToMidi(S_A, 0)),
        highNote(guitarToMidi(S_G, 3)),

        // lowNote(guitarToMidi(S_D, 0)),
        // highNote(guitarToMidi(S_B, 4)),

        // lowNote(guitarToMidi(S_G, 0)),
        // highNote(guitarToMidi(S_HIGH_E, 4)),

        intervalBlacklist({0, 1, 2, 12}),
        // intervalBlacklist({0, 1, 2, 3, 4, 5, 6, 7, 12}),
        playing(false)
    {
        generateNextNote();
    }

    void tick(std::string& message, int curTick) {
        if (message == " ") {
            playing = !playing;
        }
        if (!playing) {
            return;
        }

        if (beats.isBeat(curTick, B_4)) {
            if (((counter % 4) == 0)) {
                generateNextNote();
            }
            playNote(curTick, curNote, B_8);
            ++counter;
        }
    }

    void generateNextNote() {
        curNote = rngService.getRand(lowNote, highNote);
        while (checkNote(prevNote, curNote)) {
            curNote = rngService.getRand(lowNote, highNote);
        }
        prevNote = curNote;
    }

    bool checkNote(int prevNote, int curNote) {
        int absDiff = abs(prevNote - curNote);
        for (auto interval : intervalBlacklist) {
            if (absDiff == interval) {
                return true;
            }
        }
        return false;
    }

    void playNote(int curTick, int note, BeatUnit duration) {
        midiQueue.noteOnOff(channel, note, 100, curTick, beats.ticksPerBeat(duration));
    }
};
