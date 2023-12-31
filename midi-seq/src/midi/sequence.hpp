#pragma once

#include <iostream>
#include <variant>
#include <vector>
#include "beats.hpp"
#include "midi_queue.hpp"

struct Note {
    int note = 0;
    int velocity = 0;
    int duration = 0;
};

struct NoteRoll {
    int note = 0;
    int velocity = 0;
    int duration = 0;
    int numRepeats = 0;
    int freqRepeats = 0;
};

using EventType = std::variant<Note, NoteRoll>;

struct Event {
    bool on = false;
    EventType eventType;
};

inline Event createNoteEvent(int note, int velocity, int duration) {
    Event event;
    event.on = true;
    event.eventType = Note{note, velocity, duration};
    return event;
}

inline Event createNoteRollEvent(int note, int velocity, int duration, int numRepeats, int freqRepeats) {
    Event event;
    event.on = true;
    event.eventType = NoteRoll{note, velocity, duration, numRepeats, freqRepeats};
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

    void addNoteRollEvent(int idx, int note, int velocity, int duration, int numRepeats, int freqRepeats) {
        addEvent(idx, createNoteRollEvent(note, velocity, duration, numRepeats, freqRepeats));
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
                    [this, curTick](auto&& arg) { handleEvent(arg, curTick); },
                    event.eventType
                );
            }
            playHead = ((playHead + 1) % size);
        }
    }

    void handleEvent(Note& event, int curTick) {
        midiQueue.addNoteOnEvent(event.note, event.velocity, curTick);
        midiQueue.addNoteOffEvent(event.note, curTick + event.duration);
    }

    void handleEvent(NoteRoll& event, int curTick) {
        // do nothing
    }
};
