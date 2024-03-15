#pragma once

#include <iostream>
#include <string>

#include <windows.h>
#include <mmsystem.h>
#include <Avrt.h>
#pragma comment(lib, "Avrt")

#include "../lib/readerwriterqueue.h"
#include "midi_app.hpp"

void CALLBACK timerCallback(UINT uID, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2);

static moodycamel::ReaderWriterQueue<std::string>* queue = nullptr;
static std::string message;
static MidiApp* midiApp = nullptr;
static bool running = true;

inline int midiMain(moodycamel::ReaderWriterQueue<std::string>* _queue) {
    queue = _queue;

    MidiApp _midiApp;
    midiApp = &_midiApp;

    message.reserve(16);

    // DWORD taskIndex = 0;
    // HANDLE hTask = AvSetMmThreadCharacteristics(TEXT("Pro Audio"), &taskIndex);
    // if (hTask == NULL) {
    //     std::cerr << "Failed to set MMCSS characteristics" << std::endl;
    //     return 1;
    // }

    // if (!SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST)) {
    //     std::cerr << "Failed to set high thread priority." << std::endl;
    // }

    UINT desiredResolution = 1;
    timeBeginPeriod(desiredResolution);

    UINT timerId = timeSetEvent(
        desiredResolution,                      // trigger the timer at this interval in ms
        desiredResolution,                      // resolution -- lower values == more accurate but more CPU intensive
        timerCallback,                          // callback
        NULL,                                   // pointer to pass to callback
        TIME_PERIODIC
    );
    // if (timerId == 0) {
    //     std::cerr << "Failed to create multimedia timer" << std::endl;
    //     AvRevertMmThreadCharacteristics(hTask);
    //     return 1;
    // }

    // Keep the program running while the timer is active

    // MSG msg;
    // while (GetMessage(&msg, NULL, 0, 0) > 0) {
    //     TranslateMessage(&msg);
    //     DispatchMessage(&msg);
    // }

    while (running) {
        Sleep(10);
    }

    timeKillEvent(timerId);
    // AvRevertMmThreadCharacteristics(hTask);
    std::cout << "exiting midi thread" << std::endl;

    return 0;
}

inline void CALLBACK timerCallback(UINT uID, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2) {
    // TODO: refactor this to be a while loop so that we can handle multiple messages at once
    if (message == "q" || (running == false)) {
        running = false;
    } else {
        midiApp->tick(message);
    }

    if (!queue->try_dequeue(message)) {
        message = "";
    }
}
