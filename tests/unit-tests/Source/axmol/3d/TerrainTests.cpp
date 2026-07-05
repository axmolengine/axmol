#include <doctest.h>

#include <string>
#include <vector>

#include "axmol/3d/Terrain.h"
#include "axmol/base/PointerEvent.h"
#include "axmol/platform/FileUtils.h"
#include "axmol/platform/Image.h"
#include "axmol/rhi/GraphicsCore.h"
#include "axmol/scene/Camera.h"

using namespace ax;

namespace
{
std::string writeTestImage(std::string_view name, int width, int height, uint8_t value)
{
    std::vector<uint8_t> pixels(static_cast<size_t>(width * height * 4), 255);
    for (size_t i = 0; i < pixels.size(); i += 4)
    {
        pixels[i]     = value;
        pixels[i + 1] = value;
        pixels[i + 2] = value;
        pixels[i + 3] = 255;
    }

    auto fileUtils = FileUtils::getInstance();
    auto writable  = fileUtils->getWritablePath();
    auto filename  = std::string(name);
    auto path      = writable + filename;

    Image image;
    REQUIRE(image.initWithRawData(pixels.data(), static_cast<ssize_t>(pixels.size()), width, height, 8));
    REQUIRE(image.saveToFile(path));
    fileUtils->addSearchPath(writable, true);
    return filename;
}

void checkVec3Near(const Vec3& actual, const Vec3& expected, float epsilon = 0.001f)
{
    CHECK(actual.x == doctest::Approx(expected.x).epsilon(epsilon));
    CHECK(actual.y == doctest::Approx(expected.y).epsilon(epsilon));
    CHECK(actual.z == doctest::Approx(expected.z).epsilon(epsilon));
}
}  // namespace

TEST_CASE("Terrain ray hit keeps local intersection and pointer world hit spaces separate")
{
    if (!rhi::GraphicsCore::currentDriver())
    {
        WARN("Terrain GPU resources require an active RHI driver; skipping this environment.");
        return;
    }

    const std::string heightMap = writeTestImage("terrain-hit-height.png", 4, 4, 128);
    const std::string detailMap = writeTestImage("terrain-hit-detail.png", 4, 4, 200);

    Terrain::TerrainData data(heightMap, detailMap, Vec2(2.0f, 2.0f), 10.0f, 1.0f);
    data._detailMapAmount = 1;
    auto terrain          = Terrain::create(data, Terrain::CrackFixedType::INCREASE_LOWER);
    REQUIRE(terrain != nullptr);
    terrain->retain();

    terrain->setPosition3D(Vec3(25.0f, 7.0f, -11.0f));
    terrain->setScale(2.0f);

    Vec3 worldRayOrigin(0.0f, 20.0f, 0.0f);
    terrain->getNodeToWorldTransform().transformPoint(&worldRayOrigin);
    Ray worldRay(worldRayOrigin, Vec3(0.0f, -1.0f, 0.0f));

    Vec3 localHit;
    REQUIRE(terrain->getIntersectionPoint(worldRay, localHit));
    CHECK(localHit.x == doctest::Approx(0.0f).epsilon(0.001f));
    CHECK(localHit.z == doctest::Approx(0.0f).epsilon(0.001f));

    Vec3 expectedWorldHit = localHit;
    terrain->getNodeToWorldTransform().transformPoint(&expectedWorldHit);

    PointerInputState inputState;
    inputState.id   = 42;
    inputState.type = PointerType::Controller;

    PointerEvent event;
    event.setPointerInfo(InputPhase::PointerMove, Vec2::zero, inputState);
    event.setRay(worldRay);

    auto camera = Camera::createPerspective(60.0f, 1.0f, 0.1f, 100.0f);
    REQUIRE(camera != nullptr);

    Vec3 pointerWorldHit;
    REQUIRE(terrain->onPointerHitTest(&event, camera, &pointerWorldHit));
    checkVec3Near(pointerWorldHit, expectedWorldHit);

    const float distanceOnRay = (pointerWorldHit - worldRay.origin).dot(worldRay.direction);
    Vec3 closestOnRay         = worldRay.origin + worldRay.direction * distanceOnRay;
    checkVec3Near(pointerWorldHit, closestOnRay);

    terrain->release();
}
