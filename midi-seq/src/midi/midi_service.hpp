#pragma once

#include <cstdio>
#include <iostream>

#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h>           // MIDI functions

#include "midi_service_utils.hpp"

class MidiService {
public:
    HMIDIOUT midiDevice;

    MidiService() {
        printMidiOutputDevices();
    }

    MidiService(int midiPort) {
        printMidiOutputDevices();
        openMidiPort(midiPort);
    }

    void openMidiPort(int midiPort) {
        int rc = midiOutOpen(
            &midiDevice,
            midiPort,
            NULL,           // DWORD_PTR to callback
            NULL,           // DWORD_PTR to callback data
            CALLBACK_NULL   // enum -- CALLBACK_NULL tells windows that there's no callback
        );

        if (rc != MMSYSERR_NOERROR) {
            std::cerr << "Error opening MIDI Output" << std::endl;
            // TODO: handle error
        }
    }

    void noteOn(int channel, int note, int velocity) {
        MidiMessage midiMessage = createMidiMessageOn(channel, note, velocity);

        int rc = midiOutShortMsg(midiDevice, midiMessage.word);
        if (rc != MMSYSERR_NOERROR) {
            std::cerr << "Warning: MIDI Output is not open" << std::endl;
        }
    }

    void noteOff(int channel, int note) {
        MidiMessage midiMessage = createMidiMessageOff(channel, note);

        int rc = midiOutShortMsg(midiDevice, midiMessage.word);
        if (rc != MMSYSERR_NOERROR) {
            std::cerr << "Warning: MIDI Output is not open" << std::endl;
        }
    }

    // each midi port contains 16 channels
    // each channel typically corresponds to an instrument
    // each channel contains 128 controllers: 0 - 127
    // each controller corresponds to a parameter on an instrument
    // some controller numbers have "standard" mappings, for example:
    // controller 1: mod wheel
    // controller 7: volume
    // controller 10: pan
    // controller 64: sustain pedal
    // controller 70 - 119: typically safe to use for custom mappings
    // controller 102 - 119: even more safe for custom mappings
    // controller 121: reset all controllers

    void cc(int channel, int controller, int value) {
        unsigned int midiCCMessage = 0xB0 | (channel - 1);
        midiCCMessage |= (controller << 8);
        midiCCMessage |= (value << 16);

        midiOutShortMsg(midiDevice, midiCCMessage);
    }
};
