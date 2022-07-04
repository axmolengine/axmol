#ifndef RNGSEED_H_
#define RNGSEED_H_

/** A more effective seeded random number generator struct, made by kiss rng.
 * @since adxe-1.0.0b8
 */
struct RngSeed
{
    const unsigned long RNG_RAND_MAX        = 4294967295;
    const unsigned long RNG_RAND_MAX_SIGNED = 2147483647;
    unsigned long _x                        = 1;
    unsigned long _y                        = 2;
    unsigned long _z                        = 4;
    unsigned long _w                        = 8;
    unsigned long _carry                    = 0;
    unsigned long _k                        = 0;
    unsigned long _m                        = 0;
    unsigned long _seed                     = 0;

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
    unsigned long rand()
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
    int range(int min, int max)
    {
        return floor(min + static_cast<float>(rand()) / (static_cast<float>(RNG_RAND_MAX / (max - min))));
    }

    // returns a random unsigned integer from min to max
    unsigned int rangeu(unsigned int min, unsigned int max)
    {
        return floor(min + static_cast<float>(rand()) / (static_cast<float>(RNG_RAND_MAX / (max - min))));
    }

    // returns a random float from min to max
    float rangef(float min = -1.0F, float max = 1.0F)
    {
        return min + static_cast<float>(rand()) / (static_cast<float>(RNG_RAND_MAX / (max - min)));
    }

    // returns a random integer from 0 to max
    int max(int max = INT_MAX)
    {
        return floor(0 + static_cast<float>(rand()) / (static_cast<float>(RNG_RAND_MAX / (max - 0))));
    }

    // returns a random unsigned integer from 0 to max
    unsigned int maxu(unsigned int max = UINT_MAX)
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
