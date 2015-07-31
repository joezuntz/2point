/*
	Read the cosmolike datav_cosmolike file and write out to FITS 2pt format.

*/
#include "stdio.h"
#include "shear_only.h"


int main(){

	int ntheta = 6;
	int ntomo = 3;
	int nrow = ((ntomo*(ntomo+1))/2)*ntheta;

	double theta_max[6]  = {4.610116e+00, 1.062659e+01, 2.449490e+01, 5.646216e+01, 1.301486e+02, 3.000000e+02};
	double theta_min[6]  = {2.000000e+00, 4.610116e+00, 1.062659e+01, 2.449490e+01, 5.646216e+01, 1.301486e+02};
	double theta_mean[6] = {3.476833e+00, 8.014302e+00, 1.847343e+01, 4.258233e+01, 9.815475e+01, 2.262524e+02};

	double xi_plus[nrow];
	double xi_minus[nrow];

	FILE * infile = fopen("datav_cosmolike", "r");
	if (!infile){
		fprintf(stderr, "Need file datav_cosmolike to proceed.\n");
		exit(1);
	}
	int p;


	//Read in xi plus
	p=0;
 	for (int i=0; i<ntomo; i++){
 		for (int j=i; j<ntomo; j++){
 			for (int t=0; t<ntheta; t++){
 				int dummy;
 				fscanf(infile, "%d %le\n", &dummy, xi_plus+p);
 				p++;
 			}
 		}
 	}

	//Read in xi minus
	p=0;
 	for (int i=0; i<ntomo; i++){
 		for (int j=i; j<ntomo; j++){
 			for (int t=0; t<ntheta; t++){
 				int dummy;
 				fscanf(infile, "%d %le\n", &dummy, xi_minus+p);
 				p++;
 			}
 		}
 	}


	int status = twopt_save_xi("datavector_cosmolike.fits",
		ntheta, ntomo, 
		theta_min, theta_mean, theta_max,
		xi_plus, xi_minus);

	return status;

}