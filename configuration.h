#ifndef __CONFIGURATION_H_
#define __CONFIGURATION_H_


typedef struct _olist{
	char *	oid;
	char *  name;
	struct _olist*	next;
} olist;

typedef struct {
	char *	snmp_host;
	int	snmp_port;
	char *	snmp_community;
	int	snmp_version;
	olist *	oidlist;
	char *	carbon_host;
	int	carbon_port;
	int	period;
} configuration;

extern configuration cfg;

configuration * read_cfg(char * filename);

#endif
