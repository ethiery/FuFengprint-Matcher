#ifndef LMTS_H
#define LMTS_H

#include <stdio.h>
#include <stdlib.h>

#include "template.h"

/*
 * Local minutia topologic structure = neighbouring minutia within a given
 * distance R of a central minutia.
 *
 * Each neighbouring minutia m, forming a vector v with the central minutia c
 * is defined by 3 coordinates:
 * - the length of v (radial coordinate)
 * - the angle between v and the orientation of c (angular coordinate)
 * - the angle between the orientation of m and v(orientation coordinate)
 *
 * To make computations on angles easier and more efficients, we use
 * the wrap around behaviour on C unsigned chars.
 * Thus, angles in [0, 2pi[ are mapped over [0, 256[
 *
 * Also, in order to have equal weights for each of the coordinate when
 * computing the L1 distance between 2 neighbouring minutia, we actually
 * use the ratio (length of v / R) mapped over [0, 128[ as radial coordinate
 *
 * This way, the greatest distance between two radial coordinates or two
 * angular coordinates or two orientation coordinates is 128.
 * (The L1 distance between 2 neighbouring minutia is thus in [0, 384]).
 *
 * Note : the coordinates of a neighbouring minutia are not sensible to
 * translations or rotations of the fingerprint
 *
 */
typedef struct LocalMinutiaTopologicStructure
{
  int nbMinutiae;
  unsigned char *r;             // radial coordinate
  unsigned char *a;     // angular coordinate
  unsigned char *o;     // orientation coordinate
} LMTS;

/*
 * Computes an array of all the LMTS of radius `r` (in cm) in
 * template `t`, using the specified pre computed distance matrix (in cm)
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
