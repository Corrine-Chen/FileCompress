#pragma once
#include <vector>
#include <assert.h>
#include <algorithm>


// 仿函数
template<class T>
struct Less
{
	bool operator()(const T& left, const T& right)
	{
		return left<right;
	}
};

template<class T>
struct Greater
{
	bool operator()(const T& left, const T& right)
	{
		return left > right;
	}
};

template<class T, class Compare = Less<T>>
class Heap
{
public:
	Heap()
	{}

	Heap(const T array[], size_t size)
	{
		_heap.resize(size);
		for (size_t idx = 0; idx < size; ++idx)
		{
			//_heap.push_back(array[idx]);
			_heap[idx] = array[idx];
		}

		// 找倒数第一个非叶子结点
		int root = (_heap.size()-2)>>1;
		for (; root >= 0; root--)
			_AdjustDown(root);
	}

	size_t Size()const
	{
		return _heap.size();
	}

	bool Empty()const
	{
		return _heap.empty();
	}

	void Insert(const T& data)
	{
		_heap.push_back(data);
		if(_heap.size()>1)
			_AdjustUp();
	}

	void Remove()
	{
		assert(!_heap.empty());
		//if(_heap.size())
		size_t last = _heap.size()-1;
		std::swap(_heap[0], _heap[last]);
		_heap.pop_back();
		_AdjustDown(0);
	}

	const T& Top()const
	{
		assert(!Empty());
		return _heap[0];
	}

protected:
	void _AdjustDown(size_t parent)
	{
		size_t child = parent*2+1;
		size_t size = _heap.size();
		while(child < size)
		{
			// 如果右孩子存在，找最左右孩子中最小的
			//if(child+1 < size && _heap[child] > _heap[child+1])
			// 20  10
			if(child+1 < size && Compare()(_heap[child+1], _heap[child]))
				child +=1;

			//if(_heap[parent] > _heap[child])
			if(Compare()(_heap[child], _heap[parent]))
			{
				std::swap(_heap[parent], _heap[child]);
				parent = child;
				child = parent*2+1;
			}
			else
				return;
		}
	}

	void _AdjustUp()
	{
		size_t child = _heap.size()-1;
		size_t parent = (child-1)>>1;

		while(child != 0)
		{
			//if(_heap[child] < _heap[parent])
			if(Compare()(_heap[child], _heap[parent]))
			{
				std::swap(_heap[child], _heap[parent]);
				child = parent;
				parent = (child-1)>>1;
			}
			else
				return;
		}
	}

protected:
	std::vector<T> _heap;
};