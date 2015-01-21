#pragma once


#include <list>
#include "frame.h"


class Morceau {

private:
	
	Frame* frames;
	int n_frames;

public:

	Morceau();

	Morceau(std::list<Frame> frames_donne, int n_frames_donne);

	int get_n_frames();

	Frame* get_frames();

};