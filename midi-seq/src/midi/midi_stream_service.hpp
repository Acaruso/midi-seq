#pragma once

#include <cstdio>
#include <iostream>
#include <vector>

#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h>           // MIDI functions

#include "midi_service_utils.hpp"

// THIS DOESN'T WORK //////////////////////////////////////////////////////////////

// modern MIDI devices don't support "MIDI streams"

class MidiStreamService {
public:
    HMIDISTRM midiStream;
    UINT midiPort;
    MIDIHDR midiHeader;
    std::vector<DWORD> buffer;
    wchar_t errorMessageBuffer[128];
    bool firstError = true;

    MidiStreamService(UINT _midiPort) {
        buffer.reserve(1024);

        midiPort = _midiPort;

        printMidiOutputDevices();

        MMRESULT result = midiStreamOpen(
            &midiStream,                // pointer to stream handle
            &midiPort,                  // pointer to UINT containing MIDI port number,
                                        // aka MIDI device number
            1,                          // this should always be 1
            NULL,                       // pointer to callback
            NULL,                       // pointer to callback data
            CALLBACK_NULL               // enum -- `CALLBACK_NULL` == no callback
        );
        if (result != MMSYSERR_NOERROR) {
            // TODO: handle error
            std::cerr << "Error in midiStreamOpen" << std::endl;
        }

        // initialize MIDI header
        midiHeader.lpData = (LPSTR)buffer.data();
        midiHeader.dwBufferLength = midiHeader.dwBytesRecorded = 0;
        midiHeader.dwFlags = 0;

        // "prepare" MIDI header
        result = midiOutPrepareHeader(
            (HMIDIOUT)midiStream,           // handle to MIDI output device
            &midiHeader,                    // pointer to MIDI header -- contains buffer
            sizeof(MIDIHDR)                 // size of MIDI header
        );
        if (result != MMSYSERR_NOERROR) {
            std::cerr << "Error in midiOutPrepareHeader" << std::endl;
            midiStreamClose(midiStream);
            // return 1; // Error preparing header
        }

        if (midiHeader.dwFlags & MHDR_PREPARED) {
            std::cout << "MIDI header is prepared" << std::endl;
        }

        // start the MIDI stream
        midiStreamRestart(midiStream);
    }

    void noteOn(int channel, int note, int velocity) {
        MidiMessage midiMessage = createMidiMessageOn(channel, note, velocity);
        buffer.push_back(midiMessage.word);
    }

    void noteOff(int channel, int note) {
        MidiMessage midiMessage = createMidiMessageOff(channel, note);
        buffer.push_back(midiMessage.word);
    }

    void cc(int channel, int controller, int value) {
        // TODO: implement this
    }

    // call at beginning of each tick
    void clearOldMessages() {
        // TODO: handle this better?

        // note: if you haven't called midiStreamOut yet, then MHDR_DONE will not be set
        // MHDR_DONE is only set after midiStreamOut finishes

        if (!(midiHeader.dwFlags & MHDR_DONE)) {
            std::cerr << "MIDI buffer can't be cleared" << std::endl;
            return;
        }

        buffer.clear();
    }

    void sendMessages() {
        midiHeader.dwBufferLength = midiHeader.dwBytesRecorded = buffer.size() * sizeof(DWORD);

        MMRESULT result = midiStreamOut(
            midiStream,                         // handle to MIDI stream
            &midiHeader,                        // pointer to MIDI header
            sizeof(MIDIHDR)                     // size of MIDI header
        );

        if (result != MMSYSERR_NOERROR) {
            if (firstError) {
                std::cerr << "Error in midiStreamOut" << std::endl;
                printError(result);
            }
            firstError = false;
            midiOutUnprepareHeader((HMIDIOUT)midiStream, &midiHeader, sizeof(MIDIHDR));
            midiStreamClose(midiStream);
            // return 1; // Error sending MIDI data
        }
    }

    void printError(MMRESULT result) {
        if (mciGetErrorStringW(result, errorMessageBuffer, sizeof(errorMessageBuffer))) {
            std::wcerr << L"Error sending MIDI data: " << errorMessageBuffer << std::endl;
        } else {
            std::cerr << "Unknown MIDI error occurred." << std::endl;
        }
    }
};
