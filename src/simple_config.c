#include "simple_config.h"
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <stdio.h>
#include <ctype.h> /* isspace */
#include <stdlib.h>
#include <string.h>

/* Filter str to get rid of comment and return key value pair */
int str_split(char const *str, char *key, char *value) {
	int flag=0;
	while(*str!='\0'){
		if(*str=='#'||*str=='\n') {
			break;
		}else if(*str=='\t') {
			str++;
			continue;
		}else{
			if(*str=='=') {
				flag=1;
				str++;
				continue;
			}
			if(flag==0) {
				*key++=*str++;
			}else if(flag==1) {
				*value++=*str++;
			}
		}
	}
	*key='\0';
	*value='\0';
	return 0;
}

char *trim(char *str)
{
	char *end;

	/* Trim leading space */
	while (isspace((unsigned char)*str)) str++;
	if (*str == 0) /* All spaces? */
		return str;
	/* Trim trailing space */
	end = str + strlen(str) - 1;
	while (end > str && isspace((unsigned char)*end)) end--;
	/* Write new null terminator character */
	end[1] = '\0';
	return str;
}

char *normalize(char *str)
{
	char *end;
	char quot;
	if ((quot=((unsigned char)*str)));
	end = str + strlen(str) - 1;
	if (quot == ((unsigned char)*end)) 
		if (('"' == quot) || ('\'' == quot)) {
		str++;
		end[0] = '\0';
	}
	return str;
}

/* Traversal & display */
int SCdumpKeyValue(ConfList *head) {
	ConfList *p=head;
	while(p!=NULL) {
		printf("key:@%s@	value:[%s]\n",p->key,p->value);
		p=p->next;
	}
	return 0;
}

char* SCgetValue(ConfList *head, char *key) {
	ConfList *p=head;
	int flag=1;
	while(p!=NULL){

		if(strcmp(p->key,key)==0){
			flag=0;
			break;
		}
		p=p->next;
	}
	if(flag==0){
		return p->value;
	}else{
		return NULL;
	}
}

/* Parsing configuration file */
ConfList * SCparseConf(char *filePath) {
	FILE *fin;
	char *one_line;
	ConfList *head;
	ConfList *p1,*p2;
	char *key = (char *)malloc(SC_KEY_SIZE);
	char *value = (char *)malloc(SC_VAL_SIZE);
	p1=p2=(ConfList*)malloc(sizeof(ConfList));
	if(p1==NULL||p2==NULL) {
		return NULL;
	}

	if((fin = fopen(filePath, "r"))==NULL) {
		/* fprintf(stderr, "Can not open file \"%s\" !\n", filePath); */
		return NULL;
	}
	head=NULL;
	one_line = (char*)malloc( SC_MAX_LINE_SIZE * sizeof(char) );
	while( fgets(one_line, SC_MAX_LINE_SIZE, fin) != NULL ) {
		str_split(one_line, key,value);
		if(strlen(key)>=1&&strlen(value)>=1) {
			if(head==NULL){
				strcpy(p1->key, trim(key));
				strcpy(p1->value, normalize(trim(value)));
				head=p1;
			}else{
				strcpy(p2->key, trim(key));
				strcpy(p2->value, normalize(trim(value)));
				p1->next=p2;
				p1=p2;
			}
		}

		p2=(ConfList*)malloc(sizeof(ConfList));
	}
	p1->next = NULL;
	fclose(fin);
	return head;
}

/* Release memory*/
void SCfreeCfg(ConfList *head) {
	if (head == NULL)
		return;
	SCfreeCfg(head->next);
	free(head);
}

