#ifndef TRIE_INCLUDED
#define TRIE_INCLUDED

#include <string>
#include <vector>

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
		vector<ValueType> vals;
		vector<Node*> chn;
	};
	Node* root;
	void deleteNode(Node*);
};

#endif // TRIE_INCLUDED

template<typename ValueType>
Trie<ValueType>::Trie()
{
	root = new Node;
	root.vals = new vector<ValueType>;
	root.chn = new vector<Node*>;
}

template<typename ValueType>
Trie<ValueType>::~Trie()
{
	deleteNode(root);
}

template<typename ValueType>
void Trie<ValueType>::reset()             //this is a trie
{
	deleteNode(root);
	root = new Node;
	root.vals = new vector<ValueType>;
	root.chn = new vector<Node*>;
}

template<typename ValueType>
void Trie<ValueType>::insert(const std::string & key, const ValueType & value)
{
}

template<typename ValueType>
std::vector<ValueType> Trie<ValueType>::find(const std::string & key, bool exactMatchOnly) const
{
	return std::vector<ValueType>();
}

template<typename ValueType>
void Trie<ValueType>::deleteNode(Node *)
{
	if (this == nullptr)
		return;

	vector<ValueType>::iterator it = vals.end();
	it--;
	for( ; it != vals.begin(); it--) {       //delete the values in this current node
		vals.erase(it);        //returns an iterator to the next element
	}
	vals.erase(it);

	vector <Node*>::iterator nodeIt = chn.begin();
	for ( ; nodeIt != chn.end(); nodeIt++)        //recurse over the children
	{
		deleteNode(*it);
		Node* temp = *it;
		delete *it;
		delete *temp;
	}
}
