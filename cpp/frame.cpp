#include "frame.h"
#include "../lib/include/aquila/transform/Mfcc.h"
#include <iostream>
#include "debug.h"

	Frame::Frame(){
		taille = 0;
	}
	

	Frame::Frame(double* signal_donne, int taille_donne, int ordre_lpc, int nb_mfcc){
		signal = signal_donne;
		taille = taille_donne;
		set_lpc(ordre_lpc);
		set_mfcc(nb_mfcc);
	}

	void Frame::set_lpc(int ordre_lpc){

		lpc = new double[ordre_lpc]; 
		double autocorr[taille - ordre_lpc];

		std::cout << "taille : " << taille;
		for(int i = ordre_lpc+1; i < taille; i++){
			autocorr[i-(ordre_lpc+1)] = 0;
			for(int j = 0; j <= i ; j++){
				if (i == 25)
				std::cout << "signal decale : " << signal[taille +j-i-1] << std::endl;
				autocorr[i-(ordre_lpc+1)] += signal[j] * signal[taille +j-i-1];
			}
  		}


		///////////// test en cours
		std::cout << "Contenu de autocorr : " << autocorr;

		double tmp = -autocorr[1]/autocorr[0];
		lpc[0]=(tmp);
		double sigma2 = (1 - tmp*tmp) * autocorr[0];

		for(int i = 1; i < ordre_lpc; i++){
			tmp = autocorr[i+1];
			for (int k = 1; k < i; ++k){
				tmp += lpc[k-1]*autocorr[i-k];
			}
			tmp /= -sigma2;

			for (int k = 0; k < i-1; ++k){
				lpc[k] += tmp*lpc[i-k-1];
			}
			lpc[i-1] = tmp;

        	sigma2 *=  (1 - tmp*tmp);
		}

	}

	void Frame::set_mfcc(int nb_mfcc){
		
	}

	double* Frame::get_lpc(){


		return lpc;
	}

	double* Frame::get_mfcc(){

		return mfcc;
	}
