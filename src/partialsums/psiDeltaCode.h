#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <unistd.h>
#include <fcntl.h>
//#include "../utils/basics.h"
#include "defValues.h"
#include "delta.h"

typedef struct {
	unsigned int T;
	unsigned int negativeGap;
	unsigned int deltaCodesSize;	// En palabras
	unsigned int *deltaCodes;					
	unsigned int numberOfSamples;				
	unsigned int *samples;
	unsigned int *pointers;
	unsigned int totalMem;			// the size in bytes used;
} DeltaCompressedPsi;


// PROTOTIPOS DE FUNCI�NS
DeltaCompressedPsi deltaCompressPsi(unsigned int *Psi, unsigned int psiSize, unsigned int T);
int getDeltaPsiValue(DeltaCompressedPsi *cPsi, unsigned int position);
void storeDeltaCompressedPsi(DeltaCompressedPsi *compressedPsi, char *filename);
DeltaCompressedPsi loadDeltaCompressedPsi(char *filename);		
void destroyDeltaCodesCompressedPsi(DeltaCompressedPsi *compressedPsi);
	
