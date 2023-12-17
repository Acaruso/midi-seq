#pragma once

#include <cstdio>
#include <iostream>

#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h>               // MIDI stuff

union MidiMessage {
    unsigned long word;
    unsigned char data[4];
};

inline MidiMessage createMidiMessageOn(int note, int velocity) {
    MidiMessage midiMessage;
    midiMessage.data[0] = 0x90;
    midiMessage.data[1] = note;
    midiMessage.data[2] = velocity;
    midiMessage.data[3] = 0;
    return midiMessage;
}

inline MidiMessage createMidiMessageOff(int note) {
    MidiMessage midiMessage;
    midiMessage.data[0] = 0x90;
    midiMessage.data[1] = note;
    midiMessage.data[2] = 0;
    midiMessage.data[3] = 0;
    return midiMessage;
}

class MidiService {
public:
    HMIDIOUT midiDevice;

    MidiService() {}

    MidiService(int midiPort) {
        openMidiPort(midiPort);
    }

    void openMidiPort(int midiPort) {
        int rc = midiOutOpen(
            &midiDevice,
            midiPort,
            0,              // DWORD_PTR to callback
            0,              // DWORD_PTR to callback data
            CALLBACK_NULL   // enum -- CALLBACK_NULL tells windows that there's no callback
        );

        if (rc != MMSYSERR_NOERROR) {
            std::cout << "Error opening MIDI Output" << std::endl;
            // TODO: handle error
        }
    }

    void noteOn(int note, int velocity) {
        MidiMessage midiMessage = createMidiMessageOn(note, velocity);
        int rc = midiOutShortMsg(midiDevice, midiMessage.word);
        if (rc != MMSYSERR_NOERROR) {
            std::cout << "Warning: MIDI Output is not open" << std::endl;
        }
    }

    void noteOff(int note) {
        MidiMessage midiMessage = createMidiMessageOff(note);
        int rc = midiOutShortMsg(midiDevice, midiMessage.word);
        if (rc != MMSYSERR_NOERROR) {
            std::cout << "Warning: MIDI Output is not open" << std::endl;
        }
    }

    void printMidiOutputDevices() {
        UINT numMidiOutDevices = midiOutGetNumDevs();

        for (UINT i = 0; i < numMidiOutDevices; i++) {
            MIDIOUTCAPS midiOutCaps;
            MMRESULT result = midiOutGetDevCaps(i, &midiOutCaps, sizeof(MIDIOUTCAPS));

            if (result == MMSYSERR_NOERROR) {
                std::wcout << L"MIDI Output Device " << i << L": " << midiOutCaps.szPname << std::endl;
            } else {
                std::cout << "Error retrieving device information for device " << i << std::endl;
            }
        }
    }
};
