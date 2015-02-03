#pragma once


#include <list>
#include "frame.h"


class Morceau {

private:
	
	// attributs
	Frame* frames; 
	int n_frames;
	int classe;

public:

	// Constructeur par défaut
	Morceau();
	
	// Constructeur
	Morceau(std::list<Frame> frames_donne, int n_frames_donne, int classe_donne);

	int get_n_frames();

	Frame* get_frames();

	int get_classe();

};