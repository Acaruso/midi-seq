#pragma once

#include <string>

#include "midi_queue.hpp"
#include "midi_service.hpp"
#include "module_chord.hpp"
#include "module_chord_seq.hpp"

class MidiApp {
public:
    int midiPort;
    MidiService midiService;
    MidiQueue<MidiService> midiQueue;
    int curTick;

    ModuleChord<MidiService> moduleChord;
    ModuleChordSeq<MidiService> moduleChordSeq;

    MidiApp() :
        midiPort(1),
        midiService(midiPort),
        midiQueue(midiService),
        curTick(0),
        moduleChord(midiService, midiQueue),
        moduleChordSeq(midiService, midiQueue)
    {}

    void tick(std::string& message) {
        // moduleChord.tick(message, curTick);
        moduleChordSeq.tick(message, curTick);

        // TODO: refactor code so that handleEvents can be first
        // this will make timing more stable
        midiQueue.handleEvents(curTick);

        ++curTick;
    }
};
