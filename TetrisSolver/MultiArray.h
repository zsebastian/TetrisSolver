/*  The MultiArray is a multidimensional array designed
*	to handled any number of dimensions.
*  Use freely.
*		Written: Sebastian Zander 19:54 2013-06-05
*/
#pragma once

#include <vector>
#include <array>

template <typename T, unsigned int dimensions>
class MultiArray
{
public:
	template <typename ... SizeArgs>
	MultiArray(size_t size, SizeArgs ... sizeArgs)
	{
		resize(size, sizeArgs ...);
	}

	MultiArray()
	{
		resize(0);
	}

	template <typename ... SizeArgs>
	void resize(size_t size1, SizeArgs ... size)
	{
		//is there a way to test this at compile-time?
		static_assert(sizeof...(size)+1 == dimensions, "too many or too few dimensions on resize");

		resizeHelper(0, size1, size1, size ...);
	}

	void setSizeOf(size_t dimension, size_t size)
	{
		if (dimension > dimension - 1)
			throw "trying to set size of a dimension out of range";

		mSizes[dimension] = size;
	}

	template <int dim>
	class ElementFinder
	{
	public:
		ElementFinder(std::vector<T>& vector, const std::array<size_t, dimensions>& sizes, const size_t index)
			:mVector(vector), mSizes(sizes), mIndex(index) {};

		ElementFinder<dim - 1> operator[](size_t index)
		{
			return ElementFinder<dim - 1>(mVector, mSizes, index + mIndex * mSizes[dimensions - dim]);
		}
	private:
		const size_t mIndex;
		std::vector<T>& mVector;
		const std::array<size_t, dimensions>& mSizes;
	};

	template <>
	class ElementFinder<1>
	{
	public:
		ElementFinder(std::vector<T>& vector, const std::array<size_t, dimensions>& sizes, const size_t index)
			:mVector(vector), mSizes(sizes), mIndex(index) {};

		T& operator[](const size_t index)
		{
			return mVector[index + mIndex  * mSizes[dimensions - 1]];
		}

	private:
		const size_t mIndex;
		std::vector<T>& mVector;
		const std::array<size_t, dimensions>& mSizes;
	};

	template <int dim>
	class ConstElementFinder
	{
	public:
		ConstElementFinder(const std::vector<T>& vector, const std::array<size_t, dimensions>& sizes, const size_t index)
			:mVector(vector), mSizes(sizes), mIndex(index) {};

		ConstElementFinder<dim - 1> operator[](const size_t index) const
		{
			return ConstElementFinder<dim - 1>(mVector, mSizes, index + mIndex * mSizes[dimensions - dim]);
		}
	private:
		const size_t mIndex;
		const std::vector<T>& mVector;
		const std::array<size_t, dimensions>& mSizes;
	};

	template <>
	class ConstElementFinder<1>
	{
	public:
		ConstElementFinder(const std::vector<T>& vector, const std::array<size_t, dimensions>& sizes, const size_t index)
			:mVector(vector), mSizes(sizes), mIndex(index) {};

		const T& operator[](const size_t index) const
		{
			return mVector[index + mIndex * mSizes[dimensions - 1]];
		}

	private:
		const size_t mIndex;
		const std::vector<T>& mVector;
		const std::array<size_t, dimensions>& mSizes;
	};

	ElementFinder<dimensions - 1> operator[](const size_t index)
	{
		return ElementFinder<dimensions - 1>(mVector, mSizes, index);
	}

	const ConstElementFinder<dimensions - 1> operator[](const size_t index) const
	{
		return ConstElementFinder<dimensions - 1>(mVector, mSizes, index);
	}

private:
	template <typename ... SizeArgs>
	void resizeHelper(size_t currentDim, size_t nextSize, size_t size1, size_t size2, SizeArgs ... size)
	{
		mSizes[currentDim] = nextSize;
		resizeHelper(currentDim + 1, size2, size1 * size2, size ...);
	}

	void resizeHelper(size_t currentDim, size_t nextSize, size_t size1, size_t size2)
	{
		mSizes[currentDim] = nextSize;
		resizeHelper(currentDim + 1, size2, size1 * size2);
	}

	void resizeHelper(size_t currentDim, size_t nextSize, size_t size)
	{
		mSizes[currentDim] = nextSize;
		mVector.resize(size);
	}

	std::vector<T> mVector;
	std::array<size_t, dimensions> mSizes;
};