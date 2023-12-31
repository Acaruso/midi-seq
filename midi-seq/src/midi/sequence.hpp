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

struct NoteRollEvent {
    int note = 0;
    int velocity = 0;
    int numRepeats = 0;
    int totalDuration = 0;
    int restDuration = 0;
};

using SubEvent = std::variant<NoteEvent, NoteRollEvent>;

struct Event {
    bool on = false;
    SubEvent subEvent;
};

inline Event createNoteEvent(int note, int velocity, int duration) {
    Event event;
    event.on = true;
    event.subEvent = NoteEvent{note, velocity, duration};
    return event;
}

inline Event createNoteRollEvent(int note, int velocity, int numRepeats, int totalDuration, int restDuration) {
    Event event;
    event.on = true;
    event.subEvent = NoteRollEvent{note, velocity, numRepeats, totalDuration, restDuration};
    return event;
}

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

    void addNoteEvent(int idx, int note, int velocity, int duration) {
        addEvent(idx, createNoteEvent(note, velocity, duration));
    }

    void addNoteRollEvent(int idx, int note, int velocity, int numRepeats, int totalDuration, int restDuration) {
        addEvent(idx, createNoteRollEvent(note, velocity, numRepeats, totalDuration, restDuration));
    }

    void addEvent(int idx, Event& event) {
        if (idx >= size) {
            // TODO: handle this better
            std::cerr << "Sequence: idx >= size" << std::endl;
            return;
        }
        events[idx] = event;
    }

    void tick(int curTick) {
        if (beats.isBeat(curTick, stepSize)) {
            Event& event = events[playHead];
            if (event.on) {
                std::visit(
                    [this, curTick](auto&& subEvent) { handleEvent(subEvent, curTick); },
                    event.subEvent
                );
            }
            playHead = ((playHead + 1) % size);
        }
    }

    void handleEvent(NoteEvent& event, int curTick) {
        midiQueue.noteOnOff(event.note, event.velocity, curTick, event.duration);
    }

    void handleEvent(NoteRollEvent& event, int curTick) {
        int offset = curTick;
        int duration = event.totalDuration - event.restDuration;

        for (int i = 0; i < event.numRepeats; ++i) {
            midiQueue.noteOnOff(event.note, event.velocity, offset, duration);
            offset += event.totalDuration;
        }
    }
};
