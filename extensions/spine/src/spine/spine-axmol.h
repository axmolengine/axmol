/******************************************************************************
 * Spine Runtimes License Agreement
 * Last updated July 28, 2023. Replaces all prior versions.
 *
 * Copyright (c) 2013-2023, Esoteric Software LLC
 *
 * Integration of the Spine Runtimes into software or otherwise creating
 * derivative works of the Spine Runtimes is permitted under the terms and
 * conditions of Section 2 of the Spine Editor License Agreement:
 * http://esotericsoftware.com/spine-editor-license
 *
 * Otherwise, it is permitted to integrate the Spine Runtimes into software or
 * otherwise create derivative works of the Spine Runtimes (collectively,
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
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THE
 * SPINE RUNTIMES, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *****************************************************************************/

#ifndef SPINE_AXMOL_H_
#define SPINE_AXMOL_H_

#include "axmol.h"
#include <spine/spine.h>

#include <spine/SkeletonRenderer.h>
#include <spine/SkeletonBatch.h>
#include <spine/SkeletonTwoColorBatch.h>

#include <spine/SkeletonAnimation.h>

#define AX_SPINE_VERSION 0x040100

namespace spine {
	class SP_API AxmolAtlasAttachmentLoader : public AtlasAttachmentLoader {
	public:
		AxmolAtlasAttachmentLoader(Atlas *atlas);
		virtual ~AxmolAtlasAttachmentLoader();
		virtual void configureAttachment(Attachment *attachment);
	};

	class SP_API AxmolTextureLoader : public TextureLoader {
	public:
		AxmolTextureLoader();

		virtual ~AxmolTextureLoader();

		virtual void load(AtlasPage &page, const String &path);

		virtual void unload(void *texture);
	};

	class SP_API AxmolExtension : public DefaultSpineExtension {
	public:
		AxmolExtension();

		virtual ~AxmolExtension();

	protected:
		virtual char *_readFile(const String &path, int *length);
	};
}// namespace spine

#endif /* SPINE_AXMOL_H_ */
