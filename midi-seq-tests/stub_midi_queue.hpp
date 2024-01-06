// #pragma once

// #include <string>
// #include <vector>
// #include "../midi-seq/src/midi/midi_queue.hpp"

// template <typename MidiServiceType>
// class StubMidiQueue {
// public:
//     std::vector<std::string> messages;

//     MidiServiceType& midiService;

//     int capacity = 10000;
//     int size = 0;
//     std::vector<MidiEvent> events = std::vector<MidiEvent>(capacity, MidiEvent{});

//     StubMidiQueue(MidiServiceType& m) : midiService(m) {}

//     void noteOn(int note, int velocity, int tick) {
//         addEvent(createNoteOnEvent(note, velocity, tick));
//     }

//     void noteOff(int note, int tick) {
//         addEvent(createNoteOffEvent(note, tick));
//     }

//     void noteOnOff(int note, int velocity, int tick, int duration) {
//         noteOn(note, velocity, tick);
//         noteOff(note, tick + duration);
//     }

//     void cc(int channel, int controller, int value, int tick) {
//         addEvent(createCCEvent(channel, controller, value, tick));
//     }

//     void addEvent(MidiEvent event) {
//         if (size == capacity) {
//             // TODO: handle this better
//             std::cerr << "MidiQueue at capacity" << std::endl;
//             return;
//         }
//         events[size++] = event;
//         sortEvents();
//     }

//     void handleEvents(int curTick) {
//         if (size == 0) {
//             return;
//         }

//         while (size > 0) {
//             auto& event = events[size - 1];
//             if (event.tick > curTick) {
//                 break;
//             }
//             handleEvent(event);
//             --size;
//         }
//     }

//     void handleEvent(MidiEvent& event) {
//         switch (event.type) {
//             case NOTE_ON: {
//                 midiService.noteOn(event.note, event.velocity);
//                 break;
//             }
//             case NOTE_OFF: {
//                 midiService.noteOff(event.note);
//                 break;
//             }
//             case CC: {
//                 midiService.cc(event.channel, event.controller, event.value);
//                 break;
//             }
//         }
//     }

// private:
//     // sort events by tick in decending order
//     // example: [99, 50, 45, 12, 1]
//     void sortEvents() {
//         std::sort(
//             events.begin(),
//             events.begin() + size,
//             [](const MidiEvent &a, const MidiEvent &b) {
//                 return a.tick > b.tick;
//             }
//         );
//     }
// };
