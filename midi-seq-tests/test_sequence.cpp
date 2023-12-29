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

    // seq.addEvent(0, 1, 100, 1000);
    // seq.addEvent(2, 2, 100, 1000);

    // seq.tick(0);

    EXPECT_EQ(1, 1);
}
