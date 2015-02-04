#include <list>
#include "morceau.h"
#include <string.h>

// Constructeur par defaut
Morceau::Morceau(){
	n_frames = 0;
}

// Constructeur
Morceau::Morceau(std::list<Frame> frames_donne, int n_frames_donne, const char* nom_donne, int classe_donne = 0){
	nom[0]='\0';
	strcat(nom,nom_donne);
	Frame* frames2 = new Frame[n_frames_donne];
	copy(frames_donne.begin(), frames_donne.end(), frames2); // La liste de Frame passee en argument est transformee en un vecteur de Frame
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

char* Morceau::get_nom(){
	return nom;
}
