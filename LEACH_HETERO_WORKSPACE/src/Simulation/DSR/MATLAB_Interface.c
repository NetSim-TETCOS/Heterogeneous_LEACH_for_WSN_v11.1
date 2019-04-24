/*
 *
 *	This is a simple program that illustrates how to call the MATLAB
 *	Engine functions from NetSim C Code.
 *
 */
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "engine.h"
#include "mat.h"
#include "mex.h"
#include "main.h"
#include "../ZigBee/802_15_4.h"
#include "../BatteryModel/BatteryModel.h"
#include "direct.h"
#include "DSR.h"


char buf[BUFSIZ];
Engine *ep;
int status;
mxArray *Did = NULL, *NDid = NULL, *Xc = NULL, *Yc = NULL, *Pwr = NULL;
mxArray *out;

double fn_netsim_matlab_init()
{
	/*
	 * Start the MATLAB engine Process
	 */
	fprintf(stderr, "\nPress any key to start MATLAB...\n");
	_getch();
	if (!(ep = engOpen(NULL))) {
		MessageBox((HWND)NULL, (LPCWSTR)"Can't start MATLAB engine",
			(LPCWSTR) "MATLAB_Interface.c", MB_OK);
		exit(-1);
	}
	//Comment this line to supress MATLAB desktop window from loading
	engEvalString(ep, "desktop");

	//Setting the figure window size as per the system screen resolution
	/*int i = 0;
	sprintf(buf, "scrsz = get(groot, 'ScreenSize')");
	engEvalString(ep, buf);
	sprintf(buf, "fig1 = gcf");
	engEvalString(ep, buf);
	sprintf(buf, "fig1.Position = [scrsz(3) / 100 scrsz(4) / 10 scrsz(3) / 1.03 scrsz(4) / 1.3]");
	engEvalString(ep, buf);*/
	return 0;
}

void fn_netsim_matlab_run()
{
	int i = 0;
	double sinkx = 0, sinky = 0;
	double* num_clusters = NULL, * cluster_id = NULL, * cluster_head = NULL;
	int *temp_order=NULL;
	int grid_length;

	//Writing Input Log file for MATLAB to read
	FILE *fp = NULL;
	char filename[BUFSIZ];
	strcpy(filename, pszAppPath);
	strcat(filename, "\\netsim_input.csv");	
	fp = fopen(filename, "w+");
	if (fp)
	{
		fprintf(fp, "Device_Number,X,Y,Remaining Energy(mJ),Device_Id\n");
		fclose(fp);
	}

	char* temp = NULL;
	temp = getenv("NETSIM_SIM_AREA_X");
	if (temp)
	{
		if (atoi(temp))
			grid_length = atoi(temp);
		else
			grid_length = 0xFFFFFFFF;
	}
	

	temp_order = (int*)calloc(sensor_count, sizeof*(temp_order));
	int devid = 0;
	fp = fopen(filename, "a+");
	for (i = 0; i < NETWORK->nDeviceCount; i++)
	{
		if (!strcmp(DEVICE(i + 1)->type, "SINKNODE"))
		{
			sinkx = DEVICE_POSITION(i + 1)->X;
			sinky = DEVICE_POSITION(i + 1)->Y;
			sinkid = i + 1;
			continue;
		}
		double energy = battery_get_remaining_energy((ptrBATTERY)WSN_PHY(i + 1)->battery);
		if (!strcmp(DEVICE(i + 1)->type, "SENSOR") && WSN_MAC(i + 1)->nNodeStatus != OFF && energy>0.0)
		{	
			if (fp)
				fprintf(fp, "%d,%f,%f,%f,%d\n", (devid+1), DEVICE_POSITION(i + 1)->X, DEVICE_POSITION(i + 1)->Y,
				energy,(i+1));
			temp_order[devid] = (i + 1);
			devid++;
		}
		
	}
	if (fp)
		fclose(fp);

	//Reading the log file and creating workspace variables in MATLAB
	sprintf(buf, "Did=csvread('%s',1,0,[1,0,%d,0])",filename, sensor_count);
	engEvalString(ep, buf);
	sprintf(buf, "Xc=csvread('%s',1,1,[1,1,%d,1])", filename, sensor_count);
	engEvalString(ep, buf);
	sprintf(buf, "Yc=csvread('%s',1,2,[1,2,%d,2])", filename, sensor_count);
	engEvalString(ep, buf);
	sprintf(buf, "Pwr=csvread('%s',1,3,[1,3,%d,3])", filename, sensor_count);
	engEvalString(ep, buf);
	sprintf(buf, "NDid=csvread('%s',1,4,[1,4,%d,4])", filename, sensor_count);
	engEvalString(ep, buf);	

	//Passing input parameters to MATLAB Script file
	sprintf(buf, "[num_cls,cl_head,cl_mem]=LEACH_HETERO(NDid,Xc,Yc,Pwr,%d,%f,%f,%d,%d)", sensor_count, sinkx, sinky, grid_length, sinkid);
	status = engEvalString(ep, buf);

	//Reading MATLAB workspace variables for calculations in NetSim
	out = engGetVariable(ep, "num_cls");//contains the total number of cluster heads elected
	num_clusters = mxGetPr(out);

	out = engGetVariable(ep, "cl_head");//contains the cluster head id
	cluster_head = mxGetPr(out);

	out = engGetVariable(ep, "cl_mem");//contains the cluster head of each sensor
	cluster_id = mxGetPr(out);

	fn_NetSim_LEACH_HETERO_form_cluster(num_clusters, cluster_head, cluster_id, temp_order);
}

double fn_netsim_matlab_finish()
{
	//Close the MATLAB Engine Process
	fprintf(stderr, "\nPress any key to close MATLAB...\n");
	_getch();
	status = engEvalString(ep, "exit");
	return 0;
}

