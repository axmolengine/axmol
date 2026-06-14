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

#pragma once

#include <spine/Extension.h>
#include <spine/SkeletonAnimation.h>
#include <spine/SkeletonAssetCache.h>

#define AX_SPINE_VERSION 0x040300

namespace ax {
	class EventListener;
	class Renderer;
}// namespace ax

namespace spine {

	class SkeletonBatch;
	class SkeletonTwoColorBatch;

	/**
     * @class AxmolSpineExtension
     * @brief Spine runtime extension for Axmol.
     *
     * This extension bridges Spine with Axmol, providing texture loading
     * and lifecycle management. It ensures that Spine-related singleton
     * objects such as SkeletonAssetCache, SkeletonBatch, and
     * SkeletonTwoColorBatch are properly destroyed at the right time
     * (e.g. BEFORE_GFX_DROP or DISPOSING events) to avoid resource leaks
     * or crashes.
     */
	class SP_API AxmolSpineExtension : public DefaultSpineExtension {
	public:
		static AxmolSpineExtension *getInstance();

		spine::TextureLoader *getTextureLoader() { return _textureLoader; }

	protected:
		AxmolSpineExtension() = default;
		~AxmolSpineExtension() = default;

	protected:
		char *_readFile(const String &path, int *length) override;

		void init();
		void cleanup();

		TextureLoader *_textureLoader{nullptr};
		ax::EventListener *_directorDisposingListener{nullptr};
		ax::EventListener *_gfxDropListener{nullptr};
		static AxmolSpineExtension *_instance;
	};

	inline std::string_view to_string_view(const spine::String &str) {
		return std::string_view(str.buffer(), str.length());
	}

}// namespace spine
