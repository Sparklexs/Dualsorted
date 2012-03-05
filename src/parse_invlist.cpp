#include <iostream>
#include <fstream>
#include "utils.cpp"
#include <string>
#include <map>

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
		this->process(getData(f));
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
	   	   cout << "adding files:" << line << endl;
	   	   files.push_back(line);
	   	 }
	   	 data.close();
	  	}
	  	else cout << "Unable to open file"; 
	  	return files;
	}

	void process(vector <string> files)
	{

		string sfilter = " (),:.;\t\"\'!?-_\0\n[]=#{}";
		string line;
		vector <string> terms;
		for (size_t doc_id = 0; doc_id < files.size()-1;doc_id++)
		{
			if (doc_id % (files.size()/100) == 0)
				cout << "progress:" << (doc_id/files.size())*100 << endl;
			ifstream data (files[doc_id].c_str());
			if (data.is_open())
		  	{
		   	 while ( data.good() )
		   	 {
		   	   getline (data,line);
		  
		   	   Tokenize(line,terms,sfilter);
		   	   for (size_t i = 0 ; i < terms.size() ;i++)
		   	   {
		   	   		if(this->plists[terms[i]].size() == 0)
		   	   		{
	  	   	   			this->plists[terms[i]].push_back(Plist(1,doc_id));
		   	   		}
		   	   		else if (this->plists[terms[i]][this->plists[terms[i]].size()-1].doc_id != doc_id)
		   	   		{
		   	   			this->plists[terms[i]].push_back(Plist(1,doc_id));
		   	   		} 
		   	   		else
		   	   		{
		   	   			size_t pos = this->plists[terms[i]].size();
		   	   			this->plists[terms[i]][pos-1].frequency++;
		   	   		}
		   	   }

		   	 }
		   	 data.close();
		  	} else
		  	{
		  		cout << "error opening file" << endl;
		  	}
		}

		cout << "Sorting..." << endl;
		map<string,vector <Plist> >::iterator it;
		for ( it=plists.begin() ; it != plists.end(); it++ )
		{
			for (size_t i = 0 ;i < (*it).second.size()-1;i++ )
			{
				for (size_t j = 0 ;j < (*it).second.size();j++ )
				{
					if ((*it).second[i].frequency < (*it).second[j].frequency )
					{
						Plist aux = (*it).second[i];
						(*it).second[i] = (*it).second[j];
						(*it).second[j] = aux;
					}
				}
			}
		}

		stringstream words,invlist,invlistfreq;
		words << this->prefix << ".words";
		invlist << this->prefix << ".invlist";
		invlistfreq << this->prefix << ".invlistfreq";
		ofstream words_file (words.str().c_str());
		ofstream invlist_file (invlist.str().c_str());
		ofstream invlistfreq_file (invlistfreq.str().c_str());
		cout << "Storing Data..." << endl;
		for ( it=plists.begin() ; it != plists.end(); it++ )
		{
			words_file << (*it).first << endl;
			invlist_file << (*it).second.size();
			invlistfreq_file << (*it).second.size();
			for (size_t i = 0 ;i < (*it).second.size();i++ )
			{
				Plist aux = (*it).second[i];
				invlist_file << " " <<  aux.doc_id;
				invlistfreq_file <<  " " << aux.frequency;
		//		cout << "term = " << (*it).first << " doc_id = " << aux.doc_id << " frequency = " << aux.frequency << endl;
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
	const char* invlist = argv[1];
	const char* prefix = argv[2];
	InvList *inv = new InvList(string(invlist),string(prefix));

}
