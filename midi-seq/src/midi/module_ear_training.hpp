#pragma once

#include "../lib/readerwriterqueue.h"

#include "beats.hpp"
#include "generator_ear_training.hpp"
#include "midi_queue.hpp"
#include "rng_service.hpp"

template <typename MidiServiceType>
class ModuleEarTraining {
public:
    moodycamel::ReaderWriterQueue<std::string>* midiToMainQueue;
    MidiQueue<MidiServiceType>& midiQueue;
    RngService& rngService;

    int ticksPer64Note;
    Beats beats;
    int channel;
    GeneratorEarTraining<MidiServiceType> generatorEarTraining;

    ModuleEarTraining(
        moodycamel::ReaderWriterQueue<std::string>* midiToMainQueue,
        MidiQueue<MidiServiceType>& midiQueue,
        RngService& rngService
    ):
        midiToMainQueue(midiToMainQueue),
        midiQueue(midiQueue),
        rngService(rngService),
        ticksPer64Note(50),
        beats(ticksPer64Note),
        channel(1),
        generatorEarTraining(midiToMainQueue, beats, midiQueue, rngService, channel)
    {}

    void tick(std::string& message, int curTick) {
        generatorEarTraining.tick(message, curTick);
    }
};
