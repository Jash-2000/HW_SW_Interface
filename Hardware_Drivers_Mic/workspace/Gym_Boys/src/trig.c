#include "trig.h"
#include <math.h>

// --------------------------------------------
// Fast LUT-based sine / cosine implementation
// --------------------------------------------

#define TRIG_TABLE_SIZE 1024          // number of intervals (1025 samples)

// One-time initialized tables for sin and cos over [-PI, PI]
static int   trig_initialized = 0;
static float sin_table[TRIG_TABLE_SIZE + 1];
static float cos_table[TRIG_TABLE_SIZE + 1];

static void trig_init(void)
{
    if (trig_initialized) {
        return;
    }

    // Fill tables with true sin/cos once at startup.
    // Inputs to sine/cosine in fft.c are always in [-PI, 0], so [-PI, PI] covers everything.
    for (int i = 0; i <= TRIG_TABLE_SIZE; ++i) {
        float t     = (float)i / (float)TRIG_TABLE_SIZE; // 0..1
        float angle = -PI + 2.0f * PI * t;               // -PI .. PI

        sin_table[i] = sinf(angle);
        cos_table[i] = cosf(angle);
    }

    trig_initialized = 1;
}

// Linear lookup helper: x in [-PI, PI]
static float trig_lookup(float x, const float *table)
{
    // Clamp to table range
    if (x <= -PI) {
        return table[0];
    }
    if (x >= PI) {
        return table[TRIG_TABLE_SIZE];
    }

    // Map x from [-PI, PI] -> [0, TRIG_TABLE_SIZE]
    float t   = (x + PI) / (2.0f * PI);             // 0..1
    float pos = t * (float)TRIG_TABLE_SIZE;
    int   idx = (int)pos;                           // floor
    float frac = pos - (float)idx;                 // fractional part

    float v0 = table[idx];
    float v1 = table[idx + 1];

    // Linear interpolation between neighboring table entries
    return v0 + frac * (v1 - v0);
}

// --------------------------------------------
// Public API (used by fft.c)
// --------------------------------------------

int factorial(int a)
{
    // Not used by our fast sine/cosine anymore, but keep a cheap, safe version
    if (a <= 1) return 1;
    int r = 1;
    for (int i = 2; i <= a; ++i) {
        r *= i;
    }
    return r;
}

float sine(float x)
{
    trig_init();
    return trig_lookup(x, sin_table);
}

float cosine(float x)
{
    trig_init();
    return trig_lookup(x, cos_table);
}
