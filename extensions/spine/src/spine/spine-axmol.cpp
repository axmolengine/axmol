/******************************************************************************
 * Spine Runtimes License Agreement
 * Last updated April 5, 2025. Replaces all prior versions.
 *
 * Copyright (c) 2013-2025, Esoteric Software LLC
 * Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).
 *
 * https://axmol.dev/
 *
 * Integration of the Spine Runtimes into software or otherwise creating
 * derivative works of the Spine Runtimes is permitted under the terms and
 * conditions of Section 2 of the Spine Editor License Agreement:
 * http://esotericsoftware.com/spine-editor-license
 *
 * Otherwise, it is permitted to integrate the Spine Runtimes into software
 * or otherwise create derivative works of the Spine Runtimes (collectively,
 * "Products"), provided that each user of the Products must obtain their own
 * Spine Editor license and redistribution of the Products in any form must
 * include this license and copyright notice.
 *
 * THE SPINE RUNTIMES ARE PROVIDED BY ESOTERIC SOFTWARE LLC "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ESOTERIC SOFTWARE LLC BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES,
 * BUSINESS INTERRUPTION, OR LOSS OF USE, DATA, OR PROFITS) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THE SPINE RUNTIMES, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *****************************************************************************/

#include <spine/spine-axmol.h>

#include <spine/SkeletonBatch.h>
#include <spine/SkeletonTwoColorBatch.h>

#include "axmol/base/EventListener.h"
#include "axmol/base/Director.h"
#include "axmol/base/EventDispatcher.h"
#include "axmol/renderer/TextureCache.h"
#include "axmol/platform/FileUtils.h"

namespace spine {

	using namespace ax;

	static Texture2D::TexParams chooseTexParams(spine::AtlasPage &page) {
		Texture2D::TexParams texParams{};
		switch (page.minFilter) {
			case TextureFilter_Linear:
				texParams.minFilter = rhi::SamplerFilter::MIN_LINEAR;
				break;
			case TextureFilter_Nearest:
				texParams.minFilter = rhi::SamplerFilter::MIN_NEAREST;
				break;
			case TextureFilter_MipMap:
				texParams.minFilter = rhi::SamplerFilter::MIN_LINEAR;
				break;
			case TextureFilter_MipMapNearestNearest:
				texParams.minFilter = rhi::SamplerFilter::MIN_NEAREST;
				texParams.mipFilter = rhi::SamplerFilter::MIP_NEAREST;
				break;
			case TextureFilter_MipMapLinearNearest:
				texParams.minFilter = rhi::SamplerFilter::MIN_NEAREST;
				texParams.mipFilter = rhi::SamplerFilter::MIP_LINEAR;
				break;
			case TextureFilter_MipMapNearestLinear:
				texParams.minFilter = rhi::SamplerFilter::MIN_LINEAR;
				texParams.mipFilter = rhi::SamplerFilter::MIP_NEAREST;
				break;
			case TextureFilter_MipMapLinearLinear:
				texParams.minFilter = rhi::SamplerFilter::MIN_LINEAR;
				texParams.mipFilter = rhi::SamplerFilter::MIP_LINEAR;
				break;
		}
		switch (page.magFilter) {
			case TextureFilter_Linear:
				texParams.magFilter = rhi::SamplerFilter::MAG_LINEAR;
				break;
			case TextureFilter_Nearest:
				texParams.magFilter = rhi::SamplerFilter::MAG_NEAREST;
				break;
		}

		texParams.sAddressMode = page.uWrap == TextureWrap_ClampToEdge ? rhi::SamplerAddressMode::CLAMP_TO_EDGE
																	   : rhi::SamplerAddressMode::REPEAT;
		texParams.tAddressMode = page.vWrap == TextureWrap_ClampToEdge ? rhi::SamplerAddressMode::CLAMP_TO_EDGE
																	   : rhi::SamplerAddressMode::REPEAT;
		return texParams;
	}


	// Axmol Texture Loader for internal use
	class SP_API AxmolTextureLoader : public TextureLoader {
		friend class AxmolSpineExtension;

	protected:
		AxmolTextureLoader() : TextureLoader() {}
		~AxmolTextureLoader() = default;

	public:
		void load(AtlasPage &page, const String &path) override {
			Texture2D *texture = Director::getInstance()->getTextureCache()->addImage(to_string_view(path));
			AXASSERT(texture != nullptr, "Invalid image");
			if (texture) {
				texture->retain();
				Texture2D::TexParams texParams = chooseTexParams(page);
				texture->setTexParameters(texParams);

				page.texture = texture;
				page.width = texture->getWidth();
				page.height = texture->getHeight();
			}
		}
		void unload(void *texture) override {
			if (texture) {
				((Texture2D *) texture)->release();
			}
		}
	};

	AxmolSpineExtension *AxmolSpineExtension::_instance = nullptr;

	AxmolSpineExtension *AxmolSpineExtension::getInstance() {
		if (!_instance) {
			_instance = new AxmolSpineExtension();
			_instance->init();
		}
		return _instance;
	}

	void AxmolSpineExtension::init() {

		// Spine 4.3 defaults to a y-down coordinate system for some runtimes.
		// Axmol's 2D scene graph is y-up, so keep Spine world vertices in the
		// same coordinate space as Axmol nodes and avoid flipping in the renderer.
		spine::Bone::setYDown(false);

		if (!_textureLoader)
			_textureLoader = new AxmolTextureLoader();
		auto eventDispatcher = ax::Director::getInstance()->getEventDispatcher();

		if (!_gfxDropListener) {
			auto _onBeforeGfxDrop = [eventDispatcher](ax::CustomEvent *) {
				spine::SkeletonAssetCache::destroyInstance();
				spine::SkeletonBatch::destroyInstance();
				spine::SkeletonTwoColorBatch::destroyInstance();
				if (_instance) {
					eventDispatcher->removeEventListener(_instance->_gfxDropListener);
					_instance->_gfxDropListener = nullptr;
				}
			};
			_gfxDropListener = eventDispatcher->addCustomEventListener(ax::Director::EVENT_BEFORE_GFX_DROP, _onBeforeGfxDrop);
		}

		if (!_directorDisposingListener) {
			static constexpr int SPINE_EXTENSION_DTOR_PRIORITY = 2;
			auto _destroySpineExtension = [eventDispatcher](ax::CustomEvent *) {
				if (_instance) {
					_instance->cleanup();
				}
				AX_SAFE_DELETE(_instance);
			};
			_directorDisposingListener = eventDispatcher->addCustomEventListener(ax::Director::EVENT_DISPOSING, _destroySpineExtension, SPINE_EXTENSION_DTOR_PRIORITY);
		}
	}

	void AxmolSpineExtension::cleanup() {

		AX_SAFE_DELETE(_textureLoader);

		auto eventDispatcher = ax::Director::getInstance()->getEventDispatcher();

		AXASSERT(!_gfxDropListener, "AxmolSpineExtension: Gfx drop listener must be removed before dispose");

		if (_directorDisposingListener) {
			eventDispatcher->removeEventListener(_directorDisposingListener);
			_directorDisposingListener = nullptr;
		}
	}

	char *AxmolSpineExtension::_readFile(const spine::String &path, int *length) {
		Data data = FileUtils::getInstance()->getDataFromFile(to_string_view(path));
		if (data.isNull()) return nullptr;

		// avoid buffer overflow (int is shorter than ssize_t in certain platforms)
		ssize_t tmpLen;
		char *ret = (char *) data.takeBuffer(&tmpLen);
		*length = static_cast<int>(tmpLen);
		return ret;
	}

	SpineExtension *getDefaultExtension() {
		return AxmolSpineExtension::getInstance();
	}

}// namespace spine
