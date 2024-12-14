#pragma  once

#include <string>

enum MidiAppMode {
    CHORD,
    INTERVAL,
    SINGLE_NOTE,
    CHORD_SINGLE_NOTE,
    EAR_TRAINING,
    NUM_MODES,
};

inline MidiAppMode getNextMode(MidiAppMode curMode) {
    return static_cast<MidiAppMode>((curMode + 1) % NUM_MODES);
}

inline std::wstring modeToString(MidiAppMode mode) {
    switch (mode) {
        case CHORD: return L"CHORD";
        case INTERVAL: return L"INTERVAL";
        case SINGLE_NOTE: return L"SINGLE_NOTE";
        case CHORD_SINGLE_NOTE: return L"CHORD_SINGLE_NOTE";
        case EAR_TRAINING: return L"EAR_TRAINING";
        default: return L"UNKNOWN_MODE";
    }
}
