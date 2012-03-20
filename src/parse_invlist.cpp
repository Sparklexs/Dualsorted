#include <iostream>
#include <fstream>
#include "utils.cpp"
#include <string>
#include <map>
#include <omp.h>

using namespace std;






class Plist
{
public:
	size_t frequency;
	size_t doc_id;

	Plist(size_t frequency,size_t doc_id)
	{
		this->frequency = frequency;
		this->doc_id = doc_id;
	}
};


class InvList
{
public:
	
	map<string,vector <Plist> > plists;
	string prefix;
	InvList(string f,string prefix)
	{
		cout << "got filename:" << f << endl;
		this->prefix = prefix;
		int id;
		int doc_id = 0;
		vector <string> files = getData(f);
		map<string,vector <Plist> > plist = this->plists;
		
		for (doc_id = 0 ; doc_id < files.size()-1;doc_id++)
		{
				this->process(files,plist,doc_id);
		}
		sort_and_write(plist);
		
	}

	vector<string> getData(string f)
	{

		vector <string> files;
		string line;
	  	ifstream data (f.c_str());
	  	if (data.is_open())
	  	{
	   	 while ( data.good() )
	   	 {
	   	   getline (data,line);
	  // 	   cout << "adding files:" << line << endl;
	   	   files.push_back(line);
	   	 }
	   	 data.close();
	  	}
	  	else cout << "Unable to open file"; 
	  	return files;
	}

	map<string,vector <Plist> > process(vector <string> files,map<string,vector <Plist> > &plist,int doc_id)
	{

		string sfilter = " (),:.;\t\"\'!?-_\0\n[]=#{}";
		string line;
				int id;
				//	if (doc_id % ((files.size()+1)/100) == 0)
				//		printf("progress %f \n",(double)((double)(doc_id)/(double)files.size())*100);
					ifstream data (files[doc_id].c_str());
					if (data.is_open())
				  	{
				   	 while (data.good())
				   	 {
					   		vector <string> terms;
				   	   		getline (data,line);
				   	   		Tokenize(line,terms,sfilter);
				   	   		size_t i;
				   	   	    #pragma omp parallel for private(id)
	               	   	//	#pragma omp parallel shared ( plist )
   					   	   for (i = 0 ; i < terms.size() ;i++)
					   	   {
										
						  				#pragma omp critical
										{
											id = omp_get_thread_num();
						   	 //  			cout << "Thread = " << id <<" with term = " << terms[i] << endl;
							   	   		if(plist[terms[i]].size() == 0)
							   	   		{
							//   	  			cout << "creating a fresh new vector for : " << terms[i] << " in document = " << doc_id << endl;
						  	   	   			plist[terms[i]].push_back(Plist(1,doc_id));
						  	   	   		
							   	   		}
							   	   		else if (plist[terms[i]][plist[terms[i]].size()-1].doc_id != doc_id)
							   	   		{
							  // 	   			cout << "creating a new vector for : " << terms[i] << " in document = " << doc_id << endl;
							   	   			plist[terms[i]].push_back(Plist(1,doc_id));
							   	   		
							   	   		} 
							   	   		else
							   	   		{
							   	   			size_t pos = plist[terms[i]].size();
							   	   			plist[terms[i]][pos-1].frequency++;
							   	//   			cout << "Updating " << terms[i] << " frequency = " << plist[terms[i]][pos-1].frequency << " on doc =" << doc_id << endl;
							   	   		
							   	   		}
							   	   	}
					   	   	
					   	   		}
				   	 }
				    data.close();
					} else
				  	{
				  		cout << "error opening file" << endl;
				  	}
		return plist;
	}
	

void Sort(vector< Plist >::iterator start, vector< Plist >::iterator end)
{
    int numProcs = omp_get_num_procs();
    omp_set_num_threads(numProcs);
    #pragma omp parallel
    {
        #pragma omp single nowait
        Sort_h(start, end-1);
    }
}
 
void Sort_h(vector< Plist >::iterator start, vector< Plist >::iterator end)
{
    if(start < end)
    {        
            int q = Partition(start, end);
            #pragma omp task
            {
                Sort_h(start, start + (q - 1));
            }
            Sort_h(start + (q + 1), end);
    }
}
 
int Partition(vector< Plist >::iterator start, vector< Plist >::iterator end)
{
    int partitionIndex =  (end - start)/2;
    int privotValue = start[partitionIndex].frequency;
    Plist tmp = start[partitionIndex];
    start[partitionIndex] = *end;
    *end = tmp;
    int swapIndex = 0;
    for (int i = 0; i < end - start; i++)
    {
        if(start[i].frequency >= privotValue)
        {
            tmp = start[swapIndex];
            start[swapIndex] = start[i];
            start[i] = tmp;
            swapIndex++;
        }
    }
    tmp = start[swapIndex];
    start[swapIndex] = *end;
    *end = tmp;
    return swapIndex;
}

void sort_and_write(map<string,vector <Plist> > &plist)
	{
		cout << "Sorting..." << endl;
		#pragma parallel for 
		map<string,vector <Plist> >::iterator it;
		for ( it=plist.begin() ; it != plist.end(); it++ )
		{
			Sort((*it).second.begin(),(*it).second.end());
			/*for (size_t i = 0 ;i < (*it).second.size();i++ )
			{
				for (size_t j = 0 ;j < (*it).second.size();j++ )
				{
					if ((*it).second[i].frequency > (*it).second[j].frequency )
					{
						Plist aux = (*it).second[i];
						(*it).second[i] = (*it).second[j];
						(*it).second[j] = aux;
					}
				}
			}*/
		}
		

		stringstream words,invlist,invlistfreq;
		words << this->prefix << ".words";
		invlist << this->prefix << ".invlist";
		invlistfreq << this->prefix << ".invlistfreq";
		ofstream words_file (words.str().c_str());
		ofstream invlist_file (invlist.str().c_str());
		ofstream invlistfreq_file (invlistfreq.str().c_str());
		cout << "Storing Data..." << endl;
		for ( it=plist.begin() ; it != plist.end(); it++ )
		{
			words_file << (*it).first << endl;
			invlist_file << (*it).second.size();
			invlistfreq_file << (*it).second.size();
			for (size_t i = 0 ;i < (*it).second.size();i++ )
			{
				Plist aux = (*it).second[i];
				invlist_file << " " <<  aux.doc_id;
				invlistfreq_file <<  " " << aux.frequency;
				//cout << "term = " << (*it).first << " doc_id = " << aux.doc_id << " frequency = " << aux.frequency << endl;
			}
		invlist_file << endl;
		invlistfreq_file << endl;
		}

		words_file.close();
		invlist_file.close();
		invlistfreq_file.close();
		}


};

int main(int argc, char** argv)
{
	omp_set_num_threads(3);
	const char* invlist = argv[1];
	const char* prefix = argv[2];
	InvList *inv = new InvList(string(invlist),string(prefix));

}
