#include "delta.c"
//#include "rice.h"
#include <iostream>
#include <cmath>

using namespace std;

const int NUM_SIZE = 32;
// auxiliary functions
int msb(uint v)
{
    int count=0;
    while(v>>=1 & 1)
    {
        count++;
    }
    return count;
}



typedef uint (*encodef)(uint* output, uint pos, uint value);
typedef uint (*decodef)(uint* input, uint pos, uint* value);

class Psums
{
	public:
	uint real;
	int pos;

	Psums()
	{
		this->real = 0;
		this->pos = 0;
	}	
	void setPos(int pos)
	{
		this->pos = pos;
	}
	void setReal(uint real)
	{
		this->real = real;
	}

};

class CompressedPsums
{
	private:
	Psums **s;
	uint *a;
	uint n;
	uint new_n;
	uint k;
	uint *d;
	uint size;
	uint *duplicate;
	uint *new_a;
	encodef enc;
	decodef dec;

	public:
	CompressedPsums(uint *a,uint n, uint k,encodef enc,decodef dec)
	{
		this->n = n;
		this->k = 3;
		this->a = a;
		this->enc = enc;
		this->dec = dec;
		this->duplicate = new uint[n];
		this->new_a = new uint[n];

		uint repetitions = 0;
		for (uint i = 0 ; i < n; i++)
			this->duplicate[i] = 0;

		for (uint i = 1 ; i < n;i++)
		{
			if (a[i-1] == a[i])
			{
				repetitions++;
				this->duplicate[i] = repetitions;
				cout << "duplicate found: "<< i << " =  " << a[i-1] << endl;
			}
			else
			{
				this->duplicate[i] = repetitions;
				new_a[i-repetitions-1] = a[i-1];
			}

		}
		for (uint i = 0 ; i < n; i++)
			cout << "duplicate[" << i << "] = " << duplicate[i] << endl;

		new_a[n-repetitions-1] = a[n-1];
		
		new_n = n - repetitions;
		for (uint j = 0 ; j < new_n ; j++)
		{
			cout << "j = " << j << " = " << new_a[j] << endl;
		}
		uint NUM_BLOCKS = (new_n/this->k)+1;
		this->s = new Psums*[NUM_BLOCKS];

		for (int i = 0 ; i < NUM_BLOCKS;i++)
		{
			this->s[i] = new Psums();
		}
		uint new_i = 0;
		int i = 0;
		uint old = 0;
		while (new_i < new_n)
		{
			cout << " new_i = " << new_i << endl;
			cout << "s[" << i << "]  = "  << new_a[new_i] << endl;
			s[i]->real = new_a[new_i];
			i++;	
			new_i = i*this->k;
			
		}
		cout << "Done" << endl;
	}

	uint * encode()
	{
		uint *b = new uint[this->new_n-1];
		for (int i = 0;i<new_n-1;i++)
		{
			int delta = this->new_a[i] - this->new_a[i+1];
			b[i] = delta;
			cout << "delta [ " << i << "] " << b[i] << endl;
		}
		uint *c = new uint[2];
		uint encode_length = 0;
		uint old = 0;
		uint duplicates = 0;
		uint total = 0;
		for (int i = 0 ; i < new_n-1 ; i++)
			encode_length += this->enc(c,0,b[i]);

		this->size = encode_length;
		uint encode_n = (encode_length/NUM_SIZE)+1;
		delete [] c;
		this->d = new uint[encode_n];
		uint pos = 0;
		uint j = 1;

		this->s[0]->pos = 0;
		
		for (int i = 1;i<new_n;i++)
		{
			int modulo = j*this->k;
			cout << "modulo = " << modulo << endl;
			if (modulo !=0 && i % modulo == 0 )
			{
				this->s[j]->pos = pos;
				cout << "entre!!!!!!!!" << endl;
				cout << "j = " << j << endl;
				cout << "s = " << s[j]->pos << endl;
				j++;
			}
			else
			{
				pos += this->enc(this->d,pos,b[i-1]);	
				cout << "codificando: " << b[i-1] << endl;
				cout << "pos = " << pos << endl;
			}		
			
			
			
		}
		return d;
	}

	uint decode (uint pos)
	{

		cout << " ------- " << endl;
		cout << "pos recibido = " << pos << endl;
		int new_pos;

		if (this->duplicate[pos] != 0)
		{
			pos = pos-this->duplicate[pos];
			cout << " pos - duplicate = " << pos << endl;
		}

		if (pos == 0)
		{
			new_pos = 0;
		}
		else
		{
			cout << "entre con pos = " << pos << endl;
			cout << "this->k = " << this->k << endl;
			new_pos = (pos/this->k);
		}
		cout << "new_pos (pos/k) = " << new_pos << endl;
		uint real = this->s[new_pos]->real;
		cout << "numero real = " << real << endl;
		uint real_pos = this->s[new_pos]->pos;
		int potencia = (pos/this->k);
	
		if (pos == 0)
		{
			return real;
		}
		else
		{
			new_pos = pos-potencia*this->k;
		}
		cout << "new_new_pos = " << new_pos << endl;
		if (new_pos == 0)
		{
			return real;
		}
		int i = 0; 
		uint *r = new uint[1];
		while(i < new_pos)
		{
			real_pos += this->dec(this->d,real_pos,r);
			cout << "r[0]" << r[0] << endl;
			i++;
			real -= r[0];
		}
		return real;
	}

	uint getSize()
	{
		return this->size;
	}

};

int main()
{
	int n =  15;
	int k = 1;
	uint A[15] = {100,81,80,45,45,45,23,12,11,8,8,8,8,1,1};

	CompressedPsums * ps = new CompressedPsums(A,n,k,encodeGamma,decodeGamma);
	ps->encode();
	cout << "size = " << ps->getSize();
	for (int i = 0 ;i < 15;i++)
	{
		cout << ps->decode(i) << endl;
	}
}


