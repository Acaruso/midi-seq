#pragma once

#include "beats.hpp"
#include "interval_generator.hpp"
#include "midi_queue.hpp"
#include "rng_service.hpp"

template <typename MidiServiceType>
class ModuleInterval {
public:
    MidiServiceType& midiService;
    MidiQueue<MidiServiceType>& midiQueue;
    RngService& rngService;

    int ticksPer64Note;
    Beats beats;
    int channel;
    IntervalGenerator<MidiServiceType> intervalGenerator;

    ModuleInterval(
        MidiServiceType& midiService,
        MidiQueue<MidiServiceType>& midiQueue,
        RngService& rngService
    ) :
        midiService(midiService),
        midiQueue(midiQueue),
        rngService(rngService),
        ticksPer64Note(50),
        beats(ticksPer64Note),
        channel(1),
        intervalGenerator(beats, midiQueue, rngService, channel)
    {}

    void tick(std::string& message, int curTick) {
        intervalGenerator.tick(message, curTick);
    }
};
