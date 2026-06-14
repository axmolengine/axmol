/******************************************************************************
 * Spine Runtimes License Agreement
 * Last updated April 5, 2025. Replaces all prior versions.
 *
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

#include <spine/SkeletonAnimation.h>

#include <spine/SkeletonBatch.h>
#include <spine/SkeletonTwoColorBatch.h>
#include <spine/spine-axmol.h>

#include "axmol/scene/Camera.h"
#include "axmol/2d/DrawNode.h"
#include "axmol/math/MathUtil.h"

#include <algorithm>

// C Variable length array
#ifdef _MSC_VER
// VLA not supported, use _malloca
#define VLA(type, arr, count) \
	type *arr = static_cast<type *>(_malloca(sizeof(type) * count))
#define VLA_FREE(arr) \
	do { _freea(arr); } while (false)
#else
#define VLA(type, arr, count) \
	type arr[count]
#define VLA_FREE(arr)
#endif


namespace spine {
	using namespace ax;
	namespace {
		static axmol::Rect computeBoundingRect(const float *coords, int vertexCount) {
			assert(coords);
			assert(vertexCount > 0);

			const float *v = coords;
			float minX = v[0];
			float minY = v[1];
			float maxX = minX;
			float maxY = minY;
			for (int i = 1; i < vertexCount; ++i) {
				v += 2;
				float x = v[0];
				float y = v[1];
				minX = std::min(minX, x);
				minY = std::min(minY, y);
				maxX = std::max(maxX, x);
				maxY = std::max(maxY, y);
			}
			return {minX, minY, maxX - minX, maxY - minY};
		}

		static bool slotIsOutRange(Slot &slot, int startSlotIndex, int endSlotIndex) {
			const int index = slot.getData().getIndex();
			return startSlotIndex > index || endSlotIndex < index;
		}

		static bool nothingToDraw(Slot &slot, int startSlotIndex, int endSlotIndex) {
			auto &slotPose = slot.getAppliedPose();
			Attachment *attachment = slotPose.getAttachment();
			if (!attachment ||
				slotIsOutRange(slot, startSlotIndex, endSlotIndex) ||
				!slot.getBone().isActive())
				return true;
			const auto &attachmentRTTI = attachment->getRTTI();
			if (attachmentRTTI.isExactly(ClippingAttachment::rtti))
				return false;
			if (slotPose.getColor().a == 0)
				return true;
			if (attachmentRTTI.isExactly(RegionAttachment::rtti)) {
				if (static_cast<RegionAttachment *>(attachment)->getColor().a == 0)
					return true;
			} else if (attachmentRTTI.isExactly(MeshAttachment::rtti)) {
				if (static_cast<MeshAttachment *>(attachment)->getColor().a == 0)
					return true;
			}
			return false;
		}

		static int computeTotalCoordCount(Skeleton &skeleton, int startSlotIndex, int endSlotIndex) {
			int coordCount = 0;
			for (size_t i = 0; i < skeleton.getSlots().size(); ++i) {
				Slot &slot = *skeleton.getSlots()[i];
				if (nothingToDraw(slot, startSlotIndex, endSlotIndex)) {
					continue;
				}
				auto &slotPose = slot.getAppliedPose();
				Attachment *const attachment = slotPose.getAttachment();
				if (attachment->getRTTI().isExactly(RegionAttachment::rtti)) {
					coordCount += 8;
				} else if (attachment->getRTTI().isExactly(MeshAttachment::rtti)) {
					MeshAttachment *const mesh = static_cast<MeshAttachment *>(attachment);
					coordCount += mesh->getWorldVerticesLength();
				}
			}
			return coordCount;
		}


		static void transformWorldVertices(float *dstCoord, int coordCount, Skeleton &skeleton, int startSlotIndex, int endSlotIndex) {
			float *dstPtr = dstCoord;
#ifndef NDEBUG
			float *const dstEnd = dstCoord + coordCount;
#endif
			for (size_t i = 0; i < skeleton.getSlots().size(); ++i) {
				auto slot = *skeleton.getDrawOrder().getAppliedPose()[i];// match the draw order of SkeletonAnimation::Draw
				if (nothingToDraw(slot, startSlotIndex, endSlotIndex)) {
					continue;
				}

				auto &slotPose = slot.getAppliedPose();
				Attachment *const attachment = slotPose.getAttachment();
				if (attachment->getRTTI().isExactly(RegionAttachment::rtti)) {
					RegionAttachment *const regionAttachment = static_cast<RegionAttachment *>(attachment);
					assert(dstPtr + 8 <= dstEnd);
					regionAttachment->computeWorldVertices(slot, regionAttachment->getOffsets(slotPose).buffer(), dstPtr, 0, 2);
					dstPtr += 8;
				} else if (attachment->getRTTI().isExactly(MeshAttachment::rtti)) {
					MeshAttachment *const mesh = static_cast<MeshAttachment *>(attachment);
					assert(dstPtr + mesh->getWorldVerticesLength() <= dstEnd);
					mesh->computeWorldVertices(skeleton, slot, 0, mesh->getWorldVerticesLength(), dstPtr, 0, 2);
					dstPtr += mesh->getWorldVerticesLength();
				}
			}
			assert(dstPtr == dstEnd);
		}

		static void interleaveCoordinates(float *dst, const float *src, int count, int dstStride) {
			if (dstStride == 2) {
				memcpy(dst, src, sizeof(float) * count * 2);
			} else {
				for (int i = 0; i < count; ++i) {
					dst[0] = src[0];
					dst[1] = src[1];
					dst += dstStride;
					src += 2;
				}
			}
		}

		static BlendFunc makeBlendFunc(BlendMode blendMode, bool premultipliedAlpha) {
			BlendFunc blendFunc;
			switch (blendMode) {
				case BlendMode_Additive:
					blendFunc.src = premultipliedAlpha ? rhi::BlendFactor::ONE : rhi::BlendFactor::SRC_ALPHA;
					blendFunc.dst = rhi::BlendFactor::ONE;
					break;
				case BlendMode_Multiply:
					blendFunc.src = rhi::BlendFactor::DST_COLOR;
					blendFunc.dst = rhi::BlendFactor::ONE_MINUS_SRC_ALPHA;
					break;
				case BlendMode_Screen:
					blendFunc.src = rhi::BlendFactor::ONE;
					blendFunc.dst = rhi::BlendFactor::ONE_MINUS_SRC_COLOR;
					break;
				default:
					blendFunc.src = premultipliedAlpha ? rhi::BlendFactor::ONE : rhi::BlendFactor::SRC_ALPHA;
					blendFunc.dst = rhi::BlendFactor::ONE_MINUS_SRC_ALPHA;
			}
			return blendFunc;
		}


		static bool cullRectangle(Renderer *renderer, const Mat4 &transform, const axmol::Rect &rect) {
			if (Camera::getVisitingCamera() == nullptr)
				return false;

			auto director = Director::getInstance();
			auto scene = director->getRunningScene();

			if (!scene || (scene && Camera::getDefaultCamera() != Camera::getVisitingCamera()))
				return false;

			Rect visibleRect(director->getVisibleOrigin(), director->getVisibleSize());

			// transform center point to screen space
			float hSizeX = rect.size.width / 2;
			float hSizeY = rect.size.height / 2;
			Vec3 v3p(rect.origin.x + hSizeX, rect.origin.y + hSizeY, 0);
			transform.transformPoint(&v3p);
			Vec2 v2p = Camera::getVisitingCamera()->projectWorldToCanvas(v3p);

			// convert content size to world coordinates
			float wshw = std::max(fabsf(hSizeX * transform.m[0] + hSizeY * transform.m[4]), fabsf(hSizeX * transform.m[0] - hSizeY * transform.m[4]));
			float wshh = std::max(fabsf(hSizeX * transform.m[1] + hSizeY * transform.m[5]), fabsf(hSizeX * transform.m[1] - hSizeY * transform.m[5]));

			// enlarge visible rect half size in screen coord
			visibleRect.origin.x -= wshw;
			visibleRect.origin.y -= wshh;
			visibleRect.size.width += wshw * 2;
			visibleRect.size.height += wshh * 2;
			return !visibleRect.containsPoint(v2p);
		}

	}// namespace

	struct _TrackEntryListeners {
		StartListener startListener;
		InterruptListener interruptListener;
		EndListener endListener;
		DisposeListener disposeListener;
		CompleteListener completeListener;
		EventListener eventListener;
	};


	SkeletonAnimation *SkeletonAnimation::create(std::string_view skeletonFile,
												 std::string_view atlasFile,
												 float scale) {
		auto asset = SkeletonAssetCache::getInstance()->loadAsset(
				skeletonFile,
				atlasFile,
				scale);
		if (!asset) return nullptr;

		return SkeletonAnimation::create(asset);
	}

	SkeletonAnimation *SkeletonAnimation::create(spine::SkeletonAsset *asset) {
		if (!asset) return nullptr;
		auto obj = new SkeletonAnimation();
		obj->initWithAsset(asset);
		obj->autorelease();
		return obj;
	}

	void SkeletonAnimation::animationCallback(AnimationState *state, EventType type, TrackEntry *entry, spine::Event *event, void *userData) {
		((SkeletonAnimation *) state->getRendererObject())->onAnimationStateEvent(entry, type, event);
	}

	void SkeletonAnimation::trackEntryCallback(AnimationState *state, EventType type, TrackEntry *entry, spine::Event *event, void *userData) {
		((SkeletonAnimation *) state->getRendererObject())->onTrackEntryEvent(entry, type, event);
		if (type == EventType_Dispose) {
			if (entry->getRendererObject()) {
				delete (_TrackEntryListeners *) entry->getRendererObject();
				entry->setRendererObject(NULL);
			}
		}
	}

	_TrackEntryListeners *SkeletonAnimation::acquireTrackListeners(TrackEntry *entry) {
		if (!entry->getRendererObject()) {
			entry->setRendererObject(new _TrackEntryListeners());
			entry->setListener(SkeletonAnimation::trackEntryCallback);
		}
		return (_TrackEntryListeners *) entry->getRendererObject();
	}

	void SkeletonAnimation::initialize() {
		AX_SAFE_DELETE(_combinedSkin);
		AX_SAFE_DELETE(_state);
		AX_SAFE_DELETE(_stateData);
		AX_SAFE_DELETE(_clipper);

		_clipper = new SkeletonClipping();

		_blendFunc = BlendFunc::ALPHA_PREMULTIPLIED;
		setOpacityModifyRGB(true);

		setTwoColorTint(false);

		_skeleton->setupPose();
		_skeleton->updateWorldTransform(Physics_Update);

		if (_animationStateEnabled) {
			_stateData = new AnimationStateData(_skeleton->getData());
			_state = new AnimationState(*_stateData);
			_state->setRendererObject(this);
			_state->setListener(animationCallback);
		}
		_firstDraw = true;
	}

	SkeletonAnimation::SkeletonAnimation() {
	}

	SkeletonAnimation::SkeletonAnimation(Skeleton *skeleton, bool ownsSkeleton) {
		initWithSkeleton(skeleton, ownsSkeleton);
	}

	SkeletonAnimation::SkeletonAnimation(SkeletonAsset *asset) {
		initWithAsset(asset);
	}

	SkeletonAnimation::SkeletonAnimation(std::string_view skeletonDataFile, std::string_view atlasFile, float scale) {
		initWithJsonFile(skeletonDataFile, atlasFile, scale);
	}

	SkeletonAnimation::~SkeletonAnimation() {
		AX_SAFE_DELETE(_combinedSkin);
		AX_SAFE_DELETE(_state);
		AX_SAFE_DELETE(_stateData);
		if (_ownsSkeleton) delete _skeleton;
		delete _clipper;
		AX_SAFE_RELEASE(_asset);
	}

	void SkeletonAnimation::initWithSkeleton(Skeleton *skeleton, bool ownsSkeleton) {
		_animationStateEnabled = false;
		AX_SAFE_RELEASE_NULL(_asset);
		if (_ownsSkeleton) delete _skeleton;
		_skeleton = skeleton;
		_ownsSkeleton = ownsSkeleton;
		initialize();
	}

	void SkeletonAnimation::initWithAsset(SkeletonAsset *asset) {
		setAsset(asset);
	}

	void SkeletonAnimation::setAsset(SkeletonAsset *asset) {
		if (!asset || _asset == asset) return;

		auto oldAsset = _asset;
		asset->retain();
		_asset = asset;

		if (_ownsSkeleton) delete _skeleton;
		_ownsSkeleton = true;
		_skeleton = new Skeleton(*_asset->getSkeletonData());

		AX_SAFE_RELEASE(oldAsset);
		initialize();
	}

	void SkeletonAnimation::initWithJsonFile(std::string_view skeletonDataFile, std::string_view atlasFile, float scale) {
		auto asset = SkeletonAssetCache::getInstance()->loadAsset(skeletonDataFile, atlasFile, scale);
		AXASSERT(asset, "Error reading skeleton asset file.");
		setAsset(asset);
	}

	void SkeletonAnimation::initWithBinaryFile(std::string_view skeletonDataFile, std::string_view atlasFile, float scale) {
		initWithJsonFile(skeletonDataFile, atlasFile, scale);
	}

	void SkeletonAnimation::update(float deltaTime) {
		if (_updateOnlyIfVisible && !isVisible()) return;

		Node::update(deltaTime);
		if (!_state) return;

		deltaTime *= _timeScale;
		if (_preUpdateListener) _preUpdateListener(this);
		_state->update(deltaTime);
		_state->apply(*_skeleton);
		_skeleton->update(deltaTime);
		_skeleton->updateWorldTransform(Physics_Update);
		if (_postUpdateListener) _postUpdateListener(this);
	}

	void SkeletonAnimation::draw(Renderer *renderer, const Mat4 &transform, uint32_t transformFlags) {
		if (_firstDraw && _state) {
			_firstDraw = false;
			update(0);
		}

		// Early exit if the skeleton is invisible.
		if (getDisplayedOpacity() == 0 || _skeleton->getColor().a == 0) {
			return;
		}

		const int coordCount = computeTotalCoordCount(*_skeleton, _startSlotIndex, _endSlotIndex);
		if (coordCount == 0) {
			return;
		}
		assert(coordCount % 2 == 0);

		VLA(float, worldCoords, coordCount);
		transformWorldVertices(worldCoords, coordCount, *_skeleton, _startSlotIndex, _endSlotIndex);

#if AX_USE_CULLING
		const axmol::Rect bb = computeBoundingRect(worldCoords, coordCount / 2);

		if (cullRectangle(renderer, transform, bb)) {
			VLA_FREE(worldCoords);
			return;
		}
#endif

		const float *worldCoordPtr = worldCoords;
		SkeletonBatch *batch = SkeletonBatch::getInstance();
		SkeletonTwoColorBatch *twoColorBatch = SkeletonTwoColorBatch::getInstance();
		const bool hasSingleTint = (isTwoColorTint() == false);

		auto &&displayedColor = getDisplayedColor();
		spine::Color nodeColor;
		nodeColor.r = displayedColor.r / 255.f;
		nodeColor.g = displayedColor.g / 255.f;
		nodeColor.b = displayedColor.b / 255.f;
		nodeColor.a = displayedColor.a / 255.f;

		spine::Color color;
		spine::Color darkColor;
		const float darkPremultipliedAlpha = _opacityModifyRGB ? 1.f : 0;
		TwoColorTrianglesCommand *lastTwoColorTrianglesCommand = nullptr;
		for (int i = 0, n = (int) _skeleton->getSlots().size(); i < n; ++i) {
			Slot &slot = *_skeleton->getDrawOrder().getAppliedPose()[i];
			auto &slotPose = slot.getAppliedPose();
			if (nothingToDraw(slot, _startSlotIndex, _endSlotIndex)) {
				_clipper->clipEnd(slot);
				continue;
			}

			axmol::TrianglesCommand::Triangles triangles;
			TwoColorTriangles trianglesTwoColor;
			static unsigned short quadIndices[6] = {0, 1, 2, 2, 3, 0};
			Texture2D *texture = nullptr;
			float *attachmentUVs = nullptr;
			if (slotPose.getAttachment()->getRTTI().isExactly(RegionAttachment::rtti)) {
				RegionAttachment *attachment = static_cast<RegionAttachment *>(slotPose.getAttachment());
				auto &sequence = attachment->getSequence();
				const int sequenceIndex = sequence.resolveIndex(slotPose);
				auto region = sequence.getRegion(sequenceIndex);
				texture = static_cast<ax::Texture2D *>(region->getRendererObject());
				auto &uvs = sequence.getUVs(sequenceIndex);
				attachmentUVs = uvs.buffer();

				float *dstTriangleVertices = nullptr;
				int dstStride = 0;// in floats
				if (hasSingleTint) {
					triangles.indices = quadIndices;
					triangles.indexCount = 6;
					triangles.verts = batch->allocateVertices(4);
					triangles.vertCount = 4;
					assert(triangles.vertCount == 4);
					for (int v = 0, i = 0; v < triangles.vertCount; v++, i += 2) {
						auto &texCoords = triangles.verts[v].texCoord;
						texCoords.u = uvs[i];
						texCoords.v = uvs[i + 1];
					}
					dstStride = sizeof(V3F_T2F_C4B) / sizeof(float);
					dstTriangleVertices = reinterpret_cast<float *>(triangles.verts);
				} else {
					trianglesTwoColor.indices = quadIndices;
					trianglesTwoColor.indexCount = 6;
					trianglesTwoColor.verts = twoColorBatch->allocateVertices(4);
					trianglesTwoColor.vertCount = 4;
					assert(trianglesTwoColor.vertCount == 4);
					for (int v = 0, i = 0; v < trianglesTwoColor.vertCount; v++, i += 2) {
						auto &texCoords = trianglesTwoColor.verts[v].texCoord;
						texCoords.u = uvs[i];
						texCoords.v = uvs[i + 1];
					}
					dstTriangleVertices = reinterpret_cast<float *>(trianglesTwoColor.verts);
					dstStride = sizeof(V3F_C4B_C4B_T2F) / sizeof(float);
				}
				// Copy world vertices to triangle vertices.
				interleaveCoordinates(dstTriangleVertices, worldCoordPtr, 4, dstStride);
				worldCoordPtr += 8;

				color = attachment->getColor();
			} else if (slotPose.getAttachment()->getRTTI().isExactly(MeshAttachment::rtti)) {
				MeshAttachment *attachment = (MeshAttachment *) slotPose.getAttachment();
				auto &sequence = attachment->getSequence();
				const int sequenceIndex = sequence.resolveIndex(slotPose);
				auto region = sequence.getRegion(sequenceIndex);
				texture = static_cast<ax::Texture2D *>(region->getRendererObject());
				auto &uvs = sequence.getUVs(sequenceIndex);
				attachmentUVs = uvs.buffer();

				float *dstTriangleVertices = nullptr;
				int dstStride = 0;// in floats
				int dstVertexCount = 0;
				if (hasSingleTint) {
					triangles.indices = attachment->getTriangles().buffer();
					triangles.indexCount = (unsigned int) attachment->getTriangles().size();
					triangles.verts = batch->allocateVertices((int) attachment->getWorldVerticesLength() / 2);
					triangles.vertCount = (int) attachment->getWorldVerticesLength() / 2;
					for (int v = 0, i = 0; v < triangles.vertCount; v++, i += 2) {
						auto &texCoords = triangles.verts[v].texCoord;
						texCoords.u = uvs[i];
						texCoords.v = uvs[i + 1];
					}
					dstTriangleVertices = (float *) triangles.verts;
					dstStride = sizeof(V3F_T2F_C4B) / sizeof(float);
					dstVertexCount = triangles.vertCount;
				} else {
					trianglesTwoColor.indices = attachment->getTriangles().buffer();
					trianglesTwoColor.indexCount = (int) attachment->getTriangles().size();
					trianglesTwoColor.verts = twoColorBatch->allocateVertices((int) attachment->getWorldVerticesLength() / 2);
					trianglesTwoColor.vertCount = (int) attachment->getWorldVerticesLength() / 2;
					for (int v = 0, i = 0; v < trianglesTwoColor.vertCount; v++, i += 2) {
						auto &texCoords = trianglesTwoColor.verts[v].texCoord;
						texCoords.u = uvs[i];
						texCoords.v = uvs[i + 1];
					}
					dstTriangleVertices = (float *) trianglesTwoColor.verts;
					dstStride = sizeof(V3F_C4B_C4B_T2F) / sizeof(float);
					dstVertexCount = trianglesTwoColor.vertCount;
				}

				// Copy world vertices to triangle vertices.
				//assert(dstVertexCount * 2 == attachment->super.worldVerticesLength);
				interleaveCoordinates(dstTriangleVertices, worldCoordPtr, dstVertexCount, dstStride);
				worldCoordPtr += dstVertexCount * 2;

				color = attachment->getColor();
			} else if (slotPose.getAttachment()->getRTTI().isExactly(ClippingAttachment::rtti)) {
				ClippingAttachment *clip = (ClippingAttachment *) slotPose.getAttachment();
				_clipper->clipStart(*_skeleton, slot, clip);
				continue;
			} else {
				_clipper->clipEnd(slot);
				continue;
			}

			if (slotPose.hasDarkColor()) {
				darkColor = slotPose.getDarkColor();
			} else {
				darkColor.r = 0;
				darkColor.g = 0;
				darkColor.b = 0;
			}
			darkColor.a = darkPremultipliedAlpha;

			color.a *= nodeColor.a * _skeleton->getColor().a * slotPose.getColor().a;
			if (color.a == 0) {
				_clipper->clipEnd(slot);
				continue;
			}
			color.r *= nodeColor.r * _skeleton->getColor().r * slotPose.getColor().r;
			color.g *= nodeColor.g * _skeleton->getColor().g * slotPose.getColor().g;
			color.b *= nodeColor.b * _skeleton->getColor().b * slotPose.getColor().b;
			if (_opacityModifyRGB) {
				color.r *= color.a;
				color.g *= color.a;
				color.b *= color.a;
			}

			const ax::Color32 color_r{color};
			const ax::Color32 darkColor_r{darkColor};
			const BlendFunc blendFunc = makeBlendFunc(slot.getData().getBlendMode(), texture->hasPremultipliedAlpha());
			_blendFunc = blendFunc;

			if (hasSingleTint) {
				if (_clipper->isClipping()) {
					_clipper->clipTriangles((float *) &triangles.verts[0].position, triangles.indices, triangles.indexCount, attachmentUVs, sizeof(axmol::V3F_T2F_C4B) / 4);
					batch->deallocateVertices(triangles.vertCount);

					if (_clipper->getClippedTriangles().size() == 0) {
						_clipper->clipEnd(slot);
						continue;
					}

					triangles.vertCount = (int) _clipper->getClippedVertices().size() / 2;
					triangles.verts = batch->allocateVertices(triangles.vertCount);
					triangles.indexCount = (int) _clipper->getClippedTriangles().size();
					triangles.indices = batch->allocateIndices(triangles.indexCount);
					memcpy(triangles.indices, _clipper->getClippedTriangles().buffer(), sizeof(unsigned short) * _clipper->getClippedTriangles().size());

					const float *verts = _clipper->getClippedVertices().buffer();
					const float *uvs = _clipper->getClippedUVs().buffer();
					auto vertex = triangles.verts;
					for (int v = 0, vn = triangles.vertCount, vv = 0; v < vn;
						 ++v, vv += 2, ++vertex) {
						vertex->position.x = verts[vv];
						vertex->position.y = verts[vv + 1];
						vertex->texCoord.u = uvs[vv];
						vertex->texCoord.v = uvs[vv + 1];
						vertex->color = color_r;
					}
					batch->addCommand(renderer, _globalZOrder, texture, _programState, blendFunc, triangles, transform, transformFlags);
				} else {
					// Not clipping.
					auto vertex = triangles.verts;
					for (int v = 0, vn = triangles.vertCount; v < vn;
						 ++v, ++vertex) {
						vertex->color = color_r;
					}
					batch->addCommand(renderer, _globalZOrder, texture, _programState, blendFunc, triangles, transform, transformFlags);
				}
			} else {
				// Two color tinting.

				if (_clipper->isClipping()) {
					_clipper->clipTriangles((float *) &trianglesTwoColor.verts[0].position, trianglesTwoColor.indices, trianglesTwoColor.indexCount, attachmentUVs, sizeof(V3F_C4B_C4B_T2F) / 4);
					twoColorBatch->deallocateVertices(trianglesTwoColor.vertCount);

					if (_clipper->getClippedTriangles().size() == 0) {
						_clipper->clipEnd(slot);
						continue;
					}

					trianglesTwoColor.vertCount = (int) _clipper->getClippedVertices().size() / 2;
					trianglesTwoColor.verts = twoColorBatch->allocateVertices(trianglesTwoColor.vertCount);
					trianglesTwoColor.indexCount = (int) _clipper->getClippedTriangles().size();
					trianglesTwoColor.indices = twoColorBatch->allocateIndices(trianglesTwoColor.indexCount);
					memcpy(trianglesTwoColor.indices, _clipper->getClippedTriangles().buffer(), sizeof(unsigned short) * _clipper->getClippedTriangles().size());

					const float *verts = _clipper->getClippedVertices().buffer();
					const float *uvs = _clipper->getClippedUVs().buffer();

					V3F_C4B_C4B_T2F *vertex = trianglesTwoColor.verts;
					for (int v = 0, vn = trianglesTwoColor.vertCount, vv = 0; v < vn;
						 ++v, vv += 2, ++vertex) {
						vertex->position.x = verts[vv];
						vertex->position.y = verts[vv + 1];
						vertex->texCoord.u = uvs[vv];
						vertex->texCoord.v = uvs[vv + 1];
						vertex->color = color_r;
						vertex->color2 = darkColor_r;
					}
					lastTwoColorTrianglesCommand = twoColorBatch->addCommand(renderer, _globalZOrder, texture, _programState, blendFunc, trianglesTwoColor, transform, transformFlags);
				} else {
					V3F_C4B_C4B_T2F *vertex = trianglesTwoColor.verts;
					for (int v = 0, vn = trianglesTwoColor.vertCount; v < vn; ++v, ++vertex) {
						vertex->color = color_r;
						vertex->color2 = darkColor_r;
					}
					lastTwoColorTrianglesCommand = twoColorBatch->addCommand(renderer, _globalZOrder, texture, _programState, blendFunc, trianglesTwoColor, transform, transformFlags);
				}
			}
			_clipper->clipEnd(slot);
		}
		_clipper->clipEnd();

		if (lastTwoColorTrianglesCommand) {
			Node *parent = this->getParent();

			// We need to decide if we can postpone flushing the current batch. We can postpone if the next sibling node is a two color
			// tinted skeleton with the same global-z.
			// The parent->getChildrenCount() > 100 check is a hack as checking for a sibling is an O(n) operation, and if all children
			// of this nodes parent are skeletons, we are in O(n2) territory.
			if (!parent || parent->getChildrenCount() > 100 || getChildrenCount() != 0) {
				lastTwoColorTrianglesCommand->setForceFlush(true);
			} else {
				const axmol::Vector<Node *> &children = parent->getChildren();
				Node *sibling = nullptr;
				for (ssize_t i = 0; i < children.size(); i++) {
					if (children.at(i) == this) {
						if (i < children.size() - 1) {
							sibling = children.at(i + 1);
							break;
						}
					}
				}
				if (!sibling) {
					lastTwoColorTrianglesCommand->setForceFlush(true);
				} else {
					SkeletonAnimation *siblingSkeleton = dynamic_cast<SkeletonAnimation *>(sibling);
					if (!siblingSkeleton ||                                               // flush is next sibling isn't a SkeletonAnimation
						!siblingSkeleton->isTwoColorTint() ||                             // flush if next sibling isn't two color tinted
						!siblingSkeleton->isVisible() ||                                  // flush if next sibling is two color tinted but not visible
						(siblingSkeleton->getGlobalZOrder() != this->getGlobalZOrder())) {// flush if next sibling is two color tinted but z-order differs
						lastTwoColorTrianglesCommand->setForceFlush(true);
					}
				}
			}
		}

		if (_debugBoundingRect || _debugSlots || _debugBones || _debugMeshes) {
			drawDebug(renderer, transform, transformFlags);
		}

		VLA_FREE(worldCoords);
	}


	void SkeletonAnimation::drawDebug(Renderer *renderer, const Mat4 &transform, uint32_t transformFlags) {

#if !defined(USE_MATRIX_STACK_PROJECTION_ONLY)
		Director *director = Director::getInstance();
		director->pushMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
		director->loadMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW, transform);
#endif

		DrawNode *drawNode = DrawNode::create();
		drawNode->setGlobalZOrder(getGlobalZOrder());

		// Draw bounding rectangle
		if (_debugBoundingRect) {
			const axmol::Rect brect = getBoundingBox();
			const Vec2 points[4] =
					{
							brect.origin,
							{brect.origin.x + brect.size.width, brect.origin.y},
							{brect.origin.x + brect.size.width, brect.origin.y + brect.size.height},
							{brect.origin.x, brect.origin.y + brect.size.height}};
			drawNode->drawPoly(points, 4, true, ax::Color::GREEN, 2.0f);
		}

		if (_debugSlots) {
			// Slots.
			for (int i = 0, n = (int) _skeleton->getSlots().size(); i < n; i++) {
				Slot *slot = _skeleton->getDrawOrder().getAppliedPose()[i];
				auto &slotPose = slot->getAppliedPose();
				if (!slot->getBone().isActive()) continue;
				if (!slotPose.getAttachment() || !slotPose.getAttachment()->getRTTI().isExactly(RegionAttachment::rtti)) continue;

				if (slotIsOutRange(*slot, _startSlotIndex, _endSlotIndex)) {
					continue;
				}

				RegionAttachment *attachment = (RegionAttachment *) slotPose.getAttachment();
				float worldVertices[8];
				attachment->computeWorldVertices(*slot, attachment->getOffsets(slotPose).buffer(), worldVertices, 0, 2);
				const Vec2 points[4] =
						{
								{worldVertices[0], worldVertices[1]},
								{worldVertices[2], worldVertices[3]},
								{worldVertices[4], worldVertices[5]},
								{worldVertices[6], worldVertices[7]}};
				drawNode->drawPoly(points, 4, true, ax::Color::BLUE, 2.0f);
			}
		}

		if (_debugBones) {
			// Bone lengths.
			for (int i = 0, n = (int) _skeleton->getBones().size(); i < n; i++) {
				Bone *bone = _skeleton->getBones()[i];
				if (!bone->isActive()) continue;
				auto &bonePose = bone->getAppliedPose();
				float x = bone->getData().getLength() * bonePose.getA() + bonePose.getWorldX();
				float y = bone->getData().getLength() * bonePose.getC() + bonePose.getWorldY();
				drawNode->drawLine(Vec2(bonePose.getWorldX(), bonePose.getWorldY()), Vec2(x, y), ax::Color::RED, 2.0f);
			}
			// Bone origins.
			auto color = ax::Color::BLUE;// Root bone is blue.
			for (int i = 0, n = (int) _skeleton->getBones().size(); i < n; i++) {
				Bone *bone = _skeleton->getBones()[i];
				if (!bone->isActive()) continue;
				auto &bonePose = bone->getAppliedPose();
				drawNode->drawPoint(Vec2(bonePose.getWorldX(), bonePose.getWorldY()), 4, color);
				if (i == 0) color = ax::Color::GREEN;
			}
		}

		if (_debugMeshes) {
			// Meshes.
			for (int i = 0, n = (int) _skeleton->getSlots().size(); i < n; ++i) {
				Slot *slot = _skeleton->getDrawOrder().getAppliedPose()[i];
				auto &slotPose = slot->getAppliedPose();
				if (!slot->getBone().isActive()) continue;
				if (!slotPose.getAttachment() || !slotPose.getAttachment()->getRTTI().isExactly(MeshAttachment::rtti)) continue;
				MeshAttachment *const mesh = static_cast<MeshAttachment *>(slotPose.getAttachment());
				VLA(float, worldCoord, mesh->getWorldVerticesLength());
				mesh->computeWorldVertices(*_skeleton, *slot, 0, mesh->getWorldVerticesLength(), worldCoord, 0, 2);
				for (size_t t = 0; t < mesh->getTriangles().size(); t += 3) {
					// Fetch triangle indices
					const int idx0 = mesh->getTriangles()[t + 0];
					const int idx1 = mesh->getTriangles()[t + 1];
					const int idx2 = mesh->getTriangles()[t + 2];
					const Vec2 v[3] =
							{
									worldCoord + (idx0 * 2),
									worldCoord + (idx1 * 2),
									worldCoord + (idx2 * 2)};
					drawNode->drawPoly(v, 3, true, ax::Color::YELLOW, 2.0f);
				}
				VLA_FREE(worldCoord);
			}
		}

		drawNode->draw(renderer, transform, transformFlags);
#if !defined(USE_MATRIX_STACK_PROJECTION_ONLY)
		director->popMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
#endif
	}

	axmol::Rect SkeletonAnimation::getBoundingBox() const {
		const int coordCount = computeTotalCoordCount(*_skeleton, _startSlotIndex, _endSlotIndex);
		if (coordCount == 0) return {0, 0, 0, 0};
		VLA(float, worldCoords, coordCount);
		transformWorldVertices(worldCoords, coordCount, *_skeleton, _startSlotIndex, _endSlotIndex);
		const axmol::Rect bb = computeBoundingRect(worldCoords, coordCount / 2);
		VLA_FREE(worldCoords);
		return bb;
	}

	// --- Convenience methods for Skeleton_* functions.

	void SkeletonAnimation::updateWorldTransform(Physics physics) {
		_skeleton->updateWorldTransform(physics);
	}

	Bone *SkeletonAnimation::findBone(std::string_view boneName) const {
		return _skeleton->findBone(spine::String{boneName});
	}

	Slot *SkeletonAnimation::findSlot(std::string_view slotName) const {
		return _skeleton->findSlot(spine::String{slotName});
	}

	void SkeletonAnimation::setSkin(std::string_view skinName) {
		_skeleton->setSkin(spine::String{skinName});
	}

	Attachment *SkeletonAnimation::getAttachment(std::string_view slotName, std::string_view attachmentName) const {
		return _skeleton->getAttachment(spine::String{slotName}, spine::String{attachmentName});
	}

	void SkeletonAnimation::setAttachment(std::string_view slotName, std::string_view attachmentName) {
		_skeleton->setAttachment(spine::String{slotName}, spine::String{attachmentName});
	}

	void SkeletonAnimation::setTwoColorTint(bool enabled) {
		_twoColorTint = enabled;
	}

	bool SkeletonAnimation::isTwoColorTint() const {
		return _twoColorTint;
	}

	void SkeletonAnimation::setSlotsRange(int startSlotIndex, int endSlotIndex) {
		_startSlotIndex = startSlotIndex == -1 ? 0 : startSlotIndex;
		_endSlotIndex = endSlotIndex == -1 ? std::numeric_limits<int>::max() : endSlotIndex;
	}

	void SkeletonAnimation::setTimeScale(float scale) {
		_timeScale = scale;
	}

	float SkeletonAnimation::getTimeScale() const {
		return _timeScale;
	}

	void SkeletonAnimation::setDebugSlotsEnabled(bool enabled) {
		_debugSlots = enabled;
	}
	bool SkeletonAnimation::getDebugSlotsEnabled() const {
		return _debugSlots;
	}

	void SkeletonAnimation::setDebugBonesEnabled(bool enabled) {
		_debugBones = enabled;
	}
	bool SkeletonAnimation::getDebugBonesEnabled() const {
		return _debugBones;
	}

	void SkeletonAnimation::setDebugMeshesEnabled(bool enabled) {
		_debugMeshes = enabled;
	}
	bool SkeletonAnimation::getDebugMeshesEnabled() const {
		return _debugMeshes;
	}

	void SkeletonAnimation::setDebugBoundingRectEnabled(bool enabled) {
		_debugBoundingRect = enabled;
	}

	bool SkeletonAnimation::getDebugBoundingRectEnabled() const {
		return _debugBoundingRect;
	}

	void SkeletonAnimation::onEnter() {
		Node::onEnter();
		scheduleUpdate();
	}

	void SkeletonAnimation::onExit() {
		Node::onExit();
		unscheduleUpdate();
	}

	// --- CCBlendProtocol

	const BlendFunc &SkeletonAnimation::getBlendFunc() const {
		return _blendFunc;
	}

	void SkeletonAnimation::setBlendFunc(const BlendFunc &blendFunc) {
		_blendFunc = blendFunc;
	}

	void SkeletonAnimation::setOpacityModifyRGB(bool value) {
		_opacityModifyRGB = value;
	}

	bool SkeletonAnimation::isOpacityModifyRGB() const {
		return _opacityModifyRGB;
	}

	SkeletonAnimation *SkeletonAnimation::clone(int startSlotIndex, int endSlotIndex) {
		auto obj = new SkeletonAnimation(_skeleton, false);
		obj->setSlotsRange(startSlotIndex, endSlotIndex);
		obj->autorelease();
		return obj;
	}

	void SkeletonAnimation::setAnimationStateEnabled(bool enabled) {
		_animationStateEnabled = enabled;
		if (!enabled) {
			AX_SAFE_DELETE(_state);
			AX_SAFE_DELETE(_stateData);
			return;
		}

		if (_state || !_skeleton) return;
		_stateData = new AnimationStateData(_skeleton->getData());
		_state = new AnimationState(*_stateData);
		_state->setRendererObject(this);
		_state->setListener(animationCallback);
		_firstDraw = true;
	}

	void SkeletonAnimation::setMix(std::string_view fromAnimation, std::string_view toAnimation, float duration) {
		setAnimationStateEnabled(true);
		_state->getData().setMix(spine::String{fromAnimation}, spine::String{toAnimation}, duration);
	}

	TrackEntry *SkeletonAnimation::setAnimation(int trackIndex, std::string_view name, bool loop) {
		setAnimationStateEnabled(true);
		spine::Animation *animation = _skeleton->getData().findAnimation(spine::String{name});
		if (!animation) {
			AXLOGW("Spine: Animation not found: {}", name);
			return nullptr;
		}
		return &_state->setAnimation(trackIndex, *animation, loop);
	}

	TrackEntry *SkeletonAnimation::addAnimation(int trackIndex, std::string_view name, bool loop, float delay) {
		setAnimationStateEnabled(true);
		spine::Animation *animation = _skeleton->getData().findAnimation(spine::String{name});
		if (!animation) {
			AXLOGW("Spine: Animation not found: {}", name);
			return nullptr;
		}
		return &_state->addAnimation(trackIndex, *animation, loop, delay);
	}

	TrackEntry *SkeletonAnimation::setEmptyAnimation(int trackIndex, float mixDuration) {
		setAnimationStateEnabled(true);
		return &_state->setEmptyAnimation(trackIndex, mixDuration);
	}

	void SkeletonAnimation::setEmptyAnimations(float mixDuration) {
		setAnimationStateEnabled(true);
		_state->setEmptyAnimations(mixDuration);
	}

	TrackEntry *SkeletonAnimation::addEmptyAnimation(int trackIndex, float mixDuration, float delay) {
		setAnimationStateEnabled(true);
		return &_state->addEmptyAnimation(trackIndex, mixDuration, delay);
	}

	spine::Animation *SkeletonAnimation::findAnimation(std::string_view name) const {
		return _skeleton->getData().findAnimation(spine::String{name});
	}

	TrackEntry *SkeletonAnimation::getTrack(int trackIndex) {
		return _state ? _state->getTrack(trackIndex) : nullptr;
	}

	bool SkeletonAnimation::hasAnimation(std::string_view name) const {
		return _skeleton && _skeleton->getData().findAnimation(spine::String{name});
	}

	std::string_view SkeletonAnimation::getAnimationName(int trackIndex) const {
		if (!_state) return ""sv;
		auto entry = _state->getTrack(trackIndex);
		return entry ? to_string_view(entry->getAnimation().getName().buffer()) : ""sv;
	}

	bool SkeletonAnimation::isAnimationComplete(int trackIndex) const {
		if (!_state) return false;
		auto entry = _state->getTrack(trackIndex);
		return entry && entry->isComplete();
	}

	bool SkeletonAnimation::isAnimationLooping(int trackIndex) const {
		if (!_state) return false;
		auto entry = _state->getTrack(trackIndex);
		return entry && entry->getLoop();
	}

	void SkeletonAnimation::setAnimationLoop(int trackIndex, bool loop) {
		if (!_state) return;
		auto entry = _state->getTrack(trackIndex);
		if (entry) entry->setLoop(loop);
	}

	void SkeletonAnimation::setAnimationTimeScale(int trackIndex, float scale) {
		if (!_state) return;
		auto entry = _state->getTrack(trackIndex);
		if (entry) entry->setTimeScale(scale);
	}

	void SkeletonAnimation::setAnimationNormalizedTime(int trackIndex, float progress) {
		if (!_state) return;
		auto entry = _state->getTrack(trackIndex);
		if (!entry) return;

		progress = std::clamp(progress, 0.0f, 1.0f);
		entry->setTrackTime(ax::MathUtil::lerp(0.0f, entry->getAnimationEnd() - entry->getAnimationStart(), progress));
	}

	void SkeletonAnimation::clearTracks() {
		if (_state) _state->clearTracks();
	}

	void SkeletonAnimation::clearTrack(int trackIndex) {
		if (_state) _state->clearTrack(trackIndex);
	}

	void SkeletonAnimation::onAnimationStateEvent(TrackEntry *entry, EventType type, spine::Event *event) {
		switch (type) {
			case EventType_Start:
				if (_startListener) _startListener(entry);
				break;
			case EventType_Interrupt:
				if (_interruptListener) _interruptListener(entry);
				break;
			case EventType_End:
				if (_endListener) _endListener(entry);
				break;
			case EventType_Dispose:
				if (_disposeListener) _disposeListener(entry);
				break;
			case EventType_Complete:
				if (_completeListener) _completeListener(entry);
				break;
			case EventType_Event:
				if (_eventListener) _eventListener(entry, event);
				break;
		}
	}

	void SkeletonAnimation::onTrackEntryEvent(TrackEntry *entry, EventType type, spine::Event *event) {
		if (!entry->getRendererObject()) return;
		_TrackEntryListeners *listeners = (_TrackEntryListeners *) entry->getRendererObject();
		switch (type) {
			case EventType_Start:
				if (listeners->startListener) listeners->startListener(entry);
				break;
			case EventType_Interrupt:
				if (listeners->interruptListener) listeners->interruptListener(entry);
				break;
			case EventType_End:
				if (listeners->endListener) listeners->endListener(entry);
				break;
			case EventType_Dispose:
				if (listeners->disposeListener) listeners->disposeListener(entry);
				break;
			case EventType_Complete:
				if (listeners->completeListener) listeners->completeListener(entry);
				break;
			case EventType_Event:
				if (listeners->eventListener) listeners->eventListener(entry, event);
				break;
		}
	}

	void SkeletonAnimation::setStartListener(const spine::StartListener &listener) {
		_startListener = listener;
	}

	void SkeletonAnimation::setInterruptListener(const spine::InterruptListener &listener) {
		_interruptListener = listener;
	}

	void SkeletonAnimation::setEndListener(const spine::EndListener &listener) {
		_endListener = listener;
	}

	void SkeletonAnimation::setDisposeListener(const spine::DisposeListener &listener) {
		_disposeListener = listener;
	}

	void SkeletonAnimation::setCompleteListener(const spine::CompleteListener &listener) {
		_completeListener = listener;
	}

	void SkeletonAnimation::setEventListener(const spine::EventListener &listener) {
		_eventListener = listener;
	}

	void SkeletonAnimation::setPreUpdateWorldTransformsListener(const spine::UpdateWorldTransformsListener &listener) {
		_preUpdateListener = listener;
	}

	void SkeletonAnimation::setPostUpdateWorldTransformsListener(const spine::UpdateWorldTransformsListener &listener) {
		_postUpdateListener = listener;
	}

	void SkeletonAnimation::setTrackStartListener(TrackEntry *entry, const spine::StartListener &listener) {
		acquireTrackListeners(entry)->startListener = listener;
	}

	void SkeletonAnimation::setTrackInterruptListener(TrackEntry *entry, const spine::InterruptListener &listener) {
		acquireTrackListeners(entry)->interruptListener = listener;
	}

	void SkeletonAnimation::setTrackEndListener(TrackEntry *entry, const spine::EndListener &listener) {
		acquireTrackListeners(entry)->endListener = listener;
	}

	void SkeletonAnimation::setTrackDisposeListener(TrackEntry *entry, const spine::DisposeListener &listener) {
		acquireTrackListeners(entry)->disposeListener = listener;
	}

	void SkeletonAnimation::setTrackCompleteListener(TrackEntry *entry, const spine::CompleteListener &listener) {
		acquireTrackListeners(entry)->completeListener = listener;
	}

	void SkeletonAnimation::setTrackEventListener(TrackEntry *entry, const spine::EventListener &listener) {
		acquireTrackListeners(entry)->eventListener = listener;
	}

	void SkeletonAnimation::setUpdateOnlyIfVisible(bool status) {
		_updateOnlyIfVisible = status;
	}

	int SkeletonAnimation::findBoneIndex(std::string_view boneName) const {
		Bone *bone = findBone(boneName);
		return bone ? bone->getData().getIndex() : -1;
	}

	int SkeletonAnimation::findSlotIndex(std::string_view slotName) const {
		Slot *slot = findSlot(slotName);
		return slot ? slot->getData().getIndex() : -1;
	}

	bool SkeletonAnimation::setBonePosition(std::string_view boneName, const Vec2 &position) {
		Bone *bone = findBone(boneName);
		if (!bone) return false;
		auto &bonePose = bone->getAppliedPose();
		bonePose.setX(position.x);
		bonePose.setY(position.y);
		return true;
	}

	bool SkeletonAnimation::setBoneWorldPosition(std::string_view boneName, const Vec2 &position) {
		Bone *bone = findBone(boneName);
		if (!bone) return false;
		Bone *parent = bone->getParent();
		float localX = position.x;
		float localY = position.y;
		if (parent) parent->getAppliedPose().worldToLocal(position.x, position.y, localX, localY);
		auto &bonePose = bone->getAppliedPose();
		bonePose.setX(localX);
		bonePose.setY(localY);
		return true;
	}

	bool SkeletonAnimation::setCombinedSkin(std::string_view skinName, std::span<const std::string_view> skinNames) {
		auto skin = new Skin(spine::String{skinName});
		for (auto &name : skinNames) {
			Skin *source = _skeleton->getData().findSkin(spine::String{name});
			if (!source) {
				delete skin;
				AXLOGW("Spine: Skin not found: {}", name);
				return false;
			}
			skin->addSkin(*source);
		}

		delete _combinedSkin;
		_combinedSkin = skin;
		_skeleton->setSkin(_combinedSkin);
		return true;
	}
}// namespace spine
