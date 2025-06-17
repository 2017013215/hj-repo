#pragma once

#include <map>
#include <list>
#include <vector>
#include <string>

#include "entityType.h"

template<class T>
class ByyTrieTree
{
public:
	template<class T>
	class TrieNode
	{
	public:
		TrieNode(int val)/*:myVal(val)*/:myDataSeted(false)
		{
		}

		~TrieNode()
		{
			Children::iterator it=myChildren.begin();

			for (;it!=myChildren.end();++it)
			{
				delete it->second;
			}
		}

		TrieNode* findChild(int val)
		{
			Children::iterator it=myChildren.find(val);

			TrieNode* result=0;

			if (it!=myChildren.end())
			{
				result=it->second;
			}

			return result;
		}

		TrieNode* getOrCreateChild(int val)
		{
			TrieNode* child=findChild(val);

			if (!child)
			{
				child=new TrieNode(val);

				myChildren[val]=child;
			}

			return child;
		}

		void setData(const T& data)
		{
			myData=data;

			myDataSeted=true;
		}

		const T& data()
		{
			return myData;
		}

		bool hasData()
		{
			return myDataSeted;
		}

	protected:
		//int  myVal;
		bool myDataSeted;

		typedef std::map<int,TrieNode<T>*> Children;

		Children myChildren;

		T myData;
	};

	ByyTrieTree():myRoot(-1)
	{

	}

	

	void insert(const ByyEntityType& et,const T& data)
	{
		std::vector<int> valVec=et.toVector();

		TrieNode<T> *node=&myRoot;

		for (int i=0;i!=valVec.size();++i)
		{
			if (valVec[i]==-1)
				break;

			node=node->getOrCreateChild(valVec[i]);
		}

		node->setData(data);
	}

	T getData(const ByyEntityType& et)
	{
		std::vector<int> valVec=et.toVector();

		TrieNode<T> *node=&myRoot;
		TrieNode<T> *temp=0;

		int tempBit=0;

		std::list<TrieNode<T>*> node_path;



		for (int i=0;i!=valVec.size();++i)
		{
			tempBit=valVec[i];

			if (tempBit==-1)
				break;

			node_path.push_back(node);

			temp=node->findChild(tempBit);

			if (!temp)
				break;

			node=temp;
		}

		std::list<TrieNode<T>*>::reverse_iterator it=node_path.rbegin();
		std::list<TrieNode<T>*>::reverse_iterator it1=node_path.rend();

		for (;it!=it1;++it)
		{
			if ((*it)->hasData())
			{
				return (*it)->data();
			}
		}

		return T();
	}

	TrieNode<T>* root(){return &myRoot;}

protected:
	TrieNode<T> myRoot;
};
