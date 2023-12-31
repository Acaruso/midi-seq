#pragma once

#include <iostream>
#include <vector>
#include "beats.hpp"
#include "midi_queue.hpp"

class Event {
public:
    bool on = false;
    int note = 0;
    int velocity = 0;
    int duration = 0;
};

template <typename MidiServiceType>
class Sequence {
public:
    Beats& beats;
    MidiQueue<MidiServiceType>& midiQueue;
    std::vector<Event> events;
    int size;
    BeatUnit stepSize;
    int playHead;

    Sequence(
        Beats& beats,
        MidiQueue<MidiServiceType>& midiQueue,
        int size,
        BeatUnit stepSize
    ) :
        beats(beats),
        midiQueue(midiQueue),
        events(size, Event{}),
        size(size),
        stepSize(stepSize),
        playHead(0)
    {}

    void addEvent(int idx, int note, int velocity, int duration) {
        if (idx >= size) {
            // TODO: handle this better
            std::cerr << "Sequence: idx >= size" << std::endl;
            return;
        }

        events[idx] = Event{true, note, velocity, duration};
    }

    void tick(int curTick) {
        if (beats.isBeat(curTick, stepSize)) {
            Event& event = events[playHead];
            if (event.on) {
                handleEvent(curTick, event);
            }
            playHead = ((playHead + 1) % size);
        }
    }

    void handleEvent(int curTick, Event& event) {
        midiQueue.addNoteOnEvent(event.note, event.velocity, curTick);
        midiQueue.addNoteOffEvent(event.note, curTick + event.duration);
    }
};
