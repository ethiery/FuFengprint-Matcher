#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "lmts.h"
#include "pair.h"
#include "template.h"

#ifndef EPS
  #define EPS 1e-5
#endif

#ifndef M_PI
  #define M_PI 3.141592653589793
#endif


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

void initTestTemplate(T *template)
{
  template->hDensity = 197;
  template->vDensity = 197;
  template->nbMinutiae = 3;
  template->x = (unsigned short *) malloc(template->nbMinutiae * sizeof(unsigned short));
  template->y = (unsigned short *) malloc(template->nbMinutiae * sizeof(unsigned short));
  template->o = (unsigned char *) malloc(template->nbMinutiae * sizeof(unsigned char));
  template->t = (unsigned char *) malloc(template->nbMinutiae * sizeof(unsigned char));
  template->q = (unsigned char *) malloc(template->nbMinutiae * sizeof(unsigned char));

  for (int i = 0; i < 3; i++)
  {
    template->x[i] = i*10;
    template->y[i] = 10 + i*10;
    template->o[i] = i*64;
  }
}

void initTestDistances(float *distances)
{
  float sqrt2 = sqrtf(2);
  for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++)
      distances[i * 3 + j]  = sqrt2 * ((j-i)*10/197.0);
}

void initTestNbNeighbours(int *nbNeighbours)
{
  nbNeighbours[0] = 1;
  nbNeighbours[1] = 2;
  nbNeighbours[2] = 1;
}

float initTestRadius()
{
  return 20.0 / 197;
}

int initTestNbMinutiae()
{
  return 3;
}

void initTestLMTS(LMTS *lmts)
{
  lmts[0].nbMinutiae = 1;
  lmts[1].nbMinutiae = 2;
  lmts[2].nbMinutiae = 1;
  for (int i = 0; i < 3; i++)
  {
    lmts[i].r = (unsigned char *) malloc(lmts[i].nbMinutiae * sizeof(unsigned char));
    lmts[i].a = (unsigned char *) malloc(lmts[i].nbMinutiae * sizeof(unsigned char));
    lmts[i].o = (unsigned char *) malloc(lmts[i].nbMinutiae * sizeof(unsigned char));
  }


  short r = 90; // (short)(128 * sqrtf(2) * (10/20.0));
  lmts[0].r[0] = r;
  lmts[1].r[0] = r;
  lmts[1].r[1] = r;
  lmts[2].r[0] = r;

  lmts[0].a[0] = 32;
  lmts[1].a[0] = 96;
  lmts[1].a[1] = 224;
  lmts[2].a[0] = 32;

  lmts[0].o[0] = 32;
  lmts[1].o[0] = 96;
  lmts[1].o[1] = 96;
  lmts[2].o[0] = 160;
}

void testComputeDistances()
{
  printf("Computing distances between the minutiae of a template: ");

  T template;
  initTestTemplate(&template);

  float r = initTestRadius();
  int n = initTestNbMinutiae();
  float distances[n * n];
  float expectedDistances[n * n];
  int nbNeighbours[n];
  int expectedNbNeighbours[n];

  T_computeDistances(&template, (float *)distances, r, (int *)nbNeighbours);

  initTestDistances(expectedDistances);
  initTestNbNeighbours(expectedNbNeighbours);
  for (int i = 0; i < n; i++)
  {
    assert(nbNeighbours[i] == expectedNbNeighbours[i]);
    for (int j = 0; j < n; j++)
    {
      assert(abs(distances[i * n + j] - expectedDistances[i * n + j]) < EPS);
    }
  }

  T_free(&template);
  printf("ok\n");
}

void testLMTSbuildAll()
{
  printf("Computing the LMTS of a template: ");

  T template;
  float r = initTestRadius();
  int n = initTestNbMinutiae();
  float distances[n * n];
  int nbNeighbours[n];
  initTestTemplate(&template);
  initTestDistances(distances);
  initTestNbNeighbours(nbNeighbours);

  LMTS lmts[n];
  LMTS_buildAll(lmts, &template, r, distances, nbNeighbours);
  LMTS expectedLmts[n];
  initTestLMTS(expectedLmts);

  for (int i = 0; i < n; i++)
  {
    assert(lmts[i].nbMinutiae == expectedLmts[i].nbMinutiae);
    for (int j = 0; j < lmts[i].nbMinutiae; j++)
    {
      assert(lmts[i].r[j] == expectedLmts[i].r[j]);
      assert(lmts[i].a[j] == expectedLmts[i].a[j]);
      assert(lmts[i].o[j] == expectedLmts[i].o[j]);
    }
  }

  T_free(&template);
  LMTS_free(n, lmts);
  LMTS_free(n, expectedLmts);
  printf("ok\n");
}

void testPairBuildAll()
{
  printf("Computing all pairs of LMTS between 2 templates: ");

  LMTS lmts[2];
  lmts[0].nbMinutiae = 2;
  lmts[1].nbMinutiae = 3;
  for (int i = 0; i < 2; i++)
  {
    lmts[i].r = (unsigned char *) malloc(lmts[i].nbMinutiae * sizeof(unsigned char));
    lmts[i].a = (unsigned char *) malloc(lmts[i].nbMinutiae * sizeof(unsigned char));
    lmts[i].o = (unsigned char *) malloc(lmts[i].nbMinutiae * sizeof(unsigned char));
  }

  lmts[0].r[0] = 100;
  lmts[0].a[0] = 0;
  lmts[0].o[0] = 10;

  lmts[0].r[1] = 20;
  lmts[0].a[1] = 200;
  lmts[0].o[1] = 250;

  lmts[1].r[0] = 50;
  lmts[1].a[0] = 0;
  lmts[1].o[0] = 10;

  lmts[1].r[1] = 100;
  lmts[1].a[1] = 216;
  lmts[1].o[1] = 10;

  lmts[1].r[2] = 100;
  lmts[1].a[2] = 0;
  lmts[1].o[2] = 30;

  Pair pairs[lmts[0].nbMinutiae * lmts[1].nbMinutiae];
  int nbPairs;

  Pair_buildAll(pairs, &nbPairs, &lmts[0], &lmts[1], 0.1, 1e-4);

  assert(nbPairs == 3);
  assert(pairs[0].m1 == 0 && pairs[0].m2 == 0 && fabs(pairs[0].sim - expf(-5)) < EPS);
  assert(pairs[1].m1 == 0 && pairs[1].m2 == 1 && fabs(pairs[1].sim - expf(-4)) < EPS);
  assert(pairs[2].m1 == 0 && pairs[2].m2 == 2 && fabs(pairs[2].sim - expf(-2)) < EPS);

  printf("ok\n");
}

int main(int argc, char** argv)
{
  testValidISO2005Load();
  testInexistantLoad();
  testInvalidISO2005Load();
  testUnsuppportedFormat();
  testComputeDistances();
  testLMTSbuildAll();
  testPairBuildAll();
  return EXIT_SUCCESS;
}
