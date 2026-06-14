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

#include "axmol/axmol.h"
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

	class SP_API SkeletonAnimation : public axmol::Node, public axmol::BlendProtocol {
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
        * @brief Create a SkeletonAnimation from an existing SkeletonData.
        *
        * @note The lifetime of the SkeletonData object is managed externally.
        *       Caller must ensure SkeletonData remains valid for the duration
        *       of the SkeletonAnimation instance.
        *
        * @param skeletonData Pointer to a SkeletonData object.
        * @return A new SkeletonAnimation instance.
        */
		static SkeletonAnimation *create(spine::SkeletonData *skeletonData);

		/**
        * @brief Create a SkeletonAnimation from an existing Skeleton.
        *
        * @note The lifetime of the Skeleton object is managed externally.
        *       Caller must ensure Skeleton remains valid for the duration
        *       of the SkeletonAnimation instance.
        *
        * @param skeleton Pointer to a Skeleton object.
        * @return A new SkeletonAnimation instance.
        */
		static SkeletonAnimation *create(spine::Skeleton *skeleton);

		/**
        * @brief Clone the current SkeletonAnimation with an optional slot range.
        *
        * @param startSlotIndex Start slot index (inclusive), or -1 for default.
        * @param endSlotIndex End slot index (inclusive), or -1 for default.
        * @return A new SkeletonAnimation instance cloned from this one.
        */
		SkeletonAnimation *clone(int startSlotIndex = -1, int endSlotIndex = -1);


		axmol::Rect getBoundingBox() const override;

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

		void setBlendFunc(const axmol::BlendFunc &blendFunc) override;
		const axmol::BlendFunc &getBlendFunc() const override;
		void setOpacityModifyRGB(bool value) override;
		bool isOpacityModifyRGB() const override;

		SkeletonAnimation();
		SkeletonAnimation(spine::Skeleton *skeleton,
						  bool ownsSkeleton = false,
						  bool ownsSkeletonData = false,
						  bool ownsAtlas = false);
		SkeletonAnimation(spine::SkeletonData *skeletonData, bool ownsSkeletonData = false);
		SkeletonAnimation(std::string_view skeletonDataFile, spine::Atlas *atlas, float scale = 1);
		SkeletonAnimation(std::string_view skeletonDataFile, std::string_view atlasFile, float scale = 1);
		~SkeletonAnimation() override;

		void initWithSkeleton(spine::Skeleton *skeleton,
							  bool ownsSkeleton = false,
							  bool ownsSkeletonData = false,
							  bool ownsAtlas = false);
		void initWithData(spine::SkeletonData *skeletonData, bool ownsSkeletonData = false);
		void initWithJsonFile(std::string_view skeletonDataFile, spine::Atlas *atlas, float scale = 1);
		void initWithJsonFile(std::string_view skeletonDataFile, std::string_view atlasFile, float scale = 1);
		void initWithBinaryFile(std::string_view skeletonDataFile, spine::Atlas *atlas, float scale = 1);
		void initWithBinaryFile(std::string_view skeletonDataFile, std::string_view atlasFile, float scale = 1);


		[[internal]] spine::Animation *findAnimation(std::string_view name) const;
		[[internal]] spine::TrackEntry *getTrack(int trackIndex = 0);
		[[internal]] spine::Bone *findBone(std::string_view boneName) const;
		[[internal]] spine::Slot *findSlot(std::string_view slotName) const;

		[[internal]] spine::Attachment *getAttachment(std::string_view slotName,
													  std::string_view attachmentName) const;
		[[internal]] spine::Skeleton *getSkeleton() const { return _skeleton; }
		[[internal]] spine::AnimationState *getAnimationState() const { return _state; }
		[[internal]] spine::SkeletonData *getSkeletonData() const { return _skeletonData; }

	protected:
		static void animationCallback(AnimationState *state, EventType type, TrackEntry *entry, spine::Event *event, void *userData);
		static void trackEntryCallback(AnimationState *state, EventType type, TrackEntry *entry, spine::Event *event, void *userData);
		static _TrackEntryListeners *acquireTrackListeners(TrackEntry *entry);

		virtual void initialize();
		virtual void onAnimationStateEvent(spine::TrackEntry *entry, spine::EventType type, spine::Event *event);
		virtual void onTrackEntryEvent(spine::TrackEntry *entry, spine::EventType type, spine::Event *event);

		void update(float deltaTime) override;
		void draw(axmol::Renderer *renderer, const axmol::Mat4 &transform, uint32_t transformFlags) override;
		void onEnter() override;
		void onExit() override;

		void setSkeletonData(spine::SkeletonData *skeletonData, bool ownsSkeletonData);
		void setAnimationStateEnabled(bool enabled);
		virtual void drawDebug(axmol::Renderer *renderer, const axmol::Mat4 &transform, uint32_t transformFlags);

		spine::StartListener _startListener;
		spine::InterruptListener _interruptListener;
		spine::EndListener _endListener;
		spine::DisposeListener _disposeListener;
		spine::CompleteListener _completeListener;
		spine::EventListener _eventListener;
		spine::UpdateWorldTransformsListener _preUpdateListener;
		spine::UpdateWorldTransformsListener _postUpdateListener;

		spine::Atlas *_atlas = nullptr;
		spine::AttachmentLoader *_attachmentLoader = nullptr;

		spine::Skeleton *_skeleton = nullptr;
		spine::SkeletonData *_skeletonData = nullptr;

		spine::Pool<spine::AttachmentVertices *> _verticesPool;

		spine::AnimationState *_state = nullptr;
		spine::AnimationStateData *_stateData = nullptr;
		spine::SkeletonClipping *_clipper = nullptr;
		spine::Skin *_combinedSkin = nullptr;

		axmol::CustomCommand _debugCommand;
		axmol::BlendFunc _blendFunc;

		axmol::Rect _boundingRect;

		float _timeScale = 1.0F;

		int _startSlotIndex = 0;
		int _endSlotIndex = std::numeric_limits<int>::max();

		bool _ownsSkeletonData = false;
		bool _ownsSkeleton = false;
		bool _ownsAtlas = false;
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
