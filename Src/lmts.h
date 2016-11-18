#ifndef LMTS_H
#define LMTS_H

#include <stdio.h>
#include <stdlib.h>

#include "template.h"

/*
 * Local minutia topologic structure. Essentially the coordinates of a central minutia,
 * and the relative displacements of a set of neighbouring minutiae
 */
typedef struct LocalMinutiaTopologicStructure
{
  int nbMinutiae;
  float *r;          // radial coordinates in cm
  float *p;          // angular coordinates in radian
  float *o;          // orientation differences in radian;
} LMTS;

/*
 * Computes an array of all the LMTS of radius `r` (in cm) in
 * template `t`, using the specified pre computed squared distance matrix (in cm²)
 * `distances` and nb nbNeighbours array `nbNeighbours`.
 * Stores it in `lmts`
 */
void LMTS_buildAll(LMTS *lmts, T* t, float r, float *distances, int *nbNeighbours);

/**
 * Releases ressources associated with the n first LMTS of the specified
 * LMTS array
 */
void LMTS_free(int n, LMTS *lmts);

/**
 * Prints the specified lmts (if it is not NULL) on the specified stream
 */
void LMTS_print(FILE *stream, LMTS *lmts);


#endif
