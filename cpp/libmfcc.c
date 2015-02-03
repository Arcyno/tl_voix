/*
 * libmfcc.c - Code implementation for libMFCC
 * Copyright (c) 2010 Jeremy Sawruk
 *
 * This code is released under the MIT License. 
 * For conditions of distribution and use, see the license in LICENSE
 */

#include <math.h>
#include "libmfcc.h"




double log_adapte(double var){
	if (var > 0.0f){
		return log(var);
	}
	return var;
}



/* 
 * Computes the specified (mth) MFCC
 *
 * spectralData - array of doubles containing the results of FFT computation. This data is already assumed to be purely real
 * samplingRate - the rate that the original time-series data was sampled at (i.e 44100)
 * NumFilters - the number of filters to use in the computation. Recommended value = 48
 * binSize - the size of the spectralData array, usually a power of 2
 * m - The mth MFCC coefficient to compute
 *
 */
 double GetCoefficient(double* spectralData, unsigned int samplingRate, unsigned int NumFilters, unsigned int binSize, unsigned int m)
 {
 	double result = 0.0f;
 	double outerSum = 0.0f;
 	double innerSum[8] = {0.0f};
 	unsigned int k, l;

	// 0 <= m < L
 	if(m >= NumFilters)
 	{
		// This represents an error condition - the specified coefficient is greater than or equal to the number of filters. The behavior in this case is undefined.
 		return 0.0f;
 	}

 	result = NormalizationFactor(NumFilters, m);

 	for(l = 1; l <= NumFilters; l++)
 	{
		// Compute inner sum
 		innerSum[0] = 0.0f;
 		innerSum[1] = 0.0f;
 		innerSum[2] = 0.0f;
 		innerSum[3] = 0.0f;
 		innerSum[4] = 0.0f;
 		innerSum[5] = 0.0f;
 		innerSum[6] = 0.0f;
 		innerSum[7] = 0.0f;
 		innerSum[8] = 0.0f;
 		innerSum[9] = 0.0f;
 		innerSum[10] = 0.0f;
 		innerSum[11] = 0.0f;
 		innerSum[12] = 0.0f;
 		innerSum[13] = 0.0f;
 		innerSum[14] = 0.0f;
 		innerSum[15] = 0.0f;

 		for(k = 0; k < binSize - 1; k+=16)
 		{
 			innerSum[0] += fabs(spectralData[k] * GetFilterParameter(samplingRate, binSize, k, l));
 			innerSum[1] += fabs(spectralData[k+1] * GetFilterParameter(samplingRate, binSize, k, l));
 			innerSum[2] += fabs(spectralData[k+2] * GetFilterParameter(samplingRate, binSize, k, l));
 			innerSum[3] += fabs(spectralData[k+3] * GetFilterParameter(samplingRate, binSize, k, l));
 			innerSum[4] += fabs(spectralData[k+4] * GetFilterParameter(samplingRate, binSize, k, l));
 			innerSum[5] += fabs(spectralData[k+5] * GetFilterParameter(samplingRate, binSize, k, l));
 			innerSum[6] += fabs(spectralData[k+6] * GetFilterParameter(samplingRate, binSize, k, l));
 			innerSum[7] += fabs(spectralData[k+7] * GetFilterParameter(samplingRate, binSize, k, l));
 			innerSum[8]  += fabs(spectralData[k+8] * GetFilterParameter(samplingRate, binSize, k, l));
 			innerSum[9]  += fabs(spectralData[k+9] * GetFilterParameter(samplingRate, binSize, k, l));
 			innerSum[10] += fabs(spectralData[k+10] * GetFilterParameter(samplingRate, binSize, k, l));
 			innerSum[11] += fabs(spectralData[k+11] * GetFilterParameter(samplingRate, binSize, k, l));
 			innerSum[12] += fabs(spectralData[k+12] * GetFilterParameter(samplingRate, binSize, k, l));
 			innerSum[13] += fabs(spectralData[k+13] * GetFilterParameter(samplingRate, binSize, k, l));
 			innerSum[14] += fabs(spectralData[k+14] * GetFilterParameter(samplingRate, binSize, k, l));
 			innerSum[15] += fabs(spectralData[k+15] * GetFilterParameter(samplingRate, binSize, k, l));
 				   // std::cout << "GetFilterParameter" << GetFilterParameter(samplingRate, binSize, k, l) << std::endl;

 		}

		double sumTot = innerSum[0]+innerSum[1]+innerSum[2]+innerSum[3]+innerSum[4]+innerSum[5]+innerSum[6]+innerSum[7]+innerSum[8]+innerSum[9]+innerSum[10]+innerSum[11]+innerSum[12]+innerSum[13]+innerSum[14]+innerSum[15];

 	// 	if(innerSum > 0.0f)
 	// 	{

			// innerSum = log(innerSum); // The log of 0 is undefined, so don't use it

		// }
		// innerSum = innerSum * cos(((m * PI) / NumFilters) * (l - 0.5f));


		sumTot = log_adapte(sumTot) * cos(((m * PI) / NumFilters) * (l - 0.5f));

		// innerSum[0] = log_adapte(innerSum[0]) * cos(((m * PI) / NumFilters) * (l - 0.5f));
		// innerSum[1] = log_adapte(innerSum[1]) * cos(((m * PI) / NumFilters) * (l - 0.5f));
		// innerSum[2] = log_adapte(innerSum[2]) * cos(((m * PI) / NumFilters) * (l - 0.5f));
		// innerSum[3] = log_adapte(innerSum[3]) * cos(((m * PI) / NumFilters) * (l - 0.5f));
		// innerSum[4] = log_adapte(innerSum[4]) * cos(((m * PI) / NumFilters) * (l - 0.5f));
		// innerSum[5] = log_adapte(innerSum[5]) * cos(((m * PI) / NumFilters) * (l - 0.5f));
		// innerSum[6] = log_adapte(innerSum[6]) * cos(((m * PI) / NumFilters) * (l - 0.5f));
		// innerSum[7] = log_adapte(innerSum[7]) * cos(((m * PI) / NumFilters) * (l - 0.5f));
		// innerSum[8]  = log_adapte(innerSum[8]) * cos(((m * PI) / NumFilters) * (l - 0.5f));
		// innerSum[9]  = log_adapte(innerSum[9]) * cos(((m * PI) / NumFilters) * (l - 0.5f));
		// innerSum[10] = log_adapte(innerSum[10]) * cos(((m * PI) / NumFilters) * (l - 0.5f));
		// innerSum[11] = log_adapte(innerSum[11]) * cos(((m * PI) / NumFilters) * (l - 0.5f));
		// innerSum[12] = log_adapte(innerSum[12]) * cos(((m * PI) / NumFilters) * (l - 0.5f));
		// innerSum[13] = log_adapte(innerSum[13]) * cos(((m * PI) / NumFilters) * (l - 0.5f));
		// innerSum[14] = log_adapte(innerSum[14]) * cos(((m * PI) / NumFilters) * (l - 0.5f));
		// innerSum[15] = log_adapte(innerSum[15]) * cos(((m * PI) / NumFilters) * (l - 0.5f));

		// outerSum += innerSum[0]+innerSum[1]+innerSum[2]+innerSum[3]+innerSum[4]+innerSum[5]+innerSum[6]+innerSum[7]+innerSum[8]+innerSum[9]+innerSum[10]+innerSum[11]+innerSum[12]+innerSum[13]+innerSum[14]+innerSum[15];
		outerSum += sumTot;
	}

	result *= outerSum;
	   // std::cout << "result " << outerSum << std::endl;

   // std::cout << "time1 = " << (t22-t2) << std::endl;
   // std::cout << "time3 = " << (t4-t3) << std::endl;

	return result;
}


/* 
 * Computes the Normalization Factor (Equation 6)
 * Used for internal computation only - not to be called directly
 */
 double NormalizationFactor(int NumFilters, int m)
 {
 	double normalizationFactor = 0.0f;

 	if(m == 0)
 	{
 		normalizationFactor = sqrt(1.0f / NumFilters);
 	}
 	else 
 	{
 		normalizationFactor = sqrt(2.0f / NumFilters);
 	}

 	return normalizationFactor;
 }

/* 
 * Compute the filter parameter for the specified frequency and filter bands (Eq. 2)
 * Used for internal computation only - not the be called directly
 */
 double GetFilterParameter(unsigned int samplingRate, unsigned int binSize, unsigned int frequencyBand, unsigned int filterBand)
 {
 	double filterParameter = 0.0f;

	double boundary = (frequencyBand * samplingRate) / binSize;		// k * Fs / N
	double prevCenterFrequency = GetCenterFrequency(filterBand - 1);		// fc(l - 1) etc.
	double thisCenterFrequency = GetCenterFrequency(filterBand);
	double nextCenterFrequency = GetCenterFrequency(filterBand + 1);

	if(boundary >= 0 && boundary < prevCenterFrequency)
	{
		filterParameter = 0.0f;
	}
	else if(boundary >= prevCenterFrequency && boundary < thisCenterFrequency)
	{
		filterParameter = (boundary - prevCenterFrequency) / (thisCenterFrequency - prevCenterFrequency);
		filterParameter *= GetMagnitudeFactor(filterBand);
	}
	else if(boundary >= thisCenterFrequency && boundary < nextCenterFrequency)
	{
		filterParameter = (boundary - nextCenterFrequency) / (thisCenterFrequency - nextCenterFrequency);
		filterParameter *= GetMagnitudeFactor(filterBand);
	}
	else if(boundary >= nextCenterFrequency && boundary < samplingRate)
	{
		filterParameter = 0.0f;
	}

	return filterParameter;
}

/* 
 * Compute the band-dependent magnitude factor for the given filter band (Eq. 3)
 * Used for internal computation only - not the be called directly
 */
 double GetMagnitudeFactor(unsigned int filterBand)
 {
 	double magnitudeFactor = 0.0f;

 	if(filterBand >= 1 && filterBand <= 14)
 	{
 		magnitudeFactor = 0.015;
 	}
 	else if(filterBand >= 15 && filterBand <= 48)
 	{
 		magnitudeFactor = 2.0f / (GetCenterFrequency(filterBand + 1) - GetCenterFrequency(filterBand -1));
 	}

 	return magnitudeFactor;
 }

/*
 * Compute the center frequency (fc) of the specified filter band (l) (Eq. 4)
 * This where the mel-frequency scaling occurs. Filters are specified so that their
 * center frequencies are equally spaced on the mel scale
 * Used for internal computation only - not the be called directly
 */
 double GetCenterFrequency(unsigned int filterBand)
 {
 	double centerFrequency = 0.0f;
 	double exponent;

 	if(filterBand == 0)
 	{
 		centerFrequency = 0;
 	}
 	else if(filterBand >= 1 && filterBand <= 14)
 	{
 		centerFrequency = (200.0f * filterBand) / 3.0f;
 	}
 	else
 	{
 		exponent = filterBand - 14.0f;
 		centerFrequency = pow(1.0711703, exponent);
 		centerFrequency *= 1073.4;
 	}

 	return centerFrequency;
 }