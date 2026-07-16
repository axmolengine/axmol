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

#include "axmol/rhi/ProgramState.h"
#include "axmol/renderer/TrianglesCommand.h"
#include "axmol/renderer/Renderer.h"
#include "axmol/renderer/Texture2D.h"

#include <spine/spine.h>

#include <vector>

namespace spine {
	struct SkeletonCommand : public ax::TrianglesCommand {
		ax::rhi::UniformLocation _locMVP;
		ax::rhi::UniformLocation _locTexture;
	};
	class SP_API SkeletonBatch {
	public:
		static SkeletonBatch *getInstance();

		static void destroyInstance();

		void update(float delta);

		ax::V3F_T2F_C4B *allocateVertices(uint32_t numVertices);
		void deallocateVertices(uint32_t numVertices);
		unsigned short *allocateIndices(uint32_t numIndices);
		void deallocateIndices(uint32_t numVertices);
		ax::TrianglesCommand *addCommand(ax::Renderer *renderer, float globalOrder, ax::Texture2D *texture, ax::rhi::ProgramState *programState, ax::BlendFunc blendType, const ax::TrianglesCommand::Triangles &triangles, const ax::Mat4 &mv, uint32_t flags);

		ax::rhi::ProgramState* updateCommandPipelinePS(SkeletonCommand* command, ax::rhi::ProgramState* programState);

	protected:
		SkeletonBatch();
		virtual ~SkeletonBatch();

		void reset();

		SkeletonCommand* nextFreeCommand ();

		SkeletonCommand* newCommand();

		ax::rhi::ProgramState*                     _programState; // The default program state

		// pool of commands
		std::vector<SkeletonCommand *> _commandsPool;
		uint32_t _nextFreeCommand;

		// pool of vertices
		std::vector<ax::V3F_T2F_C4B> _vertices;
		uint32_t _numVertices;

		// pool of indices
		::spine::Array<unsigned short> _indices;

        ax::EventListener* _afterDrawListener{nullptr};
	};

}// namespace spine
