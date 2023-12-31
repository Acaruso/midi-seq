#include "pch.h"
#include <string>
#include <vector>
#include "../midi-seq/src/midi/midi_queue.hpp"
#include "stub_midi_service.hpp"

TEST(MidiQueueTests, TestMidiQueueOperations) {
    StubMidiService midiService(0);
    MidiQueue<StubMidiService> q(midiService);

    q.addEvent(createNoteOnEvent(1, 100, 99));
    q.addEvent(createNoteOnEvent(1, 100, 2));
    q.addEvent(createNoteOnEvent(1, 100, 55));
    q.addEvent(createNoteOnEvent(1, 100, 43));

    EXPECT_EQ(q.size, 4);
    EXPECT_EQ(q.events[0].tick, 99);
    EXPECT_EQ(q.events[1].tick, 55);
    EXPECT_EQ(q.events[2].tick, 43);
    EXPECT_EQ(q.events[3].tick, 2);

    q.handleEvents(43);

    EXPECT_EQ(q.size, 2);
    EXPECT_EQ(q.events[0].tick, 99);
    EXPECT_EQ(q.events[1].tick, 55);

    q.addEvent(createNoteOnEvent(1, 100, 999));

    EXPECT_EQ(q.size, 3);
    EXPECT_EQ(q.events[0].tick, 999);
    EXPECT_EQ(q.events[1].tick, 99);
    EXPECT_EQ(q.events[2].tick, 55);

    q.handleEvents(999);

    EXPECT_EQ(q.size, 0);

    q.addEvent(createNoteOnEvent(1, 100, 123));

    EXPECT_EQ(q.size, 1);
    EXPECT_EQ(q.events[0].tick, 123);
}

TEST(MidiQueueTests, TestMidiQueueSideEffects) {
    StubMidiService m(0);
    MidiQueue<StubMidiService> q(m);

    q.addEvent(createNoteOnEvent(1, 10, 99));
    q.addEvent(createNoteOnEvent(2, 11, 2));
    q.addEvent(createNoteOnEvent(3, 12, 55));
    q.addEvent(createNoteOnEvent(4, 13, 43));

    q.handleEvents(43);

    EXPECT_EQ(m.getMessagesSize(), 2);
    EXPECT_EQ(m.getMessage(0), "noteOn 2 11");
    EXPECT_EQ(m.getMessage(1), "noteOn 4 13");

    q.addEvent(createCCEvent(1, 1, 7, 45));

    q.handleEvents(46);

    EXPECT_EQ(m.messages.size(), 3);
    EXPECT_EQ(m.messages[2], "cc 1 1 7");
}
