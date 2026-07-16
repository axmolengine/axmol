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
#include "spine/SkeletonAsset.h"

#include "spine/spine-axmol.h"
#include "axmol/tlx/format.hpp"
#include "yasio/tlx/string_view.hpp"

namespace spine {
	SkeletonAsset *SkeletonAsset::obtain(std::string_view dataFile,
										 std::string_view atlasFile,
										 float scale) {
		spine::Atlas *atlas = nullptr;
		spine::AttachmentLoader *loader = nullptr;
		spine::SkeletonData *skeletonData = nullptr;

		do {
			atlas = new spine::Atlas(spine::String{atlasFile}, spine::AxmolSpineExtension::getInstance()->getTextureLoader());
			if (!atlas)
				break;

			loader = new spine::AtlasAttachmentLoader(*atlas);

			if (tlx::ic::ends_with(dataFile, ".skel")) {
				spine::SkeletonBinary binary(*loader);
				binary.setScale(scale);
				skeletonData = binary.readSkeletonDataFile(spine::String{dataFile});
				if (!binary.getError().isEmpty()) {
					AXLOGE("#parse spine .skel data file failed, error:{}", binary.getError().buffer());
					break;
				}
			} else {
				spine::SkeletonJson json(*loader);
				json.setScale(scale);
				skeletonData = json.readSkeletonDataFile(spine::String{dataFile});
				if (!json.getError().isEmpty()) {
					AXLOGE("#parse spine .json data file failed, error:{}", json.getError().buffer());
					break;
				}
			}

			if (!skeletonData)
				break;

			return new SkeletonAsset(dataFile, atlasFile, scale, atlas, loader, skeletonData);
		} while (false);

		delete skeletonData;
		delete loader;
		delete atlas;
		return nullptr;
	}

	SkeletonAsset::SkeletonAsset(std::string_view dataFile,
								 std::string_view atlasFile,
								 float scale,
								 spine::Atlas *atlas,
								 spine::AttachmentLoader *loader,
								 spine::SkeletonData *data)
		: _dataFile(dataFile), _atlasFile(atlasFile), _scale(scale), _atlas(atlas), _attachmentLoader(loader), _skeletonData(data) {
	}

	SkeletonAsset::~SkeletonAsset() {
		delete _skeletonData;
		delete _attachmentLoader;
		delete _atlas;
	}
}// namespace spine
