#pragma once

#include <iostream>

#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h>           // MIDI functions

union MidiMessage {
    unsigned long word;
    unsigned char data[4];
};

// pass in channel as an int in range [1, 16] inclusive
// however, internally, channel is represented as an int in range [0, 15] inclusive
inline MidiMessage createMidiMessageOn(int channel, int note, int velocity) {
    MidiMessage midiMessage;
    midiMessage.data[0] = 0x90 | (channel - 1);
    midiMessage.data[1] = note;
    midiMessage.data[2] = velocity;
    midiMessage.data[3] = 0;
    return midiMessage;
}

inline MidiMessage createMidiMessageOff(int channel, int note) {
    MidiMessage midiMessage;
    midiMessage.data[0] = 0x80 | (channel - 1);
    midiMessage.data[1] = note;
    midiMessage.data[2] = 0;
    midiMessage.data[3] = 0;
    return midiMessage;
}

inline void printMidiOutputDevices() {
    UINT numMidiOutDevices = midiOutGetNumDevs();

    for (UINT i = 0; i < numMidiOutDevices; i++) {
        MIDIOUTCAPS midiOutCaps;
        MMRESULT result = midiOutGetDevCaps(i, &midiOutCaps, sizeof(MIDIOUTCAPS));

        if (result == MMSYSERR_NOERROR) {
            std::wcout << L"MIDI Output Device " << i << L": " << midiOutCaps.szPname << std::endl;
            if (midiOutCaps.dwSupport & MIDICAPS_STREAM) {
                std::cout << "This device supports midiStreamOut()" << std::endl;
            } else {
                std::cout << "This device doesn't support midiStreamOut()" << std::endl;
            }
        } else {
            std::cerr << "Error retrieving device information for device " << i << std::endl;
        }
    }
}
