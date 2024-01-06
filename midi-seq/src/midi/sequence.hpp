#pragma once

#include <iostream>
#include <variant>
#include <vector>
#include "beats.hpp"
#include "midi_queue.hpp"

struct NoteEvent {
    int note = 0;
    int velocity = 0;
    int duration = 0;
};

struct RollEvent {
    int note = 0;
    int velocity = 0;
    int numRepeats = 0;
    int totalDuration = 0;
    int restDuration = 0;
};

using SubEvent = std::variant<NoteEvent, RollEvent>;

struct Event {
    bool on = false;
    bool oneShot = false;
    int offset = 0;             // TODO: handle offset
    SubEvent subEvent;
};

template <typename MidiServiceType>
class Sequence {
public:
    Beats& beats;
    MidiQueue<MidiServiceType>& midiQueue;
    std::vector<Event> events;
    BeatUnit stepSize;
    int numSteps;
    int curStep;

    Sequence(
        Beats& beats,
        MidiQueue<MidiServiceType>& midiQueue,
        int size,
        BeatUnit stepSize
    ) :
        beats(beats),
        midiQueue(midiQueue),
        events(size, Event{}),
        stepSize(stepSize),
        numSteps(size),
        curStep(0)
    {}

    void addEvent(int idx, Event event) {
        if (idx >= numSteps) {
            // TODO: handle this better
            std::cerr << "Sequence: idx >= size" << std::endl;
            return;
        }
        events[idx] = event;
    }

    bool isBeat(int curTick) {
        return (beats.isBeat(curTick, stepSize));
    }

    void tick(int curTick) {
        // if (beats.isBeat(curTick, stepSize)) {
        if (isBeat(curTick)) {
            Event& event = events[curStep];
            if (event.on) {
                std::visit(
                    [this, curTick](auto&& subEvent) { handleEvent(subEvent, curTick); },
                    event.subEvent
                );
                if (event.oneShot) {
                    event.on = false;
                }
            }
            curStep = ((curStep + 1) % numSteps);
        }
    }

    void handleEvent(NoteEvent& subEvent, int curTick) {
        midiQueue.noteOnOff(subEvent.note, subEvent.velocity, curTick, subEvent.duration);
    }

    void handleEvent(RollEvent& subEvent, int curTick) {
        int offset = curTick;
        int duration = subEvent.totalDuration - subEvent.restDuration;

        for (int i = 0; i < subEvent.numRepeats; ++i) {
            midiQueue.noteOnOff(subEvent.note, subEvent.velocity, offset, duration);
            offset += subEvent.totalDuration;
        }
    }
};
