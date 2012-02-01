#include "Datastream.cpp"
#include "Terms.cpp"
#include "Dualsorted.cpp"
#include <ctime>

//#include "utils.cpp"
using namespace std;
#include <boost/algorithm/string.hpp>
//#define  int2char (integer) integer+0x30



int main(int argc, char** argv)
{
	if (argc < 2)
	{
		cout  << "usage: ./dualsorted <inverted list> <inverted list w/freqs > <vocabulary> <queries>" << endl;
		return 0;
	}
	const char* invlist = argv[1];
	const char* invlistfreq = argv[2];
	const char* vocab = argv[3];	
	const char* queries = argv[4];	

	ifstream wordsfile;
	wordsfile.open(queries);
	vector <string> words;
	string line;

	while(wordsfile.good())
	{
		getline(wordsfile,line);
		words.push_back(line);
	}
	uint count = words.size();
	wordsfile.close();

	ifstream docfile;
	docfile.open(invlist);
	vector < vector <int> > result;
	vector<string> strs;
	
	while (docfile.good())
	{
		vector<int> r;
		getline(docfile,line);
		boost::split(strs, line, boost::is_any_of(" "));
		int doc_size = atoi(strs[0].c_str());
		for (int i = 1 ; i < strs.size()-1;i++)
		{
			r.push_back(atoi(strs[i].c_str()));
		}
		result.push_back(r);
	}
	docfile.close();

	ifstream freqfile;
	docfile.open(invlistfreq);
	vector <int> freqs;
	vector <string> strs2;
	while (docfile.good())
	{
		getline(docfile,line);
	   	boost::split(strs2, line, boost::is_any_of(" "));
		int freq_size = atoi(strs2[0].c_str());
		for (int i = 1 ; i < strs2.size()-1;i++)
		{
			freqs.push_back(atoi(strs2[i].c_str()));
		}
	}
	docfile.close();
	words.pop_back();
	result.pop_back();
	
// End of Filtering

	Dualsorted *ds = new Dualsorted(words, result, freqs, words.size());
//	ds->test();


clock_t start,finish;
double time;
double total = 0;
//
// Filtering Queries
    ifstream qfile;
    qfile.open(queries);
    string filter = " (),:.;\t\"\'!?-_\0\n[]=#{}";
    while(qfile.good())
	{
	    vector <string> q;
 	    string str;
	    getline (qfile,str);

		if (str.length() >=3)
		{
			transform(str.begin(), str.end(),str.begin(), ::tolower);
   			Tokenize(str,q,filter);

            google::sparse_hash_map<uint, uint>  documents;
            start = clock();
            for (int x = 0;x<q.size();x++)
            {
                vector <uint> test = ds->range(q[x],0,2);	
                for (uint i = 0 ; i < test.size();i++)
	            {
                    documents[test[i]] = documents[test[i]]+ds->getFreq(q[x],ds->getPosTerm(q[x],test[i])); 
	            }
            }
            finish = clock();
		      time = (double(finish)-double(start))/CLOCKS_PER_SEC;
		      total += time;

         //google::sparse_hash_map<uint, uint> :: const_iterator it;
   		 //for (it = documents.begin();it != documents.end();it++)
   		// {
    	//    cout << it->first << " -> " << it->second << endl;
   		// }
        }
    }

    ds->test();
    cout << "Total time = " << total << endl;
	cout << "END RESULT!" << endl;
	cout << ds->getSize() << endl;
	delete ds;
}
