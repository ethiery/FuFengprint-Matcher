#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "template.h"


void testValidISO2005Load()
{
  printf("Loading a valid ISO 2005 template: ");
  fflush(stdout);
  T template;
  int ret = T_load(&template, "Data/validISO2005.dat");
  printLoadError(stdout, ret);
  T_free(&template);
  assert(ret == 0);
}


void testInexistantLoad()
{
  printf("Loading an inexistant template: ");
  fflush(stdout);
  T template;
  int ret = T_load(&template, "Data/inexistant.dat");
  printLoadError(stdout, ret);
  assert(ret == 2);
}

void testInvalidISO2005Load()
{
  printf("Loading an invalid ISO 2005 template: ");
  fflush(stdout);
  T template;
  int ret = T_load(&template, "Data/invalidISO2005.dat");
  printLoadError(stdout, ret);
  assert(ret == 4);
}

void testUnsuppportedFormat()
{
  printf("Loading a template of unsupported format: ");
  fflush(stdout);
  T template;
  int ret = T_load(&template, "Data/unsupportedFormat.dat");
  printLoadError(stdout, ret);
  assert(ret == 3);
}


int main(int argc, char** argv)
{
  testValidISO2005Load();
  testInexistantLoad();
  testInvalidISO2005Load();
  testUnsuppportedFormat();
  return EXIT_SUCCESS;
}
