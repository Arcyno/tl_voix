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
		double lpc_up[ordre_lpc]; 
		double autocorr[taille - ordre_lpc];

		for(int i = 0; i < ordre_lpc; i++){
			autocorr[i] = 0;
			for(int j = 0; j <= taille-1-i ; j++){
				autocorr[i] += signal[j] * signal[j + i];
			}
  		}
  		///////////// test en cours
		std::cout << "Contenu de autocorr : " << autocorr << std::endl;


		double tmp = -autocorr[1]/autocorr[0];
		lpc[0]=(tmp);
		double sigma2 = (1 - tmp*tmp) * autocorr[0];

		for(int k = 2; k< ordre_lpc; k++){
			tmp = autocorr[k];

			for (int j = 1; j < k; ++j){
				tmp += lpc[j-1]*autocorr[k-j];
			}
			tmp /= -sigma2;

			//std::cout << "tmp = " << tmp << std::endl;

			for (int j = 1; j < k; ++j){
				if (k==3){
					std::cout << "j = " << j << std::endl;
					std::cout << "tmp = " << tmp << std::endl;
					std::cout << "lpc[j-1] = " << lpc[j-1] << std::endl;
					std::cout << "lpc[k-1-j] = " << lpc[k-1-j] << std::endl;
				}
				lpc_up[j-1] = lpc[j-1] + tmp*lpc[k-1-j];
			}
			lpc = lpc_up; //attention recopie, faire une boucle ?
			lpc[k-1] = tmp;

        	sigma2 *=  (1 - tmp*tmp);
		}
		///////////// test en cours
		std::cout << "Contenu de lpc : " << lpc << std::endl;
	}

	void Frame::set_mfcc(int nb_mfcc){
		
	}

	double* Frame::get_lpc(){


		return lpc;
	}

	double* Frame::get_mfcc(){

		return mfcc;
	}
