#pragma once

#include <iostream>


class Frame {
private:
	double* signal;
	int taille;
	double* lpc;
	double mfcc[];
	int f_ech;

	void set_lpc(int ordre_lpc);

	void set_mfcc(int nb_mfcc);

	int FFT(int dir,long m,float *x,float *y);

public:

	Frame(); //constructeur par défaut

	Frame(double* signal_donne, int taille_donne, int f_ech, int ordre_lpc, int nb_mfcc);

	double* get_lpc();

	double* get_mfcc();

};
