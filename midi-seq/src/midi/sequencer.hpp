#pragma once

#include <iostream>
#include <vector>

#include "midi_service.hpp"

struct SeqEvent {
    int note;
    int duration;
};

class Sequencer {
public:
    MidiService& midiService;
    int tick = 0;
    int ticksPer64Note = 24;
    int ticksPer32Note = ticksPer64Note * 2;
    int ticksPer16Note = ticksPer32Note * 2;
    int ticksPer8Note  = ticksPer16Note * 2;
    int ticksPer4Note  = ticksPer8Note  * 2;

    int ticksPerBar = ticksPer4Note * 4;

    int bdNote = 60;
    int sdNote = 61;
    int hhNote = 62;

    std::vector<SeqEvent> events;
    std::vector<int> eventsToRemove;

    Sequencer(MidiService& midiService) : midiService(midiService) {}

    void doTick() {
        handleEvents();

        if (is4Note()) {
            int n4 = get4Note();
            addEvent(bdNote, 100, ticksPer8Note);
            if (n4 % 2 != 0) {
                addEvent(sdNote, 100, ticksPer8Note);
            }
        }

        if (is8Note()) {
            addEvent(hhNote, 100, ticksPer8Note);
            int n8 = get8Note();
            if (n8 == 7) {
                addEvent(bdNote, 100, ticksPer8Note);
            }
        }

        if (is16Note()) {
            int n16 = get16Note();
            if (n16 % 3 == 0) {
                addEvent(hhNote, 100, ticksPer8Note);
            }
        }

        if (is32Note()) {
            int n32 = get32Note();
            if (n32 < 8) {
                addEvent(hhNote, 100, ticksPer32Note);
            }
        }

        tick = (tick + 1) % ticksPerBar;
    }

    bool is4Note() {
        return ((tick % ticksPer4Note) == 0);
    }

    int get4Note() {
        return tick / ticksPer4Note;
    }

    bool is8Note() {
        return ((tick % ticksPer8Note) == 0);
    }

    int get8Note() {
        return tick / ticksPer8Note;
    }

    bool is16Note() {
        return ((tick % ticksPer16Note) == 0);
    }

    int get16Note() {
        return tick / ticksPer16Note;
    }

    bool is32Note() {
        return ((tick % ticksPer32Note) == 0);
    }

    int get32Note() {
        return tick / ticksPer32Note;
    }

    void addEvent(int note, int velocity, int duration) {
        midiService.noteOn(note, 100);
        events.push_back(SeqEvent{note, duration});
    }

    void handleEvents() {
        for (int i = events.size() - 1; i >= 0; --i) {
            auto& event = events[i];
            --event.duration;
            if (event.duration == 0) {
                midiService.noteOff(event.note);
                eventsToRemove.push_back(i);
            }
        }

        for (auto i : eventsToRemove) {
            events.erase(events.begin() + i);
        }

        eventsToRemove.clear();
    }
};
