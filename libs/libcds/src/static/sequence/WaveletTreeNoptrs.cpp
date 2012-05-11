/* WaveletTreeNoptrs.cpp
 * Copyright (C) 2008, Francisco Claude, all rights reserved.
 *
 * WaveletTreeNoptrs definition
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include <WaveletTreeNoptrs.h>

namespace cds_static
{
    WaveletTreeNoptrs::WaveletTreeNoptrs(Array & a, BitSequenceBuilder * bmb, Mapper * am) : Sequence(0) {
	bmb->use();
	this->n=a.getLength();
	this->length = n;
	this->am=am;
	am->use();
	uint * symbols = new uint[n];
	for(size_t i=0;i<n;i++)
	    symbols[i] = am->map(a[i]);
	max_v=am->map(a.getMax());
	height=bits(max_v);
	uint *occurrences=new uint[max_v+1];
	for(uint i=0;i<=max_v;i++) occurrences[i]=0;
	for(uint i=0;i<n;i++)
	    occurrences[symbols[i]]++;
	uint to_add=0;
		for(uint i=0;i<max_v;i++)
		    if(occurrences[i]==0) to_add++;
		uint * new_symb = new uint[n+to_add];
		for(uint i=0;i<n;i++)
		    new_symb[i] = symbols[i];

		delete [] symbols;

		to_add = 0;
		for(uint i=0;i<max_v;i++)
		if(occurrences[i]==0) {
		    occurrences[i]++;
		    new_symb[n+to_add]=i;
		    to_add++;
		}
		uint new_n = n+to_add;
		for(uint i=1;i<=max_v;i++)
		    occurrences[i] += occurrences[i-1];
		uint *oc = new uint[(new_n+1)/W+1];
		for(uint i=0;i<(new_n+1)/W+1;i++)
		    oc[i] = 0;
		for(uint i=0;i<=max_v;i++)
		    bitset(oc,occurrences[i]-1);
		bitset(oc,new_n);
		occ = bmb->build(oc,new_n+1);
		delete [] occurrences;
		this->n = new_n;
		uint ** _bm=new uint*[height];
		for(uint i=0;i<height;i++) {
		    _bm[i] = new uint[new_n/W+1];
		    for(uint j=0;j<new_n/W+1;j++)
			_bm[i][j]=0;
		}
		build_level(_bm,new_symb,0,new_n,0);
		bitstring = new BitSequence*[height];
		for(uint i=0;i<height;i++) {
		    bitstring[i] = bmb->build(_bm[i],new_n);
		    delete [] _bm[i];
		}
		delete [] _bm;

		// delete [] new_symb; // already deleted in build_level()!
		delete [] oc;
		bmb->unuse();
	    }

	    WaveletTreeNoptrs::WaveletTreeNoptrs(uint * symbols, size_t n, BitSequenceBuilder * bmb, Mapper * am, bool deleteSymbols) : Sequence(n) {
		bmb->use();
		this->n=n;
		this->am=am;
		am->use();
		for(uint i=0;i<n;i++)
		    symbols[i] = am->map(symbols[i]);
		max_v=max_value(symbols,n);
		height=bits(max_v);
		uint *occurrences=new uint[max_v+1];
		for(uint i=0;i<=max_v;i++) occurrences[i]=0;
		for(uint i=0;i<n;i++)
		    occurrences[symbols[i]]++;
		uint to_add=0;
		for(uint i=0;i<max_v;i++)
		    if(occurrences[i]==0) to_add++;
		uint * new_symb = new uint[n+to_add];
		for(uint i=0;i<n;i++)
		    new_symb[i] = symbols[i];

		if (deleteSymbols) {
		    delete [] symbols;
		    symbols = 0;
		}

		to_add = 0;
		for(uint i=0;i<max_v;i++)
		if(occurrences[i]==0) {
		    occurrences[i]++;
		    new_symb[n+to_add]=i;
		    to_add++;
		}
		uint new_n = n+to_add;
		for(uint i=1;i<=max_v;i++)
		    occurrences[i] += occurrences[i-1];
		uint *oc = new uint[(new_n+1)/W+1];
		for(uint i=0;i<(new_n+1)/W+1;i++)
		    oc[i] = 0;
		for(uint i=0;i<=max_v;i++)
		    bitset(oc,occurrences[i]-1);
		bitset(oc,new_n);
		occ = bmb->build(oc,new_n+1);
		delete [] occurrences;
		this->n = new_n;
		uint ** _bm=new uint*[height];
		for(uint i=0;i<height;i++) {
		    _bm[i] = new uint[new_n/W+1];
		    for(uint j=0;j<new_n/W+1;j++)
			_bm[i][j]=0;
		}
		build_level(_bm,new_symb,0,new_n,0);
		bitstring = new BitSequence*[height];
		for(uint i=0;i<height;i++) {
		    bitstring[i] = bmb->build(_bm[i],new_n);
		    delete [] _bm[i];
		}
		delete [] _bm;

		if (!deleteSymbols)
		    for(uint i=0;i<n;i++)
			symbols[i] = am->unmap(symbols[i]);

		// delete [] new_symb; // already deleted in build_level()!
		delete [] oc;
		bmb->unuse();
	    }

	    // symbols is an array of elements of "width" bits
	    WaveletTreeNoptrs::WaveletTreeNoptrs(uint * symbols, size_t n, uint width, BitSequenceBuilder * bmb, Mapper * am, bool deleteSymbols) : Sequence(n) {
		bmb->use();
		this->n=n;
		this->am=am;
		am->use();
		for(uint i=0;i<n;i++)
		    set_field(symbols, width, i, am->map(get_field(symbols, width, i)));
		max_v=max_value(symbols, width, n);
			height=bits(max_v);
			uint *occurrences=new uint[max_v+1];
			for(uint i=0;i<=max_v;i++) occurrences[i]=0;
			for(uint i=0;i<n;i++)
			    occurrences[get_field(symbols, width, i)]++;
			uint to_add=0;
			for(uint i=0;i<max_v;i++)
			    if(occurrences[i]==0) to_add++;
			uint * new_symb = new uint[((n+to_add)*width)/W + 1];
			for(uint i=0;i<n;i++)
			    set_field(new_symb, width, i, get_field(symbols, width, i));

			if (deleteSymbols) {
			    delete [] symbols;
			    symbols = 0;
			}

			to_add = 0;
			for(uint i=0;i<max_v;i++)
			if(occurrences[i]==0) {
			    occurrences[i]++;
			    set_field(new_symb, width, n+to_add, i);
			    to_add++;
			}
			uint new_n = n+to_add;
			for(uint i=1;i<=max_v;i++)
			    occurrences[i] += occurrences[i-1];
			uint *oc = new uint[(new_n+1)/W+1];
			for(uint i=0;i<(new_n+1)/W+1;i++)
			    oc[i] = 0;
			for(uint i=0;i<=max_v;i++)
			    bitset(oc,occurrences[i]-1);
			bitset(oc,new_n);
			occ = bmb->build(oc,new_n+1);
			delete [] occurrences;
			this->n = new_n;
			uint ** _bm=new uint*[height];
			for(uint i=0;i<height;i++) {
			    _bm[i] = new uint[new_n/W+1];
			    for(uint j=0;j<new_n/W+1;j++)
				_bm[i][j]=0;
			}
			build_level(_bm,new_symb,width,0,new_n,0);
			bitstring = new BitSequence*[height];
			for(uint i=0;i<height;i++) {
			    bitstring[i] = bmb->build(_bm[i],new_n);
			    delete [] _bm[i];
			}
			delete [] _bm;

			if (!deleteSymbols)
			    for(uint i=0;i<n;i++)
				set_field(symbols, width, i, am->unmap(get_field(symbols, width, i)));

			// delete [] new_symb; // already deleted in build_level()!
			delete [] oc;
			bmb->unuse();
		    }

		    WaveletTreeNoptrs::WaveletTreeNoptrs():Sequence(0) {
			bitstring = NULL;
			occ = NULL;
			am = NULL;
		    }

		    WaveletTreeNoptrs::~WaveletTreeNoptrs() {
			if(bitstring) {
			    for(uint i=0;i<height;i++)
				if(bitstring[i])
				    delete bitstring[i];
			    delete [] bitstring;
			}
			if(occ)
			    delete occ;
			if(am)
			    am->unuse();
		    }

		    void WaveletTreeNoptrs::save(ofstream & fp) const
		    {
			uint wr = WVTREE_NOPTRS_HDR;
			saveValue(fp,wr);
			saveValue<size_t>(fp,n);
			saveValue(fp,max_v);
			saveValue(fp,height);
			am->save(fp);
			for(uint i=0;i<height;i++)
			    bitstring[i]->save(fp);
			occ->save(fp);
		    }

		    WaveletTreeNoptrs * WaveletTreeNoptrs::load(ifstream & fp) {
			uint rd = loadValue<uint>(fp);
			if(rd!=WVTREE_NOPTRS_HDR) return NULL;
			WaveletTreeNoptrs * ret = new WaveletTreeNoptrs();
			ret->n = loadValue<size_t>(fp);
			ret->max_v = loadValue<uint>(fp);
			ret->height = loadValue<uint>(fp);
			ret->am = Mapper::load(fp);
			if(ret->am==NULL) {
			    delete ret;
			    return NULL;
			}
			ret->am->use();
			ret->bitstring = new BitSequence*[ret->height];
			for(uint i=0;i<ret->height;i++) 
			    ret->bitstring[i] = NULL;
			for(uint i=0;i<ret->height;i++) {
			    ret->bitstring[i] = BitSequence::load(fp);
			    if(ret->bitstring[i]==NULL) {
				cout << "damn" << i << " " << ret->height << endl;
				delete ret;
				return NULL;
			    }
			}
			ret->occ = BitSequence::load(fp);
			if(ret->occ==NULL) {
			    delete ret;
			    return NULL;
			}
			return ret;
		    }

		    inline uint get_start(uint symbol, uint mask) {
			return symbol&mask;
		    }

		    inline uint get_end(uint symbol, uint mask) {
			return get_start(symbol,mask)+!mask+1;
		    }

    bool WaveletTreeNoptrs::is_set(uint val, uint ind) const
    {
	assert(ind<height);
	return (val & (1<<(height-ind-1)))!=0;
    }

	    uint WaveletTreeNoptrs::set(uint val, uint ind) const
	    {
		assert(ind<=height);
		return val | (1<<(height-ind-1));
	    }

		// RKONOW MODIFICATION

		vector < pair<uint,size_t> > WaveletTreeNoptrs::mrqq(size_t i,size_t j, size_t k, size_t kp)
		{
			vector < pair<uint,size_t> > result;
			mrqq_aux(0,i,j,k,kp,0,0,n-1,result);
			return result;
		}
		size_t rmax(size_t a,size_t b)
		{
			if (a >= b) 
			{
				return a;
			}
			else
			{
				 return b;
			}
		}
		size_t rmin(size_t a, size_t b)
		{
			if (a <= b) return a;
			else return b;
		}
		void  WaveletTreeNoptrs::mrqq_aux(uint lev,size_t i, size_t j,size_t k, size_t kp,uint sym,size_t start,size_t end,vector < pair<uint,size_t> > result)
		{
			size_t i_left,j_left,i_right,j_right,n_left;
			size_t before,end_left,end_right,start_left,start_right;

			
			if (lev < height)
			{
				BitSequence* bs = bitstring[lev];
				if (start==0) before=0;
				else before = bs->rank1(start-1);

				size_t rank_left = bs->rank1(start+i-1);
				size_t rank_right = bs->rank1(start+j);
				
				uint n1 = rank_right - rank_left;		
				n_left = (j-i+1)-n1;
				if (i > n-1 || j > n-1)
					return;
					
				if (k <= n_left)
				{
					i_left = i - (rank_left - before);
					j_left = j - (rank_right - before);
					end_left = end - (bs->rank1(end) - before);
					start_left = start;

					mrqq_aux(lev+1,i_left,j_left,k,rmin(n_left,kp),sym,start_left,end_left,result);;
				}
				if (kp > n_left) 
				{
					sym=set(sym,lev);						

					start_right = end - (bs->rank1(end)-before) + 1;
					end_right = end;
					i_right = rank_left - before ;
					j_right = rank_right - before -1; 	

					size_t gr = 1;
					if (k>n_left)
						 gr = rmax(k-n_left,1);
					mrqq_aux(lev+1,i_right,j_right,gr,kp,sym,start_right,end_right,result);
				}
			}
			else
			{	
				if (i > j)
				{
					return;
				}
				//result.push_back(pair<uint,size_t>(am->unmap(sym),j-i+1));
	//			cout << "Adding -> " << am->unmap(sym) << " , "  << j-i+1 << endl;
			}	

		}
		


			void  WaveletTreeNoptrs::range_intersect_aux(size_t x_start,size_t x_end,size_t y_start, size_t y_end)
			{
				range_intersect(0,x_start,x_end,y_start,y_end,0,0,n-1);
			}

			void WaveletTreeNoptrs::range_intersect(uint lev, size_t x_start, size_t x_end,size_t y_start, size_t y_end,uint sym, size_t start, size_t end)
			{
		//		cout << "x_start = " << x_start << endl;
		//		cout << "x_end = " << x_end << endl;
		//		cout << "x_start = " << y_start << endl;
		//		cout << "y_end = " << y_end << endl;
				size_t x_start_left,x_start_right,x_end_left,x_end_right;
		size_t y_start_left,y_start_right,y_end_left,y_end_right;

		size_t before;
		size_t end_left;
		size_t end_right;
		size_t start_left;
		size_t start_right;
		
		if (x_start > x_end || x_end > n-1 || y_start > y_end || y_end > n-1)
			{	
				return;
			}
		if (lev < height)
		{
			BitSequence* bs = bitstring[lev];
			if (start==0) before=0;
			else before = bs->rank1(start-1);

				size_t rank_left_x = bs->rank1(start+x_start-1);
				size_t rank_right_x = bs->rank1(start+x_end);

				size_t rank_left_y = bs->rank1(start+y_start-1);
				size_t rank_right_y = bs->rank1(start+y_end);
		// First go left
				x_start_left = x_start - (rank_left_x - before);
				x_end_left = x_end - (rank_right_x - before);
				y_start_left = y_start - (rank_left_y - before);
				y_end_left = y_end - (rank_right_y - before);
		// set the new Start and End for the goLeft case
				end_left = end - (bs->rank1(end) - before);
				start_left = start;		

				range_intersect(lev+1,x_start_left,x_end_left,y_start_left,y_end_left,sym,start_left,end_left);

			    //go Right
				sym=set(sym,lev);						
		// set the new Start and End for the goRight case
				start_right = end - (bs->rank1(end)-before) + 1;
				end_right = end;

				x_start_right = rank_left_x - before ;
				x_end_right = rank_right_x - before -1; 
				y_start_right = rank_left_y - before;
				y_end_right = rank_right_y - before -1;
				range_intersect(lev+1,x_start_right,x_end_right,y_start_right,y_end_right,sym,start_right,end_right);
		}
		else
		{	
			if (x_start > x_end)
			{
				return;
			}
//			cout << "Adding -> " << am->unmap(sym) << " , "  << endl;
		}	

	}
	

	
  // los hice globales para no pasarlos por toda la recursion, aunque esto
  // impide usarla multithreaded...
  
static	size_t *x_start_left;
static	size_t *x_start_right;
static	size_t *x_end_left;
static	size_t *x_end_right;

void  WaveletTreeNoptrs::n_range_intersect_aux(size_t *x_start,size_t *x_end,size_t n_ranges)
{
    
	x_start_left = new size_t[n_ranges * height];
	x_start_right = new size_t[n_ranges * height];
	x_end_left = new size_t[n_ranges * height];
	x_end_right = new size_t[n_ranges * height];

	n_range_intersect(0,x_start,x_end,0,0,n-1,n_ranges,
		x_start_left,x_start_right,x_end_left,x_end_right);

        delete x_start_left; // OJO se escribe asi?
        delete x_start_right; // OJO se escribe asi?
        delete x_end_left; // OJO se escribe asi?
        delete x_end_right; // OJO se escribe asi?
}

void WaveletTreeNoptrs::n_range_intersect(uint lev, size_t * x_start, size_t *x_end,uint sym, size_t start, size_t end,size_t n_ranges,
	size_t *x_start_left, size_t *x_start_right, size_t *x_end_left, size_t *x_end_right)
{


// creo que estos tests estan de mas
//	// llegamos a una hoja vacia
//	if (start > end)
//		return;
//	// nos fuimos a la cresta
//	if (end > n-1)
//		return;
//
//	// revisamos que ningun rango se haya hecho vacio
//	for (size_t i = 0 ; i < n_ranges;i++)
//	{
//		if (x_start[i] >  x_end[i])
//			return;
//	}

	// si aun no hemos llegado a la hoja
	if (lev < height)
	{

		// construimos los arreglos para mantener los rangos
		size_t rank_left_x;
		size_t rank_right_x;

		size_t before;
		size_t end_left;
		size_t end_right;
		size_t start_left;
		size_t start_right;

	    int liveleft = 1, liveright = 1;

		BitSequence* bs = bitstring[lev];
		if (start==0) before=0;
			else before = bs->rank1(start-1);
		// calculamos para cada rango, su nuevo intervalo
		for (size_t i = 0 ; i < n_ranges;i++)
		{
			
			rank_left_x = bs->rank1(start+x_start[i]-1); 
			rank_right_x = bs->rank1(start+x_end[i]);
			if (liveleft)
			   { x_start_left[i] = x_start[i] - (rank_left_x - before);
			     x_end_left[i] = x_end[i] - (rank_right_x-before);
			     if (x_end_left[i] < x_start_left[i])
				{  if (!liveright) return; liveleft = 0; }
			   }
			if (liveright)
			   { x_start_right[i] = rank_left_x - before + 1;
		  	     x_end_right[i] = rank_right_x - before; 
			     if (x_end_right[i] < x_start_right[i])
				{ if (!liveleft) return ;liveright = 0;  }
			   }
	
		}
			// estas 2 no las optimizo porque en el wmatrix no estan
		end_left = end - (bs->rank1(end) - before);
		start_left = start;
		// si no nos pasamos, ejecutamos a la izquierda
		if (liveleft)
		  n_range_intersect(lev+1,x_start_left,x_end_left,sym,start_left,end_left,n_ranges,x_start_left+n_ranges,x_start_right+n_ranges,x_end_left+n_ranges,x_end_right+n_ranges); 
		// si no nos pasamos, ejecutamos a la derecha
		 

		if (liveright)
		{ 
		  sym=set(sym,lev);	
		  start_right = end - end_left + 1;
		  end_right = end;
		  n_range_intersect(lev+1,x_start_right,x_end_right,sym,start_right,end_right,n_ranges,x_start_left+n_ranges,x_start_right+n_ranges,x_end_left+n_ranges,x_end_right+n_ranges);
		}
	}
	// llegamos a una hoja
	else
	{
		//cout << "Adding sym = " << sym << endl;		
	}
}




vector<uint>  WaveletTreeNoptrs::range_report_aux(size_t x_start,size_t x_end)
{
vector<uint> result;
//		result.reserve(1500);
range_report(0,x_start,x_end,0,0,n-1,result);
return result;
}
void WaveletTreeNoptrs::range_report(uint lev, size_t x_start, size_t x_end,uint sym, size_t start, size_t end,vector <uint> &result)
{
size_t x_start_left,x_start_right,x_end_left,x_end_right;
size_t before;
size_t end_left;
size_t end_right;
size_t start_left;
size_t start_right;
//	cout << "x_start= " <<  x_start << " x_end= " << x_end << endl;
if (x_start > x_end || x_end > n-1)		
	return;
	
if (lev < height)
{
	BitSequence* bs = bitstring[lev];
	//cout << "LEVEl " << lev << endl;
	//cout << "start -> " << start << " end -> " << end << endl;
	if (start==0) before=0;
	else before = bs->rank1(start-1);

		size_t rank_left = bs->rank1(start+x_start-1);
		size_t rank_right = bs->rank1(start+x_end);
		x_start_left = x_start - (rank_left - before);
		x_end_left = x_end - (rank_right - before);
		end_left = end - (bs->rank1(end) - before);
		start_left = start;
		//cout << "izquierda | x_start_left = " << x_start_left << " x_end_left = " << x_end_left << endl;
		range_report(lev+1,x_start_left,x_end_left,sym,start_left,end_left,result);
	
		sym=set(sym,lev);						
		start_right = end - (bs->rank1(end)-before) + 1;
		end_right = end;
		x_start_right = rank_left - before ;
		x_end_right = rank_right - before -1; 	
		//cout << "derecha | x_start_left = " << x_start_right << " x_end_left = " << x_end_right << endl;
	range_report(lev+1,x_start_right,x_end_right,sym,start_right,end_right,result);

}
else
{	
		if (x_start > x_end)
		return;

		result.push_back(am->unmap(sym));
			cout << "Adding -> " << am->unmap(sym) << " , "  << endl;
}	
}
		uint WaveletTreeNoptrs::range(size_t posl,size_t posr,size_t i) const
		{
			uint level=0;
			uint ret=0;
			size_t start=0;
			size_t end=n-1;
			size_t before;
			uint n1 = 0;
			uint n0 = 0;    


			while(level<height) {
				if (start==0) before=0;
				else before = bitstring[level]->rank1(start-1);

				size_t rank_left = bitstring[level]->rank1(start+posl-1);
				size_t rank_right = bitstring[level]->rank1(start+posr);
				n1 = rank_right - rank_left;
				n0 = (posr-posl+1)-n1;   
				if (i <= n0)
				{
					posl = posl - (rank_left - before);
					posr = posr - (rank_right - before);
					end = end - (bitstring[level]->rank1(end) - before);
				}
				else
				{
					ret=set(ret,level);
					posl = rank_left - before ;
					posr = rank_right - before -1; 
					i = i - n0;
					start = end - (bitstring[level]->rank1(end)-before) + 1;
				}
		level++;
		}
		return am->unmap(ret);
	    }

	    uint WaveletTreeNoptrs::access(size_t pos) const
	    {
		uint level=0;
		uint ret=0;
		size_t start=0;
		size_t end=n-1;
		while(level<height) {
		    assert(pos>=start && pos<=end);
		    if(bitstring[level]->access(pos)) {
			ret=set(ret,level);
			pos=bitstring[level]->rank1(pos-1)-bitstring[level]->rank1(start-1);
			start=(bitstring[level]->rank1(end)-bitstring[level]->rank1(start-1));
			start=end-start+1;
			pos+=start;
		    }
		    else {
			pos=pos-start-(bitstring[level]->rank1(pos)-bitstring[level]->rank1(start-1));
			end=end-start-(bitstring[level]->rank1(end)-bitstring[level]->rank1(start-1));
			end+=start;
			pos+=start;
		    }
		    level++;
		}
		return am->unmap(ret);
	    }

	    size_t WaveletTreeNoptrs::rank(uint symbol, size_t pos) const
	    {
		symbol = am->map(symbol);
		uint level=0;
		size_t start=0;
		size_t end=n-1;
		size_t count=0;
		while(level<height) {
		    if(is_set(symbol,level)) {
			pos=bitstring[level]->rank1(pos)-bitstring[level]->rank1(start-1)-1;
			count=pos+1;
			start=(bitstring[level]->rank1(end)-bitstring[level]->rank1(start-1));
			start=end-start+1;
			pos+=start;
		    }
		    else {
			pos=pos-start+bitstring[level]->rank1(start-1)-bitstring[level]->rank1(pos);
			count=pos+1;
			end=end-start-(bitstring[level]->rank1(end)-bitstring[level]->rank1(start-1));
			end+=start;
			pos+=start;
		    }
		    level++;
		    if(count==0) return 0;
		}
		return count;
	    }

	    size_t WaveletTreeNoptrs::select(uint symbol, size_t j) const
	    {
		symbol = am->map(symbol);
		uint mask = (1<<height)-2;
		uint sum=2;
		uint level = height-1;
		size_t pos=j;
		while(true) {
		    size_t start = get_start(symbol,mask);
		    size_t end = min((size_t)max_v+1,start+sum);
		    start = (start==0)?0:(occ->select1(start)+1);
		    end = occ->select1(end+1)-1;
		    if(is_set(symbol,level)) {
			uint ones_start = bitstring[level]->rank1(start-1);
			pos = bitstring[level]->select1(ones_start+pos)-start+1;
		    }
		    else {
			uint ones_start = bitstring[level]->rank1(start-1);
			pos = bitstring[level]->select0(start-ones_start+pos)-start+1;
		    }
		    mask <<=1;
		    sum <<=1;
		    if(level==0) break;
		    level--;
		}
		return pos-1;
	    }

	    size_t WaveletTreeNoptrs::getSize() const
	    {
		size_t ptrs = sizeof(WaveletTreeNoptrs)+height*sizeof(Sequence*);
        size_t bytesBitstrings = 0;
        for(uint i=0;i<height;i++)
            bytesBitstrings += bitstring[i]->getSize();
        return bytesBitstrings+occ->getSize()+ptrs;
    }

    void WaveletTreeNoptrs::build_level(uint **bm, uint *symbols, uint level, uint length, uint offset) {
        if(level==height) {
            delete [] symbols;
            return;
        }
        uint cleft=0;
        for(size_t i=0;i<length;i++)
            if(!is_set(symbols[i],level))
                cleft++;
        uint cright=length-cleft;
        uint *left=new uint[cleft], *right=new uint[cright];
        cleft=cright=0;
        for(size_t i=0;i<length;i++)
        if(!is_set(symbols[i],level)) {
            left[cleft++]=symbols[i];
            bitclean(bm[level],offset+i);
        }
        else {
            right[cright++]=symbols[i];
            bitset(bm[level],offset+i);
        }

        delete [] symbols;
        symbols = 0;

        build_level(bm,left,level+1,cleft,offset);
        left = 0;                // Gets deleted in recursion.
        build_level(bm,right,level+1,cright,offset+cleft);
        right = 0;               // Gets deleted in recursion.
        //delete [] left;
        //delete [] right;
    }

    // symbols is an array of elements of "width" bits.
    void WaveletTreeNoptrs::build_level(uint **bm, uint *symbols, unsigned width, uint level, uint length, uint offset) {
        if(level==height) {
            delete [] symbols;
            return;
        }
        uint cleft=0;
        for(size_t i=0;i<length;i++)
            if(!is_set(get_field(symbols, width, i),level))
                cleft++;
        uint cright=length-cleft;
        uint *left=new uint[(cleft*width)/W + 1],
            *right=new uint[(cright*width)/W + 1];
        cleft=cright=0;
        for(size_t i=0;i<length;i++)
        if(!is_set(get_field(symbols,width,i),level)) {
            set_field(left,width,cleft++,get_field(symbols, width,i));
            bitclean(bm[level],offset+i);
        }
        else {
            set_field(right,width,cright++,get_field(symbols,width,i));
            bitset(bm[level],offset+i);
        }

        delete [] symbols;
        symbols = 0;

        build_level(bm,left,width,level+1,cleft,offset);
        left = 0;                // Gets deleted in recursion.
        build_level(bm,right,width,level+1,cright,offset+cleft);
        right = 0;               // Gets deleted in recursion.
        //delete [] left;
        //delete [] right;
    }

    uint WaveletTreeNoptrs::max_value(uint *symbols, size_t n) {
        uint max_v = 0;
        for(size_t i=0;i<n;i++)
            max_v = max(symbols[i],max_v);
        return max_v;
    }

    uint WaveletTreeNoptrs::max_value(uint *symbols, unsigned width, size_t n) {
        uint max_v = 0;
        for(size_t i=0;i<n;i++)
            max_v = max(get_field(symbols, width, i),max_v);
        return max_v;
    }

    uint WaveletTreeNoptrs::bits(uint val) {
        uint ret = 0;
        while(val!=0) {
            ret++;
            val >>= 1;
        }
        return ret;
    }

    size_t WaveletTreeNoptrs::count(uint symbol) const
    {
        return occ->select1(am->map(symbol))-occ->select1(am->map(symbol)-1)+1;
    }

};
