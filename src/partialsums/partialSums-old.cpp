#include "delta.c"
#include <iostream>
#include <cmath>

using namespace std;

int msb(uint a)
{
    while (a & 1)
    {
        a = a>>1;
    }

}
int main()
{
	int N = 15;
	int K = 2; // top k

	int A[15]  = {100,80,78,50,45,34,23,12,11,8,4,3,2,1,0};

	int NUM_BLOCKS = ceil(log2(ceil(N/K)));
	int *indices = new int[NUM_BLOCKS];
	cout << "NUM BLOCKS = " << NUM_BLOCKS << endl;

	int i = 0;
	int new_i = 0 ;
	while (new_i < N)
	{
		
		cout << "new_i = " << new_i << endl;
		indices[i] = A[new_i];
		i++;
		new_i += pow(2,i)*K;
	}
	for (i = 0 ; i < NUM_BLOCKS;i++)
	{
		cout << "i=" << i << " -> " << indices[i] << endl;
	}
	int B[N];
	int j = 0;
	int t = K;
	for (i = 0 ; i < N-1;i++)
	{
		int delta = A[i] - A[i+1];
		B[i] = delta;
		cout << "t = " << t << endl;
		if (i % t == 0)
		{
			j++;
			t += pow(2,j)*K;
			cout << A[i] << endl;
		}
	}
    uint *c = new uint[10];
    uint pos = 0;
	for (i = 0 ; i < N-1;i++)
	{
		cout << B[i] << endl;
        pos += encodeGamma(c,pos,B[i]);
    }

   
    cout << "pos = " << pos << endl;
    uint posj = 0;
    cout << "- ---- -- - - " << endl;

    while (posj < pos)
    {
        uint *d = new uint[1];
        posj += decodeGamma(c,posj,d);
        cout << d[0] << endl;
    }

}
