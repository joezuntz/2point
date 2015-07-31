#include "fitsio.h"

// Main library function.
// Purpose: Save xi plus and xi minus data to a FITS file
//     filename: the filename to create
//     ntheta: the number of angular bins
//     ntomo:  the number of tomographic bins
//     theta_min: lower edge of angular bins length ntheta
//     theta_mean: mid-point of angular bins length ntheta
//     theta_max: upper edge of angular bins length ntheta
//     xi_plus: upper edge of angular bins length ((ntomo*(ntomo+1))/2)*ntheta
//     xi_minue: xi_minus data, length ((ntomo*(ntomo+1))/2)*ntheta

//expected ordering in xi_minus and xi_plus is (e.g. for ntheta=6, ntomo=3):
//     xi_1_1(theta_1) xi_1_1(theta_2) ... xi_1_1(theta_6)
// ... xi_1_2(theta_1) xi_1_2(theta_2) ... xi_1_2(theta_6)
// ... xi_1_3(theta_1) xi_1_3(theta_2) ... xi_1_3(theta_6)
// ... xi_2_2(theta_1) xi_2_2(theta_2) ... xi_2_2(theta_6)
// ... xi_2_3(theta_1) xi_2_3(theta_2) ... xi_2_3(theta_6)
// ... xi_3_3(theta_1) xi_3_3(theta_2) ... xi_3_3(theta_6)

int twopt_save_xi(
	char * filename,
	int ntheta, int ntomo, 
	double * theta_min, double * theta_mean, double * theta_max,  // in arcmin
	double * xi_plus, double * xi_minus);


// Utility function to write a complete FITS file column
void twopt_fits_write_whole_column(fitsfile * fptr, int datatype, int colnum, int len, 
	void * col, int * status);

// Internal function to write extension.
void twopt_write_extension(fitsfile * fptr, int ntheta, int ntomo, int nrow,
	double * theta_min, double * theta_mean, double * theta_max, double *xi,
	int *status);

