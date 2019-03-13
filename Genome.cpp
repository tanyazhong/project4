#include "provided.h"
#include <string>
#include <vector>
#include <iostream>
#include <istream>
#include <fstream>
using namespace std;

class GenomeImpl
{
public:
    GenomeImpl(const string& nm, const string& sequence);
    static bool load(istream& genomeSource, vector<Genome>& genomes);
    int length() const;
    string name() const;
    bool extract(int position, int length, string& fragment) const;
private:
	string m_name;
	string m_dna;
};

GenomeImpl::GenomeImpl(const string& nm, const string& sequence)
{
	m_name = nm;
	m_dna = sequence;
}

bool GenomeImpl::load(istream& genomeSource, vector<Genome>& genomes) 
{
	if (!genomeSource)		        // Did opening the file fail?
	{
		cerr << "Error: Cannot open file" << endl;
		return false;
	}
	char c; string s; string dna; bool justNewlined = false; Genome* g;
	genomeSource.get(c);
	if (c != '>')                //check that the file starts with a name line
	{
		cerr << "Error: file does not start with a name line" << endl;
		return false;
	}
	else 
	{
		getline(genomeSource, s);  //read in the name of the first organism in the file
	}
	while (genomeSource.get(c))
	{
		switch (c)
		{
		case 'A':
		case 'C':
		case 'T':
		case 'G':
		case 'N':
		case 'a':
		case 'c':
		case 't':
		case 'g':
		case 'n':
			justNewlined = false;
			dna += toupper(c);
			break;
		case '\n':
			if (justNewlined)
			{
				cerr << "Error: file contains empty line" << endl;
				return false;
			}
			else
				justNewlined = true;
			break;
		case '>':
			if (dna == "")
			{
				cerr << "Error: no bases after name line" << endl;
				return false;
			}
			if (s == "")
			{
				cerr << "Error: no characters after > in name line" << endl;
				return false;
			}
			g = new Genome(s, dna);
			genomes.push_back(*g);
			s = ""; dna = "";
			getline(genomeSource, s);
			justNewlined = false;
			break;
		default:
			cerr << "Error: invalid base character" << endl;
			return false;
		}
	}

	if (dna == "")
	{
		cerr << "Error: no bases after name line" << endl;
		return false;
	}
	if (s == "")
	{
		cerr << "Error: no characters after > in name line" << endl;
		return false;
	}
	g = new Genome(s, dna);
	genomes.push_back(*g);
	return true;
}

int GenomeImpl::length() const
{
	return m_dna.size();
}

string GenomeImpl::name() const
{
	return m_name;
}

bool GenomeImpl::extract(int position, int length, string& fragment) const
{
	if (length < 0 || position < 0)
		return false; 
	if (position >= this->length() || (position + length) > this->length())
		return false;
	fragment = m_dna.substr(position, length);
}

//******************** Genome functions ************************************

// These functions simply delegate to GenomeImpl's functions.
// You probably don't want to change any of this code.

Genome::Genome(const string& nm, const string& sequence)
{
    m_impl = new GenomeImpl(nm, sequence);
}

Genome::~Genome()
{
    delete m_impl;
}

Genome::Genome(const Genome& other)
{
    m_impl = new GenomeImpl(*other.m_impl);
}

Genome& Genome::operator=(const Genome& rhs)
{
    GenomeImpl* newImpl = new GenomeImpl(*rhs.m_impl);
    delete m_impl;
    m_impl = newImpl;
    return *this;
}

bool Genome::load(istream& genomeSource, vector<Genome>& genomes) 
{
    return GenomeImpl::load(genomeSource, genomes);
}

int Genome::length() const
{
    return m_impl->length();
}

string Genome::name() const
{
    return m_impl->name();
}

bool Genome::extract(int position, int length, string& fragment) const
{
    return m_impl->extract(position, length, fragment);
}

/*

int main()
{
	ifstream inF("C:/Users/Tanya/Documents/cs32/Gee-nomics/data/test.txt");
	vector<Genome>	vg;
	bool success = Genome::load(inF, vg);
	string s;
	if (success)
	{
		cout << "Loaded " << vg.size() << " genomes successfully:" << endl;
		for (int k = 0; k != vg.size(); k++)
		{
			cout << vg[k].name() << endl;
			cout << vg[k].length() << endl;
			if (vg[k].extract(0, 3, s))
				cout << s << endl;
			else
				cout << "couldn't extract fragment" << endl;
		}
	}
	else
		cout << "Error loading genome data" << endl;
}

*/

