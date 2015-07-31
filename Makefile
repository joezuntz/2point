LDFLAGS+=-L. -ltwopoint -lcfitsio

test: test.c libtwopoint.a 
	$(CC) $(CFLAGS) -o test test.c $(LDFLAGS) 

libtwopoint.a: shear_only.o
	ar rc libtwopoint.a shear_only.o


clean:
	rm -rf libtwopoint.a shear_only.o test *.dSYM