// MIT License

// Copyright (c) 2019 Erin Catto

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
#include "box2d/b2_broad_phase.h"
#include "box2d/b2_fixture.h"
#include "box2d/b2_circle_shape.h"

b2IntHashTable::b2IntHashTable(int32 initialCapacity) {
	m_mapCapacity = initialCapacity;
	m_mapNodes = (b2MapNode**) b2Alloc(m_mapCapacity * sizeof(b2MapNode*));
	memset(m_mapNodes, 0, m_mapCapacity * sizeof(b2MapNode*));
}

b2IntHashTable::~b2IntHashTable() {
	for (int32 i = 0; i < m_mapCapacity; ++i) {
		b2MapNode* c = m_mapNodes[i];

		while (c != nullptr) {
			b2MapNode* next = c->next;
			b2Free(c);
			c = next;
		}
	}

	b2Free(m_mapNodes);
}

inline uint32 hash(uint32 id) {
	return (id * 2654435769);
}

void b2IntHashTable::Grow() {
	int32 newMapCapacity = m_mapCapacity * 2;
	b2MapNode** newMap = (b2MapNode**) b2Alloc(newMapCapacity * sizeof(b2MapNode*));
	
	for (int32 i = 0; i < m_mapCapacity; i++) {
		b2MapNode** src = &m_mapNodes[i];
		b2MapNode* cur = *src;
		b2MapNode* list = nullptr;
		
		while (cur != nullptr) {
			// if the new msd bit to be used is 1 then cur must be moved
			if ((hash(cur->key) & m_mapCapacity) == m_mapCapacity) {
				// remove from old bucket
				*src = cur->next;
				
				// insert in the linked list for the new bucket
				cur->next = list;
				list = cur;
			} else {
				src = &(cur->next);
			}
			
			cur = *src;
		}
		
		newMap[i] = m_mapNodes[i];
		newMap[i + m_mapCapacity] = list;
	}
	
	b2Free(m_mapNodes);
	
	m_mapCapacity = newMapCapacity;
	m_mapNodes = newMap;
}

b2MapNode** b2IntHashTable::Get(b2Fixture* fixture) {
	uint32 id = fixture->GetId();
	b2MapNode** src = &m_mapNodes[hash(id) & (m_mapCapacity - 1)];

	while (*src != nullptr && (*src)->key != id) {
		src = &((*src)->next);
	}

	return src;
}

b2BroadPhase::b2BroadPhase(int32 initialCapacity) : m_map(initialCapacity) {
	b2Assert(initialCapacity > 0);

	m_count = 0;
	m_capacity = initialCapacity;
	m_needsRebuild = false;

	m_root = nullptr;
	m_rootDynamic = nullptr;
	m_rootStatic = nullptr;
  m_bufferStack = nullptr;

	m_links = (b2TreeNode**) b2Alloc(m_capacity * sizeof(b2TreeNode*));
	m_nodes = (b2TreeNode*) b2Alloc(m_capacity * 2 * sizeof(b2TreeNode));
	maxBufferSize = -1;
}

b2BroadPhase::~b2BroadPhase() {
	b2Free(m_links);
	b2Free(m_nodes);
}

bool b2BroadPhase::Add(b2Fixture* fixture) {
	b2MapNode** src = m_map.Get(fixture);
	
	if (*src != nullptr) {
		// This fixture already exists in this tree
		return false;
	}
	
	// check if we need to resize the node pool
	if (m_count >= m_capacity) {
		m_capacity *= 2;

		b2TreeNode* oldNodes = m_nodes;
		// allocate twice the capacity; the extra nodes are used to build the tree
		m_nodes = (b2TreeNode*) b2Alloc(m_capacity * 2 * sizeof(b2TreeNode));
		memcpy(m_nodes, oldNodes, m_count * sizeof(b2TreeNode));

		b2Free(oldNodes);
		b2Free(m_links);

		m_links = (b2TreeNode**) b2Alloc(m_capacity * sizeof(b2TreeNode*));
	}
	
	// initialize the new tree node
	int32 idx = m_count;
	m_count++;
	
	b2TreeNode* leaf = &m_nodes[idx];
	m_links[idx] = leaf;
	
	leaf->left = nullptr;
	leaf->right = nullptr;
	leaf->parent = nullptr;
	leaf->aabb = fixture->GetAABB();
	leaf->fixture = fixture;
	
	// add this node to the hash table
	// TODO use block allocator
	b2MapNode* newNode = (b2MapNode*) b2Alloc(sizeof(b2MapNode));
	
	newNode->key = fixture->GetId();
	newNode->value = idx;
	newNode->next = nullptr;
	
	// insert at the end of the bucket
	*src = newNode;
	
	// grow the hash table if needed
	// note: that prefferably we don't want to grow both the array and hash table
	// so the cost is spread out to multiple inserts; So use a different condition
	// reminder: we don't use open addressing so the penalty for almost full table is not high
	if (m_count >= m_map.m_mapCapacity * 3 / 4) {
		m_map.Grow();
	}

	m_needsRebuild = true;

	if (fixture->GetBody()->GetType() == b2_staticBody) {
	  m_rootStatic = nullptr;
	}

	return true;
}

bool b2BroadPhase::Update(b2Fixture* fixture) {
	b2MapNode** src = m_map.Get(fixture);
	
	if (*src == nullptr) {
		// nothing to update
		return false;
	}
	
	int32 idx = (*src)->value;
	fixture->UpdateAABB();
	m_nodes[idx].aabb = fixture->GetAABB();

	m_needsRebuild = true;

	if (fixture->GetBody()->GetType() == b2_staticBody) {
	  m_rootStatic = nullptr;
	}

	return true;
}

bool b2BroadPhase::UpdateNoRebuild(b2Fixture* fixture) {
	b2MapNode** src = m_map.Get(fixture);

	if (*src == nullptr) {
		// nothing to update
		return false;
	}

	int32 idx = (*src)->value;
	fixture->UpdateAABB();
	m_nodes[idx].aabb = fixture->GetAABB();

  b2TreeNode* node = m_nodes[idx].parent;
  while (node) {
    node->aabb.Combine(node->left->aabb, node->right->aabb);
    node = node->parent;
  }

	return true;
}

bool b2BroadPhase::Remove(b2Fixture* fixture) {
	b2MapNode** src = m_map.Get(fixture);
	b2MapNode* forRemoval = *src;

	if (forRemoval == nullptr) {
		// nothing to remove
		return false;
	}
	
	m_count--;
	
	int32 nodeIdx = forRemoval->value;
	int32 lastIdx = m_count;
	b2Fixture* lastFixture = m_nodes[lastIdx].fixture;

	b2Swap(m_nodes[nodeIdx], m_nodes[lastIdx]);

	// We need to update the hashtable entry that was pointing to the lastIdx node
	b2MapNode** lastMapNode = m_map.Get(lastFixture);
	b2Assert(*lastMapNode != nullptr);

	(*lastMapNode)->value = nodeIdx;

	// remove from the bucket of the hash table
	*src = forRemoval->next;
	b2Free(forRemoval);

	m_needsRebuild = true;

	if (fixture->GetBody()->GetType() == b2_staticBody) {
	  m_rootStatic = nullptr;
	}
	
	return true;
}

void b2BroadPhase::Build() {
	m_needsRebuild = false;

	if (m_count == 0) {
	  m_root = m_rootStatic = m_rootDynamic = nullptr;
	  return;
	}

  int32 staticGroup = 0;

	for (int32 i = 0; i < m_count; i++) {
		m_links[i] = &m_nodes[i];
		b2Fixture* f = m_links[i]->fixture;

		if (f->GetBody()->GetType() == b2_staticBody) {
		  b2Swap(m_links[i], m_links[staticGroup]);
		  staticGroup++;
		}
	}

  if (staticGroup <= m_count / 8) {
    staticGroup = 0;
    m_rootStatic = nullptr;
  } else if (m_rootStatic == nullptr) {
    m_treeAllocator = m_nodes + m_capacity;
	  m_rootStatic = Build(0, staticGroup);
  }

  m_treeAllocator = m_nodes + m_capacity + staticGroup;
	m_rootDynamic = Build(staticGroup, m_count);

  if (m_rootDynamic == nullptr) {
    m_root = m_rootStatic;
  } else if (m_rootStatic == nullptr) {
    m_root = m_rootDynamic;
  } else {
    m_root = &m_treeMergeNode;
    m_root->aabb.Combine(m_rootStatic->aabb, m_rootDynamic->aabb);
    m_root->left = m_rootStatic;
    m_root->right = m_rootDynamic;
    m_rootStatic->parent = m_root;
    m_rootDynamic->parent = m_root;
  }

  m_root->parent = nullptr;
}

b2TreeNode* b2BroadPhase::Build(int32 start, int32 end) {
	int count = end - start;
  int32 group0;

  if (count == 0) {
    return nullptr;
  } else if (count == 1) {
		return m_links[start];
	} else if (count == 2) {
	  group0 = start + 1;
	} else {
		b2Vec2 c = GetCenter2(m_links[start]->aabb);
		float minx = c.x, maxx = minx;
		float miny = c.y, maxy = miny;

		for (int32 i = start + 1; i < end; i++) {
			c = GetCenter2(m_links[i]->aabb);

			minx = b2Min(minx, c.x);
			miny = b2Min(miny, c.y);
			maxx = b2Max(maxx, c.x);
			maxy = b2Max(maxy, c.y);
		}

    if (b2Abs(maxx - minx) < b2_epsilon && b2Abs(maxy - miny) < b2_epsilon) {
      // If all the centers coincide then we have a degenerate O(n^2) collision case
      // This is quite bad for the physics pipeline, but we can help with this special handling
		  group0 = (start + end) / 2;
    } else {
      bool splitX = (maxx - minx) > (maxy - miny);
		  float mid = splitX? (minx + maxx) / 2 : (miny + maxy) / 2;
  	  group0 = start;

      for (int32 i = start; i < end; i++) {
			  b2AABB aabb = m_links[i]->aabb;
			  float nodeMid = splitX? (aabb.lowerBound.x + aabb.upperBound.x) : (aabb.lowerBound.y + aabb.upperBound.y);

			  if (nodeMid < mid) {
				  b2Swap(m_links[i], m_links[group0++]);
			  }
		  }

		  // prevent a degenrate tree with O(n) height
		  // while this will degrade tree quality but linear height is even worse
		  int32 lim = b2Max(count / 16, 1);
		  if (group0 < start + lim) {
			  group0 = start + lim;
			} else if (group0 > end - lim) {
  		  group0 = end - lim;
			}
    }
	}

	b2TreeNode* cur = m_treeAllocator++;
	cur->left = Build(start, group0);
	cur->right = Build(group0, end);
  cur->left->parent = cur;
  cur->right->parent = cur;
	cur->aabb.Combine(cur->left->aabb, cur->right->aabb);

  return cur;
}

int32 b2BroadPhase::GetTreeHeight() const {
	return (m_root != nullptr)? ComputeHeight(m_root) : 0;
}

int32 b2BroadPhase::ComputeHeight(b2TreeNode* node) const {
	if (node->IsLeaf()) {
		return 0;
	}
	
	int32 height1 = ComputeHeight(node->left);
	int32 height2 = ComputeHeight(node->right);
	
	return b2Max(height1, height2) + 1;
}

void b2BroadPhase::ShiftOrigin(const b2Vec2& newOrigin) {
	// Shift all b2TreeNodes: leaf nodes, internal nodes and some unused nodes as well
	for (int32 i = 0; i < m_capacity * 2; ++i) {
		m_nodes[i].aabb.lowerBound -= newOrigin;
		m_nodes[i].aabb.upperBound -= newOrigin;
	}
}
