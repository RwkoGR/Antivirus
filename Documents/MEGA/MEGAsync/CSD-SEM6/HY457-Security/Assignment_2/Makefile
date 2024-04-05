##############################################
#
# file: Makefile
#
# @Author:   Athanasios Ioannis Xanthopoulos
# @Version:  30-03-2024
# @email:    csd4702@csd.uoc.gr
#
# Makefile
#
##############################################
CC = gcc
CFLAGS = -g

all: antivirus


antivirus: antivirus.c
	$(CC) $(CFLAGS) $< -o $@ -lssl -lcrypto -lcurl

clean:
	@-rm -f *.out *.o *.exe antivirus