#pragma once

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <vector>

#include "chords.hpp"

static std::vector<std::vector<int>> s_modes = {
    {0, 2, 4, 5, 7, 9, 11},
    {0, 2, 3, 5, 7, 9, 10},
    {0, 1, 3, 5, 7, 8, 10},
    {0, 2, 4, 6, 7, 9, 11},
    {0, 2, 4, 5, 7, 9, 10},
    {0, 2, 3, 5, 7, 8, 10},
    {0, 1, 3, 5, 6, 8, 10},
};

static std::vector<ChordType> s_chordTypes = {
    MAJOR,
    MINOR,
    MINOR,
    MAJOR,
    MAJOR,
    MINOR,
    DIM,
};

class Scale {
public:
    int root;
    int modalOffset;

    std::vector<int> scale;
    std::vector<ChordType> chordTypes;

    Scale(
        int root,
        int modalOffset
    ) :
        root(root),
        modalOffset(modalOffset)
    {
        if (modalOffset > 6 || modalOffset < 0) {
            std::cerr << "Scale.modalOffset out of range" << std::endl;
            return;
        }

        scale = s_modes[modalOffset];

        chordTypes = s_chordTypes;
        std::rotate(
            chordTypes.begin(),
            chordTypes.begin() + modalOffset,
            chordTypes.end()
        );
    }

    int getNote(int degree) {
        if (degree < 0) {
            std::cerr << "negative number passed to Scale.getNote()" << std::endl;
            return 0;
        }
        int mod = degree % 7;
        int octave = degree / 7;
        return root + (12 * octave) + scale[mod];
    }

    ChordType getChordType(int degree) {
        return chordTypes[degree % 7];
    }
};
