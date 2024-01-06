#pragma once

#include <algorithm>
#include <iostream>
#include <vector>

enum MidiEventType {
    NOTE_ON,
    NOTE_OFF,
    CC,
};

struct MidiEvent {
    MidiEventType type;
    int note;
    int velocity;
    int tick;
    int channel;        // for midi cc
    int controller;     // for midi cc
    int value;          // for midi cc
};

inline MidiEvent createNoteOnEvent(int note, int velocity, int tick) {
    MidiEvent e;
    e.type = NOTE_ON;
    e.note = note;
    e.velocity = velocity;
    e.tick = tick;
    return e;
}

inline MidiEvent createNoteOffEvent(int note, int tick) {
    MidiEvent e;
    e.type = NOTE_OFF;
    e.note = note;
    e.tick = tick;
    return e;
}

inline MidiEvent createCCEvent(int channel, int controller, int value, int tick) {
    MidiEvent e;
    e.type = CC;
    e.channel = channel;
    e.controller = controller;
    e.value = value;
    e.tick = tick;
    return e;
}

template <typename MidiServiceType>
class MidiQueue {
public:
    MidiServiceType& midiService;

    int capacity = 10000;
    int size = 0;
    std::vector<MidiEvent> events = std::vector<MidiEvent>(capacity, MidiEvent{});

    MidiQueue(MidiServiceType& m) : midiService(m) {}

    void noteOn(int note, int velocity, int tick) {
        addEvent(createNoteOnEvent(note, velocity, tick));
    }

    void noteOff(int note, int tick) {
        addEvent(createNoteOffEvent(note, tick));
    }

    void noteOnOff(int note, int velocity, int tick, int duration) {
        noteOn(note, velocity, tick);
        noteOff(note, tick + duration);
    }

    void cc(int channel, int controller, int value, int tick) {
        addEvent(createCCEvent(channel, controller, value, tick));
    }

    void addEvent(MidiEvent event) {
        if (size == capacity) {
            // TODO: handle this better
            std::cerr << "MidiQueue at capacity" << std::endl;
            return;
        }
        events[size++] = event;
        sortEvents();
    }

    void handleEvents(int curTick) {
        if (size == 0) {
            return;
        }

        while (size > 0) {
            auto& event = events[size - 1];
            if (event.tick > curTick) {
                break;
            }
            handleEvent(event);
            --size;
        }
    }

    void handleEvent(MidiEvent& event) {
        switch (event.type) {
            case NOTE_ON: {
                midiService.noteOn(event.note, event.velocity);
                break;
            }
            case NOTE_OFF: {
                midiService.noteOff(event.note);
                break;
            }
            case CC: {
                midiService.cc(event.channel, event.controller, event.value);
                break;
            }
        }
    }

private:
    // sort events by tick in decending order
    // example: [99, 50, 45, 12, 1]
    void sortEvents() {
        std::sort(
            events.begin(),
            events.begin() + size,
            [](const MidiEvent &a, const MidiEvent &b) {
                return a.tick > b.tick;
            }
        );
    }
};
