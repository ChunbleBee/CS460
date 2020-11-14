/********************************************************************
Copyright 2010-2017 K.C. Wang, <kwang@eecs.wsu.edu>
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
********************************************************************/
#pragma once

#define strcat kstrcat
#define strcpy kstrcpy
#define strtok kstrtok

int kprintf(char *, ...);
int strcmp(char *s1, char *s2);
int strlen(char *s);
char *kstrtok(char *string, char delim);

int strlen(char *s)
{
  int i = 0;
  while(*s){
    i++; s++;
  }
  return i;
}

int strcmp(char *s1, char *s2)
{
  while((*s1 && *s2) && (*s1==*s2)){
      s1++; s2++;
  }
  if ((*s1==0) && (*s2==0))
    return 0;
  return(*s1 - *s2);

}

int kstrcpy(char *dest, char *src)
{
  while(*src){
    *dest++ = *src++;
  }
  *dest = 0;
}

int kstrcat(char *dest, char *src)
{
  while(*dest)
    dest++;

  while(*src){
    *dest++ = *src++;
  }
  *dest = 0;
}

int atoi(char *s)
{
  int v = 0;
  while (*s){
    v = 10*v + (*s - '0');
    s++;
  }
  //kprintf("v=%d\n", v);
  return v;
}

int geti()
{
  char s[16];
  kgets(s);
  return atoi(s);
}

char *kstrtok(char *string, char delim)
{
  static char *oldstring;
  char * output;
  printf("Here, str: %s, old: %s\n", string, oldstring);

  if (string == NULL)
  {
    string = oldstring;
    printf("Here 01, str:%s\n", string);
  }

  if (*string == '\0')
  {
    printf("Here 02");
    return NULL;
  }
  output = string;
  while(*string != '\0' && *string != delim)
  {
    string++;
  }

  if (*string == delim)
  {
    *string = '\0';
    string++;
  }

  oldstring = string;
  return output;
}

int memcpy(char *dest, char *src, int size)
{
  int i;
  for (i=0; i<size; i++){
    *dest++ = *src++;
  }
}
