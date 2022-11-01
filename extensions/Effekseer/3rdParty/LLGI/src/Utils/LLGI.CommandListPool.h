
#pragma once

#include "../LLGI.CommandList.h"
#include "../LLGI.Graphics.h"

namespace LLGI
{

class CommandListPool
{
private:
	Graphics* graphics_ = nullptr;

	int32_t current_ = 0;
	std::vector<CommandList*> commandLists_;

public:
	CommandListPool(Graphics* graphics, SingleFrameMemoryPool* memoryPool, int32_t count)
	{
		SafeAssign(graphics_, graphics);

		commandLists_.reserve(count);

		for (int32_t i = 0; i < count; i++)
		{
			auto commandList = graphics_->CreateCommandList(memoryPool);
			commandLists_.push_back(commandList);
		}
	}

	~CommandListPool()
	{
		for (auto o : commandLists_)
		{
			o->Release();
		}

		SafeRelease(graphics_);
	}

	CommandList* Get(bool addRef = false)
	{
		CommandList* commandList = nullptr;

		commandLists_[current_]->WaitUntilCompleted();

		commandList = commandLists_[current_];

		if (addRef)
		{
			SafeAddRef(commandList);
		}

		current_++;
		current_ %= commandLists_.size();

		return commandList;
	}
};

} // namespace LLGI
