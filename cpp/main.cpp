#include "frame.h"
#include "morceau.h"
#include <sndfile.h>
#include <stdio.h>
#include <iostream>
#include <list>
#include "parameters.h"
#include "debug.h"
#include <cmath>

// faire normalisation et se débrouiller pour avoir une belle classif
// on a inclu cmath (utile ?)

Morceau lecture(char* fileName, int classe_donne = 0){

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

	Morceau morceau = Morceau(frames, n_frames, classe_donne);
	
	return morceau;
};



double distance(Frame &f1, Frame &f2){

	double dist = 0;
	double * mfcc1 = f1.get_mfcc();
	double * mfcc2 = f2.get_mfcc();
	double moy_mfcc=0;
	double moy_lpc = 0;
	for(int i = 0; i < NB_MFCC; i++){
		dist += std::sqrt((mfcc1[i] - mfcc2[i])*(mfcc1[i] - mfcc2[i]));
		// moy_mfcc += std::sqrt(mfcc2[i]*mfcc2[i]);
	}
	// std::cout << "mfcc : " << moy_mfcc/NB_MFCC << std::endl;

	//std::cout << "dist : " << dist << std::endl;
	double * lpc1 = f1.get_lpc();
	double * lpc2 = f2.get_lpc();
	for(int i = 0; i < ORDRE_LPC; i++){
		dist += std::sqrt((lpc1[i] - lpc2[i])*(lpc1[i] - lpc2[i]))*100;
		// moy_lpc += std::sqrt(lpc2[i]*lpc2[i]);
	}
	// std::cout << "lpc : " << moy_lpc/ORDRE_LPC << std::endl;

	return dist;
}



double dtw(Morceau &m1, Morceau &m2){

	int i = 0;
	int j = 0;
	double distances[m1.get_n_frames()][m2.get_n_frames()];
	Frame* frames1 = m1.get_frames();
	Frame* frames2 = m2.get_frames();

	distances[0][0] = distance(frames1[0], frames2[0]);
	for(j = 1; j < m2.get_n_frames(); j++){
		distances[0][j] = distances[0][j-1] + distance(frames1[0], frames2[j]);
			// distances[0][j] = 0;

	}
	for(i = 1; i < m1.get_n_frames(); i++){
		distances[i][0] = distances[i-1][0] + distance(frames1[i], frames2[0]);
		for(j = 1; j < m2.get_n_frames(); j++){
			// distances[i][j] = std::min(distances[i-1][j],distances[i][j-1]) + distance(frames1[i], frames2[j]);
			distances[i][j] = std::min(distances[i-1][j-1],std::min(distances[i-1][j],distances[i][j-1])) + distance(frames1[i], frames2[j]);
		}
	}

	//plotMatrix
	//  std::cout<< std::endl<< std::endl<< std::endl<< " nouvelle matrice : " << m1.get_classe() << " avec " << m2.get_classe() << std::endl;  // when the inner loop is done, go to a new line
	// int n = m2.get_n_frames()/4;
	// int m = m1.get_n_frames()/4;
	// for(int x=0;x<m;x++){
	// 	std::cout<< " | ";
	// 	for(int y=0;y<n;y++){
 //            std::cout<< static_cast<int>(distances[x][y]) << " ";  // display the current element out of the array
 //        }
 //    	std::cout<< " | " << std::endl;  // when the inner loop is done, go to a new line
 //    }
	return distances[i-1][j-1]/(m1.get_n_frames()+m2.get_n_frames());
}



int main(int argc, char *argv[]) {

	// Creation de la base
	int l_base = 4;
	Morceau base[l_base];

	char nom1[] = "../test3/adroite2.wav";
	base[0] = lecture(nom1, 1);
	char nom2[] = {"../test3/agauche.wav"};
	base[1] = lecture(nom2, 2);
	char nom3[] = "../test3/enavant2.wav";
	base[2] = lecture(nom3, 3);
	char nom4[] = "../test3/stop.wav";
	base[3] = lecture(nom4, 4);


	char nom0[] = "../test3/agauche2.wav";
	Morceau m2 = lecture(nom0);

	double dist = dtw(m2, base[0]);
	double cl = base[0].get_classe();
	std::cout << "classe : " << cl << ", distance : " << dist << std::endl;
	double temp = 0;
	for (int i = 1; i < l_base; i++){
		temp = dtw(m2, base[i]);
		std::cout << "classe : " << base[i].get_classe() << ", distance : " << temp << std::endl;
		if(temp < dist){
			dist = temp;
			cl = base[i].get_classe();
		}
	}
	std::cout << "nom du fichier : " << nom0 << std::endl;
	std::cout << "classe choisie : " << cl << std::endl;
	
	return -1;
}


// sans norme :
// adroite2,3 x,x
// agauche2 x,v
// enavant2,3 v,x
// stop2,3 v,v

// avec norme :
// adroite2,3 x,x
// agauche2 x,v
// enavant2,3 x,x
// stop2,3 v,v