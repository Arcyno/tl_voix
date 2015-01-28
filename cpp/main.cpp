#include "frame.h"
#include "morceau.h"
#include <sndfile.h>
#include <stdio.h>
#include <iostream>
#include <list>
#include "parameters.h"
#include "debug.h"

// finir mfcc ? (on n'utilise pas le tableau mfcc !)
// puis vérifier lpc, mfcc depuis dehors
// et vérifier résultats avec Ofast.


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
	int ordre_lpc = ORDRE_LPC;
	int nb_mfcc = NB_MFCC;	
	int frames_length = TAILLE_FRAME;


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



double distance(Frame f1, Frame f2){

	double dist = 0;

	double * mfcc1 = f1.get_mfcc();
	double * mfcc2 = f2.get_mfcc();
	for(int i = 0; i < NB_MFCC; i++){
		dist += (mfcc1[i] - mfcc2[i])*(mfcc1[i] - mfcc2[i]);
	}
	double * lpc1 = f1.get_lpc();
	double * lpc2 = f2.get_lpc();
	for(int i = 0; i < ORDRE_LPC; i++){
		dist += (lpc1[i] - lpc2[i])*(lpc1[i] - lpc2[i]);
	}

	return dist;
}



double dtw(Morceau m1, Morceau m2){

	int i = 0;
	int j = 0;
	double distances[m1.get_n_frames()][m2.get_n_frames()];
	Frame* frames1 = m1.get_frames();
	Frame* frames2 = m2.get_frames();
	distances[0][0] = distance(frames1[0], frames2[0]);
	for(j = 1; j < m2.get_n_frames(); j++){
		distances[0][j] = distances[0][j-1] + distance(frames1[i], frames2[j]);
	}
	for(i = 1; i < m1.get_n_frames(); i++){
		distances[i][0] = distances[i-1][0] + distance(frames1[i], frames2[0]);
		for(j = 1; j < m2.get_n_frames(); j++){
			distances[i][j] = std::min(distances[i-1][j-1],std::min(distances[i-1][j],distances[i][j-1])) + distance(frames1[i], frames2[j]);
		}
	}

	return distances[i][j];
}



int main(int argc, char *argv[]) {

	char nom1[] = "../test3/adroite.wav";
	Morceau m1 = lecture(nom1);

	char nom2[] = "../test3/adroite2.wav";
	Morceau m2 = lecture(nom2);

	double dist = dtw(m1,m2);

	std::cout << "dist : " << dist << std::endl;
	
	return -1;
}
