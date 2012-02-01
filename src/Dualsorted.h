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
using namespace std;
using namespace cds_static;
using namespace __gnu_cxx;
class Dualsorted
{
	private:
		// Main Data Structures
		BitSequence **Rt;
		BitSequenceRRR **Tt;
		WaveletTreeNoptrs *L;
		BitSequence *st;
		Sequence *vt;

        google::sparse_hash_map<const char*, int, hash<const char*>, eqstr> terms;
		// Temporal use for construction
		vector<int> freqs;
		vector<vector<int>> result;
	    //	vector<string> terms;

		// Lengths
		uint size_terms;
		uint L_size;

	public:
		Dualsorted(vector<string> &terms, vector< vector<int> > &result, vector<int> &freqs,uint size_terms);
		// Build methods
		BitSequence *buildSt();
		BitSequence **buildRt();
		BitSequenceRRR **buildTt();
		Sequence * buildVt();
		Sequence * buildL();
		
		// Requested function implementations
		uint getDocid(string term,uint i);
		uint getFreq(string term,int i);
		vector < pair<uint,size_t> > mrqq(string term, size_t k, size_t kp);
		vector <uint> range(string t,size_t x,size_t y);
		void intersect(string t,string k);
		
		// others
		uint getPosTerm(string t,uint d);
		uint getTermPosition(const char *t);

		size_t getSize();
		void test();
};
