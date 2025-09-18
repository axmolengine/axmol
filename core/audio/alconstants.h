#pragma once

namespace ax
{
namespace al
{
enum class Sourcei
{
    EfxDirectFilter       = 0x20005,
    EfxAuxilarySendFilter = 0x20006,
};

enum class Sourcef
{
    Pitch             = 0x1003,
    Gain              = 0x100A,
    ReferenceDistance = 0x1020,
    StereoAngles      = 0x1030
};

enum class Listener3f
{
    Position = 0x1004,
};

enum class Source3f
{
    Position = 0x1004,
    Velocity = 0x1006,
};

enum class DistanceModel
{
    None                   = 0,
    InverseDistanceClamped = 0xD002,
};

namespace efx
{
enum class Filteri
{
    FilterType = 0x8001,
};

enum class Filterf
{
    LowpassGain    = 0x0001,
    LowpassGainHF  = 0x0002,
    HighpassGain   = 0x0001,
    HighpassGainLF = 0x0002,
    BandpassGain   = 0x0001,
    BandpassGainLF = 0x0002,
    BandpassGainHF = 0x0003,
};

enum class FilterType
{
    None     = 0x0000,
    Lowpass  = 0x0001,
    Highpass = 0x0002,
    Bandpass = 0x0003,
};

enum class Effecti
{
    EffectType = 0x8001,
    SlotEffect = 0x0001,
};

enum class EffectSlotf
{
    EffectSlotGain = 0x0002,
};

enum class ReverbParamIds
{
    Density             = 0x0001,
    Diffusion           = 0x0002,
    Gain                = 0x0003,
    GainHF              = 0x0004,
    DecayTime           = 0x0005,
    DecayHFRatio        = 0x0006,
    ReflectionsGain     = 0x0007,
    ReflectionsDelay    = 0x0008,
    LateReverbGain      = 0x0009,
    LateReverbDelay     = 0x000A,
    AirAbsorptionGainHF = 0x000B,
    RoomRolloffFactor   = 0x000C,
    DecayHFLimit        = 0x000D,
};

enum class EaxReverbParamIds
{
    Density             = 0x0001,
    Diffusion           = 0x0002,
    Gain                = 0x0003,
    GainHF              = 0x0004,
    GainLF              = 0x0005,
    DecayTime           = 0x0006,
    DecayHFRatio        = 0x0007,
    DecayLFRatio        = 0x0008,
    ReflectionsGain     = 0x0009,
    ReflectionsDelay    = 0x000A,
    ReflectionsPan      = 0x000B,
    LateReverbGain      = 0x000C,
    LateReverbDelay     = 0x000D,
    LateReverbPan       = 0x000E,
    EchoTime            = 0x000F,
    EchoDepth           = 0x0010,
    ModulationTime      = 0x0011,
    ModulationDepth     = 0x0012,
    AirAbsorptionGainHF = 0x0013,
    HFReference         = 0x0014,
    LFReference         = 0x0015,
    RoomRolloffFactor   = 0x0016,
    DecayHFLimit        = 0x0017,
};

enum class EffectType
{
    Reverb    = 0x0001,
    EaxReverb = 0x8000,
};

}  // namespace efx
}  // namespace al
}  // namespace ax
