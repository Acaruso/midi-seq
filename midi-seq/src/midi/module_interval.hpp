#pragma once

#include "beats.hpp"
#include "generator_interval.hpp"
#include "midi_queue.hpp"
#include "rng_service.hpp"

template <typename MidiServiceType>
class ModuleInterval {
public:
    MidiQueue<MidiServiceType>& midiQueue;
    RngService& rngService;

    int ticksPer64Note;
    Beats beats;
    int channel;
    GeneratorInterval<MidiServiceType> generatorInterval;

    ModuleInterval(
        MidiQueue<MidiServiceType>& midiQueue,
        RngService& rngService
    ) :
        midiQueue(midiQueue),
        rngService(rngService),
        ticksPer64Note(50),
        beats(ticksPer64Note),
        channel(1),
        generatorInterval(beats, midiQueue, rngService, channel)
    {}

    void tick(std::string& message, int curTick) {
        generatorInterval.tick(message, curTick);
    }
};
