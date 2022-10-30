
#pragma once

#include <array>
#include <assert.h>
#include <stdint.h>
#include <unordered_set>

namespace LLGI
{

template <typename T, size_t N> struct FixedSizeVector
{
private:
	std::array<T, N> internal_;
	size_t size_ = 0;

public:
	T& at(size_t n)
	{
		assert(n < size_);
		return internal_.at(n);
	}

	const T& at(size_t n) const
	{
		assert(n < size_);

		return internal_.at(n);
	}

	const T* data() const { return internal_.data(); }

	void resize(size_t nsize)
	{
		assert(nsize <= internal_.size());
		size_ = nsize;
	}

	bool operator==(FixedSizeVector<T, N> const& rhs) const
	{
		if (size_ != rhs.size_)
			return false;

		for (size_t i = 0; i < size_; i++)
		{
			if (internal_[i] != rhs.internal_[i])
				return false;
		}

		return true;
	}

	bool operator!=(FixedSizeVector<T, N> const& rhs) const { return !(*this == rhs); }

	size_t size() const { return size_; }

	size_t get_hash() const
	{
		auto h = std::hash<size_t>()(size());
		for (size_t i = 0; i < size(); i++)
		{
			h += std::hash<T>()(at(i));
		}
		return h;
	}
};

} // namespace LLGI
