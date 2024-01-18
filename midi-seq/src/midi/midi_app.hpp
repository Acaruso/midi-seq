#pragma once

#include <string>

#include "midi_queue.hpp"
#include "midi_service.hpp"
#include "module_chord.hpp"
#include "module_chord_seq.hpp"
#include "module_single_note.hpp"

class MidiApp {
public:
    int midiPort;
    MidiService midiService;
    MidiQueue<MidiService> midiQueue;
    int curTick;

    ModuleChord<MidiService> moduleChord;
    ModuleChordSeq<MidiService> moduleChordSeq;
    ModuleSingleNote<MidiService> moduleSingleNote;

    MidiApp() :
        midiPort(1),
        midiService(midiPort),
        midiQueue(midiService),
        curTick(0),
        moduleChord(midiService, midiQueue),
        moduleChordSeq(midiService, midiQueue),
        moduleSingleNote(midiService, midiQueue)
    {}

    void tick(std::string& message) {
        // moduleChordSeq.tick(message, curTick);
        // moduleChord.tick(message, curTick);
        moduleSingleNote.tick(message, curTick);

        // TODO: refactor code so that handleEvents can be first
        // this will make timing more stable
        midiQueue.handleEvents(curTick);

        ++curTick;
    }
};
