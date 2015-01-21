#include "frame.h"
#include "morceau.h"
#include <sndfile.h>
#include <stdio.h>
#include <iostream>
#include <list>
// cool tout fonctionne mais mfcc trop long


Morceau lecture(char* fileName){

	SF_INFO sfinfo;
	SNDFILE* infile;
	//infile = sf_open(fileName, SFM_READ, & sfinfo );	
	if ( !(infile = sf_open(fileName, SFM_READ, &sfinfo)) ){   
		// Open failed so print an error message.
	    printf ("Not able to open input file %s.\n", fileName) ;
	    sf_perror (NULL);
	    return Morceau();
	} 	
	
	int f_ech = sfinfo.samplerate;
	int ordre_lpc = 10;
	int nb_mfcc = 40;	
	int frames_length = 256;


	double data[frames_length/2];
	auto size_data = frames_length/2;
	std::list<Frame> frames;
	int n_frames = 0;
	double concat[frames_length];
	sf_read_double (infile, data, frames_length/2);
	std::copy(data, data + size_data, concat);

	while(frames_length/2 == sf_read_double (infile, data, frames_length/2)){
		n_frames++;
		std::copy(data, data + size_data, concat + size_data);
		Frame frame = Frame(concat, frames_length, f_ech, ordre_lpc, nb_mfcc);
		std::copy(data, data + size_data, concat);
		frames.push_back(frame);
		//std::cout<< "dernier point de la frame n°" << n_frames << " : " << data[frames_length-1] << std::endl;
	}

	sf_close(infile);

	Morceau morceau = Morceau(frames, n_frames);
	
	return morceau;
};






int main(int argc, char *argv[]) {

	char nom1[] = "../test3/adroite.wav";
	Morceau m1 = lecture(nom1);
	
	return -1;
}
