#include "fitsio.h"
#include "stdbool.h"
#include "shear_only.h"



// Debug mode:
#define LOGSTAT if (status!=0) fprintf(stderr, "Error status at line %d in %s = %d. Quitting\n",__LINE__, __FILE__, status); exit(1)

int twopt_save_xi(
	char * filename,
	int ntheta, int ntomo, 
	double * theta_min, double * theta_mean, double * theta_max,  // in arcmin
	double * xi_plus, double * xi_minus)
{
	int status=0;

	// Create the new FITS file.
	fitsfile * fptr;
	fits_create_file(&fptr, filename, &status);
	LOGSTAT;
	// The columns and info for the table
	const int ncol = 5; // bin1 bin2 angbin value ang (ang is optional and nominal) 	
	int nrow = ((ntomo*(ntomo+1))/2)*ntheta;

	char * colnames[5] = {"BIN1", "BIN2", "ANGBIN", "VALUE", "ANG"};
	char * types[5] = {"1J", "1J", "1J", "1D", "1D"};
	char * units[5] = {"", "", "", "", "ARCMIN"};


	// Create the first extension, for xi+.
	fits_create_tbl(fptr, BINARY_TBL, (LONGLONG) nrow, ncol, colnames,
       types, units, "XI_PLUS", &status);
	LOGSTAT;

	// Write inside that it is xi+.
	// Most two-point measurements are a simple <XY>, so in general it is 
	// useful to specify QUANT1=X and QUANT2=Y.
	// xi plus and minus are a bit of an exception as they are a sum of
	// two quantities.
  	fits_write_key_str(fptr, "QUANT1", "G+", "Measurement is xi_+", &status);
  	fits_write_key_str(fptr, "QUANT2", "G+", "Measurement is xi_+", &status);
	LOGSTAT;

  	// Write the theta and xi data for xiplus
	twopt_write_extension(fptr, ntheta, ntomo, nrow, 
		theta_min, theta_mean, theta_max, xi_plus, &status);
	LOGSTAT;

	//Now create ximinus extension
	fits_create_tbl(fptr, BINARY_TBL, (LONGLONG) nrow, ncol, colnames,
       types, units, "XI_MINUS", &status);
	LOGSTAT;

	//Record that this is xi-minus
  	fits_write_key_str(fptr, "QUANT1", "G-", "Measurement is xi_-", &status);
  	fits_write_key_str(fptr, "QUANT2", "G-", "Measurement is xi_-", &status);
	LOGSTAT;

  	//Save data
	twopt_write_extension(fptr, ntheta, ntomo, nrow, 
		theta_min, theta_mean, theta_max, xi_minus, &status);
	LOGSTAT;

	fits_close_file(fptr, &status);
	LOGSTAT;

	return status;
}

void twopt_write_extension(fitsfile * fptr, int ntheta, int ntomo, int nrow,
	double * theta_min, double * theta_mean, double * theta_max, double *xi,
	int *status){

	// Write useful keywords for the header
  	fits_write_key_log(fptr, "2PTDATA", true, "Extension contains 2-point data", status);
  	fits_write_key_lng(fptr, "NANGLE", ntheta, "Number of theta bins", status);
  	fits_write_key_lng(fptr, "NBIN", ntomo, "Number of redshift bins", status);
  	fits_write_key_log(fptr, "WINDOWS", false, "Angular bins simple min/max", status);
  	fits_write_key_log(fptr, "SIMULATD", true, "Simulated data", status);
  	fits_write_key_log(fptr, "BLINDED", false, "Data not blinded", status);

  	// Write the bin ranges
  	for (int t=0; t<ntheta; t++){
  		char key[8];
  		int decimals = 8;
  		snprintf(key, 8, "MIN%d",t+1);
  		fits_write_key_dbl(fptr, key, theta_min[t], decimals, "Min of this theta bin", status);
  		snprintf(key, 8, "MAX%d",t+1);
  		fits_write_key_dbl(fptr, key, theta_max[t], decimals, "Max of this theta bin", status);
  	}

  	// We write all these columns in a minute
  	int * bin1_col = malloc(sizeof(int)*nrow);
  	int * bin2_col = malloc(sizeof(int)*nrow);
  	int * angbin_col = malloc(sizeof(int)*nrow);
  	double * ang_col = malloc(sizeof(double)*nrow);

	// Work out which bins each row corresponds tom
	// for all three kinds of bin.
	// This loop ordering is supposed to reflect
	// the ordering I've put in the note at the top
	// of this file.
	int b=0;
	for (int b1=1; b1<=ntomo; b1++){
		for (int b2=b1; b2<=ntomo; b2++){
			for (int t=1; t<=ntheta; t++){
				bin1_col[b] = b1;
				bin2_col[b] = b2;
				angbin_col[b] = t;
				ang_col[b] = theta_mean[t-1];
				b++;				
			}
		}
	}

	// Save the columns.  Utility function defined above
	// Then we get to the actual saving
	twopt_fits_write_whole_column(fptr, TINT,    1, nrow, bin1_col,   status);
	twopt_fits_write_whole_column(fptr, TINT,    2, nrow, bin2_col,   status);
	twopt_fits_write_whole_column(fptr, TINT,    3, nrow, angbin_col, status);
	twopt_fits_write_whole_column(fptr, TDOUBLE, 4, nrow, xi,         status);
	twopt_fits_write_whole_column(fptr, TDOUBLE, 5, nrow, ang_col,    status);

	free(bin1_col);
	free(bin2_col);
	free(angbin_col);
	free(ang_col);

}

// Convenience function to write a complete FITS column
void twopt_fits_write_whole_column(fitsfile * fptr, int datatype, int colnum, int len, 
	void * col, int * status){
	int row = 1; // The row to start with.  In our case this is easy.
	int first_elem = 1; // Not relavent for scalar quantities

	// Then we get to the actual saving
	fits_write_col(fptr, datatype, colnum, row, first_elem, len, col, status);


}
