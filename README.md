A file format for 2-point function measurements.
================================================


As we start working on combined probes we will need to keep track of complicated combinations of two-point functions.  So it is useful to put together a well-defined file format to avoid ambiguity when describing a set of observed or simulated quantities. 

This repository describes a specification for such a file format and will collect code to process it.

See the initial notes file for some early thoughts that led to this format, and the included fits file as an example.

File Structure
--------------

A 2-point data file is a FITS file containing extension headers with specified contents.

It should contain:
 - one or more 2PTDATA extensions, each containing a two-point function between two specified types of quantity
 - zero or more NZDATA extensions, each containing a collection of redshift distributions
 - zero or more COVDATA extensions, each containing a covariance matrix for all the data in the file
 - zero or more WINDATA extensions, each containing a collection of window functions of angle

Quantity Codes
---------------

A two point measurement is a correlation function of two quantities, usually as a function of some angular measurement (or angular wavenumber in Fourier space).  Many different quantities can be used in two-point measurements. For example, if you see the quantity C^EE_ell, that could be a CMB polarization measurement or a galaxy shear measurement.

Three-letter quantity codes are used in the format described here to define more carefully what quantities are being used.  The First letter describes the type of object or field being measured, the second what kind of statistical measurement is being used, and the third the basis (real, Fourier, or any other).

For example, the code GPF would represent Galaxy position C_ell values: the G stands for galaxy, the P for position, and the F for Fourier.  A collection of other values are defined here, but more will be added as we progress.  Not all possible combinations are valid.

First letter:

    G : Galaxy
    C : CMB
    L : Clusters
    P : Point-spread function

Second letter:

    E : E-mode, for polarization (for C) or shear (for G)
    B : B-mode
    K : convergence
    + : For xi_+(theta)
    - : For xi_-(theta)

Third letter:

    F : Fourier space, such as C_ell
    R : For real space such as correlation functions
    C : For COSEBIS (?)






2PTDATA extensions
-------------------

A 2PTDATA Binary Table extension is intended to contain the actual 2-point data values, for any required angular bin combinations, for a specific pair of quantities, such as galaxy shear E-mode, B-mode, position, xi(theta) CMB convergence, or any other quantities that can be correlated as a function of angle or angular scale.

The extension must contain the following parameters in its header:

    2PTDATA  = T          // This sentinel marks the extension as 2PTDATA
    QUANT1   = GPF        // See "quantity codes" above
    QUANT2   = GPF 
    NANGLE   = 20         // The number of angular bins.
    NBIN_1   = 3          // The number of redshift/distance bins. 
    NBIN_2   = 3          // The number of redshift/distance bins. 
    WINDOWS  = SAMPLE     // The form of the window function - at the moment this must be SAMPLE.
    SIMULATD = T          // This is simulated data
    BLINDED  = F          // This is blinded data

For many 2-point data sets (but not, for example, CMB ones), the extension must also contain these header parameters:

  KERNEL_1 = NZ_SOURCE  // The n(z) data set for quantity 1.  This is a reference to an NZDATA file elsewhere in the fits file
  KERNEL_2 = NZ_SOURCE


The extension must then contain these data columns:

    BIN1   8-byte integer  // Integer, the index of the first bin
    BIN2   8-byte integer  // Integer, the index of the second bin
    ANGBIN 8-byte integer  // The angular bin index
    VALUE  8-byte real     // The data value for this combination.

Optionally, it can also contain this column:

    ANG    8-byte real


NZDATA Extension
----------------

An NZDATA Binary Table extension is intended to contain the number density distributions as a 
function of redshift for a quantity.

Not that we use the term "bin" to mean two things here - tomographic bins, meaning selections
of galaxies hopefully in a particular redshift range, and histogram bins, the small division
in redshift that we get n(z) measured in.

The extension must contain the following parameters in its header:

    NZDATA  = T          // This sentinel marks the extension as n(z) data
    EXTNAME = NAME       // The name of this n(z) kernel.
    NBIN    = 5          // Integer number of tomographic bins
    NZ      = 100        // Integer number of histogram bins

The extension must then contain these data columns:

    Z_LOW   8-byte real  // Real, the z value for the lower end of each redshift histogram bin
    Z_MID   8-byte real  // Real, the z value for a middle point of each redshift histogram bin
    Z_HIGH  8-byte real  // Real, the z value for the upper end of each redshift histogram bin
    BIN1    8-byte real  // Real, the n(z) value for this histogram bin for the first tomographic bin
    etc.    BIN2, BIN3, etc.



COVDATA Extension
----------------

A covdata extension contains a covariance matrix for all the data points in the file, stored as an image HDU.

Although the ordering of the points can be derived from the order of the data in the file, for convenience the header also includes information about which chunks of the matrix correspond to which data.


The header must contain these keys:

    COVDATA =       True     // Sentinel for covariance matrix
    EXTNAME =     COVMAT     // Name of this covariance matrix
    NAME_0 = 'SHEAR_SHEAR'  // Name of the 2PTDAT extension of the first chunk of covmat
    STRT_0 =          0     // Integer, starting position of the first data set stored here
    LEN_0  =        300     // Integer, length of the first data set stored here
    NAME_1 =      'GGL'     // Name of the 2PTDAT extension of the second chunk of covmat
    STRT_1 =        300     // Integer, starting position of the second data set stored here
    LEN_1  =        180     // Integer, length of the second data set stored here

The image in the extension should be of type 8-byte reals and be a square representation of the covariance matrix. The order of both the chunks of data and the values within each chunk must exactly match the order of the data in the file.


WINDATA Extension
-----------------

We have not yet considered this extension properly. At the moment only WINDOWS=sample is accepted.
