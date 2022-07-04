#pragma warning(suppress : 4244)

#ifndef RNGSEED_H_
#define RNGSEED_H_

/** A more effective seeded random number generator struct, made by kiss rng.
 * The numbers that are generated are inclusive of rhs value
 * @since adxe-1.0.0b8
 */
struct RngSeed
{
    const uint32_t RNG_RAND_MAX        = 4294967295;
    const uint32_t RNG_RAND_MAX_SIGNED = 2147483647;
    uint32_t _x                        = 1;
    uint32_t _y                        = 2;
    uint32_t _z                        = 4;
    uint32_t _w                        = 8;
    uint32_t _carry                    = 0;
    uint32_t _k                        = 0;
    uint32_t _m                        = 0;
    uint32_t _seed                     = 0;

    RngSeed() { seed_rand(time(NULL)); }

    // initialize this object with seed
    void seed_rand(unsigned long seed)
    {
        _seed  = seed;
        _x     = seed | 1;
        _y     = seed | 2;
        _z     = seed | 4;
        _w     = seed | 8;
        _carry = 0;
    }

    // returns an unsigned long random value
    uint32_t rand()
    {
        _x = _x * 69069 + 1;
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
        return floor(min + static_cast<float>(rand()) / (static_cast<float>(RNG_RAND_MAX / (max - min))));
    }

    // returns a random unsigned integer from min to max
    uint32_t rangeu(uint32_t min, uint32_t max)
    {
        return floor(min + static_cast<float>(rand()) / (static_cast<float>(RNG_RAND_MAX / (max - min))));
    }

    // returns a random float from min to max
    float rangef(float min = -1.0F, float max = 1.0F)
    {
        return min + static_cast<float>(rand()) / (static_cast<float>(RNG_RAND_MAX / (max - min)));
    }

    // returns a random integer from 0 to max
    int32_t max(int32_t max = INT_MAX)
    {
        return floor(0 + static_cast<float>(rand()) / (static_cast<float>(RNG_RAND_MAX / (max - 0))));
    }

    // returns a random unsigned integer from 0 to max
    uint32_t maxu(uint32_t max = UINT_MAX)
    {
        return floor(0 + static_cast<float>(rand()) / (static_cast<float>(RNG_RAND_MAX / (max - 0))));
    }

    // returns a random float from 0.0 to max
    float maxf(float max) { return 0 + static_cast<float>(rand()) / (static_cast<float>(RNG_RAND_MAX / (max - 0))); }

    // returns a random float from 0.0 to 1.0
    float float01() { return 0 + static_cast<float>(rand()) / (static_cast<float>(RNG_RAND_MAX / (1 - 0))); }

    // returns either false or true randomly
    bool bool01() { return (bool)floor(0 + static_cast<float>(rand()) / (static_cast<float>(RNG_RAND_MAX / (2 - 0)))); }
};

#endif
