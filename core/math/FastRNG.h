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
    uint32_t s[4];

    uint64_t nextSeed(uint64_t& state)
    {
        uint64_t z = (state += 0x9e3779b97f4a7c15);
        z          = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9;
        z          = (z ^ (z >> 27)) * 0x94d049bb133111eb;
        return z ^ (z >> 31);
    }

    FastRNG() { seed(static_cast<uint64_t>(rand()) << 32 | rand()); }

    void seed(uint64_t seed)
    {
        uint64_t state = seed;
        uint64_t states[2];
        memset(states, 0, 16);
        states[0] = nextSeed(state);
        states[1] = nextSeed(state);
        memcpy(s, states, 16);
    }

    static inline uint32_t rotL(const uint32_t x, int k) { return (x << k) | (x >> (32 - k)); }

    uint32_t next()
    {
        const uint32_t result = rotL(s[1] * 5, 7) * 9;

        const uint32_t t = s[1] << 9;

        s[2] ^= s[0];
        s[3] ^= s[1];
        s[1] ^= s[2];
        s[0] ^= s[3];

        s[2] ^= t;

        s[3] = rotL(s[3], 11);

        return result;
    }

    // generates a random real that ranges from 0.0 to 1.0
    template <typename T>
    T nextReal()
    {
        if (std::is_same<T, float>::value)
            return static_cast<T>(next() >> 8) * 0x1.0p-24f;
        else if (std::is_same<T, double>::value)
            return static_cast<T>((static_cast<uint64_t>(next()) << 32 | next()) >> 11) * 0x1.0p-53;
        return 0;  // possibly assert?
    }

    // generates a random real that ranges from min to max and is uniformly distributed
    template <typename T>
    T nextReal(T min, T max)
    {
        return static_cast<T>(min + nextReal<T>() * (max - min));
    }

    // generates a random integer that ranges from min inclusive to max exclusive [min, max) and is uniformly
    // distributed
    template <typename T>
    T nextInt(T min, T max)
    {
        return min + static_cast<T>(nextMax(static_cast<uint32_t>(max - min)));
    }

    // generates a random integer from 0 to max that is uniformly distributed
    uint32_t nextMax(uint32_t max)
    {
        uint64_t multiresult = static_cast<uint64_t>(next()) * max;
        uint32_t leftover    = static_cast<uint32_t>(multiresult);
        if (leftover < max)
        {
            uint32_t threshold = (0 - max) % max;
            while (leftover < threshold)
            {
                multiresult = static_cast<uint64_t>(next()) * max;
                leftover    = static_cast<uint32_t>(multiresult);
            }
        }
        return multiresult >> 32;
    }

    // wrapper for nextInt<int32_t>(min, max)
    int32_t range(int32_t min, int32_t max) { return nextInt<int32_t>(min, max); }
    // wrapper for nextInt<int32_t>(0, max)
    int32_t max(int32_t max = INT32_MAX) { return nextInt<int32_t>(0, max); }

    // wrapper for nextInt<uint32_t>(min, max)
    uint32_t rangeu(uint32_t min, uint32_t max) { return nextInt<uint32_t>(min, max); }
    // wrapper for nextInt<uint32_t>(0, max)
    uint32_t maxu(uint32_t max = UINT_MAX) { return nextInt<uint32_t>(0, max); }

    // wrapper for nextReal<float>(min, max)
    float rangef(float min = -1.0F, float max = 1.0F) { return nextReal<float>(min, max); }
    // wrapper for nextReal<float>(0, max)
    float maxf(float max) { return nextReal<float>(0, max); }

    // wrapper for nextReal<double>(min, max)
    double ranged(double min = -1.0F, double max = 1.0F) { return nextReal<double>(min, max); }
    // wrapper for nextReal<double>(0, max)
    double maxd(double max) { return nextReal<double>(0, max); }

    // wrapper for nextReal<float>()
    float float01() { return nextReal<float>(); }
    // wrapper for nextReal<double>()
    float double01() { return nextReal<double>(); }
    // wrapper for next() & 1, true or false based on LSB
    float bool01() { return next() & 1; }
};

#endif // _FAST_RNG_H__
