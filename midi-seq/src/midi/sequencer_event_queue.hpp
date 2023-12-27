#pragma once

#include <algorithm>
#include <iostream>
#include <vector>

enum SeqEventType {
    NOTE_ON = 0,
    NOTE_OFF = 1,
};

struct SeqEvent {
    SeqEventType type;
    int note;
    int timestamp;
};

template <typename MidiServiceType>
class SequencerEventQueue {
public:
    MidiServiceType& midiService;

    int eventsCapacity = 10000;
    int eventsLen = 0;
    std::vector<SeqEvent> events = std::vector<SeqEvent>(eventsCapacity, SeqEvent{});

    SequencerEventQueue(MidiServiceType& _midiService)
        : midiService(_midiService)
    {}

    void addEvent(SeqEvent event) {
        if (eventsLen == eventsCapacity) {
            // TODO: handle this better
            std::cout << "events at capacity" << std::endl;
            return;
        }
        events[eventsLen] = event;
        ++eventsLen;
        sortEvents();
    }

    void handleEvents(int curTimestamp) {
        if (eventsLen == 0) {
            return;
        }

        while (eventsLen > 0) {
            auto& event = events[eventsLen - 1];
            if (event.timestamp > curTimestamp) {
                break;
            }
            handleEvent(event);
            --eventsLen;
        }
    }

    void handleEvent(SeqEvent& event) {
        switch (event.type) {
            case NOTE_ON: {
                midiService.noteOn(event.note, 100); // TODO: add velocity
                break;
            }
            case NOTE_OFF: {
                midiService.noteOff(event.note);
                break;
            }
        }
    }

private:
    void sortEvents() {
        std::sort(
            events.begin(), 
            events.begin() + eventsLen, 
            [](const SeqEvent &a, const SeqEvent &b) {
                return a.timestamp > b.timestamp;
            }
        );
    }
};
