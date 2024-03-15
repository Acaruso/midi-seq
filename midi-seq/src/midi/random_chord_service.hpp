#pragma once

#include <vector>

#include "chords.hpp"
#include "rng_service.hpp"
#include "scale.hpp"

class RandomChordService {
public:
    RngService& rngService;

    int root;
    int mode;
    Scale scale;

    int curLowDegree;
    int prevLowDegree;

    // how many notes are in the mode above the 2nd octave
    std::vector<int> modeUpperNotes;

    RandomChordService(
        RngService& rngService,
        int root,
        int mode
    ) :
        rngService(rngService),
        root(root),
        mode(mode),
        scale(root, mode),
        curLowDegree(0),
        prevLowDegree(0),
        modeUpperNotes({
            1,
            2,
            2,
            1,
            1,
            2,
            2,
        })
    {}

    // generate a random (potentially inverted) chord in range [root, root + 13]
    // where root is the root note of a major scale
    std::vector<int> getChord() {
        ChordInversion inv = (ChordInversion)rngService.getRand(0, 2);

        int topNote = 13 + modeUpperNotes[mode];

        int lowDegreeLimit = 0;
        if (inv == ROOT) {
            lowDegreeLimit = topNote - 4;
        } else {
            lowDegreeLimit = topNote - 5;
        }

        getCurLowDegree(lowDegreeLimit);

        std::vector<int> chord(3, 0);

        switch (inv) {
            case ROOT:
                chord[0] = scale.getNote(curLowDegree);
                chord[1] = scale.getNote(curLowDegree + 2);
                chord[2] = scale.getNote(curLowDegree + 4);
                break;
            case FIRST_INV:
                chord[0] = scale.getNote(curLowDegree);
                chord[1] = scale.getNote(curLowDegree + 2);
                chord[2] = scale.getNote(curLowDegree + 5);
                break;
            case SECOND_INV:
                chord[0] = scale.getNote(curLowDegree);
                chord[1] = scale.getNote(curLowDegree + 3);
                chord[2] = scale.getNote(curLowDegree + 5);
                break;
        }

        return chord;
    }

    void getCurLowDegree(int lowDegreeLimit) {
        getRandIntNonRepeating(
            curLowDegree,
            prevLowDegree,
            lowDegreeLimit
        );
    }

    void getRandIntNonRepeating(int& cur, int& prev, int highLimitInclusive) {
        cur = rngService.getRand(0, highLimitInclusive);
        while (cur == prev) {
            cur = rngService.getRand(0, highLimitInclusive);
        }
        prev = cur;
    }
};
