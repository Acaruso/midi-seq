#pragma once

#include <iostream>
#include <variant>
#include <vector>

#include "../main/util.hpp"
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
    int offset = 0;
    int probability = 100;
    int channel = 1;
    SubEvent subEvent;
};

// TODO: use RngService in Sequence to generate random numbers
template <typename MidiServiceType>
class Sequence {
public:
    Beats& beats;
    MidiQueue<MidiServiceType>& midiQueue;
    int channel;
    std::vector<Event> events;
    BeatUnit stepSize;
    int numSteps;
    int curStep;

    Sequence(
        Beats& beats,
        MidiQueue<MidiServiceType>& midiQueue,
        int channel,
        int numSteps,
        BeatUnit stepSize
    ) :
        beats(beats),
        midiQueue(midiQueue),
        channel(channel),
        events(numSteps, Event{}),
        stepSize(stepSize),
        numSteps(numSteps),
        curStep(0)
    {}

    void addEvent(int idx, Event event) {
        if (idx >= numSteps) {
            std::cerr << "Sequence: idx >= size" << std::endl;
            return;
        }
        events[idx] = event;
    }

    void tick(int curTick) {
        if (isBeat(curTick)) {
            Event& event = events[curStep];
            if (eventShouldTrigger(event)) {
                handleEvent(event, curTick);
                if (event.oneShot) {
                    event.on = false;
                }
            }
            curStep = (curStep + 1) % numSteps;
        }
    }

    bool isBeat(int curTick) {
        return (beats.isBeat(curTick, stepSize));
    }

    bool eventShouldTrigger(Event& e) {
        if (e.probability == 100) {
            return e.on;
        } else {
            int r = getRand(0, 99);
            return (r < e.probability) && e.on;
        }
    }

    void handleEvent(Event& event, int curTick) {
        std::visit(
            [this, &event, curTick](auto&& subEvent) { v_handleEvent(subEvent, event, curTick); },
            event.subEvent
        );
    }

    void v_handleEvent(NoteEvent& subEvent, Event& event, int curTick) {
        midiQueue.noteOnOff(
            event.channel,
            subEvent.note,
            subEvent.velocity,
            curTick + event.offset,
            subEvent.duration
        );
    }

    void v_handleEvent(RollEvent& subEvent, Event& event, int curTick) {
        int offset = curTick + event.offset;
        int duration = subEvent.totalDuration - subEvent.restDuration;

        for (int i = 0; i < subEvent.numRepeats; ++i) {
            midiQueue.noteOnOff(event.channel, subEvent.note, subEvent.velocity, offset, duration);
            offset += subEvent.totalDuration;
        }
    }
};
