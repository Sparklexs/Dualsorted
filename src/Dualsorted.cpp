#include "Dualsorted.h"


using namespace cds_static;


Dualsorted::Dualsorted(vector<string> &terms, vector< vector<int> > &result, vector<int> &freqs,uint size_terms, uint size_freq)
{
    cout << "Amount of terms: " << size_terms << endl;
    for (uint i = 0 ; i < size_terms; i++)
    {
        this->terms[terms[i].c_str()] = i;
    }
	this->result = result;
	this->freqs = freqs;
	this->size_terms = size_terms;
	this->size_freq = size_freq;

	cout << "Building St..." << endl;
	this->buildSt();
	
	cout << "Building Tt..." << endl;
	this->buildTt();
	
	cout << "Building Rt..." << endl;
	this->buildRt();

	cout << "Building Vt..." << endl;
	this->buildVt();	
	
	cout << "Building L" << endl;
	this->buildL();	
	
	this->result.clear();
	this->freqs.clear();

}

// Fix this
size_t Dualsorted::getSize()
{
	size_t size = 0;
	size_t size_tt = 0;
	size_t size_rt = 0;
	
    double sparse_tt = 0;
    double sparse_rt = 0;
    double len_tt = 0;
    double len_rt = 0;
    
	cout << "L size: " << this->L->getSize() << endl;
	cout << "st size: " << this->st->getSize() << endl;
	cout << "vt size: " << this->vt->getSize() << endl;

	size += this->L->getSize();
	size += this->st->getSize();
	size += this->vt->getSize();

	for (uint i = 0 ; i < this->size_terms;i++)
	{
		size_tt += this->Tt[i]->getSize();
		sparse_tt += this->Tt[i]->countOnes();
		len_tt += this->Tt[i]->getLength();
		
		size_rt += this->Rt[i]->getSize();
		sparse_rt += this->Rt[i]->countOnes();
		len_rt += this->Rt[i]->getLength();
	}
	double tt_sp = (sparse_tt/len_tt)*100;
	double rt_sp = (sparse_rt/len_rt)*100;

    cout << "tt sparse: " << tt_sp << " % | " << sparse_tt << " / " << len_tt <<  endl;
    cout << "rt sparse: " << rt_sp << " % | " << sparse_rt << " / " << len_rt <<  endl;
	cout << "tt size: " << size_tt << endl;
	cout << "rt size: " << size_rt << endl;

	size += size_tt;
	size += size_rt;
	cout << "Total size: " << size << endl;
	return size;
}
// two options: 
// 1.Hashmap for this->Terms? 
// 2.Leave it this way?

Sequence * Dualsorted::buildL()
{
	uint *sequence = new uint[this->L_size];
	uint m2a = 0;
	for (int i = 0 ; i < this->size_terms;i++)
	{
		for (int j = 0 ; j < result[i].size();j++)
		{
			sequence[m2a] = result[i][j];
			m2a++;
		}
	}
	Array *A = new Array(sequence,this->L_size);
	MapperNone * map = new MapperNone();
    
   	//BitSequenceBuilder * bsb = new BitSequenceBuilderRRR(128);
   	BitSequenceBuilder * bsb = new BitSequenceBuilderRG(30);
   	WaveletTreeNoptrs* seq = new WaveletTreeNoptrs(*A, bsb, map);		

	this->L = seq;
        return this->L;

}


uint Dualsorted::getDocid(string term,uint i)
{
	uint f = this->getTermPosition(term.c_str());
	uint end,start;
	(f != this->terms.size()-1) ? end = this->st->select1(f+2)-1 : end = this->L_size-1;		                                             
	start = this->st->select1(f+1);
	return this->L->range(start,end,i);
}

uint Dualsorted::getPosTerm(string t,uint d)
{  
    uint f = this->getTermPosition(t.c_str());
	if(f==0) return 0; // FIXME I'm paja
    uint start = this->st->select1(f+1);
    return this->L->select(d,1+this->L->rank(d,start-1)) - start;
}
uint Dualsorted::getFreq(string term,int i)
{
	uint f = this->getTermPosition(term.c_str());
	return (this->Tt[f]->select1(this->vt->access(f) - this->Rt[f]->rank1(i)+1))+1;
}

uint Dualsorted::getTermPosition(const char *t)
{
	return this->terms[t];
}

vector <uint> Dualsorted::range(string term, size_t x, size_t y)
{
	uint f = this->getTermPosition(term.c_str());
	uint start = this->st->select1(f+1);	
	return this->L->range_report_aux(start+x,start+y);
}

void Dualsorted::intersect(string term, string term2)
{
	uint f = this->getTermPosition(term.c_str());
	uint f2 = this->getTermPosition(term2.c_str());
	
	uint end1 = -1;
	uint end2 = -1;

	uint start1 = this->st->select1(f+1);
	uint start2 = this->st->select1(f2+1);
	
	if (f != this->terms.size()-1)
		end1 = this->st->select1(f+2)-1;
	else
		 end1 = this->L_size-1;		

	if (f2 != this->terms.size()-1)
		end2 = this->st->select1(f2+2)-1;
	else
		 end2 = this->L_size-1;		
	
	this->L->range_intersect_aux(start1,end1,start2,end2);
}
vector < pair<uint,size_t> > Dualsorted::mrqq(string term, size_t k, size_t kp)
{
	uint f = this->getTermPosition(term.c_str());
	uint end,start;
	(f != this->terms.size()-1) ? end = this->st->select1(f+2)-1 : end = this->L_size-1;		                                             
	start = this->st->select1(f+1);
	return this->L->mrqq(start,end,k,kp);
}
void Dualsorted::test()
{
//	cout << "testing st.... " << endl;
//	for (uint i = 0 ; i < this->L_size;i++)
//		cout << this->st->access(i) << endl;
	/*
	// Testing Tt
	uint now = 0;
	uint next = 0;
//	cout << "testing Tt.... " << endl;

	for (uint i = 0 ; i < this->size_terms;i++)
	{
	//	cout << this->terms[i] << endl;
		for (int j = 0 ;j<this->Tt[i]->getLength();j++)
		{
//			cout << this->Tt[i]->access(j) << endl;
		}
	}
//	cout << "testing Rt.... " << endl;
	for (uint i = 0 ; i < this->size_terms;i++)
	{
	//	cout << this->terms[i] << endl;
	
		for (int j = 0 ;j<this->Rt[i]->getLength();j++)
		{
//			cout << this->Rt[i]->access(j) << endl;
		}
	}
//	cout << "testing vt.... " << endl;
	for (int i = 0 ; i < this->size_terms;i++)
	{
//		cout << this->vt->access(i) << endl;
	}
	cout << "testing L " << endl;
	for (int i = 0 ; i < L->getLength();i++)
	{
		cerr << this->L->access(i) << " ";
	}
//	cout << "End testing... " << endl;
*/

}


BitSequence *Dualsorted::buildSt()
{
	int m2a = 0;
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

	BitSequenceRG *bsrg = new BitSequenceRG(*bs,20);
	this->st = bsrg;

	return this->st;
}

BitSequenceRRR **Dualsorted::buildTt()
{

	this->Tt =  new BitSequenceRRR*[this->size_terms];
	int count = 0;
	uint now = 0;
	uint next = 0;
	int max = -1;
	for (uint i = 0 ; i < this->size_terms;i++)
	{
		max = -1;
		now = this->st->select1(i+1);
		next = this->st->select1(i+2);
		if (i == this->size_terms - 1)
		next = now+1;	
		for (uint j = now ;j<next;j++)
		{
			if (max < (int)this->freqs[j])
				max = (int)this->freqs[j];
		}
		BitString *Tt_aux = new BitString(max);
		for (uint j = now;j<next;j++)
		{
			Tt_aux->setBit(this->freqs[j]-1);
		}
		BitSequenceRRR *btrg = new BitSequenceRRR(*Tt_aux);
		this->Tt[count] = btrg;
		count++;

	}
	return this->Tt;
}

Sequence *Dualsorted::buildVt()
{
	uint *sequence = new uint[this->size_terms];
	
	for (uint i = 0 ; i < this->size_terms;i++)
	{	
		int aux =0;
		for (size_t j = 0 ; j < this->Tt[i]->getLength();j++)
		{	
			if (this->Tt[i]->access(j))
				aux++;
		}
		sequence[i] = aux;
	}
	Array *A = new Array(sequence,this->size_terms);
	MapperNone * map = new MapperNone();
    	BitSequenceBuilder * bsb = new BitSequenceBuilderRG(20);
	Sequence * seq = new WaveletTreeNoptrs(*A, bsb, map);		
	this->vt = seq;
	return this->vt;
}

BitSequence **Dualsorted::buildRt()
{
	this->Rt =  new BitSequence*[this->size_terms];
	uint count = 0;
	uint countf = 1;
	uint now = 0;
	uint next = 0;
	for (int i = 0 ; i < this->size_terms;i++)
	{	
		int next = 0;
		now = this->st->select1(i+1);
		next = this->st->select1(i+2);
		if (i == this->size_terms - 1)
		next = now+2;		
		BitString *bs = new BitString((size_t)(next-now));
		bs->setBit(0);
		countf = 0;
		for (int j = now;j<next;j++)
		{
			countf++;
			if (this->freqs[j] != this->freqs[j+1])
					bs->setBit(countf);
		}
		this->Rt[count] = new BitSequenceRG(*bs,1000);
		count++;
	}
	return this->Rt;

}

