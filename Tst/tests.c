#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "template.h"


void testValidISO2005Load()
{
  printf("Loading a valid ISO 2005 template: ");
  fflush(stdout);
  T template;
  int ret = T_load(&template, "Data/validISO2005.fmr");
  printLoadError(stdout, ret);
  assert(template.width == 300);
  assert(template.height == 400);
  assert(template.hDensity == 197);
  assert(template.vDensity == 197);
  assert(template.quality == 42);
  assert(template.nbMinutiae == 3);

  assert(template.t[0] == 0);
  assert(template.x[0] == 10);
  assert(template.y[0] == 20);
  assert(template.o[0] == 30);
  assert(template.q[0] == 40);

  assert(template.t[1] == 1);
  assert(template.x[1] == 11);
  assert(template.y[1] == 21);
  assert(template.o[1] == 31);
  assert(template.q[1] == 41);

  assert(template.t[2] == 2);
  assert(template.x[2] == 12);
  assert(template.y[2] == 22);
  assert(template.o[2] == 32);
  assert(template.q[2] == 42);


  T_free(&template);
  assert(ret == 0);
}


void testInexistantLoad()
{
  printf("Loading an inexistant template: ");
  fflush(stdout);
  T template;
  int ret = T_load(&template, "Data/inexistant.fmr");
  printLoadError(stdout, ret);
  assert(ret == 2);
}

void testInvalidISO2005Load()
{
  printf("Loading an invalid ISO 2005 template: ");
  fflush(stdout);
  T template;
  int ret = T_load(&template, "Data/invalidISO2005.fmr");
  printLoadError(stdout, ret);
  assert(ret == 4);
}

void testUnsuppportedFormat()
{
  printf("Loading a template of unsupported format: ");
  fflush(stdout);
  T template;
  int ret = T_load(&template, "Data/unsupportedFormat.fmr");
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
