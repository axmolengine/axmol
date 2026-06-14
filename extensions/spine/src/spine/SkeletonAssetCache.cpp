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
#include "spine/SkeletonAssetCache.h"

#include "spine/spine-axmol.h"
#include "axmol/tlx/format.hpp"
#include "yasio/tlx/string_view.hpp"

namespace spine {
	SkeletonAssetCache *SkeletonAssetCache::_instance = nullptr;

	SkeletonAssetCache *SkeletonAssetCache::getInstance() {
		if (!_instance) _instance = new SkeletonAssetCache();
		return _instance;
	}

	void SkeletonAssetCache::destroyInstance() {
		AX_SAFE_DELETE(_instance);
	}

	SkeletonAssetCache::SkeletonAssetCache() {
		_hashState = XXH64_createState();
	}

	SkeletonAssetCache::~SkeletonAssetCache() {
		XXH64_freeState(_hashState);
		unloadAllAssets();
	}

	uint64_t SkeletonAssetCache::makeKey(std::string_view dataFile, std::string_view atlasFile, float scale) {
		XXH64_reset(_hashState, 0);
		XXH64_update(_hashState, dataFile.data(), dataFile.size());
		XXH64_update(_hashState, atlasFile.data(), atlasFile.size());
		XXH64_update(_hashState, &scale, sizeof(scale));
		return XXH64_digest(_hashState);
	}

	SkeletonAsset *SkeletonAssetCache::loadAsset(std::string_view dataFile, std::string_view atlasFile, float scale) {
		const auto key = makeKey(dataFile, atlasFile, scale);
		auto target = _cacheEntries.find(key);
		if (target != _cacheEntries.end())
			return target->second;

		auto asset = SkeletonAsset::obtain(dataFile, atlasFile, scale);
		if (asset)
			_cacheEntries.emplace(key, asset);
		return asset;
	}

	void SkeletonAssetCache::unloadAsset(std::string_view dataFile, std::string_view atlasFile, float scale) {
		auto key = makeKey(dataFile, atlasFile, scale);
		auto it = _cacheEntries.find(key);
		if (it != _cacheEntries.end()) {
			it->second->release();
			_cacheEntries.erase(it);
		}
	}

	void SkeletonAssetCache::unloadAssets(std::string_view dataFile) {
		for (auto it = _cacheEntries.begin(); it != _cacheEntries.end();) {
			if (it->second->getDataFile() == dataFile) {
				it->second->release();
				it = _cacheEntries.erase(it);
				continue;
			}
			++it;
		}
	}

	void SkeletonAssetCache::unloadAllAssets() {
		for (auto &&e : _cacheEntries)
			e.second->release();
		_cacheEntries.clear();
	}

	void SkeletonAssetCache::unloadAllUnusedAssets() {
		for (auto it = _cacheEntries.begin(); it != _cacheEntries.end();) {
			if (it->second->getReferenceCount() == 1) {
				it->second->release();
				it = _cacheEntries.erase(it);
				continue;
			}
			++it;
		}
	}

}// namespace spine
