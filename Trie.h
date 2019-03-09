#ifndef TRIE_INCLUDED
#define TRIE_INCLUDED

#include <string>
#include <vector>
#include <list>

template<typename ValueType>
class Trie
{
public:
    Trie();
    ~Trie();
    void reset();
    void insert(const std::string& key, const ValueType& value);
    std::vector<ValueType> find(const std::string& key, bool exactMatchOnly) const;

      // C++11 syntax for preventing copying and assignment
    Trie(const Trie&) = delete;
    Trie& operator=(const Trie&) = delete;
private:
	struct Node
	{
		char label = ' ';
		std::list<ValueType> vals;
		std::list<Node*> chn;
	};
	Node* m_root;
	void deleteTree(Node* cur);
	void insertHelper(Node* cur, const std::string & key, const ValueType & value);
};

#endif // TRIE_INCLUDED

template<typename ValueType>
Trie<ValueType>::Trie()
{
	m_root = new Node;
}

template<typename ValueType>
Trie<ValueType>::~Trie()
{
	deleteTree(m_root);
}

template<typename ValueType>
void Trie<ValueType>::reset()             //this is a trie
{
	deleteNode(m_root);
	m_root = new Node;
}

template<typename ValueType>
void Trie<ValueType>::insert(const std::string & key, const ValueType & value)
{
	insertHelper(m_root, key, value);
}

template<typename ValueType>
void Trie<ValueType>::insertHelper(Node* cur, const std::string & key, const ValueType & value) 
{
	if (key.size() == 1 && cur->label == key[0]) //if key is one char and that char matches the label of current node, insert value
	{
		cur->vals.push_back(value);
		return;
	}

	typename list<Node*>::iterator it = cur->chn.begin();   //iterator point's to a node's child
	for (; it != cur->chn.end(); it++)
	{
		Node* child = *it;
		if (key[0] == child->label)                          //key char matches label of a child
		{
			if (key.size() == 1) 
				child->vals.push_back(value);
			else
				insertHelper(child, key.substr(1), value);       //so go into that child
			return;
		}
	}
	
	//here, no children match the key, so make a child
	Node* newChild = new Node;
	newChild->label = key[0];
	cur->chn.push_back(newChild);

	if (key.size() == 1) {
		newChild->vals.push_back(value);
		return;
	}
	else
		insertHelper(newChild, key.substr(1), value);
}

template<typename ValueType>
std::vector<ValueType> Trie<ValueType>::find(const std::string & key, bool exactMatchOnly) const
{
	return std::vector<ValueType>();
}

template<typename ValueType>
void Trie<ValueType>::deleteTree(Node * cur) //ASK ABOUT DELETE FUNC
{
	if (this == nullptr)
		return;

	/*
	typename std::list<ValueType>::iterator it = cur->vals.begin();
	for ( ; it != cur->vals.end(); ) 
	{       
		 it = cur->vals.erase(it);        
	}
	*/

	typename std::list<Node*>::iterator nodeIt = cur->chn.begin();
	for (; nodeIt != cur->chn.end(); nodeIt++)        //recurse over the children
	{
		nodeIt = cur->chn.begin();
		deleteTree(*nodeIt);
	}

	delete cur;
}