#include <math.h>
#include <stdio.h>

#include "lmts.h"

#define TO_RAD 0.024639942381096400

#ifndef M_PI
  #define M_PI 3.14159265358979323846
#endif

/*
 * Computes an array of all the LMTS of radius `r` (in cm) in
 * template `t`, using the specified pre computed squared distance matrix (in cm²)
 * `distances` and nb nbNeighbours array `nbNeighbours`.
 * Stores it in `lmts`
 */
void LMTS_buildAll(LMTS *lmts, T* t, float r, float *distances, int *nbNeighbours)
{
  float r2 = r * r;
  int n = t->nbMinutiae;
  float p, o;

  for (int i = 0; i < n; i++)
  {
    lmts[i].nbMinutiae = 0;
    lmts[i].r = (float *) malloc(nbNeighbours[i] * sizeof(float));
    lmts[i].p = (float *) malloc(nbNeighbours[i] * sizeof(float));
    lmts[i].o = (float *) malloc(nbNeighbours[i] * sizeof(float));
  }

  for (int i = 0; i < n; i++)
  {
    for (int j = i + 1; j < n; j++)
    {
      if (distances[i * n + j] < r2)
      {
          lmts[i].r[lmts[i].nbMinutiae] = distances[i * n + j];
          lmts[j].r[lmts[j].nbMinutiae] = distances[i * n + j];
          p = atan2f(t->y[j] - t->y[i], t->x[j] - t->x[i]);
          lmts[i].p[lmts[i].nbMinutiae] = p;
          lmts[j].p[lmts[j].nbMinutiae] = (p > 0) ? p - M_PI : p + M_PI;
          o = TO_RAD * (t->o[j] - t->o[i]);
          lmts[i].o[lmts[i].nbMinutiae] = o;
          lmts[j].o[lmts[j].nbMinutiae] = -o;
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
      free(lmts[i].p);
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
      fprintf(stream, "(%.2f, %.2f, %.2f)\n", lmts->r[i], lmts->p[i], lmts->o[i]);
    }
  }
}
