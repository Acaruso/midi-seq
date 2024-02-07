#pragma once

#include "rng_service.hpp"

class GeneratorChordSingleNote {
public:
    RngService& rngService;

    GeneratorChordSingleNote(
        RngService& rngService
    ) :
        rngService(rngService)
    {}

};
