#pragma once

#include <algorithm>
#include <iostream>
#include <vector>

enum SeqEventType {
    NOTE_ON  = 0,
    NOTE_OFF = 1,
    CC       = 2,
};

struct SeqEvent {
    SeqEventType type;
    int note;
    int velocity;
    int timestamp;
    int channel;        // for midi cc
    int controller;     // for midi cc
    int value;          // for midi cc
};

inline SeqEvent createNoteOnEvent(int note, int velocity, int timestamp) {
    SeqEvent s;
    s.type = NOTE_ON;
    s.note = note;
    s.velocity = velocity;
    s.timestamp = timestamp;
    return s;
}

inline SeqEvent createNoteOffEvent(int note, int timestamp) {
    SeqEvent s;
    s.type = NOTE_OFF;
    s.note = note;
    s.timestamp = timestamp;
    return s;
}

inline SeqEvent createCCEvent(int channel, int controller, int value, int timestamp) {
    SeqEvent s;
    s.type = CC;
    s.channel = channel;
    s.controller = controller;
    s.value = value;
    s.timestamp = timestamp;
    return s;
}

template <typename MidiServiceType>
class SequencerEventQueue {
public:
    MidiServiceType& midiService;

    int capacity = 10000;
    int size = 0;
    std::vector<SeqEvent> events = std::vector<SeqEvent>(capacity, SeqEvent{});

    SequencerEventQueue(MidiServiceType& midiService)
        : midiService(midiService)
    {}

    void addEvent(SeqEvent event) {
        if (size == capacity) {
            // TODO: handle this better
            std::cerr << "SequencerEventQueue at capacity" << std::endl;
            return;
        }
        events[size++] = event;
        sortEvents();
    }

    void addNoteOnEvent(int note, int velocity, int timestamp) {
        addEvent(createNoteOnEvent(note, velocity, timestamp));
    }

    void addNoteOffEvent(int note, int timestamp) {
        addEvent(createNoteOffEvent(note, timestamp));
    }

    void addCCEvent(int channel, int controller, int value, int timestamp) {
        addEvent(createCCEvent(channel, controller, value, timestamp));
    }

    void handleEvents(int curTimestamp) {
        if (size == 0) {
            return;
        }

        while (size > 0) {
            auto& event = events[size - 1];
            if (event.timestamp > curTimestamp) {
                break;
            }
            handleEvent(event);
            --size;
        }
    }

    void handleEvent(SeqEvent& event) {
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
    // sort events by timestamp in decending order
    // example: [99, 50, 45, 12, 1]
    void sortEvents() {
        std::sort(
            events.begin(), 
            events.begin() + size, 
            [](const SeqEvent &a, const SeqEvent &b) {
                return a.timestamp > b.timestamp;
            }
        );
    }
};
