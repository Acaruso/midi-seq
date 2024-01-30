#pragma once

#include "../main/util.hpp"
#include "beats.hpp"
#include "midi_queue.hpp"
#include "rng_service.hpp"

template <typename MidiServiceType>
class ModuleStressTest {
public:
    MidiServiceType& midiService;
    MidiQueue<MidiServiceType>& midiQueue;
    RngService& rngService;

    int ticksPer64Note;
    Beats beats;
    int channel;

    ModuleStressTest(
        MidiServiceType& midiService,
        MidiQueue<MidiServiceType>& midiQueue,
        RngService& rngService
    ) :
        midiService(midiService),
        midiQueue(midiQueue),
        rngService(rngService),
        ticksPer64Note(50),
        beats(ticksPer64Note),
        channel(1)
    {}

    void tick(std::string& message, int curTick) {
        if (beats.isBeat(curTick, B_16)) {
            play(curTick);
        }
    }

    void play(int curTick) {
        int offset = 45;
        int r = rngService.getRand(0, 10);
        int dur = beats.ticksPerBeat(B_32);
        midiQueue.noteOnOff(channel, offset + r,       100, curTick, dur);
        midiQueue.noteOnOff(channel, offset + r + 3,   100, curTick, dur);
        midiQueue.noteOnOff(channel, offset + r + 7,   100, curTick, dur);
        midiQueue.noteOnOff(channel, offset + r + 10,  100, curTick, dur);
        midiQueue.noteOnOff(channel, offset + r + 14,  100, curTick, dur);
    }
};
