
libs1 = -lsnmp

ifeq ($(OPTIONS),systemd)
libs=$(libs1) -lsystemd
cdefs = -DUSE_SYSTEMD -g
else
libs=$(libs1)
cdefs = -g
endif

CC=gcc $(cdefs)
snmp2carbon: snmp2carbon.o configuration.o ini.o senddata.o
	gcc $(libs) -o $@ $+
