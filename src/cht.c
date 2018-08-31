#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rlutil.h"
#define OPTPARSE_IMPLEMENTATION
#define OPTPARSE_API static
#include "optparse.h"
#include "cfgpath.h"
#include "simple_config.h"
#include <curl/curl.h>

#define DEFAULT_SERVER "cheat.sh"

#define VERSION "0.5"

enum state_machine {
  NORMAL_PRINTABLE_CHAR,
    BEGIN_ESCAPE_SEQ_CHAR1, /* <esc> */
    BEGIN_ESCAPE_SEQ_CHAR2, /* [ */
  ESCAPE_SEQ,               /* <something between that goes to escape */
    END_ESCAPE_SEQ_CHAR    /* m */
};

struct MemoryStruct {
  char *memory;
  short state;
  char currEscape[13];
  size_t size;
};

char escape[13];
short mode = 1; /* if mode == 0 do not use colors at all */
short qmode = 0; /* 1: query mode 2: help mode */

int change_color()
{
    if (!strcmp((const char*)escape, "[39"))                return CYAN;         /*  */
    if (!strcmp((const char*)escape, "[38;5;252"))          return WHITE;        /*  */
    if (!strcmp((const char*)escape, "[38;5;31"))           return LIGHTCYAN;    /*  */
    if (!strcmp((const char*)escape, "[38;5;70;01"))        return LIGHTGREEN;   /* keyword */
    if (!strcmp((const char*)escape, "[38;5;246"))          return DARKGREY;     /* comment */
    if (!strcmp((const char*)escape, "[38;5;214"))          return YELLOW;       /* string */
    if (!strcmp((const char*)escape, "[38;5;67"))           return LIGHTMAGENTA; /* number  */
    if (!strcmp((const char*)escape, "[38;5;68"))           return LIGHTCYAN;    /* function names */
    return CYAN; /* default color */

/**
 * Enums: Color codes
 *
 * 0 BLACK - Black 
 * 1 BLUE - Blue
 * 2 GREEN - Green
 * 3 CYAN - Cyan
 * 4 RED - Red
 * 5 MAGENTA - Magenta / purple
 * 6 BROWN - Brown / dark yellow
 * 7 GREY - Grey / dark white
 * 8 DARKGREY - Dark grey / light black
 * 9 LIGHTBLUE - Light blue
 *10 LIGHTGREEN - Light green
 *11 LIGHTCYAN - Light cyan
 *12 LIGHTRED - Light red
 *13 LIGHTMAGENTA - Light magenta / light purple
 *14 YELLOW - Yellow (bright)
 *15 WHITE - White (bright)
 */

}

/* handling states and printing to stdout BEGIN */

void print_char(const char c, short *cs) /* and changing current state */
{
    short len;
    
       /* transitions */
    if (27 == c)
        *cs = BEGIN_ESCAPE_SEQ_CHAR1;
    else if (BEGIN_ESCAPE_SEQ_CHAR1 == *cs && '[' == c) {
        memset(escape, 0, sizeof(escape));
        *cs = ESCAPE_SEQ;
    }
    else if (ESCAPE_SEQ == *cs && 'm' == c)
        *cs = END_ESCAPE_SEQ_CHAR;
    
      /* actions */
    if (NORMAL_PRINTABLE_CHAR == *cs)
        putchar(c); /* print only notspecial chars */
    if (ESCAPE_SEQ == *cs) {
        len = strlen(escape);
        escape[len] =c; len++;
    }
}

void perform_parse_and_coloring(struct MemoryStruct *doc)
{
    char c;
    size_t p;
    doc->state = NORMAL_PRINTABLE_CHAR;
    for(p = 0; p < doc->size; p++) 
    {
        c = ((char *)doc->memory)[p];
        print_char(c, &(doc->state));
            if (END_ESCAPE_SEQ_CHAR == doc->state) 
            {
                /* fprintf(stderr,"%s\n", escape); */
                if (mode) setColor(change_color());
                doc->state = NORMAL_PRINTABLE_CHAR;
            }
    }
}

/* handling states and printing to stdout END */

static size_t
WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
  size_t realsize = size * nmemb;
  struct MemoryStruct *mem = (struct MemoryStruct *)userp;

  mem->memory = realloc(mem->memory, mem->size + realsize + 1);
  if(mem->memory == NULL) {
    /* out of memory! */
    fprintf(stderr, "not enough memory (realloc returned NULL)\n");
    return 0;
  }

  memcpy(&(mem->memory[mem->size]), contents, realsize);
  mem->size += realsize;
  mem->memory[mem->size] = 0;

  return realsize;
}

/* modified strcmp to work only till end of str1 */
int strCmp(const char* s1, const char* s2)
{
    while(*s1 == *s2) {
        s1++; s2++;
    }
    if (0==*s1) return 0; /* end of str1 */
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

void print_usage(f, util_name, srv)
#ifdef WIN32
  FILE *f;
#else
  struct _IO_FILE *f;
#endif
  char *util_name;
  char *srv;
{
    fprintf(f, "\nUsage: %s  [options...] <URL>\n(", util_name);
    fprintf(f, "  without http:// or file:// scheme using %s server)\n", srv);
    fprintf(f, "Options:\n");
    fprintf(f, " -Q, --query       space delimetered arguments are parts of query\n");
    fprintf(f, " -T, --no_colors   disabling coloring globally\n");
    fprintf(f, "\n");
}

int main(int argc, char *argv[])
{
  struct optparse_long longopts[] = {
      {"query",       'Q', OPTPARSE_NONE},
      {"no_colors",   'T', OPTPARSE_NONE},
      {"version",     'v', OPTPARSE_NONE},
      {0}
  };
  char *arg;
  int option;
  struct optparse options;
    
  CURL *curl_handle;
  CURLcode res;
  char *fullUrlStr;
  char *server;
  char *query;
  int word_count;
  
  struct MemoryStruct chunk;

  char cfgfile[262];
  char* host;
  int port = 0;
  char *filePath;
  FILE *file;

  server = malloc(111);
  query  = malloc(256);

  get_user_config_file(cfgfile, sizeof(cfgfile), DEFAULT_SERVER);
  if (cfgfile[0] == 0) {
      fprintf(stderr, "Unable to find home directory.\n");
      return 1;
  }

  filePath=cfgfile;
  SCcfg=SCparseConf(filePath);
  host=SCgetValue(SCcfg,"CHTSH_URL");
  if (host) {
      strcpy(server, host);
  }
  else {
      strcpy(server, DEFAULT_SERVER);
  }
  SCfreeCfg(SCcfg);

  /* not enough parameters */
  if(argc <2) {
    print_usage(stderr, argv[0], server);
    return EXIT_FAILURE;
  }
  
  optparse_init(&options, argv);
  while ((option = optparse_long(&options, longopts, NULL)) != -1) {
      switch (option) {
      case 'v':
          fprintf(stdout, "  %s version: " VERSION "      (c) Tadej Panjtar\n", argv[0]);
          file = fopen(cfgfile, "r");
          if (file) {
            fprintf(stdout, "  Successfully read config from: %s\n", cfgfile);
            fclose(file);
          }
          else {
            fprintf(stdout, "  Config file: %s do not exits\n", cfgfile);
          }
          exit(0);
          break;
      case 'T': /* for disabling coloring globally */
          mode = 0;
          break;
      case 'Q': /* query mode */
          qmode = 1;
          break;
      case 'h':
      case '?':
          print_usage(stdout, argv[0], server);
          qmode = 2;
          break;
      }
  }

  if (2==qmode) {
    strcpy(query, ":help");
  }
  else if (1==qmode) {
    word_count = 0;
    strcpy(query, "/");
    while ((arg = optparse_arg(&options))) { 
      strcat(query, arg);
      if (0==word_count++)
        strcat(query, "/");
      else
        strcat(query, "+");
    }
    query[strlen(query)-1]=0; /* delete last "+" */
  }
  else {
      arg = optparse_arg(&options);
      strcpy(query, arg);
      strcat(query, "//"); /* strange, but this works for two letters quotes */
  }

  chunk.memory = malloc(1);  /* will be grown as needed by the realloc above */
  chunk.size = 0;    /* no data at this point */

  curl_global_init(CURL_GLOBAL_ALL);

  /* init the curl session */
  curl_handle = curl_easy_init();
  
  /* specify URL to get */
  if (0==strCmp("http://", query) || 0==strCmp("file://", query))
      curl_easy_setopt(curl_handle, CURLOPT_URL, query);
  else {
      fullUrlStr = malloc(strlen(query) + strlen(server) + 1);
      strcpy(fullUrlStr, server);
      strcat(fullUrlStr, "/");
      strcat(fullUrlStr, query);
      curl_easy_setopt(curl_handle, CURLOPT_URL, fullUrlStr);
#ifdef DEBUG
      printf("Opening url: %s\n", fullUrlStr);
#endif  
  }

  /* send all data to this function  */
  curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);

  /* we pass our 'chunk' struct to the callback function */
  curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);

  /* some servers don't like requests that are made without a user-agent
     field, so we provide one */
  curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");

  /* get it! */
  res = curl_easy_perform(curl_handle);

  /* check for errors */
  if(res != CURLE_OK) {
    fprintf(stderr, "%s failed: %s\n", argv[0],
            curl_easy_strerror(res));
  }
  else {
    /*
     * Now, our chunk.memory points to a memory block that is chunk.size
     * bytes big and contains the remote file.
     *
     * Do something nice with it!
     */

    /* printf("%lu bytes retrieved\n", (unsigned long)chunk.size); */

    /* we have full received document, so recode it */
    perform_parse_and_coloring(&chunk);

    /* reset color */
    setColor(GREY);
  }

  /* cleanup curl stuff */
  curl_easy_cleanup(curl_handle);
  
  free(chunk.memory);

  /* we're done with libcurl, so clean it up */
  curl_global_cleanup();

  return 0;
}
