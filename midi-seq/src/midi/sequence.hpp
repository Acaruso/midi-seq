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
    MidiQueue<MidiServiceType>& queue;
    std::vector<Event> events;
    int size;
    BeatUnit stepSize;
    int playHead;

    Sequence(
        Beats& beats,
        MidiQueue<MidiServiceType>& queue,
        int size,
        BeatUnit stepSize
    ) :
        beats(beats),
        queue(queue),
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

    void tick(int tick) {
        if (beats.isBeat(tick, stepSize)) {
            Event& e = events[playHead];
            if (e.on) {
                handleEvent(tick, e);
            }
            playHead = ((playHead + 1) % size);
        }
    }

    void handleEvent(int tick, Event& e) {
        queue.addNoteOnEvent(e.note, e.velocity, tick);
        queue.addNoteOffEvent(e.note, tick + e.duration);
    }
};
