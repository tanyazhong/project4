#include "provided.h"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
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

		bool operator< (const DNAMatchPlus other) {
			if (length < other.length)
				return true;
			return false;
		}
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
		DNAMatchPlus newDNA;          //create a new dnamatch for each fragment
		newDNA.gnome = &genome;
		newDNA.position = i;
		newDNA.length = m_searchMin;
		string frag;
		if (genome.extract(i, m_searchMin, frag)) 
			m_genomeData.insert(frag, newDNA);
	}
}

int GenomeMatcherImpl::minimumSearchLength() const
{
	return m_searchMin;
}

bool GenomeMatcherImpl::findGenomesWithThisDNA(const string& fragment, int minimumLength,
	bool exactMatchOnly, vector<DNAMatch>& matches) const
{
	int fsize = fragment.size();
	if (fsize < minimumLength || minimumLength < m_searchMin || minimumLength < 0)
		return false;

	vector<DNAMatchPlus> someMatches;
	string minFrag = fragment.substr(0, m_searchMin);  //get the first searchMin bases of the fragment
	someMatches = m_genomeData.find(minFrag, exactMatchOnly);       //now search for thme
	//now somematches holds fragment matches of the first searchMin bases, found by the trie
	int n = someMatches.size();
	for (int i = 0; i != n; i++)      //iterate over the matches
	{
		DNAMatchPlus* curMatch = &someMatches[i];
		int curMatchPos = curMatch->position;      //get the match's position in the genome
		string genNextBase;
		string fragNextBase;
		int curPos = m_searchMin;
		if (curPos <= fsize)
			fragNextBase += fragment[curPos];      //get the next base of the fragment
		if (!curMatch->gnome->extract(curPos, 1, genNextBase))  //get the next base from the genome
			genNextBase = "";
		while (genNextBase == fragNextBase && fragNextBase != "")         //matches
		{
			curMatch->length++;                  //increase length info
			curPos++;
			if (curPos <= fsize)
				fragNextBase = fragment[curPos];      //get the next base of the fragment
			if (!curMatch->gnome->extract(curPos, 1, genNextBase))  //get the next base from the genome
				genNextBase = "";
		}
	}
	//now somematches should hold the longest fragments. need to sort
	sort(someMatches.begin(), someMatches.end());
	DNAMatchPlus* source = &someMatches[0];
	DNAMatch target;
	target.genomeName = source->gnome->name();
	target.length = source->length;
	target.position = source->position;
	if (target.length >= minimumLength) {
		matches.push_back(target);
		return true;
	}
	return false;
}

bool GenomeMatcherImpl::findRelatedGenomes(const Genome& query, int fragmentMatchLength, 
	bool exactMatchOnly, double matchPercentThreshold, vector<GenomeMatch>& results) const
{
    return false;  // This compiles, but may not be correct
}

/*
int main() {
	GenomeMatcher gm(3);
	Genome org1("org1", "actgactg");
	gm.addGenome(org1);
}
*/

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
