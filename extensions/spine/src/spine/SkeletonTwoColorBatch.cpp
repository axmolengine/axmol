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

#include <spine/spine-axmol.h>

#include "axmol/base/Types.h"
#include "axmol/base/Utils.h"
#include <algorithm>
#include <spine/Extension.h>
#include <stddef.h>// offsetof

#include "axmol/rhi/DriverContext.h"
#include "axmol/renderer/Shaders.h"
#include "xxhash.h"

USING_NS_AX;
#define EVENT_AFTER_DRAW_RESET_POSITION "director_after_draw"
#define INITIAL_SIZE (2000)
#define MAX_VERTICES 64000
#define MAX_INDICES 64000

namespace spine {

	TwoColorTrianglesCommand::TwoColorTrianglesCommand() : _materialID(0), _texture(nullptr), _blendType(BlendFunc::DISABLE) {
		_type = RenderCommand::Type::CUSTOM_COMMAND;
	}

	void TwoColorTrianglesCommand::init(float globalOrder,
                                            axmol::Texture2D* texture,
                                            axmol::rhi::ProgramState* programState,
                                            BlendFunc blendType,
                                            const TwoColorTriangles& triangles,
                                            const Mat4& mv,
                                            uint32_t flags)
        {

		RenderCommand::init(globalOrder, mv, flags);

		_triangles = triangles;
		if (_triangles.indexCount % 3 != 0) {
			int count = _triangles.indexCount;
			_triangles.indexCount = count / 3 * 3;
			AXLOGE("Resize indexCount from {} to {}, size must be multiple times of 3", count, _triangles.indexCount);
		}

		_mv = mv;

		if (_blendType.src != blendType.src || _blendType.dst != blendType.dst ||
                    _texture != texture->getRHITexture() || _pipelineDesc.programState != programState)
        {
			_texture = texture->getRHITexture();
			_blendType = blendType;

			auto &blendDescriptor = _pipelineDesc.blendDesc;
			blendDescriptor.blendEnabled = true;
			blendDescriptor.sourceRGBBlendFactor = blendDescriptor.sourceAlphaBlendFactor = blendType.src;
			blendDescriptor.destinationRGBBlendFactor = blendDescriptor.destinationAlphaBlendFactor = blendType.dst;

			generateMaterialID(programState->getProgram());
		}
	}

	TwoColorTrianglesCommand::~TwoColorTrianglesCommand() {
	}

	void TwoColorTrianglesCommand::generateMaterialID(void* prog)
    {
		// do not batch if using custom uniforms (since we cannot batch) it


		struct
		{
			void *texture;
			void *prog;
			rhi::BlendFactor src;
			rhi::BlendFactor dst;
		} hashMe;

		// NOTE: Initialize hashMe struct to make the value of padding bytes be filled with zero.
		// It's important since XXH32 below will also consider the padding bytes which probably
		// are set to random values by different compilers.
		memset(&hashMe, 0, sizeof(hashMe));

		hashMe.texture = _texture;
		hashMe.src = _blendType.src;
		hashMe.dst = _blendType.dst;
        hashMe.prog    = prog;
		_materialID = XXH32((const void *) &hashMe, sizeof(hashMe), 0);
	}


	void TwoColorTrianglesCommand::draw(Renderer *r) {
		SkeletonTwoColorBatch::getInstance()->batch(r, this);
	}

	void TwoColorTrianglesCommand::updateVertexAndIndexBuffer(Renderer *r, V3F_C4B_C4B_T2F *vertices, int verticesSize, uint16_t *indices, int indicesSize) {
		if (verticesSize != _vertexCapacity)
			createVertexBuffer(sizeof(V3F_C4B_C4B_T2F), verticesSize, CustomCommand::BufferUsage::DYNAMIC);
		if (indicesSize != _indexCapacity)
			createIndexBuffer(CustomCommand::IndexFormat::U_SHORT, indicesSize, CustomCommand::BufferUsage::DYNAMIC);

		updateVertexBuffer(vertices, sizeof(V3F_C4B_C4B_T2F) * verticesSize);
		updateIndexBuffer(indices, sizeof(uint16_t) * indicesSize);
	}


	static SkeletonTwoColorBatch *s_TwoColorInstance = nullptr;

	SkeletonTwoColorBatch *SkeletonTwoColorBatch::getInstance() {
        if (!s_TwoColorInstance)
        {
            s_TwoColorInstance = new SkeletonTwoColorBatch();

            auto eventDispatcher = Director::getInstance()->getEventDispatcher();

            // callback after drawing is finished so we can clear out the batch state
            // for the next frame
            eventDispatcher->addCustomEventListener(EVENT_AFTER_DRAW_RESET_POSITION,
                                                    [](EventCustom*) { s_TwoColorInstance->update(0); });

            eventDispatcher->addCustomEventListener(Director::EVENT_DESTROY,
                                                    [](EventCustom*) { SkeletonTwoColorBatch::destroyInstance(); });
        }
        return s_TwoColorInstance;
	}

	void SkeletonTwoColorBatch::destroyInstance() {
        if (s_TwoColorInstance)
        {
            Director::getInstance()->getEventDispatcher()->removeCustomEventListeners(
                EVENT_AFTER_DRAW_RESET_POSITION);
            delete s_TwoColorInstance;
            s_TwoColorInstance = nullptr;
		}
	}

	SkeletonTwoColorBatch::SkeletonTwoColorBatch() : _vertexBuffer(0), _indexBuffer(0) {
		_commandsPool.reserve(INITIAL_SIZE);
		for (unsigned int i = 0; i < INITIAL_SIZE; i++) {
			_commandsPool.push_back(new TwoColorTrianglesCommand());
		}

		reset();

        auto program          = ProgramManager::getInstance()->loadProgram("custom/spineTwoColorTint_vs",
                                                                                   "custom/spineTwoColorTint_fs");
        _twoColorProgramState = new rhi::ProgramState(program);

        _locPMatrix = _twoColorProgramState->getUniformLocation("u_PMatrix");
        _locTexture = _twoColorProgramState->getUniformLocation("u_tex0");

        auto locPosition = _twoColorProgramState->getVertexInputDesc("a_position");
        auto locTexcoord = _twoColorProgramState->getVertexInputDesc("a_texCoord");
        auto locColor    = _twoColorProgramState->getVertexInputDesc("a_color");
        auto locColor2   = _twoColorProgramState->getVertexInputDesc("a_color2");

        auto layoutDesc = axvlm->allocateVertexLayoutDesc();
        layoutDesc.startLayout(4);
        layoutDesc.addAttrib("a_position", locPosition, rhi::VertexFormat::FLOAT3,
                                offsetof(spine::V3F_C4B_C4B_T2F, position), false);
        layoutDesc.addAttrib("a_color", locColor, rhi::VertexFormat::UBYTE4,
                                offsetof(spine::V3F_C4B_C4B_T2F, color), true);
        layoutDesc.addAttrib("a_color2", locColor2, rhi::VertexFormat::UBYTE4,
                                offsetof(spine::V3F_C4B_C4B_T2F, color2), true);
        layoutDesc.addAttrib("a_texCoord", locTexcoord, rhi::VertexFormat::FLOAT2,
                                offsetof(spine::V3F_C4B_C4B_T2F, texCoord), false);
        layoutDesc.endLayout();

        Object::assign(_twoColorVertexLayout, axvlm->getVertexLayout(std::move(layoutDesc)));
	}

	SkeletonTwoColorBatch::~SkeletonTwoColorBatch() {
        for (auto& command : _commandsPool)
        {
            if (command)
            {
                auto pipelinePS = command->unsafePS();
                assert(!pipelinePS || pipelinePS->getReferenceCount() == 1);
                delete command;
            }
        }
        _commandsPool.clear();

        AX_SAFE_RELEASE_NULL(_twoColorProgramState);
        AX_SAFE_RELEASE_NULL(_twoColorVertexLayout);

		delete[] _vertexBuffer;
		delete[] _indexBuffer;
	}

	void SkeletonTwoColorBatch::update(float delta) {
		reset();
	}

	V3F_C4B_C4B_T2F *SkeletonTwoColorBatch::allocateVertices(uint32_t numVertices) {
		if (_vertices.size() - _numVertices < numVertices) {
			V3F_C4B_C4B_T2F *oldData = _vertices.data();
			_vertices.resize((_vertices.size() + numVertices) * 2 + 1);
			V3F_C4B_C4B_T2F *newData = _vertices.data();
			for (uint32_t i = 0; i < this->_nextFreeCommand; i++) {
				TwoColorTrianglesCommand *command = _commandsPool[i];
				TwoColorTriangles &triangles = (TwoColorTriangles &) command->getTriangles();
				triangles.verts = newData + (triangles.verts - oldData);
			}
		}

		V3F_C4B_C4B_T2F *vertices = _vertices.data() + _numVertices;
		_numVertices += numVertices;
		return vertices;
	}


	void SkeletonTwoColorBatch::deallocateVertices(uint32_t numVertices) {
		_numVertices -= numVertices;
	}


	unsigned short *SkeletonTwoColorBatch::allocateIndices(uint32_t numIndices) {
		if (_indices.getCapacity() - _indices.size() < numIndices) {
			unsigned short *oldData = _indices.buffer();
			int oldSize = (int)_indices.size();
			_indices.ensureCapacity(_indices.size() + numIndices);
			unsigned short *newData = _indices.buffer();
			for (uint32_t i = 0; i < this->_nextFreeCommand; i++) {
				TwoColorTrianglesCommand *command = _commandsPool[i];
				TwoColorTriangles &triangles = (TwoColorTriangles &) command->getTriangles();
				if (triangles.indices >= oldData && triangles.indices < oldData + oldSize) {
					triangles.indices = newData + (triangles.indices - oldData);
				}
			}
		}

		unsigned short *indices = _indices.buffer() + _indices.size();
		_indices.setSize(_indices.size() + numIndices, 0);
		return indices;
	}

	void SkeletonTwoColorBatch::deallocateIndices(uint32_t numIndices) {
		_indices.setSize(_indices.size() - numIndices, 0);
	}

	TwoColorTrianglesCommand *SkeletonTwoColorBatch::addCommand(axmol::Renderer *renderer, float globalOrder, axmol::Texture2D *texture, rhi::ProgramState *programState, axmol::BlendFunc blendType, const TwoColorTriangles &triangles, const axmol::Mat4 &mv, uint32_t flags) {
        TwoColorTrianglesCommand *command = nextFreeCommand();

        auto pipelinePS = command->unsafePS();
        if (programState != nullptr)
        {
            if (pipelinePS != programState)
            {
                Object::assign(pipelinePS, programState); // Node owned the programState, don't need clone
            }
        }
        else
        {
            pipelinePS = _twoColorProgramState->clone(); // Unlike other clone methods, this function does not use autorelease
        }

        AXASSERT(pipelinePS, "programState should not be null");

        const axmol::Mat4& projectionMat =
        Director::getInstance()->getMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION);

        auto finalMatrix = projectionMat * mv;

        pipelinePS->setUniform(_locPMatrix, finalMatrix.m, sizeof(finalMatrix.m));
        pipelinePS->setTexture(_locTexture, 0, texture->getRHITexture());

		command->init(globalOrder, texture, pipelinePS, blendType, triangles, mv, flags);

        command->setOwnPSVL(pipelinePS, _twoColorVertexLayout, RenderCommand::ADOPT_FLAG_PS);

		command->updateVertexAndIndexBuffer(renderer, triangles.verts, triangles.vertCount, triangles.indices, triangles.indexCount);
		renderer->addCommand(command);
		return command;
	}

	void SkeletonTwoColorBatch::batch(axmol::Renderer *renderer, TwoColorTrianglesCommand *command) {
		if (_numVerticesBuffer + command->getTriangles().vertCount >= MAX_VERTICES || _numIndicesBuffer + command->getTriangles().indexCount >= MAX_INDICES) {
			flush(renderer, _lastCommand);
		}

		uint32_t materialID = command->getMaterialID();
		if (_lastCommand && _lastCommand->getMaterialID() != materialID) {
			flush(renderer, _lastCommand);
		}

		memcpy(_vertexBuffer + _numVerticesBuffer, command->getTriangles().verts, sizeof(V3F_C4B_C4B_T2F) * command->getTriangles().vertCount);
		const Mat4 &modelView = command->getModelView();
		for (int i = _numVerticesBuffer; i < _numVerticesBuffer + command->getTriangles().vertCount; i++) {
			modelView.transformPoint(&_vertexBuffer[i].position);
		}

		unsigned short vertexOffset = (unsigned short) _numVerticesBuffer;
		unsigned short *indices = command->getTriangles().indices;
		for (int i = 0, j = _numIndicesBuffer; i < command->getTriangles().indexCount; i++, j++) {
			_indexBuffer[j] = indices[i] + vertexOffset;
		}

		_numVerticesBuffer += command->getTriangles().vertCount;
		_numIndicesBuffer += command->getTriangles().indexCount;

		if (command->isForceFlush()) {
			flush(renderer, command);
		}
		_lastCommand = command;
	}

	void SkeletonTwoColorBatch::flush(axmol::Renderer *renderer, TwoColorTrianglesCommand *materialCommand) {
		if (!materialCommand)
			return;

		materialCommand->updateVertexAndIndexBuffer(renderer, _vertexBuffer, _numVerticesBuffer, _indexBuffer, _numIndicesBuffer);

		renderer->addCommand(materialCommand);

		_numVerticesBuffer = 0;
		_numIndicesBuffer = 0;
		_numBatches++;
	}

	void SkeletonTwoColorBatch::reset() {
		_nextFreeCommand = 0;
		_numVertices = 0;
		_indices.setSize(0, 0);
		_numVerticesBuffer = 0;
		_numIndicesBuffer = 0;
		_lastCommand = nullptr;
		_numBatches = 0;
	}

	TwoColorTrianglesCommand *SkeletonTwoColorBatch::nextFreeCommand() {
		if (_commandsPool.size() <= _nextFreeCommand) {
			unsigned int newSize = (int)_commandsPool.size() * 2 + 1;
			for (int i = (int)_commandsPool.size(); i < newSize; i++) {
				_commandsPool.push_back(new TwoColorTrianglesCommand());
			}
		}
		TwoColorTrianglesCommand *command = _commandsPool[_nextFreeCommand++];
		command->setForceFlush(false);
		return command;
	}
}// namespace spine
