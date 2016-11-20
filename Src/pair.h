#ifndef PAIR_H
#define PAIR_H

#include "lmts.h"

/*
 * Minutia pair.
 * First index m1 is the index of a neighbouring minutia in a LMTS l1 of a template
 * Second index m2 is the index of a neighbouring minutia in another LMTS l2 of another template.
 *
 * The similarity score of a minutia pair (m1, m2) is defined as:
 * sim(m1, m2) = exp(- alpha * (|l1.r[m1] - l2.r[m2]| + |l1.p[m1] - l2.p[m2]| + |l1.o[m1] - l2.o[m2]|))
 *
 * where alpha is a manually set positive parameters
 *
 * Thus, a similarity score of 1 means m1 and m2 are identical, and a similarity
 * score of ~0 means they are not similar at all.
 */
typedef struct Pair
{
  int m1;
  int m2;
  float sim;
} Pair;

/**
 * Computes all the pairs of a minutia in `l1` and a minutia in `l2` with a similarity
 * score higher than `threshold`, and stores them in `pairs`.
 * Stores the nb of pairs in `nbPairs`.
 */
void Pair_buildAll(Pair* pairs, int *nbPairs, LMTS *l1, LMTS *l2,
                   float alpha, float thresh);

/*
 * Returns:
 * - -1 if the pair a has a higher similiary score than the pair b
 * - 0 if the pairs a and b have the same similarity score
 * - 1 if the pair b has a lower similiarity than the pair a
 *
 * Used with qsort, sorts a an array of Pairs by decreasing similarity
 */
int Pair_cmp(const void * a, const void * b);

#endif
