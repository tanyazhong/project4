#include "provided.h"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include "Trie.h"
using namespace std;

class GenomeMatcherImpl
{
public:
    GenomeMatcherImpl(int minSearchLength);
    void addGenome(const Genome& genome);
    int minimumSearchLength() const;
    bool findGenomesWithThisDNA(const string& fragment, int minimumLength, 
		bool exactMatchOnly, vector<DNAMatch>& matches) const;
    bool findRelatedGenomes(const Genome& query, int fragmentMatchLength,
		bool exactMatchOnly, double matchPercentThreshold, vector<GenomeMatch>& results) const;
private:
	struct DNAMatchPlus
	{
		const Genome* gnome;
		int length;
		int position;
	};

	int m_searchMin;
	list<Genome> m_genomeList;
	Trie<DNAMatchPlus> m_genomeData;
};

GenomeMatcherImpl::GenomeMatcherImpl(int minSearchLength)
	:m_searchMin(minSearchLength)
{}

void GenomeMatcherImpl::addGenome(const Genome& genome)
{
	m_genomeList.push_back(genome);
	int size = genome.length();
	for (int i = 0; i <= size - m_searchMin; i++) 
	{
		DNAMatchPlus* newDNA = new DNAMatchPlus;          //create a new dnamatch for each fragment
		newDNA->gnome = &genome;
		newDNA->position = i;
		newDNA->length = m_searchMin;
		string frag;
		if (genome.extract(i, m_searchMin, frag))
			m_genomeData.insert(frag, *newDNA);
	}
}

int GenomeMatcherImpl::minimumSearchLength() const
{
	return m_searchMin;
}

bool GenomeMatcherImpl::findGenomesWithThisDNA(const string& fragment, int minimumLength,
	bool exactMatchOnly, vector<DNAMatch>& matches) const
{
	int size = fragment.size();
	if (size < minimumLength || minimumLength < m_searchMin || minimumLength < 0)
		return false;

	vector<DNAMatchPlus> someMatches;
	string minFrag = fragment.substr(0, m_searchMin);
	someMatches = m_genomeData.find(minFrag, exactMatchOnly);
	//now somematches holds fragment matches of the first searchMin bases, found by the trie
	int n = someMatches.size();
	for (int i = 0; i != n; i++)
	{
		DNAMatchPlus* curMatch = &someMatches[i];
		int curMatchPos = curMatch->position;
		string s;
		if(curMatch->gnome->extract(m_searchMin, 1, s))



	}


}

bool GenomeMatcherImpl::findRelatedGenomes(const Genome& query, int fragmentMatchLength, 
	bool exactMatchOnly, double matchPercentThreshold, vector<GenomeMatch>& results) const
{
    return false;  // This compiles, but may not be correct
}


//******************** GenomeMatcher functions ********************************

// These functions simply delegate to GenomeMatcherImpl's functions.
// You probably don't want to change any of this code.

GenomeMatcher::GenomeMatcher(int minSearchLength)
{
    m_impl = new GenomeMatcherImpl(minSearchLength);
}

GenomeMatcher::~GenomeMatcher()
{
    delete m_impl;
}

void GenomeMatcher::addGenome(const Genome& genome)
{
    m_impl->addGenome(genome);
}

int GenomeMatcher::minimumSearchLength() const
{
    return m_impl->minimumSearchLength();
}

bool GenomeMatcher::findGenomesWithThisDNA(const string& fragment, int minimumLength, bool exactMatchOnly, vector<DNAMatch>& matches) const
{
    return m_impl->findGenomesWithThisDNA(fragment, minimumLength, exactMatchOnly, matches);
}

bool GenomeMatcher::findRelatedGenomes(const Genome& query, int fragmentMatchLength, bool exactMatchOnly, double matchPercentThreshold, vector<GenomeMatch>& results) const
{
    return m_impl->findRelatedGenomes(query, fragmentMatchLength, exactMatchOnly, matchPercentThreshold, results);
}
