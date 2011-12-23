#include <iostream>
#include <fstream>
#include <cassert>
 
using namespace std;
 
#include <WaveletTree.h>
#include <Sequence.h>
#include <Mapper.h>
#include <BitSequenceBuilder.h>
 
using namespace cds_static;
 
void run(Sequence * seq) ;
size_t count(Sequence * seq, char s) ;
void list(Sequence * seq, char s) ;
 
int main(int argc, char ** argv) {
    if(argc!=2) {
        cout << "usage: " << argv[0] << " <file>" << endl;
        return 0;
    }
 
    cout << "opening file" << endl;
    ifstream input(argv[1]); // open file
    assert(input.good()); // check open 
 
    cout << "getting size" << endl;
    input.seekg(0, ios_base::end); // move to the end
    size_t input_size = input.tellg(); // get size
    input.seekg(0, ios_base::beg); // move back to the beginning
 
    cout << "reading sequence" << endl;
    uchar * sequence = new uchar[input_size]; // store the sequence in an array
    input.read((char*)sequence,input_size); // read the sequence
    assert(!input.fail()); // check 
    input.close(); // close input
 
    cout << "building mapper" << endl;
    MapperNone * map = new MapperNone();
 
    cout << "building wt_coder_huff" << endl;
    wt_coder_huff * wc = new wt_coder_huff(sequence,input_size,map);
 
    cout << "biulding bitsequence builder" << endl;
    BitSequenceBuilder * bsb = new BitSequenceBuilderRG(20);
 
    cout << "building wavelet tree" << endl;
    Sequence * seq = new WaveletTree(sequence, input_size, wc, bsb, map);
 
    delete [] sequence;
 
    cout << "done, entering command line" << endl << endl;
    run(seq);
     
    cout << "finishing, freeing memory" << endl;
    delete seq;
 
    return 0;
}
 
// here we answer the queries, the list function answers list 
// and the count function the count operations
void run(Sequence * seq) {
    string qry;
    cout << "> ";
    while((cin >> qry)) {
        char s;
        if(qry == "count") {
            cin >> s;
            count(seq,s);
        } else if(qry == "list") {
            cin >> s;
            list(seq,s);
        } else if(qry == "size") {
            cout << "WT length: " << seq->getLength() << endl;
            cout << "WT size: " << seq->getSize() << endl;
        } else if(qry == "quit") {
            break;
        } else {
            cout << "Command unknown" << endl;
        }
        cout << "> ";
    }
}
 
size_t count(Sequence * seq, char s) {
    size_t ret = seq->rank(s,seq->getLength());
    cout << "count: " << ret << endl;
    return ret;
}
 
void list(Sequence * seq, char s) {
    size_t cnt = count(seq,s);
    for(size_t i=1; i<=cnt; i++) 
        cout << " " << seq->select(s,i);
    cout << endl;
}

