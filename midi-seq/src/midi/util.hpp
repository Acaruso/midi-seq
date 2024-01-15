#pragma once

// MIDI notes:
// 40 == E1
// 45 == A1
// 50 == D1
// 55 == G2
// 59 == B2
// 64 == E3

static int guitarStringsToMidi[6] = {
    40,
    45,
    50,
    55,
    59,
    64
};

enum Strang {
    S_LOW_E,
    S_A,
    S_D,
    S_G,
    S_B,
    S_HIGH_E
};

inline int guitarToMidi(int strang, int fret) {
    return guitarStringsToMidi[strang] + fret;
}

inline int guitarToMidi(Strang strang, int fret) {
    return guitarStringsToMidi[strang] + fret;
}
