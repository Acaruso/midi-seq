#pragma once

#include <iostream>

#include <windows.h>
#include <mmsystem.h>
#include <Avrt.h>
#pragma comment(lib, "Avrt")

// inline int midiMain() {
//     return 0;
// }

// Function prototype for the timer callback
void CALLBACK TimerCallback(UINT uID, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2);

// The interval for the timer in milliseconds (e.g., 1000ms = 1 second)
const UINT TIMER_INTERVAL_MS = 1000;

inline int midiMain() {
    // Initialize the Multimedia Class Scheduler Service
    DWORD taskIndex = 0;
    HANDLE hTask = AvSetMmThreadCharacteristics(TEXT("Pro Audio"), &taskIndex);
    if (hTask == NULL) {
        std::cerr << "Failed to set MMCSS characteristics" << std::endl;
        return 1;
    }

    // Set up the multimedia timer
    UINT timerId = timeSetEvent(TIMER_INTERVAL_MS, 0, TimerCallback, 0, TIME_PERIODIC);
    if (timerId == 0) {
        std::cerr << "Failed to create multimedia timer" << std::endl;
        AvRevertMmThreadCharacteristics(hTask);
        return 1;
    }

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // // Keep the program running while the timer is active
    // std::cout << "Press Enter to stop..." << std::endl;
    // std::cin.get();

    // // Clean up
    // timeKillEvent(timerId);
    // AvRevertMmThreadCharacteristics(hTask);

    return 0;
}

inline void CALLBACK TimerCallback(UINT uID, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2) {
    // Send a MIDI message here
    // For example: midiOutShortMsg(hMidiOut, MIDI_MESSAGE);

    std::cout << "MIDI message sent" << std::endl;
}
