#pragma once

#include <cstdint>

#include "Vector3.h"
#include "Vector4.h"

struct Emitter {
    Vector3 position;
    int32_t inOnce;
    int32_t aliveTime;
    bool isAlive;
};

struct ParticleMotion {
    Vector3 position;
    Vector3 velocity;
    Angel angle;
    Color color;
    Scale scale;
    Vector3 acceleration;
    uint32_t aliveTime;
    bool isAlive;
    ParticleForGPU 
};

struct ParticleForGPU {
    Matrix4x4 matWorld;
    Vector4 color;
};

struct Scale {
    Vector4 startScale;
    Vector4 endScale;
    Vector4 scale;
};

struct Color {
    Vector4 startColor;
    Vector4 endColor;
    Vector4 color;
};

struct Angel {
    float start;
    float end;
};

struct AliveTime {
    int32_t time;
    int32_t randomRange;
};

struct Velocity {
    Vector3 velocity;
    Vector3 randomRange;

};