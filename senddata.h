#ifndef __SENDDATA_H_
#define __SENDDATA_H_

typedef struct _data2send {
	char *	metric;
	int	value;
	struct _data2send * next;
} data2send;

int senddata(configuration * pcfg, data2send *dat);

#endif
