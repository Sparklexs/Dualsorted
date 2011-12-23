#include "Datastream.cpp"
#include "Terms.cpp"
#include "Dualsorted.cpp"
#include <ctime>

//#include "utils.cpp"
using namespace std;
#include <boost/algorithm/string.hpp>
//#define  int2char (integer) integer+0x30

void clearFiles(string name)
{
	ofstream outfile;
	outfile.open(name + ".invlistfreq");
	outfile.close();

	outfile.open(name + ".words");
	outfile.close();

	outfile.open(name + ".invlist");
	outfile.close();
}


uint process(string directory)
{
	vector <string> files = getFiles(directory);
	vector<string> d;
	vector<string> v;
	Terms *t = new Terms(250000,files.size());
	vector <uint> frequencies;
	uint count = 0;
    uint curr = 0;
    int size_files = files.size();
	for (int i = 0 ; i < size_files;i++)
	{
	//	cout << files[i] << endl;
		Datastream *ds = new Datastream(directory + "/" + files[i]);
		d = ds->getTerms();
//		cout << "amount of terms:" << ds->getTermSize() << endl;
		v = ds->getDistinctTerms();
//		cout << "amount of distinct terms" << v.size() << endl;	
		count += ds->getTermSize();
		t->addDataStream(ds);	
        curr += 1;
        if (curr % 10 == 0) 
            cout << curr << " / " << files.size() << endl << " Term size = " << count << endl;
		delete ds;
	}
	
	clearFiles("testing");
	t->writeFiles("testing");
	delete t;
	return count;
}

int main(int argc, char** argv)
{
       
	string directory = argv[1];
	int p = atoi(argv[2]);
	uint count;
	if (p)
		count = process(directory);

	ifstream wordsfile;
	wordsfile.open("testing.words");
	vector <string> words;
	string line;
	while(wordsfile.good())
	{
		getline(wordsfile,line);
		words.push_back(line);
	}
	wordsfile.close();

	ifstream docfile;
	docfile.open("testing.invlist");
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
	docfile.open("testing.invlistfreq");
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
	

	Dualsorted *ds = new Dualsorted(words, result, freqs, words.size(), count);
//	ds->test();

clock_t start,finish;
double time;
double total = 0;
//
// Filtering Queries
    ifstream qfile;
    qfile.open("queries.txt");
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
