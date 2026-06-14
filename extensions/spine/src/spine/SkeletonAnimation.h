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

#ifndef AX_EXT_SPINE_ANIMATION_H_
#define AX_EXT_SPINE_ANIMATION_H_

#include "axmol/scene/Node.h"
#include "axmol/math/Rect.h"
#include "axmol/math/Vec2.h"
#include "axmol/renderer/CustomCommand.h"
#include "spine/SkeletonAsset.h"
#include <spine/spine.h>
#include <span>
#include <string_view>

namespace spine {
	class AttachmentVertices;
	class SkeletonAnimation;
	struct _TrackEntryListeners;

	using StartListener = std::function<void(spine::TrackEntry *entry)>;
	using InterruptListener = std::function<void(spine::TrackEntry *entry)>;
	using EndListener = std::function<void(spine::TrackEntry *entry)>;
	using DisposeListener = std::function<void(spine::TrackEntry *entry)>;
	using CompleteListener = std::function<void(spine::TrackEntry *entry)>;
	using EventListener = std::function<void(spine::TrackEntry *entry, spine::Event *event)>;
	using UpdateWorldTransformsListener = std::function<void(SkeletonAnimation *node)>;

	class SP_API SkeletonAnimation : public ax::Node, public ax::BlendProtocol {
	public:
		CREATE_FUNC(SkeletonAnimation);

		/**
        * @brief Create a SkeletonAnimation from a skeleton data file and atlas file.
        *
        * Internally determines whether the skeleton file is JSON or binary (.skel).
        * Uses SkeletonAssetCache to avoid duplicate parsing.
        *
        * @param skeletonJsonFile Path to the skeleton data file (.json or .skel).
        * @param atlasFile Path to the atlas file.
        * @param scale Scale factor applied to skeleton data.
        * @return A new SkeletonAnimation instance, or nullptr if loading fails.
        */
		static SkeletonAnimation *create(std::string_view skeletonJsonFile,
										 std::string_view atlasFile,
										 float scale = 1);

		/**
        * @brief Create a SkeletonAnimation from an existing SkeletonAsset.
        *
        * The SkeletonAnimation retains the asset and creates its own Skeleton instance.
        * @param asset Pointer to a SkeletonAsset object.
        * @return A new SkeletonAnimation instance.
        */
		static SkeletonAnimation *create(spine::SkeletonAsset *asset);

		/**
        * @brief Clone the current SkeletonAnimation with an optional slot range.
        *
        * @param startSlotIndex Start slot index (inclusive), or -1 for default.
        * @param endSlotIndex End slot index (inclusive), or -1 for default.
        * @return A new SkeletonAnimation instance cloned from this one.
        */
		SkeletonAnimation *clone(int startSlotIndex = -1, int endSlotIndex = -1);

		ax::Rect getBoundingBox() const override;

		void setMix(std::string_view fromAnimation, std::string_view toAnimation, float duration);

		spine::TrackEntry *setAnimation(int trackIndex, std::string_view name, bool loop);
		spine::TrackEntry *addAnimation(int trackIndex, std::string_view name, bool loop, float delay = 0);
		spine::TrackEntry *setEmptyAnimation(int trackIndex, float mixDuration);
		void setEmptyAnimations(float mixDuration);
		spine::TrackEntry *addEmptyAnimation(int trackIndex, float mixDuration, float delay = 0);
		void clearTracks();
		void clearTrack(int trackIndex = 0);
		bool hasAnimation(std::string_view name) const;
		std::string_view getAnimationName(int trackIndex = 0) const;
		bool isAnimationComplete(int trackIndex = 0) const;
		bool isAnimationLooping(int trackIndex = 0) const;
		void setAnimationLoop(int trackIndex, bool loop);
		void setAnimationTimeScale(int trackIndex, float scale);
		void setAnimationNormalizedTime(int trackIndex, float progress);

		void setStartListener(const spine::StartListener &listener);
		void setInterruptListener(const spine::InterruptListener &listener);
		void setEndListener(const spine::EndListener &listener);
		void setDisposeListener(const spine::DisposeListener &listener);
		void setCompleteListener(const spine::CompleteListener &listener);
		void setEventListener(const spine::EventListener &listener);
		void setPreUpdateWorldTransformsListener(const spine::UpdateWorldTransformsListener &listener);
		void setPostUpdateWorldTransformsListener(const spine::UpdateWorldTransformsListener &listener);

		void setTrackStartListener(spine::TrackEntry *entry, const spine::StartListener &listener);
		void setTrackInterruptListener(spine::TrackEntry *entry, const spine::InterruptListener &listener);
		void setTrackEndListener(spine::TrackEntry *entry, const spine::EndListener &listener);
		void setTrackDisposeListener(spine::TrackEntry *entry, const spine::DisposeListener &listener);
		void setTrackCompleteListener(spine::TrackEntry *entry, const spine::CompleteListener &listener);
		void setTrackEventListener(spine::TrackEntry *entry, const spine::EventListener &listener);

		void setUpdateOnlyIfVisible(bool status);
		void setAsset(spine::SkeletonAsset *asset);

		void updateWorldTransform(spine::Physics physics);

		int findBoneIndex(std::string_view boneName) const;
		int findSlotIndex(std::string_view slotName) const;
		bool setBonePosition(std::string_view boneName, const ax::Vec2 &position);
		bool setBoneWorldPosition(std::string_view boneName, const ax::Vec2 &position);

		void setSkin(std::string_view skinName);
		bool setCombinedSkin(std::string_view skinName, std::span<const std::string_view> skinNames);
		void setAttachment(std::string_view slotName, std::string_view attachmentName);

		void setTwoColorTint(bool enabled);
		bool isTwoColorTint() const;
		void setSlotsRange(int startSlotIndex, int endSlotIndex);

		void setTimeScale(float scale);
		float getTimeScale() const;

		void setDebugSlotsEnabled(bool enabled);
		bool getDebugSlotsEnabled() const;
		void setDebugBonesEnabled(bool enabled);
		bool getDebugBonesEnabled() const;
		void setDebugMeshesEnabled(bool enabled);
		bool getDebugMeshesEnabled() const;
		void setDebugBoundingRectEnabled(bool enabled);
		bool getDebugBoundingRectEnabled() const;

		void setBlendFunc(const ax::BlendFunc &blendFunc) override;
		const ax::BlendFunc &getBlendFunc() const override;
		void setOpacityModifyRGB(bool value) override;
		bool isOpacityModifyRGB() const override;

		SkeletonAnimation();
		SkeletonAnimation(spine::Skeleton *skeleton,
						  bool ownsSkeleton = false);
		SkeletonAnimation(spine::SkeletonAsset *asset);
		SkeletonAnimation(std::string_view skeletonDataFile, std::string_view atlasFile, float scale = 1);
		~SkeletonAnimation() override;

		void initWithAsset(spine::SkeletonAsset *asset);
		void initWithJsonFile(std::string_view skeletonDataFile, std::string_view atlasFile, float scale = 1);
		void initWithBinaryFile(std::string_view skeletonDataFile, std::string_view atlasFile, float scale = 1);
		void initWithSkeleton(spine::Skeleton *skeleton,
							  bool ownsSkeleton = false);

		[[internal]] spine::Animation *findAnimation(std::string_view name) const;
		[[internal]] spine::TrackEntry *getTrack(int trackIndex = 0);
		[[internal]] spine::Bone *findBone(std::string_view boneName) const;
		[[internal]] spine::Slot *findSlot(std::string_view slotName) const;

		[[internal]] spine::Attachment *getAttachment(std::string_view slotName,
													  std::string_view attachmentName) const;
		[[internal]] spine::Skeleton *getSkeleton() const { return _skeleton; }
		[[internal]] spine::AnimationState *getAnimationState() const { return _state; }
		[[internal]] spine::SkeletonAsset *getAsset() const { return _asset; }

	protected:
		static void animationCallback(AnimationState *state, EventType type, TrackEntry *entry, spine::Event *event, void *userData);
		static void trackEntryCallback(AnimationState *state, EventType type, TrackEntry *entry, spine::Event *event, void *userData);
		static _TrackEntryListeners *acquireTrackListeners(TrackEntry *entry);

		virtual void initialize();
		virtual void onAnimationStateEvent(spine::TrackEntry *entry, spine::EventType type, spine::Event *event);
		virtual void onTrackEntryEvent(spine::TrackEntry *entry, spine::EventType type, spine::Event *event);

		void update(float deltaTime) override;
		void draw(ax::Renderer *renderer, const ax::Mat4 &transform, uint32_t transformFlags) override;
		void onEnter() override;
		void onExit() override;

		void setAnimationStateEnabled(bool enabled);
		virtual void drawDebug(ax::Renderer *renderer, const ax::Mat4 &transform, uint32_t transformFlags);

		spine::StartListener _startListener;
		spine::InterruptListener _interruptListener;
		spine::EndListener _endListener;
		spine::DisposeListener _disposeListener;
		spine::CompleteListener _completeListener;
		spine::EventListener _eventListener;
		spine::UpdateWorldTransformsListener _preUpdateListener;
		spine::UpdateWorldTransformsListener _postUpdateListener;

		spine::SkeletonAsset *_asset = nullptr;

		spine::Skeleton *_skeleton = nullptr;

		spine::Pool<spine::AttachmentVertices *> _verticesPool;

		spine::AnimationState *_state = nullptr;
		spine::AnimationStateData *_stateData = nullptr;
		spine::SkeletonClipping *_clipper = nullptr;
		spine::Skin *_combinedSkin = nullptr;

		ax::CustomCommand _debugCommand;
		ax::BlendFunc _blendFunc;

		ax::Rect _boundingRect;

		float _timeScale = 1.0F;

		int _startSlotIndex = 0;
		int _endSlotIndex = std::numeric_limits<int>::max();

		bool _ownsSkeleton = false;
		bool _opacityModifyRGB = false;
		bool _debugSlots = false;
		bool _debugBones = false;
		bool _debugMeshes = false;
		bool _debugBoundingRect = false;
		bool _twoColorTint = false;
		bool _updateOnlyIfVisible = false;
		bool _firstDraw = true;
		bool _animationStateEnabled = true;
	};
}// namespace spine

#endif// AX_EXT_SPINE_ANIMATION_H_
