#pragma once
#include "Heap.hpp"

// 数据--->权值
template<class T>
struct HuffmanTreeNode
{
	HuffmanTreeNode(const T& weight, const T data = T())
		: _weight(weight)
		, _pLeft(NULL)
		, _pRight(NULL)
		, _pParent(NULL)
	{}

	HuffmanTreeNode<T>* _pLeft;
	HuffmanTreeNode<T>* _pRight;
	HuffmanTreeNode<T>* _pParent;
	T _weight;   //   权值
	T _data;
};

template<class T>
class HuffmanTree
{
public:
	HuffmanTree()
		: _pRoot(NULL)
	{}

	HuffmanTree(const T array[], size_t size, const T& invalid)
	{
		_Create(array, size, invalid);
	}

	HuffmanTreeNode<T>* GetRoot()
	{
		return _pRoot;
	}

protected:
	void _Create(const T array[], size_t size, const T& invalid)
	{
		struct CompareNode
		{
			bool operator()(HuffmanTreeNode<T>* pLeft, HuffmanTreeNode<T>* pRight)
			{
				return pLeft->_weight < pRight->_weight;
			}
		};

		Heap<HuffmanTreeNode<T>*, CompareNode> hp;
		for (size_t idx = 0; idx < size; ++idx)
			if(array[idx] != invalid)
				hp.Insert(new HuffmanTreeNode<T>(array[idx]));

		while(hp.Size() > 1)
		{
			HuffmanTreeNode<T>* pLeft = hp.Top();
			hp.Remove();

			HuffmanTreeNode<T>* pRight = hp.Top();
			hp.Remove();

			HuffmanTreeNode<T>* parent = new HuffmanTreeNode<T>(pLeft->_weight+pRight->_weight);
			parent->_pLeft = pLeft;
			parent->_pRight = pRight;
			pLeft->_pParent = parent;
			pRight->_pParent = parent;
			hp.Insert(parent);
		}

		_pRoot = hp.Top();
	}

protected:
	HuffmanTreeNode<T>* _pRoot;
};