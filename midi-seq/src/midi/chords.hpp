# pragma once

#include <algorithm>
#include <vector>

#include "../main/util.hpp"

enum ChordType {
    MAJOR = 0,
    MINOR = 1,
};

enum ChordInversion {
    ROOT       = 0,
    FIRST_INV  = 1,
    SECOND_INV = 2,
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
        case MAJOR:
            chord = {0, 4, 7};
            break;
        case MINOR:
            chord = {0, 3, 7};
            break;
    }

    switch (inv) {
        case ROOT:
            break;
        case FIRST_INV:         // 3rd 5th root
            rotate(chord);
            chord[0] -= 12;
            chord[1] -= 12;
            break;
        case SECOND_INV:        // 5th root 3rd
            rotate(chord);
            rotate(chord);
            chord[0] -= 12;
            break;
    }

    for (auto& elt : chord) {
        elt += root;
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
