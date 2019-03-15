#include "provided.h"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <map>
#include <algorithm>
#include "Trie.h"
using namespace std;

bool compareGenomeMatch(const GenomeMatch& lhs, const GenomeMatch& rhs);

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
	int m_searchMin;
	vector<Genome> m_genomeVec;
	Trie<DNAMatch> m_genomeData;
	void longestFragmentHelper(DNAMatch& curMatch, const Genome* curGenome, const string& fragment,
		string& genomePart, string& fragPart, bool b, bool snip) const;
};

GenomeMatcherImpl::GenomeMatcherImpl(int minSearchLength)
	:m_searchMin(minSearchLength)
{}


void GenomeMatcherImpl::addGenome(const Genome& genome)
{
	m_genomeVec.push_back(genome);
	int size = genome.length();
	for (int i = 0; i <= size - m_searchMin; i++) 
	{
		DNAMatch newDNA;          //create a new dnamatch for each fragment
		newDNA.genomeName = genome.name();
		newDNA.position = i;
		newDNA.length = m_searchMin;
		string frag;
		if (genome.extract(i, m_searchMin, frag)) {
			m_genomeData.insert(frag, newDNA);
			//cerr << "inserted " << frag << endl;
		}
	}
}

int GenomeMatcherImpl::minimumSearchLength() const
{
	return m_searchMin;
}

bool GenomeMatcherImpl::findGenomesWithThisDNA(const string& fragment, int minimumLength,
	bool exactMatchOnly, vector<DNAMatch>& matches) const
{
	const int fsize = fragment.size();
	const int genomeVecSize = m_genomeVec.size();
	if (fsize < minimumLength || minimumLength < m_searchMin || minimumLength < 0)
		return false;

	string minFrag = fragment.substr(0, m_searchMin);  //get the first searchMin bases of the fragment
	vector<DNAMatch> someMatches = m_genomeData.find(minFrag, exactMatchOnly);     

	//now somematches holds fragment matches of the first searchMin bases, found by the trie
	int n = someMatches.size();
	if (n == 0)
		return false;
	for (int i = 0; i != n; i++)      //iterate over the matches
	{
		DNAMatch* curMatch = &(someMatches[i]);
		const Genome* curGenome = nullptr;
		for (int i = 0; i != genomeVecSize; i++) {
			if (m_genomeVec[i].name() == curMatch->genomeName)
				curGenome = &m_genomeVec[i];
		}

		string fragPart = minFrag;
		string genomePart;
		curGenome->extract(curMatch->position, curMatch->length, genomePart);
		bool snip = false;
		if (fragPart != genomePart)
			snip = true;
		longestFragmentHelper(someMatches[i], curGenome, fragment, genomePart, fragPart, exactMatchOnly, snip);
	}
	//now somematches should hold the longest fragments. need to sort
	DNAMatch curMatch = someMatches[0];
	string curGenomeName = curMatch.genomeName;
	bool needToPush = true;

	for ( int i = 0; i != n; i++)
	{
		while (i != n && someMatches[i].genomeName == curGenomeName)
		{
			if (someMatches[i].length > curMatch.length)
				curMatch = someMatches[i];
			i++;
		}
		DNAMatch target = curMatch;
		if (target.length >= minimumLength) 
			matches.push_back(target);
		if (i == n) {
			needToPush = false;
			break;
		}
		curMatch = someMatches[i];
		curGenomeName = someMatches[i].genomeName;
	}
	if (needToPush) 
	{
		DNAMatch target = curMatch;
		if (target.length >= minimumLength)
			matches.push_back(target);
	}
	return matches.size() > 0;
}

void GenomeMatcherImpl::longestFragmentHelper(DNAMatch& curMatch, const Genome* curGenome, const string& fragment, 
	string& genomePart, string& fragPart, bool b, bool snip) const
{
	if (fragPart != genomePart) {
		snip = true;
		b = true;
	}
	int fsize = fragment.size();
	int length = curMatch.length;

	if (length < fsize)
		fragPart += fragment[length];      //get the next base of the fragment
	else 
		return;
	
	if (curGenome == nullptr || !curGenome->extract(curMatch.position, length + 1, genomePart))  //get the next base from the genome
		genomePart = "";
	if (genomePart[length] == fragPart[length] || (snip && !b) || !b)        //matches
	{
		if (genomePart[length] != fragPart[length])
			b = true;
		curMatch.length++;                      //increase length info
		longestFragmentHelper(curMatch, curGenome, fragment, genomePart, fragPart, b, snip);
	}
}

bool GenomeMatcherImpl::findRelatedGenomes(const Genome& query, int fragmentMatchLength, 
	bool exactMatchOnly, double matchPercentThreshold, vector<GenomeMatch>& results) const
{
	if (matchPercentThreshold < 0 || matchPercentThreshold > 100)
		return false;
	int num = query.length() / fragmentMatchLength;
	string curFrag;
	vector<DNAMatch> matches;
	int nMatches;
	map<string, int> genomeToMatchCount;
	for (int i = 0; i != num; i++)
	{
		query.extract(i*fragmentMatchLength, fragmentMatchLength, curFrag);
		findGenomesWithThisDNA(curFrag, fragmentMatchLength, exactMatchOnly, matches);
		nMatches = matches.size();
		for (int j = 0; j != nMatches; j++)  //for every genome that returns a match to this fragment 
		{
			string curGenName = matches[j].genomeName;
			if (genomeToMatchCount.find(curGenName) == genomeToMatchCount.end()) {  //if its not in the map
				genomeToMatchCount[curGenName] = 1;                                 //add it and say there's 1 match
			}
			else
				genomeToMatchCount[curGenName]++;
		}
	}
	GenomeMatch g;
	map<string, int>::iterator it = genomeToMatchCount.begin();
	for (; it != genomeToMatchCount.end(); it++)
	{
		g.genomeName = (*it).first;
		g.percentMatch = 100* (*it).second / num;
		if (g.percentMatch >= matchPercentThreshold)
			results.push_back(g);
	}
	sort(results.begin(), results.end(), compareGenomeMatch);
	return results.size() > 0;
}

bool compareGenomeMatch(const GenomeMatch & lhs, const GenomeMatch & rhs)
{
	if (lhs.percentMatch > rhs.percentMatch)
		return true;
	if (lhs.percentMatch < rhs.percentMatch)
		return false;
	return lhs.genomeName < rhs.genomeName;
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
