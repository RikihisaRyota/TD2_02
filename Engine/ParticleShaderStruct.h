#pragma once

#include <cstdint>

#include "Vector3.h"
#include "Vector4.h"

struct Emitter {
    Vector3 scale = {1.0f,1.0f,1.0f};
    Vector3 rotate = {0.0f,0.0f,0.0f};
    Vector3 position;
    Vector4 color = { 1.0f,1.0f,1.0f,1.0f };
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