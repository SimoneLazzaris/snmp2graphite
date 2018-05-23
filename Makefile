
CC=gcc -g
snmp2carbon: snmp2carbon.o configuration.o ini.o senddata.o
	gcc -static -lsnmp -o $@ $+
