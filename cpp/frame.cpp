#include "frame.h"
#include <iostream>
#include <string.h>
#include <math.h>
#include "libmfcc.c"
#include <time.h>

#define SWAP(a,b) tempr=(a);(a)=(b);(b)=tempr
#define pi 3.14159265358979323846264338327


// Constructeur par defaut
Frame::Frame(){
	taille = 0;
}


// Constructeur
Frame::Frame(double* signal_donne, int taille_donne, int f_ech_donne, int ordre_lpc, int nb_mfcc){

    // On se permet la recopie puisqu'au moment d'appeler ce constructeur,
    // le tableau signal_donne est un tampon qui est ensuite ecrase
   std::copy(signal_donne, signal_donne + taille_donne, signal);

   taille = taille_donne;
   f_ech = f_ech_donne;

   set_lpc(ordre_lpc);
   // std::cout << "lpc "<< lpc[0] << std::endl;

   set_mfcc(nb_mfcc);

   n_lpc = ordre_lpc;
   n_mfcc = nb_mfcc;
}


void Frame::set_lpc(int ordre_lpc){

	double lpc_up[ordre_lpc]; 
	double autocorr[taille - ordre_lpc];

	// Calcul de l'autocorrélation jusqu'a l'ordre des lpc
	for(int i = 0; i < ordre_lpc; i++){
		autocorr[i] = 0;
		for(int j = 0; j <= taille-1-i ; j++){
			autocorr[i] += signal[j] * signal[j + i] / taille;
		}
	}

    // Algorithme de Levinson-Durbin
	double tmp = -autocorr[1]/autocorr[0];
	lpc[0]=(tmp);
	double sigma2 = (1 - tmp*tmp)*autocorr[0];

	for(int k = 2; k< ordre_lpc; k++){
		tmp = autocorr[k];

		for (int j = 1; j < k; ++j){
			tmp += lpc[j-1]*autocorr[k-j];
		}
		tmp /= -sigma2;

		for (int j = 1; j < k; ++j){
			lpc_up[j-1] = lpc[j-1] + tmp*lpc[k-1-j];
		}
		memcpy (lpc, lpc_up, sizeof lpc_up);
		lpc[k-1] = tmp;
		sigma2 *= (1 - tmp*tmp);
	}
   lpc[ordre_lpc-1] = 1;
}


void Frame::set_mfcc(int nb_mfcc){

    // initialisation de la FFT (qui est calculee sur place)
	float fft[taille];
	float data_im[taille];
	for(int i = 0 ;i < taille ; i++){
		fft[i] = signal[i];
		data_im[i] = 0;
	}
    // Calcul de la FFT
	FFT(-1, 8, fft, data_im);
    // Formatage du resultat obtenu qui separait la partie reelle et la partie imaginaire
	double fft2[taille];
	for(int i=0; i < taille; i++){
        // On ne conserve que le module
		fft[i] = sqrt(fft[i]*fft[i] + data_im[i]*data_im[i]);
		fft2[i] = static_cast<double>(fft[i]);
	}

	// Calcul des coefficients
	// int coeff;
	// for(coeff = 1; coeff < nb_mfcc + 1; coeff++){
	// 	mfcc[coeff-1] = GetCoefficient(fft2, 16000, nb_mfcc, taille, coeff);
 //         // std::cout << "mfcc[coeff]  "<< mfcc[coeff]  << std::endl;

	// }


   int coeff;
   mfcc[0] = GetCoefficient(fft2, 16000, nb_mfcc, taille, 1);

   for(coeff = 2; coeff < nb_mfcc/2 + 2; coeff++){
      mfcc[coeff-1] = GetCoefficient(fft2, 16000, nb_mfcc, taille, coeff)*pow(0.95,coeff);
         // std::cout << "mfcc[coeff]  "<< coeff+nb_mfcc/2-1 << std::endl;
      mfcc[coeff+nb_mfcc/2-1] = mfcc[coeff-1]-mfcc[coeff-2];
   }
   
}


double* Frame::get_lpc(){
	return lpc;
}


double* Frame::get_mfcc(){
	return mfcc;
}


double* Frame::get_signal(){
   return signal;
}


/*
   This computes an in-place complex-to-complex FFT 
   x and y are the real and imaginary arrays of 2^m points.
   dir =  1 gives forward transform
   dir = -1 gives reverse transform 
*/
   int Frame::FFT(int dir,long m,float *x,float *y){

      long n,i,i1,j,k,i2,l,l1,l2;
      float c1,c2,tx,ty,t1,t2,u1,u2,z;

   /* Calculate the number of points */
      n = 1;
      for (i=0;i<m;i++) 
         n *= 2;

   /* Do the bit reversal */
      i2 = n >> 1;
      j = 0;
      for (i=0;i<n-1;i++) {
         if (i < j) {
            tx = x[i];
            ty = y[i];
            x[i] = x[j];
            y[i] = y[j];
            x[j] = tx;
            y[j] = ty;
         }
         k = i2;
         while (k <= j) {
            j -= k;
            k >>= 1;
         }
         j += k;
      }

   /* Compute the FFT */
      c1 = -1.0; 
      c2 = 0.0;
      l2 = 1;
      for (l=0;l<m;l++) {
         l1 = l2;
         l2 <<= 1;
         u1 = 1.0; 
         u2 = 0.0;
         for (j=0;j<l1;j++) {
            for (i=j;i<n;i+=l2) {
               i1 = i + l1;
               t1 = u1 * x[i1] - u2 * y[i1];
               t2 = u1 * y[i1] + u2 * x[i1];
               x[i1] = x[i] - t1; 
               y[i1] = y[i] - t2;
               x[i] += t1;
               y[i] += t2;
            }
            z =  u1 * c1 - u2 * c2;
            u2 = u1 * c2 + u2 * c1;
            u1 = z;
         }
         c2 = sqrt((1.0 - c1) / 2.0);
         if (dir == 1) 
            c2 = -c2;
         c1 = sqrt((1.0 + c1) / 2.0);
      }

   /* Scaling for forward transform */
      if (dir == 1) {
         for (i=0;i<n;i++) {
            x[i] /= n;
            y[i] /= n;
         }
      }

      return(1);
   }

