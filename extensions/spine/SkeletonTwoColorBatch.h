/******************************************************************************
 * Spine Runtimes License Agreement
 * Last updated January 1, 2020. Replaces all prior versions.
 *
 * Copyright (c) 2013-2020, Esoteric Software LLC
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

#ifndef SPINE_SKELETONTWOCOLORBATCH_H_
#define SPINE_SKELETONTWOCOLORBATCH_H_

#include "cocos2d.h"
#if COCOS2D_VERSION >= 0x00040000

#include <spine/spine.h>
#include <vector>
#include "renderer/backend/ProgramState.h"

namespace spine {
	struct V3F_C4B_C4B_T2F {
		ax::Vec3 position;
		ax::Color4B color;
		ax::Color4B color2;
		ax::Tex2F texCoords;
	};
	
	struct TwoColorTriangles {
		V3F_C4B_C4B_T2F* verts;
		unsigned short* indices;
		int vertCount;
		int indexCount;
	};
	
	class SP_API TwoColorTrianglesCommand : public ax::CustomCommand {
	public:
		TwoColorTrianglesCommand();
		
		~TwoColorTrianglesCommand();

        void init(float globalOrder, ax::Texture2D* texture, ax::backend::ProgramState* programState, ax::BlendFunc blendType, const TwoColorTriangles& triangles, const ax::Mat4& mv, uint32_t flags);

        void updateCommandPipelineDescriptor(ax::backend::ProgramState* programState);

        inline ax::backend::TextureBackend* getTexture() const { return _texture; }

        void draw(ax::Renderer *renderer);

        void updateVertexAndIndexBuffer(ax::Renderer *renderer, V3F_C4B_C4B_T2F *vertices, int verticesSize, uint16_t *indices, int indicesSize);
		
		inline uint32_t getMaterialID() const { return _materialID; }
		
		inline const TwoColorTriangles& getTriangles() const { return _triangles; }
		
		inline ssize_t getVertexCount() const { return _triangles.vertCount; }
		
		inline ssize_t getIndexCount() const { return _triangles.indexCount; }
		
		inline const V3F_C4B_C4B_T2F* getVertices() const { return _triangles.verts; }
		
		inline const unsigned short* getIndices() const { return _triangles.indices; }
		
		inline ax::BlendFunc getBlendType() const { return _blendType; }
		
		inline const ax::Mat4& getModelView() const { return _mv; }
		
		void setForceFlush (bool forceFlush) { _forceFlush = forceFlush; }
		
		bool isForceFlush () { return _forceFlush; };
		
	protected:
		void generateMaterialID();
		uint32_t _materialID;


        void *_prog = nullptr;
        ax::backend::TextureBackend    *_texture       = nullptr;
        ax::backend::ProgramState      *_programState  = nullptr;
        ax::backend::UniformLocation   _locPMatrix;
        ax::backend::UniformLocation   _locTexture;

		ax::BlendFunc  _blendType;
		TwoColorTriangles   _triangles;
		ax::Mat4       _mv;
		bool                _forceFlush;
	};

    class SP_API SkeletonTwoColorBatch {
    public:
        static SkeletonTwoColorBatch* getInstance ();

        static void destroyInstance ();

        void update (float delta);

		V3F_C4B_C4B_T2F* allocateVertices(uint32_t numVertices);
		void deallocateVertices(uint32_t numVertices);
		
		unsigned short* allocateIndices(uint32_t numIndices);
		void deallocateIndices(uint32_t numIndices);

        TwoColorTrianglesCommand* addCommand(ax::Renderer* renderer, float globalOrder, ax::Texture2D* texture, ax::backend::ProgramState* programState, ax::BlendFunc blendType, const TwoColorTriangles& triangles, const ax::Mat4& mv, uint32_t flags);

        void batch(ax::Renderer* renderer, TwoColorTrianglesCommand* command);

        void flush(ax::Renderer* renderer, TwoColorTrianglesCommand* materialCommand);

		uint32_t getNumBatches () { return _numBatches; };
		
    protected:
        SkeletonTwoColorBatch ();
        virtual ~SkeletonTwoColorBatch ();

		void reset ();

		TwoColorTrianglesCommand* nextFreeCommand ();

		// pool of commands
		std::vector<TwoColorTrianglesCommand*> _commandsPool;
		uint32_t _nextFreeCommand;

		// pool of vertices
		std::vector<V3F_C4B_C4B_T2F> _vertices;
		uint32_t _numVertices;
		
		// pool of indices
		Vector<unsigned short> _indices;
		
		
		// VBO handles & attribute locations
		V3F_C4B_C4B_T2F* _vertexBuffer;
		uint32_t _numVerticesBuffer;
        uint32_t _numIndicesBuffer;
        unsigned short* _indexBuffer;

		// last batched command, needed for flushing to set material
		TwoColorTrianglesCommand* _lastCommand = nullptr;

		// number of batches in the last frame
		uint32_t _numBatches;
	};
}

#endif

#endif // SPINE_SKELETONTWOCOLORBATCH_H_
