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
        int curTick = beats.ticksPerBeat(B_16) * stepIdx;
        sequence.tick(curTick);
        midiQueue.handleEvents(curTick);
        ++stepIdx;
    }

    void assertMidi(std::vector<std::string> messages) {
        EXPECT_EQ(midiService.getMessagesSize(), messages.size());
        int i = 0;
        for (auto& message : messages) {
            EXPECT_EQ(midiService.getMessage(i++), message);
        }
    }
};

TEST(TestSequence, NoteEvent) {
    TestSequenceHelper h(B_16);

    h.sequence.addNoteEvent(0, 1, 100, h.beats.ticksPerBeat(B_16));
    h.sequence.addNoteEvent(2, 2, 100, h.beats.ticksPerBeat(B_16));

    h.step();
    h.assertMidi(std::vector<std::string>{
        "noteOn 1 100",
    });

    h.step();
    h.assertMidi(std::vector<std::string>{
        "noteOn 1 100",
        "noteOff 1",
    });

    h.step();
    h.assertMidi(std::vector<std::string>{
        "noteOn 1 100",
        "noteOff 1",
        "noteOn 2 100",
    });

    h.step();
    h.assertMidi(std::vector<std::string>{
        "noteOn 1 100",
        "noteOff 1",
        "noteOn 2 100",
        "noteOff 2",
    });
}

TEST(TestSequence, NoteRollEvent) {
    TestSequenceHelper h(B_16);

    h.sequence.addNoteRollEvent(0, 1, 100, 3, h.beats.ticksPerBeat(B_8), h.beats.ticksPerBeat(B_16));

    h.step();
    h.assertMidi(std::vector<std::string>{
        "noteOn 1 100",
    });

    h.step();
    h.assertMidi(std::vector<std::string>{
        "noteOn 1 100",
        "noteOff 1",
    });

    h.step();
    h.assertMidi(std::vector<std::string>{
        "noteOn 1 100",
        "noteOff 1",
        "noteOn 1 100",
    });

    h.step();
    h.assertMidi(std::vector<std::string>{
        "noteOn 1 100",
        "noteOff 1",
        "noteOn 1 100",
        "noteOff 1",
    });

    h.step();
    h.assertMidi(std::vector<std::string>{
        "noteOn 1 100",
        "noteOff 1",
        "noteOn 1 100",
        "noteOff 1",
        "noteOn 1 100",
    });

    h.step();
    h.assertMidi(std::vector<std::string>{
        "noteOn 1 100",
        "noteOff 1",
        "noteOn 1 100",
        "noteOff 1",
        "noteOn 1 100",
        "noteOff 1",
    });
}
