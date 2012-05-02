//#include "Datastream.cpp"
//#include "Terms.cpp"
#include "Dualsorted.cpp"
#include <ctime>
#include <queue>
#include "utils.cpp"
using namespace std;
#include <boost/algorithm/string.hpp>
//#define  int2char (integer) integer+0x30


class Accumulator
{
	public:
		size_t doc_id;
		long double impact;	
};

class AccComparison
{
  bool reverse;
public:
  AccComparison(const bool& revparam=false)
    {reverse=revparam;}
  bool operator() (Accumulator * lhs, Accumulator *rhs) const
  {
    if (reverse) return (lhs->impact > rhs->impact);
    else return (lhs->impact<rhs->impact);
  }
};




inline void executeOR(Dualsorted* ds, string ** terms, uint *qsizes,size_t total_queries)
{

	cout << "----- executing OR ---- " << endl;
	cout << "----- Amount of queries " << total_queries << endl;
    google::sparse_hash_map<uint, uint>  documents;
	clock_t start,finish;
	double time;
	double total = 0;
    start = clock();
    size_t total_results = 0;
    for (uint i = 0 ; i < total_queries ; i++)
	{
		if (i%100 ==0)
		{
			cout << "query " << i << endl;
		}
		for (uint j = 0 ; j < qsizes[i] ; j++)
		{
    		vector <uint> test = ds->range(terms[i][j].c_str(),0,3);	
        	for (uint x = 0 ; x < test.size();x++)
	    	{
				total_results++;
	        	//documents[test[x]] = 1; 
        	}
    	}
    }
    finish = clock();
	time = (double(finish)-double(start))/CLOCKS_PER_SEC;
	total += time;
    cout << "total documents = " << total_results << endl;  
    cout << "OR time = " << total << endl;
	cout << "END RESULT!" << endl;
	cout << ds->getSize() << endl;
}

inline void executePersin(Dualsorted* ds,string ** terms,uint *qsizes,uint top_k,size_t total_queries)
{
	cout << "----- executing persin ---- " << endl;
	cout << "----- Amount of queries " << total_queries << endl;
	
	clock_t start,finish;
	double time;
	double total = 0;
	double ex_total = 0;
	size_t documents = ds->ndocuments;
	priority_queue<Accumulator*,vector<Accumulator*>,AccComparison> persin;

	for (uint i = 0 ; i < total_queries ; i++)
	{
		persin = priority_queue<Accumulator*,vector<Accumulator*>,AccComparison>();
		Accumulator **acc = new Accumulator*[ds->ndocuments];
		ex_total = total;
	//	cout << "Creating accumulators"  << endl;
		for (int x = 0 ; x < documents;x++)
		{
				acc[x] = new Accumulator();
				acc[x]->impact = 0;
				acc[x]->doc_id = i;

		}
	//	cout << "Done!" << endl;
		start = clock();
		for (uint j = 0 ; j < qsizes[i] ;j++)
		{
	//		cout << "executing range queries" << endl;
	 	  	vector <uint> results = ds->range(terms[i][j],0,2);	
	 //   	cout << "done!" << endl;
	 //   	cout << "obtaining posting size for term:" << terms[i][j] << endl;
	    	uint posting_size = ds->getPostingSize(terms[i][j].c_str());
	//    	cout << "posting size = " << posting_size << endl;
	  //  	cout << "done!" << endl;
	//    	cout << "adding results " << endl;
	        for (uint k = 0 ; k < results.size();k++)
		    {	
		  //  	cout << "adding doc_id = " << test[k] << endl;
				double wxt = ds->getFreq(terms[i][j].c_str(),k)*msb(ds->ndocuments/posting_size);
		    	acc[results[k]]->impact += wxt;
		    	acc[results[k]]->doc_id = results[k];
				//persin.push(acc[test[k]]);

	        }
	     //   cout << "done!" << endl;
	        results.clear();

	    }
	    finish = clock();
	    time = (double(finish)-double(start))/CLOCKS_PER_SEC;
		total += time;
	//	cout << " Divding by doclens..." << endl;
	    for (size_t j = 0 ; j< documents;j++)
		{
			if (acc[j]->impact != 0.0)
			{
				start = clock();
				acc[j]->impact /= ds->doclens[j];
				persin.push(acc[i]);
			    finish = clock();
	    	    time = (double(finish)-double(start))/CLOCKS_PER_SEC;
				total += time;
			}
		}

	//	size_t *result = new size_t[top_k];
		start = clock();
		for (int i = 0 ;i<top_k;i++)
		{
			//cout << "persin.size = " << persin.size() << endl;
			if (persin.size() == 0)
				break;
				//persin.top();
			//cout << "adding document = " << r->doc_id << endl;
		//	result[i] = r->doc_id;
			if (i < persin.size()-1)
			{
				persin.pop();
			}
			else
			{
				break;
			}
		}
		finish = clock();
		time = (double(finish)-double(start))/CLOCKS_PER_SEC;
		total += time;
		if (i % 100 == 0)
		{
			cout << "query " << i << "time = " << (double)(total/100.0000) << endl;
		}
		delete[] acc;
	}
    cout << "Persin time = " << total << endl;
	cout << "END RESULT!" << endl;
	cout << ds->getSize() << endl;
}


void executeQueries(Dualsorted* ds,const char* queries,int query_type)
{

	int top_k = 10;
	ifstream qfile;
    qfile.open(queries);
    string filter = " (),:.;\t\"\'!?-_\0\n[]=#{}";
	string str;
	getline (qfile,str);
   	uint total_queries = atoi(str.c_str());
   	cout << "total_queries = " << total_queries << endl;
	string ** qterms = new string*[total_queries];
	uint *qsize = new uint[total_queries];
	uint i = 0;
    while(qfile.good())
	{
	    vector <string> q;
	    getline (qfile,str);
		if (str.length() >=3)
		{
			transform(str.begin(), str.end(),str.begin(), ::tolower);
   			Tokenize(str,q,filter);
   			
  	//			cout << "adding query " << i << endl;
   				qsize[i] = q.size();
   				qterms[i] = new string[q.size()];
   				for (int j = 0 ; j < q.size();j++)
   				{
   					qterms[i][j] = q[j];
   		//			cout << qterms[i][j] << endl;
   				}

   		}
   		i++;
   	}
   	
   	if (query_type == 1)
   		executeOR(ds,qterms,qsize,total_queries);
   	if (query_type == 0)
   		executePersin(ds,qterms,qsize,top_k,total_queries);
   	
   	
}

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
	const char* doclens_file = argv[4];	
	const char* queries = argv[5];	
	cout << "Invlist = " << invlist << endl;
	cout << "Invlist w/freq =" << invlistfreq << endl;
	cout << "vocab = " << vocab << endl;
	cout << "queries " << queries << endl;
	ifstream wordsfile;
	wordsfile.open(vocab);
	vector <string> words;
	string line;

	while(wordsfile.good())
	{
		getline(wordsfile,line);
		words.push_back(line);
	}
	uint count = words.size();
	cout << "count = " << count << endl;
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

		string sfilter = " \n\0";
		size_t ndocuments;
		size_t *doclens;
		ifstream doclen_data (doclens_file); 
		cout << "constructing doc_lens data..." << endl;
		if(doclen_data.is_open()) 
		{  
			getline (doclen_data,line);
		//	cout << "line = " << line << endl;
			ndocuments = atoi(line.c_str());
			doclens = new size_t[ndocuments];				
			for(int i = 0 ; i < ndocuments;i++) 
			{ 
				vector <string> doc_data;
				getline (doclen_data,line);
	//			cout << "line = " << line << endl;
				Tokenize(line,doc_data,sfilter);
				doclens[atoi(doc_data[0].c_str())] = atoi(doc_data[1].c_str());
			}
		}
		else 
		{   
		  cout << "couldn't open file " << doclens_file << endl;
		} 
	
	docfile.close();
	words.pop_back();
	result.pop_back();
	
// End of Filtering

	Dualsorted *ds = new Dualsorted(words, result, freqs, words.size(),doclens,ndocuments);
	executeQueries(ds,queries,0);
	executeQueries(ds,queries,1);
}
