#include <stdio.h>
#include <strings.h>
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>

#include "configuration.h"
#include "senddata.h"

struct snmp_session * init(struct snmp_session *session, configuration * cfg) {
	struct snmp_session *ss;
	init_snmp("test");
	snmp_sess_init( session );
	session->peername = cfg->snmp_host;
	switch (cfg->snmp_version) {
		case 1:	session->version = SNMP_VERSION_1;
			break;
		case 2: session->version = SNMP_VERSION_2c;
			break;
	}
	//session->version = SNMP_VERSION_1;
	session->community = cfg->snmp_community;
	session->community_len = strlen(cfg->snmp_community);
	session->remote_port=cfg->snmp_port;
	SOCK_STARTUP;
	ss = snmp_open(session);
	if (!ss) {
		snmp_perror("ack");
		snmp_log(LOG_ERR, "something horrible happened!!!\n");
		exit(2);
		}

	return ss;
}

olist* oidfind(configuration * pcfg, oid* po, int l) {
	olist * ol;
	char buf[1024]={0};
	char go=1;
	for (;l>0;l--) {
		char b2[16];
		snprintf(b2,16,".%d",*po);
		strcat(buf,b2);
		if (*po++==0)
			go=0;
	}
	for (ol=pcfg->oidlist; ol!=NULL; ol=ol->next) {
		if (strcmp(ol->oid, buf)==0)
			return ol;
	}
	return NULL;
}

int main(int argc , char** argvm) {
	struct snmp_session session, *ss;
	struct snmp_pdu *pdu;
	struct snmp_pdu *response;
		
	oid anOID[MAX_OID_LEN];
	size_t anOID_len = MAX_OID_LEN;

	struct variable_list *vars;
	int status;
	olist * ol;
	data2send * dat=NULL;
	data2send **pdat=&dat;
	
	configuration * xcfg=read_cfg("test.ini");

	ss=init(&session,xcfg);
	pdu = snmp_pdu_create(SNMP_MSG_GET);
	for (ol=xcfg->oidlist; ol!=NULL; ol=ol->next) {
		read_objid(ol->oid, anOID, &anOID_len);
		snmp_add_null_var(pdu, anOID, anOID_len);
	}

	status = snmp_synch_response(ss, pdu, &response);
	if (status == STAT_SUCCESS && response->errstat == SNMP_ERR_NOERROR) {
		for(vars = response->variables; vars; vars = vars->next_variable) {
			print_variable(vars->name, vars->name_length, vars);
			ol=oidfind(xcfg, vars->name_loc, vars->name_length);
			if (ol) {
				(*pdat)=(data2send*)malloc(sizeof(data2send));
				(*pdat)->metric=strdup(ol->name);
				(*pdat)->value=vars->val.counter64->high;
				(*pdat)->next=NULL;
				pdat=&((*pdat)->next);
				printf("- %s : %u\n",ol->name,vars->val.counter64->high);
			}
		}
	}
	
	snmp_free_pdu(response);
	snmp_close(ss);
	SOCK_CLEANUP;
	senddata(xcfg,dat);
	if (dat!=NULL) {
		data2send*p,*t;
		for (p=dat; p!=NULL; p=t) {
			t=p->next;
			free(p->metric);
			free(p);
		}
	dat=NULL;
	}
}
