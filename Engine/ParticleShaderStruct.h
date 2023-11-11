#pragma once

#include <cstdint>

#include "Vector3.h"

struct Emitter {
    Vector3 position;
    int32_t aliveTime;
    int32_t createParticle;
};

struct ParticleMotion {
    Vector3 position;
    Vector3 velocity;
    Vector3 acceleration;
    int32_t aliveTime;
};