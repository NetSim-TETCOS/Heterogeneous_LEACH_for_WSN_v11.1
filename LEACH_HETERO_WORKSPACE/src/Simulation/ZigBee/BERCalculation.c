/************************************************************************************
 * Copyright (C) 2013                                                               *
 * TETCOS, Bangalore. India                                                         *
 *                                                                                  *
 * Tetcos owns the intellectual property rights in the Product and its content.     *
 * The copying, redistribution, reselling or publication of any or all of the       *
 * Product or its content without express prior written consent of Tetcos is        *
 * prohibited. Ownership and / or any other right relating to the software and all *
 * intellectual property rights therein shall remain at all times with Tetcos.      *
 *                                                                                  *
 * Author:    Shashi Kant Suman                                                       *
 *                                                                                  *
 * ---------------------------------------------------------------------------------*/
# include "main.h"
# include "802_15_4.h"

double fnNCk(int,int);
long long int fnFactorial(int);
#define minSNR -10
#define maxSNR 4
unsigned int entry=maxSNR-minSNR+1;
struct stru_BERTable
{
	double dSNR;
	double dBER;
};
struct stru_BERTable BERTable[maxSNR-minSNR+1]=
{
	//SNR,BER
	{-10,0.1},
	{-9,0.1},
	{-8,0.1},
	{-7,0.1},
	{-6,0.1},
	{-5,0.1},
	{-4,0.01},
	{-3,0.01},
	{-2,0.001},
	{-1,0.001},
	{0,0.0001},
	{1,0.00001},
	{2,0.0000001},
	{3,0.000000001},
	{4,0.0000000001},
};
/** The bit error rate (BER) is the number of bit errors divided by the total number of transferred bits
during a studied time interval. The BER results were obtained using the analytical model from
IEEE standard 802.15.2-2003 [B9]. The calculation follows the approach outlined in 5.3.2 of that
standard.
*/
int fn_NetSim_Zigbee_CalculateBER(double dSNR, double* dBER, double* dErrorRange,NetSim_EVENTDETAILS* pstruEventDetails)
{
	double temp;
	int nSNR=(int)dSNR;
	if(dSNR<minSNR-1)
	{
		*dBER = 1.0;
	}
	else if(dSNR>maxSNR+1)
	{
		*dBER=BERTable[entry-1].dBER;
	}
	else
	{
		*dBER=BERTable[nSNR-minSNR].dBER;
	}
	temp=*dBER;
	*dErrorRange=0;
	while(*dBER<1)
	{
		(*dErrorRange)++;
		(*dBER)*=10;
	}
	*dBER=temp;
	return 0;
}

