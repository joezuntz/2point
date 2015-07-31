A file format for 2-point function measurements.
================================================


As we start working on combined probes we will need to keep track of complicated combinations of two-point functions.  So it is useful to put together a well-defined file format to avoid ambiguity when describing a set of observed or simulated quantities.  The software facilities to handle this are also worth thinking about.


Basic Requirements
------------------

Definite use case:
- Galaxy position 2-point        - in Fourier or real space
- Galaxy shear 2-point           - in Fourier or real space
- Galaxy magnification 2-point   - in Fourier or real space
- Galaxy/galaxy-lensing
- COSEBIS


Possible requirements for this:

- At least five indices to a given measurement:
    - type1 e.g. galaxy position
    - type2 e.g. galaxy shear +
    - bin1 e.g. tomographic bin 1
    - bin2 e.g. tomographic bin 2
    - nominal_theta_or_ell e.g. nominal bin center/mean in arcmin

- Ugh - in future there are also bins in other quantities than redshift, e.g. luminosity.

- As well as/instead of nominal theta/ell might also want a Window function. This might be trivial min/max but could be more general function, especially for C_ell.

- Distinguish xi-, xi+, E/B etc.

- We *might* want to include n(z). Would need to tag as a particular version?
  - Pro
    - including this packages everything you need from the data in one place
    - lets you be unambiguous which bin refers to which n(z)
    - lets you make explicit what form the n(z) is in (histogram or sample points)
  - Con 
    - there will be multiple different estimates of n(z)
        - package multiple in one file? could have nz_1, nz_2, etc. fiddly.

- Could also potentially (and optionally) package covariances with this format.
    - Pro
        - Ensures that you know the ordering of the covmat as it must be same as data vector
        - Handy to keep everything together
    - Con
        - Bloat/feature creep
        - Same issue with multiple estimates
        - Much larger files, maybe
        - Might be harder to deal with sparse forms, depending on format?


Other possible use cases:
- Cross-correlations with CMB
- Pure CMB C_ell - there are already formats in use by Planck etc. for this.
- Intrinsic alignments
- PSF correlation functions
- 3D correlation function measurements??

# FITS


I suggest a FITS format for ease of use and flexibility.

Assuming we want this in a FITS file, could:
 - package all the stuff into one big table with columns for type
 - or have a different extension for different types of quantities (think this is much better)
    - are xi+ and xi- different quantities? probably easier to handle then.

Code needed for this:
 - libraries in C and in Python for making and loading these
    - concatenating all columns
    - choosing bins and angular ranges to use and just getting out those (concatenated)
    - choosing just some of the pairs of quantities e.g. ignoring xi- or B-modes
    - taking from a set of theory curves an interpolated/binned prediction for a given data set
 - scripts to/from convert existing formats - e.g. Nicaea output.

Strawman Proposal
=================

FITS format.

Header for data extensions
------------------------


Each pair of quantities has its own extension, with some standard ordering. Extension header specifies which pair it is with a 2-character code (case insensitive; none of the G versus g nonsense):

```
2PTDATA=T  // This extension contains 2-point data, not some auxiliary information


QUANT1=GT   // First quantitiy is galaxy tangential shear 
QUANT2=GP   // second quantity is galaxy position
             // Quantitiy codes should include specifying whethet Fourier space/real space
             // Since we will want to store the combination xi+(theta) = GTGT + GPGP
             // we can also use QUANT=G+ and QUANT2=G+ for this (and sim for -)

NANGLE=6   // Number of angular bins
NBIN=3     // Number of tomographic bins

WINDOWS=T   // angular quantities are specified as window functions, not as ranges

if windows=T:
WINPREFX=STD_WIN  // elsewhere in the file look for extensions STD_WIN1, STD_WIN2, etc.
                  // for angular bin 1, angular bin 2, etc. 
                  // optional.  if not specified assume G+_GP_WIN1, etc.

if WINDOWS=F:
MIN1 = 2.0
MAX1 = 10.0
MIN2 = 10.0
MAX2 = 30.0 
           // etc. for the min and max values of each bins

           // Other possible keywords??
SIMULATD=F // This is real data/this is simulated data
BLINDED=T  // This has been blinded in some way
```


Binary table for data extensions
------------------------------


 - BIN1     (32 bit integer [TINT], tomographic bin for first quantity, starting from 1)
 - BIN2     (32 bit integer [TINT], same as for bin 1)
 - ANGBIN   (32 bit integer [TINT], angular bin index, starging from one)
 - VALUE    (64 bit float [TDOUBLE], correlation value)
 - ANG      (64 bit integer [TDOUBLE], optional nominal angle, for plotting/quick tests)
 - ERR      (64 bit float [TDOUBLE], optional nominal error bar)
 - LERR     (64 bit float [TDOUBLE], optional nominal upper error bar)
 - UERR     (64 bit float [TDOUBLE], optional nominal lower error bar)


Header for window sections
--------------------------
```
EXTNAME = G+_GP_WIN1   // for bin 1
SPACE = REAL   // Or FOURIER - specifies real space or fourier space ??
DIM = 2  // Or 3D for 3D measurements e.g. P(k) ??
```
Binary table for window sections
--------------------------
- THETA or ELL (64 bit float [TDOUBLE], angular value)
- VALUE (64 bit float [TDOUBLE], measurement)
