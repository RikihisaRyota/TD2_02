#pragma once

#include <cstdint>

#include "Matrix4x4.h"
#include "Vector3.h"
#include "Vector4.h"

struct ParticleForGPU {
    Matrix4x4 matWorld;
    Vector4 color;
};

struct Scale {
    Vector3 startScale;
    Vector3 interimScale;
    Vector3 endScale;
    Vector3 currentScale;
};

struct Rotate {
    Vector3 addRotate;
    Vector3 currentRotate;
};

struct ParticleColor {
    Vector4 startColor;
    Vector4 endColor;
    Vector4 currentColor;
};

struct Angel {
    float start;
    float end;
    float current;
};

struct AliveTime {
    int32_t maxTime;
    int32_t time;
    int32_t randomRange;
};

struct Velocity {
    float speed;
    Vector3 velocity;
    float randomRange;
};

struct Spawn {
    Vector3 position;
    float rangeX;
    float rangeY;
};

struct RandomScale {
    Vector3 startRandomRange;
    Vector3 interimRandomRange;
    Vector3 endRandomRange;
};

struct Color {
    Vector4 startColor_;
    Vector4 startBeginMinRandomColor_;
    Vector4 startBeginMaxRandomColor_;

    Vector4 endColor_;
    Vector4 endBeginMinRandomColor_;
    Vector4 endBeginMaxRandomColor_;
};

struct Emitter {
    Spawn spawn;
    Angel angle;
    RandomScale randomScale;
    Scale scale;
    Color color;
    int32_t inOnce;
    int32_t flameInterval;
    int32_t aliveTime;
    bool isAlive;
};

struct ParticleMotion {
    Scale scale;
    Rotate rotate;
    Vector3 position;
    Velocity velocity;
    Vector3 acceleration_;
    ParticleColor color;
    AliveTime aliveTime;
    bool isAlive;
    ParticleForGPU constantBuff;
};
