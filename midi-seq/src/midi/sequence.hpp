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
    int offset = 0;
    int channel = 1;
    SubEvent subEvent;
};

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
            if (event.on) {
                std::visit(
                    [this, curTick, &event](auto&& subEvent) { handleEvent(subEvent, event, curTick); },
                    event.subEvent
                );
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

    void handleEvent(NoteEvent& subEvent, Event& event, int curTick) {
        midiQueue.noteOnOff(
            event.channel,
            subEvent.note,
            subEvent.velocity,
            curTick + event.offset,
            subEvent.duration
        );
    }

    void handleEvent(RollEvent& subEvent, Event& event, int curTick) {
        int offset = curTick + event.offset;
        int duration = subEvent.totalDuration - subEvent.restDuration;

        for (int i = 0; i < subEvent.numRepeats; ++i) {
            midiQueue.noteOnOff(event.channel, subEvent.note, subEvent.velocity, offset, duration);
            offset += subEvent.totalDuration;
        }
    }
};
