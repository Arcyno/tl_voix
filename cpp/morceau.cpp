#include <list>
#include "morceau.h"



Morceau::Morceau(){
	n_frames = 0;
}

Morceau::Morceau(std::list<Frame> frames_donne, int n_frames_donne, int classe_donne = 0){
	
	// Frame frames2[n_frames_donne];
	Frame* frames2 = new Frame[n_frames_donne];
	copy(frames_donne.begin(), frames_donne.end(), frames2);
	frames = frames2;

	n_frames = n_frames_donne;

	classe = classe_donne;
}

int Morceau::get_n_frames(){
	return n_frames;
}

Frame* Morceau::get_frames(){
	return frames;
}

int Morceau::get_classe(){
	return classe;
}