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

#include "spine/spine.h"
#include <string>
#include <string_view>
#include <unordered_map>
#include "xxhash/xxhash.h"
#include "axmol/base/Object.h"

namespace spine {
	class SP_API SkeletonAsset : public ax::Object {
	public:
		static SkeletonAsset *obtain(std::string_view dataFile,
									 std::string_view atlasFile,
									 float scale);

		std::string_view getDataFile() const { return _dataFile; }
		std::string_view getAtlasFile() const { return _atlasFile; }
		float getScale() const { return _scale; }

		spine::SkeletonData *getSkeletonData() const { return _skeletonData; }

	protected:
		SkeletonAsset(std::string_view dataFile,
					  std::string_view atlasFile,
					  float scale,
					  spine::Atlas *atlas,
					  spine::AttachmentLoader *loader,
					  spine::SkeletonData *data);
		~SkeletonAsset() override;

		std::string _dataFile;
		std::string _atlasFile;
		float _scale{1.0f};
		spine::Atlas *_atlas{nullptr};
		spine::AttachmentLoader *_attachmentLoader{nullptr};
		spine::SkeletonData *_skeletonData{nullptr};
	};
}// namespace spine
