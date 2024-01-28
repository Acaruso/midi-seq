#pragma once

#include <string>

#include "midi_queue.hpp"
#include "midi_service.hpp"
#include "module_chord.hpp"
#include "module_chord_seq.hpp"
#include "module_interval.hpp"
#include "module_single_note.hpp"

enum MidiAppMode {
    CHORD,
    INTERVAL,
    SINGLE_NOTE,
    NUM_MODES,
};

class MidiApp {
public:
    int midiPort;
    MidiService midiService;
    MidiQueue<MidiService> midiQueue;
    int curTick;

    ModuleChord<MidiService> moduleChord;
    ModuleChordSeq<MidiService> moduleChordSeq;
    ModuleSingleNote<MidiService> moduleSingleNote;
    ModuleInterval<MidiService> moduleInterval;

    MidiAppMode mode;

    MidiApp() :
        midiPort(1),
        midiService(midiPort),
        midiQueue(midiService),
        curTick(0),
        moduleChord(midiService, midiQueue),
        moduleChordSeq(midiService, midiQueue),
        moduleSingleNote(midiService, midiQueue),
        moduleInterval(midiService, midiQueue),
        mode(CHORD)
    {}

    void tick(std::string& message) {
        // handle events one tick in the past
        // on first iteration, (curTick - 1) == -1, but this is fine
        midiQueue.handleEvents(curTick - 1);

        if (message == "m") {
            mode = getNextMode(mode);
        }

        // generate events for current tick

        // generating events does a lot of modulus stuff, so it's good to do something like:
        // handleEvents(curTick - 1)
        // generateEvents(curTick)

        // rather than:
        // handleEvents(curTick)
        // generateEvents(curTick + 1)

        if (mode == CHORD) {
            moduleChord.tick(message, curTick);
        } else if (mode == INTERVAL) {
            moduleInterval.tick(message, curTick);
        } else if (mode == SINGLE_NOTE) {
            moduleSingleNote.tick(message, curTick);
        }

        // midiQueue.handleEvents(curTick);

        ++curTick;
    }

    MidiAppMode getNextMode(MidiAppMode curMode) {
        return static_cast<MidiAppMode>((curMode + 1) % NUM_MODES);
    }
};
