#include "frame.h"
#include <sndfile.h>
#include <stdio.h>
#include <iostream>

// test autocorr et pb debug.h


int main(int argc, char *argv[]) {

	SF_INFO sfinfo;
	SNDFILE* infile;
	const char* fileName = "../test3/adroite.wav";	
	
	

	//infile = sf_open(fileName, SFM_READ, & sfinfo );	
	if ( !(infile = sf_open(fileName, SFM_READ, &sfinfo)) ){   
		// Open failed so print an error message.
	    printf ("Not able to open input file %s.\n", fileName) ;
	    sf_perror (NULL) ;
	    return  1 ;
	} 	
	
	int f_ech = sfinfo.samplerate;
	int ordre_lpc = 10;
	int nb_mfcc = 40;	
	
	std::cout << "f_ech = " << f_ech << std::endl;

	int frames_length = 320;
	double data[frames_length];
	sf_read_double (infile, data, frames_length) ;
	sf_close(infile);
	//for(int i=0; i<frames_length; ++i){
	//	std::cout << "data(" << i << ") = "<< data[i] << std::endl;
	//}
	
    Frame frame = Frame(data, frames_length, ordre_lpc, nb_mfcc);
    std::cout << "Contenu de lpc : " << frame.get_lpc() << std::endl;

	return -1;
}
