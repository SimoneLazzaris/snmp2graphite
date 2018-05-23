#include "configuration.h"
#include "ini.h"
#include <stdlib.h>
#include <string.h>

static void oadd(configuration * p, const char * n, const char *v) {
	olist *o=(olist*)malloc(sizeof(olist));
	olist *i;
	o->next=NULL;
	o->name=strdup(n);
	o->oid=strdup(v);
	if (p->oidlist==NULL) {
		p->oidlist=o;
		return;
	}
	for (i=p->oidlist; i->next!=NULL; i=i->next)
		;
	i->next=o;
}

configuration cfg={
	.snmp_host="localhost",
	.snmp_port=161,
	.snmp_community="public",
	.snmp_version=1,
	.oidlist=NULL,
	.carbon_host="localhost",
	.carbon_port=2003,
	.period=60
};

static int handler(void* p, const char* section, const char* name,
                   const char* value) {
	
	configuration* pcfg = (configuration*)p;
	// fprintf(stderr,"Parsing [%s] '%s': '%s'\n",section,name,value);
	#define MATCH(s, n) (strcmp(section, s) == 0 && strcmp(name, n) == 0)
	if (MATCH("snmp","host")) pcfg->snmp_host=strdup(value);
	else if (MATCH("snmp","port")) pcfg->snmp_port=atoi(value);
	else if (MATCH("snmp","community")) pcfg->snmp_community=strdup(value);
	else if (MATCH("snmp","version")) pcfg->snmp_version=atoi(value);
	else if (MATCH("carbon","host")) pcfg->carbon_host=strdup(value);
	else if (MATCH("carbon","port")) pcfg->carbon_port=atoi(value);
	else if (MATCH("global","period")) pcfg->period=atoi(value);
	else if (strcmp(section,"oid")==0) oadd(pcfg,name,value);
	else {
		fprintf(stderr,"Unrecongised [%s] '%s': '%s'\n",section,name,value);
		return 0;
	}
	return 1;
}


configuration * read_cfg(char * filename) {
	ini_parse("test.ini", handler, &cfg);
	
	fprintf(stderr,"H:%s P:%d C:%s V:%d\n",cfg.snmp_host,cfg.snmp_port,cfg.snmp_community,cfg.snmp_version);
	{
		olist* i;
		for (i=cfg.oidlist; i!=NULL; i=i->next)
			fprintf(stderr," - %s : %s\n",i->name, i->oid);
	}
	return &cfg;
}
