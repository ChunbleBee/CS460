#pragma once
#include "TypesAndDefs.h"

int bcopy(const void *src, void *dest, unsigned int n) {
	const char *s = (const char *)src;
	char *d = (char *)dest;
	if (s <= d)
		for (; n>0; --n)
			d[n-1] = s[n-1];
	else
		for (; n>0; --n)
			*d++ = *s++;
}


int bzero(void *dest, unsigned int n) {
	memset(dest, 0, n);
}


char *memcpy(void *dest, const void *src, unsigned int n) {
	bcopy(src, dest, n);
	return dest;
}


char *memset(void *dest, int c, unsigned int n) {
	char *d = (char *)dest;
	for (; n>0; --n)
		*d++ = (char)c;
	return dest;
}


int memcmp(const void *s1, const void *s2, unsigned int n) {
	const char *s3 = (const char *)s1;
	const char *s4 = (const char *)s2;
	for (; n>0; --n) {
		if (*s3 > *s4)
			return 1;
		else if (*s3 < *s4)
			return -1;
		++s3;
		++s4;
	}
	return 0;
}

int strcmp(const char *s1, const char *s2)
{
  while ((*s1 && *s2) && (*s1==*s2)){
      s1++; s2++;
  }
  if (*s1==0 && *s2==0)
    return 0;
  return *s1-*s2;
}

char *strcpy(char *dest, const char *src) {
	char *p = dest;
	while ( (*dest++ = *src++))
		;
	*dest = 0;
	return p;
}


int strlen(const char *s) {
	unsigned int n = 0;
	while (*s++)
		++n;
	return n;
}

int strcat(char *dest, char *src)
{
  while(*dest)
    dest++;
  while(*src)
    *dest++ = *src++;
  *dest = 0;
}

int strncat(char *dest, char *src, int nchars)
{
  while(*dest)
    dest++;
  while(*src != '\0' && nchars > 0)
  {
    *dest++ = *src++;
    nchars--;
  }
  *dest = '\0';
}

int strncpy(char *dest, char *src, int n)
{
  char *p = dest;
  while(n && *src){
    *dest++ = *src++;
    n--;
  }
  *dest = 0;
  return (int)p;
}

int strncmp(char *s1, char *s2, int n)
{
  if (n==0) return 0;
  do{
    if (*s1 != *s2++)
      return *s1-*(s2-1);
    if (*s1++ == 0)
      break;
  } while(--n != 0);
  return 0;
}

char *strstr(char *s1, char *s2)
{
  int i, len, length;
  len = strlen(s2);
  length = strlen(s1)-strlen(s2) + 1;

  for (i=0; i < length; i++)
  {
    if (strncmp(&s1[i], s2, len)==0)
         return &s1[i];
  }
  return 0;
}

char* findNextDelim(char* string, char* delim)
{
  bool found = false;

  while(*string != '\0' && found == false)
  {
    char* d = delim;

    while(*d != '\0' && found == false)
    {
      if (*string == *d)
      {
        found = true;
      }
      else
      {    
        d++;
      }
    }

    if (*string != *d)
    {
      string++;
    }
  }

  return string;
}

// I believe I wrote this earlier in the semester?
// If not, this is KC's code with my style on top.
char *strtok(char *string, char* delim)
{
  static char *oldstring;
  char * output;

  if (string == NULL)
  {
    string = oldstring;
  }

  if (*string == '\0')
  {
    return NULL;
  }

  output = string;
  string = findNextDelim(string, delim);

  if (*string != '\0')
  {
    *string = '\0';
    string++;
  }

  printf("    return token: %s\n", output);
  oldstring = string;
  return output;
}

int setzero(char *dst, int size)
{
  int i;
  for (i=0; i<size; i++)
    *dst++ = 0;
}
void delay()
{
   int i; for (i=0; i<10000; i++);
}
 
int copy(char *dest, char *src)
{
  int i;
  for (i=0; i<1024; i++)
    *dest++ = *src++;
}

int atoi(char *s)
{
  int v = 0;
  while(*s){
    v = 10*v + (*s - '0');
    s++;
  }
  return v;
}

