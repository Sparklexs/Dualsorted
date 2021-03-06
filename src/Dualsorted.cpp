#include "Dualsorted.h"

using namespace cds_static;


inline uint search(const char** a,const char* s,uint n)
{
	uint l = 0;
	uint r = n-1;
	
	while (l <= r)
	{
		uint m = (l+r)/2;	
		if (m >= 0 && m < n)
		{
			if (strcmp(s,a[m]) == 0)
				return m;
			else if (strcmp(s,a[m]) > 0)
				l = m+1;
			else 
				r = m-1;		
		}
		else
			return -1;
	}
	return -1;
}


Dualsorted::Dualsorted(vector<string> vocab, vector< vector<int> > &result, vector<int> &freqs,uint size_terms,size_t *doclens,size_t ndocuments)
{
   // cout << "Amount of terms: " << size_terms << endl;


    for (uint i =0 ; i < vocab.size();i++)
    {
    	this->terms[vocab[i]] = i;
    }
 //   cout << "searching" << endl;
  //  cout << search(this->terms,"wikipedia",size_terms) << endl;
    
    this->doclens = doclens;
    this->ndocuments = ndocuments;   

	this->result = result;
	this->freqs = freqs;
	this->size_terms = size_terms;

	this->k = 10;
	

//	cout << "Building St...";
	this->buildSt();
//	cout << "Done!" << endl;
//	cout << "Building PartialSums...";
	this->buildSums();
//	cout << "Done!" << endl;
//	cout << "Building L..."; 
	this->buildL();	
//	cout << "Done!" << endl;
	
	this->result.clear();
	this->freqs.clear();

}

// Fix this
size_t Dualsorted::getSize()
{
	size_t size = 0;
	size_t size_ps = 0;
	
    double sparse_tt = 0;
    double sparse_rt = 0;
    double len_tt = 0;
    double len_rt = 0;
    
//	cout << "L size: " << this->L->getSize() << endl;
//	cout << "st size: " << this->st->getSize() << endl;
	size += this->L->getSize();
	size += this->st->getSize();


	for (uint i = 0 ; i < this->size_terms-1;i++)
	{
		size_ps += this->ps[i]->getSize();
	}
//	cout << "partial_sums: " << size_ps/8 << endl;
	size += size_ps/8;
//	cout << "Total size: " << size << endl;
//	cout << "Total size (MB): " << size/(1024*1024) << endl;
	return size;
}

vector <uint> Dualsorted::getRange(string term,uint i)
{
	uint f = this->getTermPosition(term.c_str());
	cout << "f = " << f << endl;
	uint end,start;
	(f != this->size_terms-1) ? end = this->st->select1(f+2)-1: end = this->L_size-1;
	start = this->st->select1(f+1);
	//if (end-start < 2)
	//	return vector<uint>();

	if (start + i < end+1)
	{
		cout << "start = " << start << endl;
		cout << "end = " << start+i << endl;
		cout << "entre 1 !!!" << endl;
		return this->L->range_report_aux(start-1,start+i-1);
	}
	else
	{
		cout << "start = " << start << endl;
		cout << "end = " << end << endl;
		cout << "entre 2 !!!" << endl;
		return this->L->range_report_aux(start,end);
	}
}
/*
uint Dualsorted::getDocid(string term,uint i)
{
	uint f = this->getTermPosition(term.c_str());
	uint end,start;
	(f != this->size_terms-1) ? end = this->st->select1(f+2)-1 : end = this->L_size-1;		                                             
	start = this->st->select1(f+1);
	return this->L->range(start,end,i);
}
*/
uint Dualsorted::getPosTerm(string t,uint d)
{  
    uint f = this->getTermPosition(t.c_str());
	if(f==0) return 0; // FIXME I'm paja
    uint start = this->st->select1(f+1);
    return this->L->select(d,1+this->L->rank(d,start-1)) - start;
}
uint Dualsorted::getFreq(const char* term,int i)
{
	uint j = this->getTermPosition(term);
	if (j == 0)
	{
		return 0;
	}
//	cout << "term = " << term << endl;
//	cout << "i = " << i << endl;
	if (j != -1 || j != 0)
	{
		if (j > 0) 
		return this->ps[j-1]->decode(i);
	}
	else
	{
		return 0;
	}
	return 0;
}

uint Dualsorted::getTermPosition(const char *t)
{
	//return search(this->terms,t,this->size_terms);
	//cout << "received = " << t << endl;
	string a(t);
	//cout << "returing = " << this->terms[a] << endl;
	return this->terms[a];
}

vector <uint> Dualsorted::range(string term, size_t x, size_t y)
{
	//cout << "searching for: " << term << endl;
	uint f = this->getTermPosition(term.c_str());
    //cout << "Executing select" << endl;
   // cout << "f = " << f << endl;
    if (f == -1 || f == 0 || f > 4294967290)
    {
    	vector <uint> blank;
    	return blank;
    }
	uint start = this->st->select1(f);	
	return this->L->range_report_aux(start+x,start+y);
}

inline void Dualsorted::intersect(string *terms,uint qsizes)
{
	//cout << "BLABLA" << endl;
	size_t *start = new size_t[qsizes];
	size_t *end = new size_t[qsizes];
	for (uint i  = 0 ; i < qsizes;i++)
	{
		uint f = this->getTermPosition(terms[i].c_str());
		if (f == 0 )
		return;
//		cout << "term = " << terms[i] << endl;
//		cout << "pos = " << f << endl;
		
		start[i] = this->st->select1(f+1);
		if (f != this->size_terms-1)
			end[i] = this->st->select1(f+2)-1;
		else
			end[i] = this->L_size-1;		

//		cout << "start[" << i << "] = " << start[i] << endl;
//		cout << "end[" << i << "] = " << end[i] << endl;
	}
//	cout << "start-end = " << end[0] - start[0] << endl;
	//size_t x_start,size_t x_end,size_t y_start, size_t y_end)
	//this->L->range_report_aux(start[0],end[0]);
	//this->L->range_intersect_aux(start[0],end[0],start[1],end[1]);
	this->L->n_range_intersect_aux(start,end,qsizes);
}
/*vector < pair<uint,size_t> > Dualsorted::mrqq(string term, size_t k, size_t kp)
{
	uint f = this->getTermPosition(term.c_str());
	uint end,start;
	(f != this->size_terms-1) ? end = this->st->select1(f+2)-1 : end = this->L_size-1;		                                             
	start = this->st->select1(f+1);
	return this->L->range_report_aux(start,end);
	return NULL
}*/

uint Dualsorted::getPostingSize(string term)
{
	uint f = this->getTermPosition(term.c_str());
	if (f == -1)
		return 1;
	uint end,start;
	(f != this->size_terms-1) ? end = this->st->select1(f+2)-1 : end = this->L_size-1;		                                             
	start = this->st->select1(f+1);
	return end-start+1;
}


BitSequence *Dualsorted::buildSt()
{
	uint m2a = 0;
	for (int i = 0 ;i<this->size_terms;i++)
	{	
		m2a = m2a + result[i].size();		
	}
	this->L_size = m2a;
	BitString *bs = new BitString(m2a);	
	m2a = 0;
	for (int i = 0 ;i<this->size_terms;i++)
	{	
		bs->setBit(m2a);
		m2a = m2a + result[i].size();		
	}

	BitSequenceRG *bsrg = new BitSequenceRG(*bs,2);
	this->st = bsrg;

	return this->st;
}
void Dualsorted::buildSums()
{
	this->ps = new CompressedPsums*[this->size_terms];
	uint now,next;
	for (int i = 0 ; i < this->size_terms-1;i++)
	{
		//	cout << "entering 2" << endl;
			now = this->st->select1(i+1);
			next = this->st->select1(i+2);
		//	cout << "i=" << i <<  " term = " << this->terms[i] << endl;
			if (next > 4294967290)
				break;
		//	cout << "now = " << now << " next = " << next << endl;
			uint f = 0;
			uint *A;
			A = new uint[next-now+1];
			for (uint j = now;j<next;j++)
			{
				
				A[f] = this->freqs[j];
		//		cout << "A[" << f << "] = " << A[f] << endl;
				f++;
				
			}	
	//		cout << "entering 3" << endl;
			//cout << "i=" << i << endl;
			ps[i] = new CompressedPsums(A,f,10,encodeGamma,decodeGamma);
			ps[i]->encode();
	}
//	cout << endl << "decodificando:" << ps[869]->decode(0) << endl;
}


Sequence * Dualsorted::buildL()
{
	uint *sequence = new uint[this->L_size];
	uint m2a = 0;
	uint m2a2 = 0;
	for (int i = 0 ; i < result.size();i++)
	{
		m2a2 += result[i].size();
		for (int j = 0 ; j < result[i].size();j++)
		{
				sequence[m2a] = result[i][j];
				m2a++;
		}
	}
	Array *A = new Array(sequence,this->L_size);
	MapperNone * map = new MapperNone();
    
//   	BitSequenceBuilder * bsb = new BitSequenceBuilderRRR(50);
   	BitSequenceBuilder * bsb = new BitSequenceBuilderRG(2);
   	WaveletMatrix* seq = new WaveletMatrix(*A, bsb, map);		

	this->L = seq;
    return this->L;

}

void Dualsorted::test()
{
/*	cout << "testing st.... " << endl;
	for (uint i = 0 ; i < this->L_size;i++)
		cout << this->st->access(i) << endl;
	
	// Testing Tt
	uint now = 0;
	uint next = 0;
	cout << "testing Tt.... " << endl;

	for (uint i = 0 ; i < this->size_terms;i++)
	{
		cout << this->terms[i] << endl;
		for (int j = 0 ;j<this->Tt[i]->getLength();j++)
		{
			cout << this->Tt[i]->access(j) << endl;
		}
	}
	cout << "testing Rt.... " << endl;
	for (uint i = 0 ; i < this->size_terms;i++)
	{
		cout << this->terms[i] << endl;
	
		for (int j = 0 ;j<this->Rt[i]->getLength();j++)
		{
			cout << this->Rt[i]->access(j) << endl;
		}
	}
	cout << "testing vt.... " << endl;
	for (int i = 0 ; i < this->size_terms;i++)
	{
		cout << this->vt->access(i) << endl;
	}
	cout << "testing L " << endl;
	for (int i = 0 ; i < L->getLength();i++)
	{
		cerr << this->L->access(i) << " ";
	}
	cout << "End testing... " << endl;*/
}
