#twopoint examples - https://github.com/joezuntz/2point/
import twopoint

#read in the fits file - no covariance matrix in this one, so put 'None' as the covmat_name
t = twopoint.TwoPointFile.from_fits('2pt_sim_1110_baseline.fits', covmat_name=None)

#print the available spectra
print 'File contains the following spectra:'
print t.spectra

#get the xip spectrum object
xip = t.get_spectrum('xip')

#this contains various useful arrays 
#e.g. - xip.bin1(2) will give you the first(second) tomographic bin index
#     - xip.value will give the measured signal
#See the SpectrumMeasurement class in the twopoint library for all the available attributes.
#To get these quantities for a particlar tomographic bin combination, e.g. (4,4), do something like:
xip_4_4 = xip.value[(xip.bin1==4)*(xip.bin2==4)]
print 'xip for bin combination 4,4:'
print xip_4_4
