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
	std::vector<ValueType> findHelper(Node* cur, const std::string & key,
		std::vector<ValueType>& matches, bool b, bool notfirstChar) const;
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
	deleteTree(m_root);
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
	typename std::list<Node*>::iterator it = cur->chn.begin();
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
	
	Node* newChild = new Node;     //here, no children match the key, so make a child
	newChild->label = key[0];
	cur->chn.push_back(newChild);

	if (key.size() == 1)
		newChild->vals.push_back(value);
	else
		insertHelper(newChild, key.substr(1), value);
}

template<typename ValueType>
std::vector<ValueType> Trie<ValueType>::find(const std::string & key, bool exactMatchOnly) const
{
	std::vector<ValueType> matches;
	findHelper(m_root, key, matches, exactMatchOnly, false);
	return matches;
}

template<typename ValueType>
std::vector<ValueType> Trie<ValueType>::findHelper(Node* cur, const std::string & key, 
	std::vector<ValueType>& matches, bool b, bool notfirstChar) const
{ 
	typename std::list<Node*>::iterator it = cur->chn.begin();
	for (; it != cur->chn.end(); it++)
	{
		Node* child = *it;
		if (key[0] == child->label || (!b && notfirstChar))
		{
			if (key.size() == 1) {
				typename std::list<ValueType>::iterator valsIt = child->vals.begin();
				for (; valsIt != child->vals.end(); valsIt++) {
					matches.push_back(*valsIt);
				}
			}
			else
			{
				if (key[0] != child->label)
					b = true;
				findHelper(child, key.substr(1), matches, b, true);
			}
		}
	}
	return matches;
}

template<typename ValueType>
void Trie<ValueType>::deleteTree(Node * cur) //ASK ABOUT DELETE FUNC
{
	if (this == nullptr)
		return;
	typename std::list<Node*>::iterator nodeIt = cur->chn.begin();
	for (; nodeIt != cur->chn.end(); )        //recurse over the children
	{
		deleteTree(*nodeIt);
		nodeIt = cur->chn.erase(nodeIt);
	}

	delete cur;
}