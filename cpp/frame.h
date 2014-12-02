#pragma once
#include <iostream>


class Frame {
private:
	double* signal;
	int taille;
	double* lpc;
	double mfcc[];

	void set_lpc(int ordre_lpc);

	void set_mfcc(int nb_mfcc);

public:

	Frame(); //constructeur par défaut

	Frame(double* signal_donne, int taille_donne, int ordre_lpc, int nb_mfcc);

	//std::ostream& operator<<(std::ostream &out, const double* tab);

	double* get_lpc();

	double* get_mfcc();

};
