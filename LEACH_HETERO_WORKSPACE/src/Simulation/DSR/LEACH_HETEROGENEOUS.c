/************************************************************************************
 * Copyright (C) 2016                                                               *
 * TETCOS, Bangalore. India                                                         *
 *                                                                                  *
 * Tetcos owns the intellectual property rights in the Product and its content.     *
 * The copying, redistribution, reselling or publication of any or all of the       *
 * Product or its content without express prior written consent of Tetcos is        *
 * prohibited. Ownership and / or any other right relating to the software and all  *
 * intellectual property rights therein shall remain at all times with Tetcos.      *
 *                                                                                  *
 * Author:    Kanagaraj K                                                           *
 *                                                                                  *
 * ---------------------------------------------------------------------------------*/

 /**********************************IMPORTANT NOTES***********************************
 1. This file contains the LEACH_HETERO code.
 2. The Network scenario can contain any number of sensors
 3. Mobility can be set to sensors by setting velocity which is set to NO_MOBILITY by default
 ************************************************************************************/


#include "main.h"
#include "DSR.h"
#include "List.h"
#include "Stack.h"
#include "../BatteryModel/BatteryModel.h"
#include "../ZigBee/802_15_4.h"

int *ClusterElements;
int *CH = NULL;
int CL_COUNT = 0;
int CL_POSITION = 0;

int fn_NetSim_LEACH_HETERO_CheckDestination(NETSIM_ID nDeviceId, NETSIM_ID nDestinationId)
//Function to check whether the Device ID is same as the Destination ID
{
	if (nDeviceId == nDestinationId)
		return 1;
	else
		return 0;
}

int fn_NetSim_LEACH_HETERO_GetNextHop(NetSim_EVENTDETAILS* pstruEventDetails)
//Function to determine the DeviceId of the next hop
{
	int nextHop = 0;
	NETSIM_ID nInterface;
	int i;
	int ClusterId;
	int cl_flag = 0;
	double energy = 0.0;
	int tx_id = pstruEventDetails->nDeviceId;
	//If the sensor is the Cluster Head, it forwards it to the Sink.
	//Otherwise, it forwards the packet to the Cluster Head of its cluster.
	if (!strcmp(DEVICE(tx_id)->type, "SENSOR"))
	{
		energy = battery_get_remaining_energy((ptrBATTERY)WSN_PHY(tx_id)->battery);
		if (energy <= 0.0) 
			return 0;
	}
repeat:
	{
		//int cl_id = ClusterElements[pstruEventDetails->nDeviceId];
		if (tx_id == sinkid|| fn_NetSim_LEACH_HETERO_CheckIfCH(tx_id))
			nextHop = get_first_dest_from_packet(pstruEventDetails->pPacket);
		else
		nextHop = ClusterElements[tx_id];

		if (nextHop == sinkid)
		{
			double dis1 = 0, dis2 = 0;
			dis1=DEVICE_DISTANCE(tx_id, sinkid);
			dis2=DEVICE_DISTANCE(tx_id, get_first_dest_from_packet(pstruEventDetails->pPacket));
			if (dis1 > dis2)
				nextHop = get_first_dest_from_packet(pstruEventDetails->pPacket);
		}
		
	}
	
	if (!strcmp(DEVICE(nextHop)->type, "SENSOR")&& WSN_MAC(nextHop)->nNodeStatus == OFF)
	{
		fprintf(stderr, "\nNode already dead\n");
		
		fn_NetSim_LEACH_HETERO_Init();
		fn_NetSim_LEACH_HETERO_run();
		goto repeat;
	}

		//Updating the Transmitter ID, Receiver ID and NextHopIP in the pstruEventDetails
	free(pstruEventDetails->pPacket->pstruNetworkData->szNextHopIp);
	pstruEventDetails->pPacket->pstruNetworkData->szNextHopIp = dsr_get_dev_ip(nextHop);
	pstruEventDetails->pPacket->nTransmitterId = pstruEventDetails->nDeviceId;
	pstruEventDetails->pPacket->nReceiverId = nextHop;

	return 1;
}

int fn_NetSim_LEACH_HETERO_run()
{
	if(sensor_count>0)
	fn_netsim_matlab_run(sensor_count);
	return 1;
}

int fn_NetSim_LEACH_HETERO_form_cluster(double *cl_count, double* cl_head, double* cl_id, int* temp)
//Cluster heads are assigned to respective clusters using the data obtained from MATLAB
{
	int i = 0,j=0, flag=0;
	CL_COUNT = (int)cl_count[0];
	fprintf(stderr, "\nCluster Heads:\n");
	for (i = 0; i < NETWORK->nDeviceCount; i++)
	{
		for (j = 0; j < CL_COUNT; j++)
		{
			if (cl_head[j] == i + 1)
			{
				CH[j + 1] = (int)cl_head[j];
				fprintf(stderr, "\nCluster %d:  Device %d", j + 1, CH[j + 1]);
			}
		}
		flag = 0;
		if (!strcmp(DEVICE(i + 1)->type, "SENSOR"))
		{
			for (j = 0; j < sensor_count; j++)
			{
				if (temp[j] == i + 1)
				{
					ClusterElements[i + 1] = cl_id[j];
					flag = 1;
				}
			}
			if (!flag)
				ClusterElements[i + 1] = CL_COUNT;
		}
	}
	
	return 1;
}

void fn_NetSim_LEACH_HETERO_Init()
{
	int i = 0;
	sensor_count = 0;
		
	for (i = 0; i < NETWORK->nDeviceCount; i++)
	{
		
		if (!strcmp(DEVICE(i + 1)->type, "SENSOR") && WSN_MAC(i + 1)->nNodeStatus != OFF)
		{
			double energy = battery_get_remaining_energy((ptrBATTERY)WSN_PHY(i + 1)->battery);
			if (energy > 0.0)
				sensor_count++;
		}
	}
	if(sensor_count>0)
	ClusterElements = (int*)calloc(sensor_count+1, sizeof*(ClusterElements));
	CH = (int*)calloc(sensor_count, sizeof * (CH));
}

int fn_NetSim_LEACH_HETERO_CheckIfCH(NETSIM_ID nDeviceId)
//Function to check whether the Device ID is same as the Destination ID
{
	int j = 0;
	for (j = 0; j < CL_COUNT; j++)
	{
		if (nDeviceId == CH[j])
			return 1;		
	}
	return 0;
}

