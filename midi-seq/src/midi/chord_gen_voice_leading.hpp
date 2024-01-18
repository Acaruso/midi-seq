#pragma once

#include <vector>

#include "chords.hpp"

struct VChord {
    int degree;
    int lowestNote;
    ChordType chordType;
    ChordInversion chordInversion;
};

// enum ChordType {
//     MAJOR,
//     MINOR,
// };

// enum ChordInversion {
//     ROOT,
//     FIRST_INV,
//     SECOND_INV,
// };

static std::vector chords = {
    VChord{1, 1, MAJOR, ROOT},
    VChord{6, 1, MINOR, ROOT},
    VChord{4, 1, MAJOR, SECOND_INV},
    VChord{2, 2, MINOR, ROOT},
    VChord{5, 2, MAJOR, SECOND_INV},
    VChord{3, 3, MINOR, ROOT},
    VChord{1, 3, MAJOR, FIRST_INV},
    VChord{6, 3, MINOR, SECOND_INV},
    VChord{4, 4, MAJOR, ROOT},
    VChord{2, 4, MINOR, FIRST_INV},
    VChord{5, 5, MAJOR, ROOT},
    VChord{3, 5, MINOR, FIRST_INV},
    VChord{1, 5, MAJOR, SECOND_INV},
    VChord{6, 6, MINOR, ROOT},
    VChord{4, 6, MAJOR, FIRST_INV},
    VChord{2, 6, MINOR, SECOND_INV},
    VChord{5, 7, MAJOR, FIRST_INV},
    VChord{3, 7, MINOR, SECOND_INV},
};
