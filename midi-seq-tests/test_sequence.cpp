#include "pch.h"
#include <string>
#include "../midi-seq/src/midi/beats.hpp"
#include "../midi-seq/src/midi/sequence.hpp"
#include "stub_midi_service.hpp"

TEST(TestSequence, NoteEvent) {
    Beats beats(24);
    StubMidiService midiService(0);
    MidiQueue<StubMidiService> midiQueue(midiService);
    Sequence<StubMidiService> seq(beats, midiQueue, 16, B_16);

    seq.addNoteEvent(0, 1, 100, beats.ticksPerBeat(B_16));
    seq.addNoteEvent(2, 2, 100, beats.ticksPerBeat(B_16));

    int curTick = beats.ticksPerBeat(B_16) * 0;
    seq.tick(curTick);
    midiQueue.handleEvents(curTick);

    EXPECT_EQ(midiService.getMessagesSize(), 1);
    EXPECT_EQ(midiService.getMessage(0), "noteOn 1 100");

    curTick = beats.ticksPerBeat(B_16) * 1;
    seq.tick(curTick);
    midiQueue.handleEvents(curTick);

    EXPECT_EQ(midiService.getMessagesSize(), 2);
    EXPECT_EQ(midiService.getMessage(0), "noteOn 1 100");
    EXPECT_EQ(midiService.getMessage(1), "noteOff 1");

    curTick = beats.ticksPerBeat(B_16) * 2;
    seq.tick(curTick);
    midiQueue.handleEvents(curTick);

    EXPECT_EQ(midiService.getMessagesSize(), 3);
    EXPECT_EQ(midiService.getMessage(0), "noteOn 1 100");
    EXPECT_EQ(midiService.getMessage(1), "noteOff 1");
    EXPECT_EQ(midiService.getMessage(2), "noteOn 2 100");

    curTick = beats.ticksPerBeat(B_16) * 3;
    seq.tick(curTick);
    midiQueue.handleEvents(curTick);

    EXPECT_EQ(midiService.getMessagesSize(), 4);
    EXPECT_EQ(midiService.getMessage(0), "noteOn 1 100");
    EXPECT_EQ(midiService.getMessage(1), "noteOff 1");
    EXPECT_EQ(midiService.getMessage(2), "noteOn 2 100");
    EXPECT_EQ(midiService.getMessage(3), "noteOff 2");
}

TEST(TestSequence, NoteRollEvent) {
    Beats beats(24);
    StubMidiService midiService(0);
    MidiQueue<StubMidiService> midiQueue(midiService);
    Sequence<StubMidiService> seq(beats, midiQueue, 16, B_16);

    seq.addNoteRollEvent(0, 1, 100, 3, beats.ticksPerBeat(B_8), beats.ticksPerBeat(B_16));

    int curTick = beats.ticksPerBeat(B_16) * 0;
    seq.tick(curTick);
    midiQueue.handleEvents(curTick);

    EXPECT_EQ(midiService.getMessagesSize(), 1);
    EXPECT_EQ(midiService.getMessage(0), "noteOn 1 100");

    curTick = beats.ticksPerBeat(B_16) * 1;
    seq.tick(curTick);
    midiQueue.handleEvents(curTick);

    EXPECT_EQ(midiService.getMessagesSize(), 2);
    EXPECT_EQ(midiService.getMessage(0), "noteOn 1 100");
    EXPECT_EQ(midiService.getMessage(1), "noteOff 1");

    curTick = beats.ticksPerBeat(B_16) * 2;
    seq.tick(curTick);
    midiQueue.handleEvents(curTick);

    EXPECT_EQ(midiService.getMessagesSize(), 3);
    EXPECT_EQ(midiService.getMessage(0), "noteOn 1 100");
    EXPECT_EQ(midiService.getMessage(1), "noteOff 1");
    EXPECT_EQ(midiService.getMessage(2), "noteOn 1 100");

    curTick = beats.ticksPerBeat(B_16) * 3;
    seq.tick(curTick);
    midiQueue.handleEvents(curTick);

    EXPECT_EQ(midiService.getMessagesSize(), 4);
    EXPECT_EQ(midiService.getMessage(0), "noteOn 1 100");
    EXPECT_EQ(midiService.getMessage(1), "noteOff 1");
    EXPECT_EQ(midiService.getMessage(2), "noteOn 1 100");
    EXPECT_EQ(midiService.getMessage(3), "noteOff 1");

    curTick = beats.ticksPerBeat(B_16) * 4;
    seq.tick(curTick);
    midiQueue.handleEvents(curTick);

    EXPECT_EQ(midiService.getMessagesSize(), 5);
    EXPECT_EQ(midiService.getMessage(0), "noteOn 1 100");
    EXPECT_EQ(midiService.getMessage(1), "noteOff 1");
    EXPECT_EQ(midiService.getMessage(2), "noteOn 1 100");
    EXPECT_EQ(midiService.getMessage(3), "noteOff 1");
    EXPECT_EQ(midiService.getMessage(4), "noteOn 1 100");

    curTick = beats.ticksPerBeat(B_16) * 5;
    seq.tick(curTick);
    midiQueue.handleEvents(curTick);

    EXPECT_EQ(midiService.getMessagesSize(), 6);
    EXPECT_EQ(midiService.getMessage(0), "noteOn 1 100");
    EXPECT_EQ(midiService.getMessage(1), "noteOff 1");
    EXPECT_EQ(midiService.getMessage(2), "noteOn 1 100");
    EXPECT_EQ(midiService.getMessage(3), "noteOff 1");
    EXPECT_EQ(midiService.getMessage(4), "noteOn 1 100");
    EXPECT_EQ(midiService.getMessage(5), "noteOff 1");
}
