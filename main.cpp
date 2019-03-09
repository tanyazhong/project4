#include "Trie.h"
#include <iostream>
using namespace std;

int main() 
{
	Trie<int> t;
	t.insert("CAT", 1);
	t.insert("C", 2);
	t.insert("CA", 3);
	cout << "i'm trie-ing" << endl;
}