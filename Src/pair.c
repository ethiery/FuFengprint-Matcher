#include <math.h>
#include <stdlib.h>

#include "pair.h"

/**
 * Computes all the pairs of a minutia in `l1` and a minutia in `l2` with a similarity
 * score higher than `threshold`, and stores them in `pairs`.
 * Stores the nb of pairs in `nbPairs`.
 */
void Pair_buildAll(Pair* pairs, int *nbPairs, LMTS *l1, LMTS *l2,
                   float alpha, float thresh)
{
  *nbPairs = 0;

  int d;
  unsigned char d1, d2, d3;
  // We can prune pairs without computing the (more or less) expensive expf:
  // exp(- alpha * D) > thresh <=> D < - log(thresh) / alpha;
  float thresh2 = - log(thresh) / alpha;

  for (int m1 = 0 ; m1 < l1->nbMinutiae; m1++)
    for (int m2 = 0; m2 < l2->nbMinutiae; m2++)
    {
      d1 = (l1->r[m1] < l2->r[m2]) ? (l2->r[m2] - l1->r[m1]) : (l1->r[m1] - l2->r[m2]);
      d2 = l1->a[m1] - l2->a[m2];
      d2 = (d2 > 128) ? 256 - d2 : d2;
      d3 = l1->o[m1] - l2->o[m2];
      d3 = (d3 > 128) ? 256 - d3 : d3;

      d = d1 + d2 + d3;
      if (d < thresh2)
      {
        pairs[*nbPairs].m1 = m1;
        pairs[*nbPairs].m2 = m2;
        pairs[*nbPairs].sim = expf(-alpha * d);
        (*nbPairs)++;
      }
    }
}


/*
 * Returns:
 * - -1 if the pair a has a higher similiary score than the pair b
 * - 0 if the pairs a and b have the same similarity score
 * - 1 if the pair b has a lower similiarity than the pair a
 *
 * Used with qsort, sorts a an array of Pairs by decreasing similarity
 */
int Pair_cmp(const void * a, const void * b)
{
  Pair *p1 = (Pair *)a;
  Pair *p2 = (Pair *)b;
  return p2->sim > p1->sim ? 1 : -1;
}
