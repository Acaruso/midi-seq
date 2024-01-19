#pragma once

#include <vector>

#include "chords.hpp"

struct MChord {
    int degree;
    int lowestNote;
    ChordType chordType;
    ChordInversion chordInversion;
};

static std::vector<MChord> chords = {
    MChord{1, 1, MAJOR, ROOT},
    MChord{6, 1, MINOR, ROOT},
    MChord{4, 1, MAJOR, SECOND_INV},
    MChord{2, 2, MINOR, ROOT},
    MChord{5, 2, MAJOR, SECOND_INV},
    MChord{3, 3, MINOR, ROOT},
    MChord{1, 3, MAJOR, FIRST_INV},
    MChord{6, 3, MINOR, SECOND_INV},
    MChord{4, 4, MAJOR, ROOT},
    MChord{2, 4, MINOR, FIRST_INV},
    MChord{5, 5, MAJOR, ROOT},
    MChord{3, 5, MINOR, FIRST_INV},
    MChord{1, 5, MAJOR, SECOND_INV},
    MChord{6, 6, MINOR, ROOT},
    MChord{4, 6, MAJOR, FIRST_INV},
    MChord{2, 6, MINOR, SECOND_INV},
    MChord{5, 7, MAJOR, FIRST_INV},
    MChord{3, 7, MINOR, SECOND_INV},
};
