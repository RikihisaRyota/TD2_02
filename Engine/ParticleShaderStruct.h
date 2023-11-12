#pragma once

#include <cstdint>

#include "Vector3.h"
#include "Vector4.h"

struct Emitter {
    Vector3 position;
    Vector4 color;
    int32_t createParticle;
    int32_t aliveTime;
    bool isAlive;
};

struct ParticleMotion {
    Vector3 position;
    Vector3 velocity;
    Vector3 acceleration;
    int32_t aliveTime;
    bool isAlive;
};