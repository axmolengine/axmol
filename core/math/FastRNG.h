#ifndef _FAST_RNG_H__
#define _FAST_RNG_H__

#include <cmath>
#include <limits.h>

/** A fast more effective seeded random number generator struct, made by kiss rng.
 * It uses a simple algorithm to improve the speed of generating random numbers with a decent quality,
 * Use this if you're planning to generate large amounts of random numbers in a single frame.
 *
 * @since axmol-1.0.0b8
 */
struct FastRNG
{
    const uint32_t RNG_RAND_MAX = std::numeric_limits<uint32_t>::max();
    uint32_t _x     = 1;
    uint32_t _y     = 2;
    uint32_t _z     = 4;
    uint32_t _w     = 8;
    uint32_t _carry = 0;
    uint32_t _k     = 0;
    uint32_t _m     = 0;
    uint64_t _seed  = 0;

    FastRNG() { seed_rng((uint32_t)time(NULL)); }
    FastRNG(uint64_t seed) { seed_rng_64(seed); }

    // initialize this object with seed
    void seed_rng(uint64_t seed)
    {
        auto seed1 = static_cast<uint32_t>(seed);
        auto seed2 = static_cast<uint32_t>(seed >> 32);

        _seed  = seed;
        _x     = static_cast<int32_t>(seed1) | 1;
        _y     = static_cast<int32_t>(seed2) | 2;
        _z     = static_cast<int32_t>(seed1) | 4;
        _w     = static_cast<int32_t>(seed2) | 8;
        _carry = 0;
    }

    // initialize this object with a uint64_t seed
    // DEPRECATED: use seed_rng instead
    void seed_rng_64(uint64_t seed) { seed_rng(seed); }

    // returns a random uint32_t value
    uint32_t rng()
    {
        _x = _x * 69069 + 12345;
        _y ^= _y << 13;
        _y ^= _y >> 17;
        _y ^= _y << 5;
        _k     = (_z >> 2) + (_w >> 3) + (_carry >> 2);
        _m     = _w + _w + _z + _carry;
        _z     = _w;
        _w     = _m;
        _carry = _k >> 30;
        return _x + _y + _w;
    }

    // returns a random integer from min to max
    int32_t range(int32_t min, int32_t max)
    {
        return static_cast<int32_t>(min + rng() / (RNG_RAND_MAX / (max - min)));
    }

    // returns a random unsigned integer from min to max
    uint32_t rangeu(uint32_t min, uint32_t max)
    {
        return static_cast<uint32_t>(min + rng() / (RNG_RAND_MAX / (max - min)));
    }

    // returns a random integer from 0 to max
    int32_t max(int32_t max = INT_MAX)
    {
        return static_cast<int32_t>(static_cast<float>(rng()) / static_cast<float>(RNG_RAND_MAX / (max - 0)));
    }

    // returns a random unsigned integer from 0 to max
    uint32_t maxu(uint32_t max = UINT_MAX) { return static_cast<uint32_t>(0 + rng() / (RNG_RAND_MAX / (max - 0))); }

    // returns a random float from min to max
    float rangef(float min = -1.0F, float max = 1.0F)
    {
        return min + static_cast<float>(rng()) / (static_cast<float>(RNG_RAND_MAX) / (max - min));
    }

    // returns a random float from 0.0 to max
    float maxf(float max) { return static_cast<float>(rng()) / (static_cast<float>(RNG_RAND_MAX) / (max - 0.0f)); }

    // returns a random float from 0.0 to 1.0
    float float01() { return static_cast<float>(rng()) / (static_cast<float>(RNG_RAND_MAX) / (1.0f - 0.0f)); }

    // returns either false or true randomly
    bool bool01() { return static_cast<bool>(rng() & 1); }
};

#endif // _FAST_RNG_H__
