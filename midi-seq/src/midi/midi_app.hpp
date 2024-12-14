#pragma once

#include <string>

#include "../lib/readerwriterqueue.h"

#include "midi_modes.hpp"
#include "midi_queue.hpp"
#include "midi_service.hpp"
#include "module_chord_seq.hpp"
#include "module_chord_single_note.hpp"
#include "module_chord.hpp"
#include "module_ear_training.hpp"
#include "module_interval.hpp"
#include "module_single_note.hpp"
#include "module_stress_test.hpp"
#include "rng_service.hpp"

class MidiApp {
public:
    moodycamel::ReaderWriterQueue<std::string>* midiToMainQueue;

    int midiPort;
    int curTick;
    bool playing;
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
    ModuleEarTraining<MidiService> moduleEarTraining;

    MidiApp(moodycamel::ReaderWriterQueue<std::string>* midiToMainQueue):
        midiToMainQueue(midiToMainQueue),
        midiPort(1),
        curTick(0),
        playing(false),
        mode(CHORD),
        midiService(midiPort),
        midiQueue(midiService),
        moduleChord(midiQueue, rngService),
        moduleChordSeq(midiQueue, rngService),
        moduleSingleNote(midiQueue, rngService),
        moduleInterval(midiQueue, rngService),
        moduleStressTest(midiQueue, rngService),
        moduleChordSingleNote(midiQueue, rngService),
        moduleEarTraining(midiQueue, rngService)
    {}

    void tick(std::string& message) {
        // handle events one tick in the past
        // on first iteration, (curTick - 1) == -1, but this is fine

        midiQueue.handleEvents(curTick - 1);

        if (message == "m") {
            mode = getNextMode(mode);
        } else if (message == " ") {
            playing = !playing;
        }

        if (!playing) {
            return;
        }

        // generate events for current tick

        // generating events does a lot of modulus stuff, so it's good to do something like:
        // handleEvents(curTick - 1)
        // generateEvents(curTick)

        // rather than:
        // handleEvents(curTick)
        // generateEvents(curTick + 1)

        // moduleStressTest.tick(message, curTick);

        switch (mode) {
            case CHORD:
                moduleChord.tick(message, curTick);
                break;
            case INTERVAL:
                moduleInterval.tick(message, curTick);
                break;
            case SINGLE_NOTE:
                moduleSingleNote.tick(message, curTick);
                break;
            case CHORD_SINGLE_NOTE:
                moduleChordSingleNote.tick(message, curTick);
                break;
            case EAR_TRAINING:
                moduleEarTraining.tick(message, curTick);
                break;
            case NUM_MODES:
                break;
        }

        ++curTick;
    }
};
