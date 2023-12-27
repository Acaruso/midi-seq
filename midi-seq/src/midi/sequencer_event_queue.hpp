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

class SequencerEventQueue {
public:
    int eventsCapacity = 10000;
    int eventsLen = 0;
    std::vector<SeqEvent> events = std::vector<SeqEvent>(eventsCapacity, SeqEvent{});

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
                break;
            }
            case NOTE_OFF: {
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
