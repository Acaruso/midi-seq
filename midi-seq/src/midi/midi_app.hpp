#pragma once

#include <string>

#include "midi_queue.hpp"
#include "midi_service.hpp"
#include "module_chord.hpp"
#include "module_chord_seq.hpp"
#include "module_chord_single_note.hpp"
#include "module_interval.hpp"
#include "module_single_note.hpp"
#include "module_stress_test.hpp"
#include "rng_service.hpp"

enum MidiAppMode {
    CHORD,
    INTERVAL,
    SINGLE_NOTE,
    CHORD_SINGLE_NOTE,
    NUM_MODES,
};

class MidiApp {
public:
    int midiPort;
    int curTick;
    MidiAppMode mode;

    MidiService midiService;
    MidiQueue<MidiService> midiQueue;
    RngService rngService;

    ModuleChord<MidiService> moduleChord;
    ModuleChordSeq<MidiService> moduleChordSeq;
    ModuleSingleNote<MidiService> moduleSingleNote;
    ModuleInterval<MidiService> moduleInterval;
    ModuleStressTest<MidiService> moduleStressTest;
    ModuleChordSingleNote<MidiService> moduleChordSingleNote;

    MidiApp() :
        midiPort(1),
        curTick(0),
        mode(CHORD),
        midiService(midiPort),
        midiQueue(midiService),
        moduleChord(midiQueue, rngService),
        moduleChordSeq(midiQueue, rngService),
        moduleSingleNote(midiQueue, rngService),
        moduleInterval(midiQueue, rngService),
        moduleStressTest(midiQueue, rngService),
        moduleChordSingleNote(midiQueue, rngService)
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

        // moduleStressTest.tick(message, curTick);

        if (mode == CHORD) {
            moduleChord.tick(message, curTick);
        } else if (mode == INTERVAL) {
            moduleInterval.tick(message, curTick);
        } else if (mode == SINGLE_NOTE) {
            moduleSingleNote.tick(message, curTick);
        } else if (mode == CHORD_SINGLE_NOTE) {
            moduleChordSingleNote.tick(message, curTick);
        }

        ++curTick;
    }

    MidiAppMode getNextMode(MidiAppMode curMode) {
        return static_cast<MidiAppMode>((curMode + 1) % NUM_MODES);
    }
};
