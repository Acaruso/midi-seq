#pragma once

#include <iostream>

#include <windows.h>
#include <mmsystem.h>
#include <Avrt.h>
#pragma comment(lib, "Avrt")

#include "midi_service.hpp"
#include "sequencer.hpp"

void CALLBACK timerCallback(UINT uID, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2);

// const UINT TIMER_INTERVAL_MS = 1000;
const UINT TIMER_INTERVAL_MS = 1;

inline int midiMain() {
    MidiService midiService(1);
    Sequencer sequencer(midiService);

    DWORD taskIndex = 0;
    HANDLE hTask = AvSetMmThreadCharacteristics(TEXT("Pro Audio"), &taskIndex);
    if (hTask == NULL) {
        std::cerr << "Failed to set MMCSS characteristics" << std::endl;
        return 1;
    }

    UINT timerId = timeSetEvent(
        TIMER_INTERVAL_MS,              // trigger the timer at this interval in ms
        0,                              // resolution -- lower values == more accurate but more CPU intensive
        timerCallback,                  // callback fn
        (DWORD_PTR)(&sequencer),        // pointer to "user data" -- gets passed into `timerCallback` as `dwUser` param
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
    // std::cout << "MIDI message sent" << std::endl;
    Sequencer* sequencer = (Sequencer*)dwUser;
    sequencer->doTick();
}
