#ifndef TEMPLATE_H
#define TEMPLATE_H

#include <stdio.h>

/*
 * Fingerprint template.
 * Typical usage:
 *    T template;
 *    int err = T_load(&template, "pathToTemplateFile");
 *    if (err != 0)
 *    {
 *      printLoadError(stderr, err);
 *    }
 *    else
 *    {
 *      // Do what you have to do with the template
 *      T_free(&template);
 *    }
 */
typedef struct Template
{
  unsigned short width;     // In pixels
  unsigned short height;    // In pixels
  unsigned short hDensity;  // In pixels per cm
  unsigned short vDensity;  // In pixels per cm
  unsigned char quality;
  unsigned char nbMinutiae;
  unsigned short *x;        // minutia abscissae
  unsigned short *y;        // minutia ordinates
  unsigned char *o;        // minutia orientations
  unsigned char *t;         // minutia types
  unsigned char *q;         // minutia quality scores
} T;

/**
 * Reads a template file, and initializes the specified template object.
 *
 * Currently, the only format supported is ISO 2005. Only the first fingerprint
 * of the template file is considered.
 *
 * Returns:
 * 0 in case of success.
 * 1 if there is an internal error (Null pointer, failed memory allocation)
 *   some allocated memory may then not be freed
 * 2 if the specified file does not exist.
 * 3 if the format of the specified file is not supported.
 * 4 if the file is invalid.
 */
int T_load(T *t, char *filePath);

/**
 * Explains the specified error code (returned by T_load) on the specified stream
 */
void printLoadError(FILE *stream, int err);

/*
 * Computes the the distances between all pairs of minutiae
 * in the template `t` and stores them in `distances`.
 * Additionnaly in the same pass, count for each minutia the number of neighbouring
 * minutiae closer than the threshold `r` (in cm) and stores them in `nbNeighbours`
 */
void T_computeDistances(T* t, float *distances, float r, int *nbNeighbours);


/**
 * Releases the resources associated with a template object
 * To be called after every successful call to T_load
 */
void T_free(T *temp);

/**
 * Prints the specified template (if it is not NULL) on the specified stream
 * To be called only after a successful call to T_load
 */
void T_print(FILE *stream, T *temp);

#endif
