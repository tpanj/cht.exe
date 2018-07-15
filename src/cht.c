// assuming download txt file does not include other escape characters
// Started from https://curl.haxx.se/libcurl/c/getinmemory.html
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rlutil.h"
#include <curl/curl.h>

#define DEFAULT_SERVER "cht.sh/"

#define VERSION "0.2"

enum state_machine {
  NORMAL_PRINTABLE_CHAR,
    BEGIN_ESCAPE_SEQ_CHAR1, // <esc>
    BEGIN_ESCAPE_SEQ_CHAR2, // [
  ESCAPE_SEQ,               // <something between that goes to escape
    END_ESCAPE_SEQ_CHAR,    // m
};

struct MemoryStruct {
  char *memory;
  short state;
  char currEscape[13];
  size_t size;
};

char escape[13];
short mode = 1; // if mode == 0 do not use colors at all

int change_color()
{
    if (!strcmp((const char*)escape, "[39"))                return CYAN;         // 
    if (!strcmp((const char*)escape, "[38;5;252"))          return WHITE;        // 
    if (!strcmp((const char*)escape, "[38;5;31"))           return LIGHTCYAN;    // 
    if (!strcmp((const char*)escape, "[38;5;70;01"))        return LIGHTGREEN;   // keyword
    if (!strcmp((const char*)escape, "[38;5;246"))          return DARKGREY;     // comment
    if (!strcmp((const char*)escape, "[38;5;214"))          return YELLOW;       // string
    if (!strcmp((const char*)escape, "[38;5;67"))           return LIGHTMAGENTA; // number 
    if (!strcmp((const char*)escape, "[38;5;68"))           return LIGHTCYAN;    // function names
    return CYAN; // default color

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

// handling states and printing to stdout BEGIN

void print_char(const char c, short *cs) // and changing current state
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
        putchar(c); // print only notspecial chars
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
                // fprintf(stderr,"%s\n", escape); 
                if (mode) setColor(change_color());
                doc->state = NORMAL_PRINTABLE_CHAR;
            }
    }
}

// handling states and printing to stdout END

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

// modified strcmp to work only till end of str1
int strCmp(const char* s1, const char* s2)
{
    while(*s1 == *s2) {
        s1++; s2++;
    }
    if (0==*s1) return 0; // end of str1
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}


int main(int argc, char *argv[])
{
  CURL *curl_handle;
  CURLcode res;
  char * fullUrlStr;

  struct MemoryStruct chunk;

  // not enough parameters
  if(argc <2) {
    fprintf(stderr, "Usage: %s URL\n", argv[0]);
    fprintf(stderr, "  without http:// or file:// scheme using cht.sh server\n");
    fprintf(stderr, "  -d parameter after URL to not to use coloring at all\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "  (c) Tadej Panjtar, version: \n" VERSION);
    return EXIT_FAILURE;
  }
  
  // last parameter is for disabling coloring globally
  if (!strcmp("-d", argv[argc-1])) {
      mode = 0;
  }
  
  chunk.memory = malloc(1);  /* will be grown as needed by the realloc above */
  chunk.size = 0;    /* no data at this point */

  curl_global_init(CURL_GLOBAL_ALL);

  /* init the curl session */
  curl_handle = curl_easy_init();
  
  /* specify URL to get */
  if (0==strCmp("http://", argv[1]) || 0==strCmp("file://", argv[1]))
      curl_easy_setopt(curl_handle, CURLOPT_URL, argv[1]);
  else {
      fullUrlStr = malloc(strlen(argv[1]) + sizeof DEFAULT_SERVER);
      strcpy(fullUrlStr, DEFAULT_SERVER);
      strcat(fullUrlStr, argv[1]);
      curl_easy_setopt(curl_handle, CURLOPT_URL, fullUrlStr);
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

    // printf("%lu bytes retrieved\n", (unsigned long)chunk.size);

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
