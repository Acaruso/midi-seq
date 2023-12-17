#pragma once

#include <cstdio>
#include <stdio.h>                  // printf()
#include <thread>
#include <vector>

#include <d2d1.h>
#include <dwrite.h>
#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h>               // MIDI stuff
#pragma comment(lib, "d2d1")
#pragma comment(lib, "dwrite")

#include "../lib/readerwriterqueue.h"

#include "../audio/audio_main.hpp"
#include "constants.hpp"
#include "graphics_service.hpp"
#include "util.hpp"

union Message {
    unsigned long word;
    unsigned char data[4];
};

class App {
public:
    HWND window;
    GraphicsService gfx;
    moodycamel::ReaderWriterQueue<std::string> queue;
    std::thread audioThread;

    HMIDIOUT midiDevice;
    int counter = 0;

    std::vector<UINT> messageTypes{
        WM_PAINT,
        WM_LBUTTONDOWN,
        WM_MOUSEMOVE
    };

    D2D1_RECT_F rect{100, 100, 150, 150};

    App(HWND window, HRESULT& hr) :
        window(window),
        gfx(window, hr)
    {
        audioThread = std::thread(&audioMain, &queue);
        listMidiOutputDevices();
        openMidiPort(midiDevice, 1);
    }

    bool shouldHandleMessage(UINT message) {
        for (auto elt : messageTypes) {
            if (message == elt) {
                return true;
            }
        }
        return false;
    }

    HRESULT handleMessage(UINT message, WPARAM wParam, LPARAM lParam) {
        HRESULT hr = S_OK;

        if (message == WM_PAINT) {
            hr = onPaint();
        } else if (message == WM_LBUTTONDOWN) {
            onLeftClick(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        } else if (message == WM_MOUSEMOVE) {
            onMouseMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        }

        return hr;
    }

    void tick() {
        if (getKeyState(VK_LEFT)) {
            rect = moveRect(rect, rect.left - 5, rect.top);
        }

        if (getKeyState(VK_RIGHT)) {
            rect = moveRect(rect, rect.left + 5, rect.top);
        }

        if (getKeyState(VK_UP)) {
            rect = moveRect(rect, rect.left, rect.top - 5);
        }

        if (getKeyState(VK_DOWN)) {
            rect = moveRect(rect, rect.left, rect.top + 5);
        }

        gfx.invalidateWindow();

        if (counter == 0) {
            noteOn(midiDevice);
        } else if (counter == 50) {
            noteOff(midiDevice);
        }

        counter = (counter + 1) % 100;
    }

    // midi //////////////////////////////////////////////////////////////////////////

    void openMidiPort(HMIDIOUT& device, int port) {
        int rc = midiOutOpen(
            &device,
            port,
            0,              // DWORD_PTR to callback
            0,              // DWORD_PTR to callback data
            CALLBACK_NULL   // enum -- tells windows that there's no callback
        );

        if (rc != MMSYSERR_NOERROR) {
            printf("Error opening MIDI Output\n");
            // handle error
        }
    }

    void noteOn(HMIDIOUT& device) {
        printf("noteOn\n");
        Message message;
        message.data[0] = 0x90;  // MIDI note-on message (requires to data bytes)
        message.data[1] = 60;    // MIDI note-on message: Key number (60 = middle C)
        message.data[2] = 100;   // MIDI note-on message: Key velocity (100 = loud)
        message.data[3] = 0;     // Unused parameter

        int rc = midiOutShortMsg(device, message.word);
        if (rc != MMSYSERR_NOERROR) {
            printf("Warning: MIDI Output is not open\n");
        }
    }

    void noteOff(HMIDIOUT& device) {
        printf("noteOff\n");
        Message message;
        message.data[0] = 0x90;  // MIDI note-on message (requires to data bytes)
        message.data[1] = 60;    // MIDI note-on message: Key number (60 = middle C)
        message.data[2] = 0;     // MIDI note-on message: Key velocity (0 = note off)
        message.data[3] = 0;     // Unused parameter

        int rc = midiOutShortMsg(device, message.word);
        if (rc != MMSYSERR_NOERROR) {
            printf("Warning: MIDI Output is not open\n");
        }
    }

    void listMidiOutputDevices() {
        UINT numMidiOutDevices = midiOutGetNumDevs();

        for (UINT i = 0; i < numMidiOutDevices; i++) {
            MIDIOUTCAPS midiOutCaps;
            MMRESULT result = midiOutGetDevCaps(i, &midiOutCaps, sizeof(MIDIOUTCAPS));

            if (result == MMSYSERR_NOERROR) {
                std::wcout << L"MIDI Output Device " << i << L": " << midiOutCaps.szPname << std::endl;
            } else {
                std::cerr << "Error retrieving device information for device " << i << std::endl;
            }
        }
    }

    // end midi //////////////////////////////////////////////////////////////////////

    HRESULT onPaint() {
        HRESULT hr = S_OK;

        gfx.beginDraw();

        gfx.clear();

        gfx.drawRect(rect, black);

        D2D1_RECT_F layoutRect = D2D1_RECT_F{0, 0, 100, 100};

        // const wchar_t* text = L"Hello World test 123456 sdfsfdsdfsdfsdfsdf";
        const wchar_t* text = L"I love Tracey so much!";
        gfx.drawText(text, layoutRect, 1);
        gfx.drawRect(layoutRect, blue);

        gfx.render();

        hr = gfx.endDraw();

        return hr;
    }

    void onLeftClick(int x, int y) {
        rect = moveRect(rect, x, y);
        queue.enqueue("trig");
    }

    void onMouseMove(int x, int y) {
    }

    void destroy() {
        gfx.destroy();
        queue.enqueue("quit");
        audioThread.join();
    }
};
