
int n =  15;
int k = 1;
uint A[15] = {100,80,78,50,45,34,23,12,11,8,4,3,2,1,0};

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

getTermPosition(const char *t)
CompressedPsums ** ps = new CompressedPsums*[terms];
for (int i = 0 ; i < terms;i++)
{
		now = this->st->select1(i+1);
		next = this->st->select1(i+2);
		if (i == this->size_terms - 1) // fix this
			next = now+1;
		uint f = 0;
		uint *A;
		for (uint j = now;j<next;j++)
		{
			A = new uint[next-now+1];
			A[f] = this->freqs[j];
			f++;
			delete []A;
		}	
		ps[i] = new CompressedPsums(A,f,this->k,encodeGamma,decodeGamma);	
}

uint getFreq(t,position)
{
	return ps[this->getTermPosition(t)]->decode(position);
}

getPrefix(term,p)
{
	// en funcion de p.
}