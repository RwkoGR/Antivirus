##############################################
#
# file: Makefile
#
# @Author:   Athanasios Ioannis Xanthopoulos
# @Version:  07-04-2024
# @email:    csd4702@csd.uoc.gr
#
# Makefile
#
##############################################
CC = gcc
CFLAGS = -g

all: antivirus ransomware
run_all: run_scan run_inspect run_monitor run_ransomware run_slice run_unlock
run_all_but_monitor: run_scan run_inspect run_slice run_unlock
run_ransomware_attack: run_monitor run_ransomware

run_scan: antivirus
	./antivirus scan Tests

run_inspect: antivirus
	./antivirus inspect Tests

run_monitor: antivirus
	timeout 5s ./antivirus monitor Tests &

run_ransomware:
	sleep 1 && ./ransomware Tests/very_important_file.txt

run_slice: antivirus
	sleep 2 && ./antivirus slice 50

run_unlock: antivirus
	./antivirus unlock "(1, 484)" "(6, 3734)" "(9, 6548)"

antivirus: antivirus.c
	$(CC) $(CFLAGS) $< -o $@ -lssl -lcrypto -lcurl

ransomware: ransomware.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	@-rm -f *.out *.o *.exe antivirus ransomware
	rm -f Tests/very_important_file.txt.locked
	touch Tests/very_important_file.txt