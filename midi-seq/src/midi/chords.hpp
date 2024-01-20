# pragma once

#include <algorithm>
#include <vector>

#include "../main/util.hpp"

enum ChordType {
    MAJOR,
    MINOR,
};

enum ChordInversion {
    ROOT,
    FIRST_INV,
    SECOND_INV,
};

inline ChordType getRandChordType() {
    return (ChordType)(getRand(0, 1));
}

inline ChordInversion getRandChordInversion() {
    return (ChordInversion)(getRand(0, 2));
}

template<typename T>
inline void rotate(std::vector<T>& vec) {
    std::rotate(vec.begin(), vec.begin() + 1, vec.end());
}

inline std::vector<int> createChordByRoot(int root, ChordType type, ChordInversion inv) {
    std::vector<int> chord;

    switch (type) {
        case MAJOR: chord = {0, 4, 7}; break;
        case MINOR: chord = {0, 3, 7}; break;
    }

    switch (inv) {
        case ROOT:              // R 3 5
            break;
        case FIRST_INV:         // 3 5 R
            rotate(chord);
            chord[0] -= 12;
            chord[1] -= 12;
            break;
        case SECOND_INV:        // 5 R 3
            rotate(chord);
            rotate(chord);
            chord[0] -= 12;
            break;
    }

    for (auto& note : chord) {
        note += root;
    }

    return chord;
}

inline std::vector<int> createChordByLowestNote(int lowestNote, ChordType type, ChordInversion inv) {
    std::vector<int> chord;

    switch (type) {
        case MAJOR: {
            switch (inv) {
                case ROOT:
                    chord = {0, 4, 7};
                    break;
                case FIRST_INV:
                    chord = {0, 3, 8};
                    break;
                case SECOND_INV:
                    chord = {0, 5, 9};
                    break;
            }
            break;
        }
        case MINOR: {
            switch (inv) {
                case ROOT:
                    chord = {0, 3, 7};
                    break;
                case FIRST_INV:
                    chord = {0, 4, 9};
                    break;
                case SECOND_INV:
                    chord = {0, 5, 8};
                    break;
            }
            break;
        }
    }

    for (auto& elt : chord) {
        elt += lowestNote;
    }

    return chord;
}
