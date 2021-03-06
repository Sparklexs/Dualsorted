#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <sstream>
//#include "utils.cpp"
#include <algorithm>
#include <google/sparse_hash_map>
#include <WaveletTree.h>
#include <Sequence.h>
#include <Mapper.h>
#include <BitSequenceRG.h>
#include <BitString.h>
#include "partialSums.cpp"
//#include "delta.c"

using namespace std;
using namespace cds_static;
using namespace __gnu_cxx;


class Dualsorted
{
	private:
		// Main Data Structures

		WaveletMatrix *L;
		BitSequence *st;
		CompressedPsums ** ps;
        google::sparse_hash_map<string, uint> terms;
        //const char ** terms;
		// Temporal use for construction
		vector<int> freqs;
		vector<vector<int>> result;

	    //	vector<string> terms;

		// Lengths
		uint size_terms;
		uint L_size;
		int k;

	public:
		size_t *doclens;
		size_t ndocuments;
		Dualsorted(vector<string> terms, vector< vector<int> > &result, vector<int> &freqs,uint size_terms,size_t *doclens,size_t ndocuments);
		BitSequence *buildSt();
		Sequence * buildL();
		void buildSums();
		
		// Requested function implementations
		uint getDocid(string term,uint i);
		uint getFreq(const char*,int i);
		uint getPostingSize(string term);
		vector < pair<uint,size_t> > mrqq(string term, size_t k, size_t kp);
		vector <uint> range(string t,size_t x,size_t y);
		void intersect(string *terms,uint qsizes);
		vector <uint> getRange(string term,uint i);
		
		// others
		uint getPosTerm(string t,uint d);
		uint getTermPosition(const char *t);

		size_t getSize();
		void test();
};
