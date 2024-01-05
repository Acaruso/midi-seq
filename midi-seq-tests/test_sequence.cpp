#include "pch.h"
#include <string>
#include <vector>
#include "../midi-seq/src/midi/beats.hpp"
#include "../midi-seq/src/midi/sequence.hpp"
#include "stub_midi_service.hpp"

class TestSequenceHelper {
public:
    Beats beats;
    StubMidiService midiService;
    MidiQueue<StubMidiService> midiQueue;
    Sequence<StubMidiService> sequence;
    BeatUnit stepInc;
    int stepIdx = 0;

    TestSequenceHelper(BeatUnit stepInc) :
        beats(24),
        midiService(0),
        midiQueue(midiService),
        sequence(beats, midiQueue, 16, B_16),
        stepInc(stepInc)
    {}

    void step() {
        int curTick = beats.ticksPerBeat(stepInc) * stepIdx;
        sequence.tick(curTick);
        midiQueue.handleEvents(curTick);
        ++stepIdx;
    }

    void assertMidiEq(std::vector<std::string> messages) {
        EXPECT_EQ(midiService.getMessagesSize(), messages.size());
        int i = 0;
        for (auto& message : messages) {
            EXPECT_EQ(midiService.getMessage(i++), message);
        }
    }
};

TEST(TestSequence, NoteEvent) {
    TestSequenceHelper h(B_16);

    h.sequence.addEvent(
        0,
        Event{
            .on = true,
            .subEvent = NoteEvent{
                .note = 1,
                .velocity = 100,
                .duration = h.beats.ticksPerBeat(B_16)
            }
        }
    );

    h.sequence.addEvent(
        2,
        Event{
            .on = true,
            .subEvent = NoteEvent{
                .note = 2,
                .velocity = 100,
                .duration = h.beats.ticksPerBeat(B_16)
            }
        }
    );

    h.step();
    h.assertMidiEq(std::vector<std::string>{
        "noteOn 1 100",
    });

    h.step();
    h.assertMidiEq(std::vector<std::string>{
        "noteOn 1 100",
        "noteOff 1",
    });

    h.step();
    h.assertMidiEq(std::vector<std::string>{
        "noteOn 1 100",
        "noteOff 1",
        "noteOn 2 100",
    });

    h.step();
    h.assertMidiEq(std::vector<std::string>{
        "noteOn 1 100",
        "noteOff 1",
        "noteOn 2 100",
        "noteOff 2",
    });
}

TEST(TestSequence, RollEvent1) {
    TestSequenceHelper h(B_16);

    h.sequence.addEvent(
        0,
        Event{
            .on = true,
            .subEvent = RollEvent{
                .note = 1,
                .velocity = 100,
                .numRepeats = 3,
                .totalDuration = h.beats.ticksPerBeat(B_8),
                .restDuration = h.beats.ticksPerBeat(B_16)
            }
        }
    );

    h.step();
    h.assertMidiEq(std::vector<std::string>{
        "noteOn 1 100",
    });

    h.step();
    h.assertMidiEq(std::vector<std::string>{
        "noteOn 1 100",
        "noteOff 1",
    });

    h.step();
    h.assertMidiEq(std::vector<std::string>{
        "noteOn 1 100",
        "noteOff 1",
        "noteOn 1 100",
    });

    h.step();
    h.assertMidiEq(std::vector<std::string>{
        "noteOn 1 100",
        "noteOff 1",
        "noteOn 1 100",
        "noteOff 1",
    });

    h.step();
    h.assertMidiEq(std::vector<std::string>{
        "noteOn 1 100",
        "noteOff 1",
        "noteOn 1 100",
        "noteOff 1",
        "noteOn 1 100",
    });

    h.step();
    h.assertMidiEq(std::vector<std::string>{
        "noteOn 1 100",
        "noteOff 1",
        "noteOn 1 100",
        "noteOff 1",
        "noteOn 1 100",
        "noteOff 1",
    });
}

TEST(TestSequence, RollEvent2) {
    TestSequenceHelper h(B_16);

    h.sequence.addEvent(
        0,
        Event{
            .on = true,
            .subEvent = RollEvent{
                .note = 1,
                .velocity = 100,
                .numRepeats = 2,
                .totalDuration = h.beats.ticksPerBeat(B_4),
                .restDuration = h.beats.ticksPerBeat(B_16)
            }
        }
    );

    h.step();
    h.assertMidiEq(std::vector<std::string>{
        "noteOn 1 100",
    });

    h.step();
    h.assertMidiEq(std::vector<std::string>{
        "noteOn 1 100",
    });

    h.step();
    h.assertMidiEq(std::vector<std::string>{
        "noteOn 1 100",
    });

    h.step();
    h.assertMidiEq(std::vector<std::string>{
        "noteOn 1 100",
        "noteOff 1",
    });

    h.step();
    h.assertMidiEq(std::vector<std::string>{
        "noteOn 1 100",
        "noteOff 1",
        "noteOn 1 100",
    });

    h.step();
    h.assertMidiEq(std::vector<std::string>{
        "noteOn 1 100",
        "noteOff 1",
        "noteOn 1 100",
    });

    h.step();
    h.assertMidiEq(std::vector<std::string>{
        "noteOn 1 100",
        "noteOff 1",
        "noteOn 1 100",
    });

    h.step();
    h.assertMidiEq(std::vector<std::string>{
        "noteOn 1 100",
        "noteOff 1",
        "noteOn 1 100",
        "noteOff 1",
    });
}
