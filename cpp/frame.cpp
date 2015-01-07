#include "frame.h"
#include "../lib/include/aquila/transform/Mfcc.h"
#include <iostream>
#include "debug.h"
#include <string.h>
#include <math.h>

#define SWAP(a,b) tempr=(a);(a)=(b);(b)=tempr
#define pi 3.14159265358979323846264338327

//probleme fft : on n'a pas la mm chose que matlab (mm pas symétrique ??)
// attention il faut des puissance de 2 en entrée fft
Frame::Frame(){
	taille = 0;
}


Frame::Frame(double* signal_donne, int taille_donne, int f_ech_donne, int ordre_lpc, int nb_mfcc){
	signal = signal_donne;
	taille = taille_donne;
	f_ech = f_ech_donne;
	set_lpc(ordre_lpc);
	set_mfcc(nb_mfcc);
}


void Frame::set_lpc(int ordre_lpc){

	lpc = new double[ordre_lpc]; 
	double lpc_up[ordre_lpc]; 
	double autocorr[taille - ordre_lpc];

		// Autocorrélation jusqu'à l'ordre des lpc
	for(int i = 0; i < ordre_lpc; i++){
		autocorr[i] = 0;
		for(int j = 0; j <= taille-1-i ; j++){
			autocorr[i] += signal[j] * signal[j + i] / taille;
		}
	}
  		// std::cout << "Contenu de autocorr : " << autocorr << std::endl;


	double tmp = -autocorr[1]/autocorr[0];
	lpc[0]=(tmp);
	double sigma2 = (1 - tmp*tmp) * autocorr[0];

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
		sigma2 *=  (1 - tmp*tmp);
	}
		// std::cout << "Contenu de lpc : " << lpc << std::endl;
}


void Frame::set_mfcc(int nb_mfcc){
		//création du tableau de données en cplxe
		//les index pairs correspondent aux réels

	float signal_cpx[taille*2];
	for(int i=0; i < taille; i++){
		signal_cpx[2*i]=signal[i];
		signal_cpx[2*i+1]=0;
	}
	float* fft = ComplexFFT(signal_cpx,taille,f_ech,1);
		//normalisation
	for(int i=0; i < taille; i++){
		fft[i]=sqrt(fft[2*i]*fft[2*i]+fft[2*i+1]*fft[2*i+1]);
		std::cout << "ff_cpp(" <<i+1<<") = " << fft[i] << ";"<< std::endl;
	}
	// std::cout << "COUCOU fft : " << fft << std::endl;
}

double* Frame::get_lpc(){


	return lpc;
}

double* Frame::get_mfcc(){

	return mfcc;
}

float* Frame::ComplexFFT(float data[], unsigned long number_of_samples, unsigned int sample_rate, int sign)
{

	//variables for the fft
	unsigned long n,mmax,m,j,istep,i;
	double wtemp,wr,wpr,wpi,wi,theta,tempr,tempi;

	//the complex array is real+complex so the array 
    //as a size n = 2* number of complex samples
    //real part is the data[index] and 
    //the complex part is the data[index+1]

	//new complex array of size n=2*sample_rate
	float* vector=new float [2*sample_rate];

	//put the real array in a complex array
	//the complex part is filled with 0's
	//the remaining vector with no data is filled with 0's
	for(n=0; n<sample_rate;n++)
	{
		if(n<number_of_samples)
			vector[2*n]=data[n];
		else
			vector[2*n]=0;
		vector[2*n+1]=0;
	}

	//binary inversion (note that the indexes 
    //start from 0 witch means that the
    //real part of the complex is on the even-indexes 
    //and the complex part is on the odd-indexes)
	n=sample_rate << 1;
	j=0;
	for (i=0;i<n/2;i+=2) {
		if (j > i) {
			SWAP(vector[j],vector[i]);
			SWAP(vector[j+1],vector[i+1]);
			if((j/2)<(n/4)){
				SWAP(vector[(n-(i+2))],vector[(n-(j+2))]);
				SWAP(vector[(n-(i+2))+1],vector[(n-(j+2))+1]);
			}
		}
		m=n >> 1;
		while (m >= 2 && j >= m) {
			j -= m;
			m >>= 1;
		}
		j += m;
	}
	//end of the bit-reversed order algorithm

	//Danielson-Lanzcos routine
	mmax=2;
	while (n > mmax) {
		istep=mmax << 1;
		theta=sign*(2*pi/mmax);
		wtemp=sin(0.5*theta);
		wpr = -2.0*wtemp*wtemp;
		wpi=sin(theta);
		wr=1.0;
		wi=0.0;
		for (m=1;m<mmax;m+=2) {
			for (i=m;i<=n;i+=istep) {
				j=i+mmax;
				tempr=wr*vector[j-1]-wi*vector[j];
				tempi=wr*vector[j]+wi*vector[j-1];
				vector[j-1]=vector[i-1]-tempr;
				vector[j]=vector[i]-tempi;
				vector[i-1] += tempr;
				vector[i] += tempi;
			}
			wr=(wtemp=wr)*wpr-wi*wpi+wr;
			wi=wi*wpr+wtemp*wpi+wi;
		}
		mmax=istep;
	}
	//end of the algorithm
	
	//determine the fundamental frequency
	//look for the maximum absolute value in the complex array
	auto fundamental_frequency=0;
	for(i=2; i<=sample_rate; i+=2)
	{
		if((pow(vector[i],2)+pow(vector[i+1],2))>(pow(vector[fundamental_frequency],2)+pow(vector[fundamental_frequency+1],2))){
			fundamental_frequency=i;
		}
	}

	//since the array of complex has the format [real][complex]=>[absolute value]
	//the maximum absolute value must be ajusted to half
	fundamental_frequency=(long)floor((float)fundamental_frequency/2);
	return vector;
}
