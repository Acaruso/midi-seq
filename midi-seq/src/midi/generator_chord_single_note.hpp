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
    bool playing;
    std::vector<int> curChord;

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
        playing(false)
    {
        curChord = randomChordService.getChord();
    }

    void tick(std::string& message, int curTick) {
        if (message == " ") {
            playing = !playing;
        } else if (message == "n") {
            curChord = randomChordService.getChord();
        }

        if (!playing) {
            return;
        }

        if (beats.isBeat(curTick, B_4)) {
            playChord(curTick, curChord, B_8);
        }
    }

    void playChord(int curTick, std::vector<int>& chord, BeatUnit duration) {
        for (auto note : chord) {
            midiQueue.noteOnOff(channel, note, 100, curTick, beats.ticksPerBeat(duration));
        }
    }
};
