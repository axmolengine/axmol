#ifndef BACKENDS_XAUDIO2_H
#define BACKENDS_XAUDIO2_H

#include "base.h"

struct XAudio2BackendFactory final : public BackendFactory
{
public:
    bool init() override;

    bool querySupport(BackendType type) override;

    std::string probe(BackendType type) override;

    BackendPtr createBackend(DeviceBase* device, BackendType type) override;

    static BackendFactory& getFactory();
};

#endif /* BACKENDS_XAUDIO2_H */
