#pragma once

#include <cstdlib>
#include <ctime>
#include <iostream>
#include <string>

#include <windows.h>
#include <mmsystem.h>
#include <Avrt.h>
#pragma comment(lib, "Avrt")

#include "../lib/readerwriterqueue.h"
#include "midi_app_seq_chord.hpp"
#include "midi_app_sequence.hpp"
#include "midi_app_chord_generator.hpp"

void CALLBACK timerCallback(UINT uID, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2);

static moodycamel::ReaderWriterQueue<std::string>* queue = nullptr;

static MidiAppSequence* midiAppSequence = nullptr;
static MidiAppChordGenerator* midiAppChordGenerator = nullptr;
static MidiAppSeqChord* midiAppSeqChord = nullptr;

static std::string message;

inline int midiMain(moodycamel::ReaderWriterQueue<std::string>* _queue) {
    // seed rand
    srand(static_cast<unsigned int>(time(0)));

    queue = _queue;

    // MidiAppSequence _midiAppSequence;
    // midiAppSequence = &_midiAppSequence;

    // MidiAppChordGenerator _midiAppChordGenerator;
    // midiAppChordGenerator = &_midiAppChordGenerator;

    MidiAppSeqChord _midiAppSeqChord;
    midiAppSeqChord = &_midiAppSeqChord;

    message.reserve(16);

    DWORD taskIndex = 0;
    HANDLE hTask = AvSetMmThreadCharacteristics(TEXT("Pro Audio"), &taskIndex);
    if (hTask == NULL) {
        std::cerr << "Failed to set MMCSS characteristics" << std::endl;
        return 1;
    }

    UINT timerId = timeSetEvent(
        1,                                      // trigger the timer at this interval in ms
        0,                                      // resolution -- lower values == more accurate but more CPU intensive
        timerCallback,                          // callback
        NULL,                                   // pointer to pass to callback
        TIME_PERIODIC
    );
    if (timerId == 0) {
        std::cerr << "Failed to create multimedia timer" << std::endl;
        AvRevertMmThreadCharacteristics(hTask);
        return 1;
    }

    // Keep the program running while the timer is active
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // TODO: run these cleanup tasks
    // timeKillEvent(timerId);
    // AvRevertMmThreadCharacteristics(hTask);

    return 0;
}

inline void CALLBACK timerCallback(UINT uID, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2) {
    // TODO: refactor this to be a while loop so that we can handle multiple messages at once
    if (!queue->try_dequeue(message)) {
        message = "";
    }

    // midiAppSequence->tick(message);
    // midiAppChordGenerator->tick(message);
    midiAppSeqChord->tick(message);
}
