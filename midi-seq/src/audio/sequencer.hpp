#pragma once

#include "midi_service.hpp"

class Sequencer {
public:
    MidiService& midiService;
    unsigned tick = 0;
    int ticksPer4Note  = 16;
    int ticksPer8Note  = 8;
    int ticksPer16Note = 4;
    int ticksPer32Note = 2;
    int ticksPer64Note = 1;
    int ticksPerBar = ticksPer4Note * 4;

    int bdNote = 60;
    int sdNote = 61;
    int hhNote = 62;

    Sequencer(MidiService& midiService) : midiService(midiService) {}

    // tick is triggered at the beginning of processing every audio buffer
    // thus, the sequencer has at best the timing granularity of the audio buffer size
    // also, the tempo will change if you change the audio buffer size

    void doTick() {
        if (is4Note()) {
            int n4 = get4Note();
            if ((n4 % 2) == 0) {
                midiService.noteOn(bdNote, 100);
                midiService.noteOff(sdNote);
            } else {
                midiService.noteOff(bdNote);
                midiService.noteOn(sdNote, 100);
            }
        }

        if (is8Note()) {
            int n8 = get8Note();
            if ((n8 % 2) == 0) {
                midiService.noteOn(hhNote, 100);
            } else {
                midiService.noteOff(hhNote);
            }
        }

        tick = (tick + 1) % ticksPerBar;
    }

    bool is4Note() {
        return ((tick % ticksPer4Note) == 0);
    }

    int get4Note() {
        return tick / ticksPer4Note;
    }

    bool is8Note() {
        return ((tick % ticksPer8Note) == 0);
    }

    int get8Note() {
        return tick / ticksPer8Note;
    }
};
