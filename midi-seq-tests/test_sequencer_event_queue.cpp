#include "pch.h"
#include <string>
#include <vector>
#include "../midi-seq/src/midi/sequencer_event_queue.hpp"

class StubMidiService {
public:
    std::vector<std::string> messages;

    StubMidiService(int midiPort) {}

    void openMidiPort(int midiPort) {}

    void noteOn(int note, int velocity) {
        messages.push_back(
            "noteOn " + std::to_string(note) + " " + std::to_string(velocity)
        );
    }

    void noteOff(int note) {
        messages.push_back(
            "noteOff " + std::to_string(note)
        );
    }

    void cc(int channel, int controller, int value) {
        messages.push_back(
            "cc " + std::to_string(channel) 
            + " " + std::to_string(controller) 
            + " " + std::to_string(value)
        );
    }

    void printMidiOutputDevices() {}
};

TEST(SequencerEventQueueTests, TestQueueOperations) {
    StubMidiService midiService(0);
    SequencerEventQueue<StubMidiService> q(midiService);

    q.addEvent(SeqEvent{NOTE_ON, 1, 99});
    q.addEvent(SeqEvent{NOTE_ON, 1, 2});
    q.addEvent(SeqEvent{NOTE_ON, 1, 55});
    q.addEvent(SeqEvent{NOTE_ON, 1, 43});

    EXPECT_EQ(q.eventsLen, 4);
    EXPECT_EQ(q.events[0].timestamp, 99);
    EXPECT_EQ(q.events[1].timestamp, 55);
    EXPECT_EQ(q.events[2].timestamp, 43);
    EXPECT_EQ(q.events[3].timestamp, 2);

    q.handleEvents(43);

    EXPECT_EQ(q.eventsLen, 2);
    EXPECT_EQ(q.events[0].timestamp, 99);
    EXPECT_EQ(q.events[1].timestamp, 55);

    q.addEvent(SeqEvent{NOTE_ON, 1, 999});

    EXPECT_EQ(q.eventsLen, 3);
    EXPECT_EQ(q.events[0].timestamp, 999);
    EXPECT_EQ(q.events[1].timestamp, 99);
    EXPECT_EQ(q.events[2].timestamp, 55);

    q.handleEvents(999);

    EXPECT_EQ(q.eventsLen, 0);

    q.addEvent(SeqEvent{NOTE_ON, 1, 123});

    EXPECT_EQ(q.eventsLen, 1);
    EXPECT_EQ(q.events[0].timestamp, 123);
}

TEST(SequencerEventQueueTests, TestMidiSideEffects) {
    StubMidiService m(0);
    SequencerEventQueue<StubMidiService> q(m);

    q.addEvent(SeqEvent{NOTE_ON, 1, 99});
    q.addEvent(SeqEvent{NOTE_ON, 2, 2});
    q.addEvent(SeqEvent{NOTE_ON, 3, 55});
    q.addEvent(SeqEvent{NOTE_ON, 4, 43});

    q.handleEvents(43);

    EXPECT_EQ(m.messages.size(), 2);
    EXPECT_EQ(m.messages[0], "noteOn 2 100");
    EXPECT_EQ(m.messages[1], "noteOn 4 100");
}
