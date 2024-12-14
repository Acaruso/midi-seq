#pragma once

#include <string>
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

#include "../midi/midi_main.hpp"
#include "../midi/midi_modes.hpp"
#include "constants.hpp"
#include "graphics_service.hpp"
#include "util.hpp"

class App {
public:
    HWND window;
    GraphicsService gfx;
    // TODO: rename `queue` to `audioQueue`
    // moodycamel::ReaderWriterQueue<std::string> queue;
    moodycamel::ReaderWriterQueue<std::string> midiQueue;
    std::thread audioThread;
    std::thread midiThread;
    MidiAppMode mode;
    std::wstring modeStr;

    std::vector<UINT> messageTypes{
        WM_PAINT,
        WM_LBUTTONDOWN,
        WM_MOUSEMOVE,
        WM_KEYDOWN
    };

    App(HWND window, HRESULT& hr):
        window(window),
        gfx(window, hr),
        mode(CHORD),
        modeStr(L"Mode: " + modeToString(mode))
    {
        // audioThread = std::thread(&audioMain, &queue);
        midiThread = std::thread(&midiMain, &midiQueue);
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

        switch (message) {
            case WM_PAINT: {
                hr = onPaint();
                break;
            }
            case WM_LBUTTONDOWN: {
                onLeftClick(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
                break;
            }
            case WM_MOUSEMOVE: {
                onMouseMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
                break;
            }
            case WM_KEYDOWN: {
                int keycode = wParam;
                if (keycode == VK_SPACE) {
                    midiQueue.enqueue(" ");
                } else if (keycode == (int('M'))) {
                    midiQueue.enqueue("m");
                    changeMode();
                } else if (keycode == (int('N'))) {
                    midiQueue.enqueue("n");
                } else if (keycode == (int('A'))) {
                    midiQueue.enqueue("a");
                } else if (keycode == (int('H'))) {
                    midiQueue.enqueue("h");
                } else if (keycode == (int('S'))) {
                    midiQueue.enqueue("s");
                } else if (keycode == (int('U'))) {
                    midiQueue.enqueue("u");
                }
                break;
            }
        }

        return hr;
    }

    void tick() {
        gfx.invalidateWindow();
    }

    HRESULT onPaint() {
        HRESULT hr = S_OK;
        gfx.beginDraw();
        gfx.clear();

        D2D1_RECT_F layoutRect = D2D1_RECT_F{0, 0, 400, 50};
        gfx.drawText(modeStr.c_str(), layoutRect, 1);
        gfx.drawRect(layoutRect, blue);

        gfx.render();
        hr = gfx.endDraw();
        return hr;
    }

    void onLeftClick(int x, int y) {
    }

    void onMouseMove(int x, int y) {
    }

    void destroy() {
        gfx.destroy();

        // queue.enqueue("quit");
        // audioThread.join();

        midiQueue.enqueue("q");
        midiThread.join();
    }

    void changeMode() {
        mode = getNextMode(mode);
        modeStr = L"Mode: " + modeToString(mode);
    }
};
