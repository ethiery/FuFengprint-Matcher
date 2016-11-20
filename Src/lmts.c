#include <math.h>
#include <stdio.h>

#include "lmts.h"

#define UINT8_TO_RAD 0.02454369260617026
#define RADS_TO_UINT8 40.7436654315252

#ifndef M_PI
  #define M_PI 3.141592653589793
#endif

#ifndef M_2PI
  #define M_2PI 6.283185307179586
#endif


/*
 * Computes an array of all the LMTS of radius `r` (in cm) in
 * template `t`, using the specified pre computed distance matrix (in cm)
 * `distances` and nb nbNeighbours array `nbNeighbours`.
 * Stores it in `lmts`
 */
void LMTS_buildAll(LMTS *lmts, T* t, float r, float *distances, int *nbNeighbours)
{
  int n = t->nbMinutiae;

  for (int i = 0; i < n; i++)
  {
    lmts[i].nbMinutiae = 0;
    lmts[i].r = (unsigned char *) malloc(nbNeighbours[i] * sizeof(unsigned char));
    lmts[i].a = (unsigned char *) malloc(nbNeighbours[i] * sizeof(unsigned char));
    lmts[i].o = (unsigned char *) malloc(nbNeighbours[i] * sizeof(unsigned char));
  }

  float pRad;
  unsigned char pMapped;
  unsigned char pMappedInverse;
  float CM_TO_0_128 = 128 / r;

  for (int i = 0; i < n; i++)
  {
    for (int j = i + 1; j < n; j++)
    {
      if (distances[i * n + j] < r)
      {
        lmts[i].r[lmts[i].nbMinutiae] = (unsigned char)(CM_TO_0_128 * distances[i * n + j]);
        lmts[j].r[lmts[j].nbMinutiae] = lmts[i].r[lmts[i].nbMinutiae];

        pRad = atan2f(t->y[j] - t->y[i], t->x[j] - t->x[i]);
        pRad += (pRad < 0) ? M_2PI : 0; // in [0, 2pi[
        pMapped = (unsigned char)(pRad * RADS_TO_UINT8);
        pMappedInverse = pMapped + 128;

        lmts[i].a[lmts[i].nbMinutiae] = pMapped - t->o[i];
        lmts[j].a[lmts[j].nbMinutiae] = pMappedInverse - t->o[j];

        lmts[i].o[lmts[i].nbMinutiae] = t->o[j] - pMapped;
        lmts[j].o[lmts[j].nbMinutiae] = t->o[i] - pMappedInverse;

        lmts[i].nbMinutiae++;
        lmts[j].nbMinutiae++;
      }
    }
  }
}

/**
 * Releases ressources associated with the n first LMTS of the specified
 * LMTS array
 */
void LMTS_free(int n, LMTS *lmts)
{
  if (lmts != NULL)
  {
    for (int i = 0; i < n; i++)
    {
      free(lmts[i].r);
      free(lmts[i].a);
      free(lmts[i].o);
    }
  }
}

/**
 * Prints the specified lmts (if it is not NULL) on the specified stream
 */
void LMTS_print(FILE *stream, LMTS *lmts)
{
  fprintf(stream, "LMTS @ %p:\n", lmts);
  if (lmts != NULL)
  {
    for (int i = 0; i < lmts->nbMinutiae; i++)
    {
      fprintf(stream, "(%hhu, %hhu, %hhu)\n", lmts->r[i], lmts->a[i], lmts->o[i]);
    }
  }
}
