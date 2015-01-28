/*
 * stringOps.c
 *
 *  Created on: Jan 19, 2015
 *      Author: adminuser
 */
#include "webclient.h"
#include <string.h>
#include <stdio.h>



void removeSubstring(char *s,const char *toremove)
{
  while(s=strstr(s,toremove))
    memmove(s,s+strlen(toremove),1+strlen(s+strlen(toremove)));
  return;
}

char *trimwhitespace(char *str)
{
  char *end;

  // Trim leading space
  while(isspace(*str)) str++;

  if(*str == 0)  // All spaces?
    return str;

  // Trim trailing space
  end = str + strlen(str) - 1;
  while(end > str && isspace(*end)) end--;

  // Write new null terminator
  *(end+1) = 0;

  return str;
}


