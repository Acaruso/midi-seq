#include "pch.h"
#include "../midi-seq/src/midi/sequencer_event_queue.hpp"

TEST(SequencerEventQueueTests, T1) {
    SequencerEventQueue q;

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
