#pragma once

#include <iostream>
#include "parameters.h"

class Frame {
private:
	double signal[TAILLE_FRAME];
	int taille;
	double lpc[ORDRE_LPC];
	double mfcc [NB_MFCC];
	int f_ech;
	int n_lpc;
	int n_mfcc;

	void set_lpc(int ordre_lpc);

	void set_mfcc(int nb_mfcc);

	int FFT(int dir,long m,float *x,float *y);

public:

	Frame(); // Constructeur par défaut

	Frame(double* signal_donne, int taille_donne, int f_ech, int ordre_lpc, int nb_mfcc); // Constructeur

	double* get_lpc();

	double* get_mfcc();

	double* get_signal();

};
