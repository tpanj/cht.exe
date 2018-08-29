#ifndef _SIMPLE_CONFIG
#define _SIMPLE_CONFIG	1

#define SC_MAX_LINE_SIZE 2048
#define SC_KEY_SIZE 48
#define SC_VAL_SIZE 86

 /* list type struct of key values */
typedef struct Conf_s {
	char key	[SC_KEY_SIZE];
	char value	[SC_VAL_SIZE];
	struct Conf_s *next;
} ConfList;

ConfList *SCcfg;

int SCdumpKeyValue(ConfList *head);
char* SCgetValue(ConfList *head,char *key);
ConfList * SCparseConf(char *filePath);
void SCfreeCfg(ConfList *head);

#endif
