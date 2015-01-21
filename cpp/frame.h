#pragma once

#include <iostream>
#define TAILLE_FRAME 256

class Frame {
private:
	double signal[TAILLE_FRAME];
	int taille;
	double* lpc;
	double mfcc[];
	int f_ech;
	int n_lpc;
	int n_mfcc;

	void set_lpc(int ordre_lpc);

	void set_mfcc(int nb_mfcc);

	int FFT(int dir,long m,float *x,float *y);

public:

	Frame(); //constructeur par défaut

	Frame(double* signal_donne, int taille_donne, int f_ech, int ordre_lpc, int nb_mfcc);

	double* get_lpc();

	double* get_mfcc();

	double* get_signal();

};
