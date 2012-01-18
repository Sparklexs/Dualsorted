#include "psiDeltaCode.h"

void destroyDeltaCodesCompressedPsi(DeltaCompressedPsi *compressedPsi) {
	free(compressedPsi->deltaCodes);
	free(compressedPsi->samples);
	free(compressedPsi->pointers);
}

			
DeltaCompressedPsi deltaCompressPsi(unsigned int *Psi, unsigned int psiSize, unsigned int T) {

	DeltaCompressedPsi cPsi;
	
	int numberOfSamples;
	register int diff;
	register uint deltaCodesPos;				 

	register unsigned int k, p, aux, diffpositive, code, index;
	unsigned int samplesIndex, codeLenght, currentInput, wordsDeltaCodes, totalSize;
	unsigned int *deltaCodes;
	unsigned int *samples;
	unsigned int *pointers;
	
	// Auxiliar para deltaCodes (estimamos como espacio maximo o do array de sufixos)
	unsigned int *deltaCodesAux;					 
	
	#ifdef USE_MAPPING_GAPS	
		printf("\n **using delta-compression [NO-RUNS] (of gaps:: mapping to negative values) for psi** \n");
	#endif
	#ifdef USE_XOR_GAPS
		printf("\n **using delta-compression [NO-RUNS] (of gaps:: XOR over gaps)for psi** \n");			
	#endif
	 
	// Calculamos o mellor valor para negativeGap <= 64
	unsigned int negativeGap;
	register unsigned int maxNegativeBits = 0;
	k = psiSize;
	while(k) {
		k >>= 1;
		maxNegativeBits++;		
	}
	printf("\n Given psiSize = %d, maxNegativeBits = %d",psiSize,maxNegativeBits);  //28 para corpus all
	
	if(maxNegativeBits<=26) negativeGap = 64;
	else negativeGap = 1<<(32-maxNegativeBits);				 
	
	// Reservamos espacio para as estructuras
	numberOfSamples = (psiSize + T - 1) / T;
	samples = (unsigned int *)malloc(sizeof(int)*numberOfSamples);
	pointers = (unsigned int *)malloc(sizeof(int)*numberOfSamples);
	
	deltaCodesAux = (unsigned int *)malloc(sizeof(int)*psiSize);
	for(index=0; index<psiSize; index++) deltaCodesAux[index] = 0;	 	 
	
	samplesIndex = 0;
	deltaCodesPos = 0;
	currentInput=0;
	for(index=0; index<psiSize; index++) {

		if(index % T) {
			

			#ifdef USE_MAPPING_GAPS			
			diff = Psi[index] - currentInput;
			currentInput = Psi[index];
			//*****//
			// Calculamos o codigo correspondente
			if(diff>0) diffpositive = (negativeGap*diff-1)/(negativeGap-1);
			else diffpositive = -negativeGap*diff;
			//*****//
			#endif
			#ifdef USE_XOR_GAPS
				//d(i) = x(i) XOR x(i-1)
			diffpositive = Psi[index] ^ currentInput;
			currentInput = Psi[index];
			#endif	

			/** ************************************************************ **/
			//encoding the value diffpositive
			  //deltaCodesPos += encodeDelta(deltaCodesAux, deltaCodesPos, diffpositive);  /*Diego's version */
			  deltaCodesPos += encodeDeltaEdu(deltaCodesAux, deltaCodesPos, diffpositive);
			  
			 //deltaCodesPos += encodeGamma(deltaCodesAux, deltaCodesPos, diffpositive);  /*Diego's version */
			 //deltaCodesPos += encodeGammaEdu(deltaCodesAux, deltaCodesPos, diffpositive);  

			/** ************************************************************ **/
		
		} else {
			samples[samplesIndex] = Psi[index];
			pointers[samplesIndex++] = deltaCodesPos;			
			currentInput = Psi[index]; 
		}	

	}
	
	// Ahora que xa sabemos o espacio necesario para os deltaCodes, reservamolo e liberamos a estructura auxiliar
	wordsDeltaCodes = (deltaCodesPos+31)/32;
	deltaCodes = (unsigned int *)malloc(sizeof(int)*wordsDeltaCodes);
	for(index=0;index<wordsDeltaCodes;index++) deltaCodes[index] = deltaCodesAux[index];
	free(deltaCodesAux);
	
	totalSize = sizeof(int)*wordsDeltaCodes + 2*sizeof(int)*numberOfSamples + 4*sizeof(int);
	printf("\n\tCompressed Psi size = %d bytes\n", totalSize);
	
	// Asignamos os valores a cPsi e devolvemolo
	cPsi.T = T;
	cPsi.negativeGap = negativeGap;
	cPsi.deltaCodesSize = wordsDeltaCodes;
	cPsi.deltaCodes = deltaCodes;
	cPsi.numberOfSamples = numberOfSamples;
	cPsi.samples = samples;
	cPsi.pointers = pointers;
	cPsi.totalMem = totalSize;

	return cPsi;
	
}


int getDeltaPsiValue(DeltaCompressedPsi *cPsi, unsigned int position) {
	
	int result;
	register unsigned int code, pointer, toDecode; 
	
	// Collemos a mostra inmediatamente inferior, e o punteiro o array de codigos
	// pointer = punteiro absoluto sobre deltaCodes
	result = cPsi->samples[position/cPsi->T];
	pointer = cPsi->pointers[position/cPsi->T];
	
	// Calculamos o numero de codigos a decodificar a partir da mostra
	toDecode = position % cPsi->T;	
	
	while(toDecode--) {
		/** ************************************************************ **/
		//decoding the next code
		  //pointer += decodeDelta(cPsi->deltaCodes, pointer, &code);    //Diego's way
		  //pointer = decodeDeltaEdu(cPsi->deltaCodes, pointer, &code);  //Edu's with Diego's interface.

		  //pointer = decodeDeltaEduPos(cPsi->deltaCodes, pointer, &code);
		  decodeDeltaEduPosMacro(cPsi->deltaCodes, pointer, code);
		  
		  //pointer += decodeGamma(cPsi->deltaCodes, pointer, &code);
		  //pointer += decodeGammaEdu(cPsi->deltaCodes, pointer, &code);
		/** ************************************************************ **/
		// Bixecci'on
		#ifdef USE_MAPPING_GAPS
		//********//
		if(code % cPsi->negativeGap) result += (code - (code/cPsi->negativeGap));
		else result -= code/cPsi->negativeGap;
		//*******//
		#endif
		#ifdef USE_XOR_GAPS
			//x(i) = d(i) XOR x(i-1) 
		result ^= code;
		#endif
			
	}
	
	return result;
}


void storeDeltaCompressedPsi(DeltaCompressedPsi *compressedPsi, char *filename) {

	int file;

	if( (file = open(filename, O_WRONLY|O_CREAT, 0700)) < 0) {
		printf("Cannot open file %s\n", filename);
		exit(0);
	}
	write(file, &(compressedPsi->T), sizeof(int));
	write(file, &(compressedPsi->negativeGap), sizeof(int));
	write(file, &(compressedPsi->deltaCodesSize), sizeof(int));
	write(file, compressedPsi->deltaCodes, compressedPsi->deltaCodesSize*sizeof(int));
	write(file, &(compressedPsi->numberOfSamples), sizeof(int));
	write(file,	compressedPsi->samples, compressedPsi->numberOfSamples*sizeof(int));
	write(file,	compressedPsi->pointers, compressedPsi->numberOfSamples*sizeof(int));
	write(file, &(compressedPsi->totalMem), sizeof(int));

	close(file);
		
}


DeltaCompressedPsi loadDeltaCompressedPsi(char *filename) {
	
	DeltaCompressedPsi compressedPsi;

	int file;

	if( (file = open(filename, O_RDONLY)) < 0) {
		printf("Cannot read file %s\n", filename);
		exit(0);
	}
	read(file, &(compressedPsi.T), sizeof(int)); 
	read(file, &(compressedPsi.negativeGap), sizeof(int));
	read(file, &(compressedPsi.deltaCodesSize), sizeof(int));
	compressedPsi.deltaCodes = (unsigned int *)malloc(compressedPsi.deltaCodesSize*sizeof(int));
	read(file, compressedPsi.deltaCodes, compressedPsi.deltaCodesSize*sizeof(int));
	read(file, &(compressedPsi.numberOfSamples), sizeof(int));	
	compressedPsi.samples = (unsigned int *)malloc(compressedPsi.numberOfSamples*sizeof(int));
	compressedPsi.pointers = (unsigned int *)malloc(compressedPsi.numberOfSamples*sizeof(int));
	read(file, compressedPsi.samples, compressedPsi.numberOfSamples*sizeof(int));
	read(file, compressedPsi.pointers, compressedPsi.numberOfSamples*sizeof(int));		
	read(file, &(compressedPsi.totalMem), sizeof(int));

	close(file);
		
	return compressedPsi;

}
