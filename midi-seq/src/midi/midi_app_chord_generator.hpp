#pragma once

#include <iostream>
#include <string>

#include "../main/util.hpp"
#include "beats.hpp"
#include "chord_generator.hpp"
#include "midi_service.hpp"
#include "midi_queue.hpp"
#include "sequence.hpp"

class MidiAppChordGenerator {
public:
    int ticksPer64Note;
    Beats beats;
    MidiService midiService;
    MidiQueue<MidiService> midiQueue;
    ChordGenerator<MidiService> chordGenerator;
    int curTick = 0;

    MidiAppChordGenerator() :
        ticksPer64Note(60),
        beats(ticksPer64Note),
        midiService(1),                         // midi port
        midiQueue(midiService),
        chordGenerator(beats, midiQueue, 1)
    {}

    void tick(std::string& message) {
        if (message == "s") {
            chordGenerator.generateNextChord();
        }

        chordGenerator.tick(curTick);

        // TODO: refactor code so that handleEvents can be first
        // this will make timing more stable
        midiQueue.handleEvents(curTick);

        ++curTick;
    }
};
