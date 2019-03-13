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
		int curMatchPos = curMatch->position + m_searchMin;      //get the match's position in the genome
		string genNextBase;
		string fragNextBase;
		int curPos = m_searchMin;
		if (curPos <= fsize)
			fragNextBase += fragment[curPos];      //get the next base of the fragment
		if (!curMatch->gnome->extract(curMatchPos, 1, genNextBase))  //get the next base from the genome
			genNextBase = "";
		while (genNextBase == fragNextBase && fragNextBase != "")         //matches
		{
			curMatch->length++;                  //increase length info
			curPos++;
			curMatchPos++;
			if (curPos <= fsize)
				fragNextBase = fragment[curPos];      //get the next base of the fragment
			if (!curMatch->gnome->extract(curMatchPos, 1, genNextBase))  //get the next base from the genome
				genNextBase = "";
		}
	}
	//now somematches should hold the longest fragments. need to sort
	if (n == 0)
		return false;

	const Genome* curGenome = someMatches[0].gnome;     //temp pointer to first element's genome
	DNAMatchPlus curMatch = someMatches[0];
	bool needToPush = true;
	for ( int i = 0; i != n; i++)
	{
		while (i != n && someMatches[i].gnome == curGenome)
		{
			if (someMatches[i].length > curMatch.length)
				curMatch = someMatches[i];
			i++;
		}
		DNAMatch target;
		target.genomeName = curMatch.gnome->name();
		target.length = curMatch.length;
		target.position = curMatch.position;
		if (target.length >= minimumLength) 
			matches.push_back(target);
		if (i == n) {
			needToPush = false;
			break;
		}
		curMatch = someMatches[i];
		curGenome = someMatches[i].gnome;
	}
	if (needToPush) 
	{
		DNAMatch target;
		target.genomeName = curMatch.gnome->name();
		target.length = curMatch.length;
		target.position = curMatch.position;
		if (target.length >= minimumLength)
			matches.push_back(target);
	}
	return matches.size() > 0;
}

bool GenomeMatcherImpl::findRelatedGenomes(const Genome& query, int fragmentMatchLength, 
	bool exactMatchOnly, double matchPercentThreshold, vector<GenomeMatch>& results) const
{
    return false;  // This compiles, but may not be correct
}

/*
int main() {
	GenomeMatcher gm(3);
	Genome yeti("yeti", "acgtacgtaaaaccccggggttttnanananana");
	gm.addGenome(yeti);
	vector <DNAMatch> m;
	if (gm.findGenomesWithThisDNA("aaaaccccggggtttt", 12, true, m))
		cerr << "Found some matches " << m.size() << endl;
	else
		cerr << "find returned false" << endl;
	for (int i = 0; i != m.size(); i++) {
		cerr << "here's a match: " << m[i].genomeName << " position " 
			<< m[i].position << " length " << m[i].length << endl;
	}
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
