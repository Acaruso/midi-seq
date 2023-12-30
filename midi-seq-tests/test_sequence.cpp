#include "pch.h"
#include <string>
#include "../midi-seq/src/midi/beats.hpp"
#include "../midi-seq/src/midi/sequence.hpp"
#include "stub_midi_service.hpp"

TEST(TestSequence, T1) {
    Beats beats(24);
    StubMidiService midiService(0);
    SequencerEventQueue<StubMidiService> queue(midiService);
    Sequence<StubMidiService> seq(beats, queue, 16, B_16);

    seq.addEvent(0, 1, 100, beats.ticksPerBeat(B_16));
    seq.addEvent(2, 2, 100, beats.ticksPerBeat(B_16));

    int curTick = 0;

    seq.tick(curTick);
    queue.handleEvents(curTick);

    EXPECT_EQ(midiService.getMessagesSize(), 1);
    EXPECT_EQ(midiService.getMessage(0), "noteOn 1 100");

    curTick = beats.ticksPerBeat(B_16) * 1;
    seq.tick(curTick);
    queue.handleEvents(curTick);

    curTick = beats.ticksPerBeat(B_16) * 2;
    seq.tick(curTick);
    queue.handleEvents(curTick);

    EXPECT_EQ(midiService.getMessagesSize(), 3);
    EXPECT_EQ(midiService.getMessage(0), "noteOn 1 100");
    EXPECT_EQ(midiService.getMessage(1), "noteOff 1");
    EXPECT_EQ(midiService.getMessage(2), "noteOn 2 100");
}
