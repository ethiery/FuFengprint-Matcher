#include <malloc.h>
#include <math.h>
// #include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "template.h"

#define FORMAT_SIZE 8
#define ISO2005_HEADER_SIZE 20
#define ISO2005_MINUTIA_SIZE 6

// Private data structures / functions declarations

typedef enum TemplateFormat { ISO2005, UNKNOWN } TemplateFormat;

TemplateFormat checkFormat(FILE *f);
int T_loadISO2005(T *t, FILE *f);

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
int T_load(T *t, char *filePath)
{
  FILE *f;

  if (t == NULL)
    return 1;

  if ((f = fopen(filePath, "r")) == NULL)
    return 2;

  TemplateFormat format = checkFormat(f);
  int ret;
  switch (format)
  {
    case UNKNOWN:
      ret = 3;
      break;

    case ISO2005:
      ret = T_loadISO2005(t, f);
      break;
  }

  fclose(f);
  return ret;
}

/**
 * Explains the specified error code (returned by T_load) on the specified stream
 */
void printLoadError(FILE *stream, int err)
{
  const char *T_loadErrorMessages[5] = {
    "Template successfuly loaded",
    "Internal error",
    "The specified template does not exist.",
    "The format of the specified template is not supported",
    "The specified template file is invalid"
  };

  if (err < 5)
    fprintf(stderr, "%s\n", T_loadErrorMessages[err]);
}

/*
 * Computes the the distances between all pairs of minutiae
 * in the template `t` and stores them in `distances`.
 * Additionnaly in the same pass, count for each minutia the number of neighbouring
 * minutiae closer than the threshold `r` (in cm) and stores them in `nbNeighbours`
 */
void T_computeDistances(T* t, float *distances, float r, int *nbNeighbours)
{
  int n = t->nbMinutiae;
  float dx, dy;
  float hFactor = 1.0 / t->hDensity;
  float vFactor = 1.0 / t->vDensity;
  int isNeighbour;

  for (int i = 0; i < n; i++)
  {
    nbNeighbours[i] = 0;
    distances[i * n + i] = 0;
    for (int j = 0; j < i; j++)
    {
      dx = (t->x[i] - t->x[j]) * hFactor;
      dy = (t->y[i] - t->y[j]) * vFactor;
      distances[i * n + j] = distances[j * n + i] = sqrtf(dx*dx + dy*dy);
      isNeighbour = (distances[i * n + j] < r) ? 1 : 0;
      nbNeighbours[i] += isNeighbour;
      nbNeighbours[j] += isNeighbour;
    }
  }
}

/**
 * Releases the resources associated with a template object
 * To be called after every successful call to T_load
 */
void T_free(T *t)
{
  if (t != NULL)
  {
    if (t->x != NULL) free(t->x);
    if (t->y != NULL) free(t->y);
    if (t->o != NULL) free(t->o);
    if (t->t != NULL) free(t->t);
    if (t->q != NULL) free(t->q);
  }
}

/**
 * Prints the specified template (if it is not NULL) on the specified stream
 * To be called only after a successful call to T_load
 */
void T_print(FILE *stream, T *t)
{
  fprintf(stream, "Template @ %p\n", t);
  if (t != NULL)
  {
    fprintf(stream, "Width = %hu pixels, Height = %hu pixels\n"
      "Horizontal density = %hu pixels per cm, Vertical density = %hu pixels per cm\n"
      "Quality = %hhu\n"
      "%hhu minutiae: ",
      t->width, t->height, t->hDensity, t->vDensity, t->quality, t->nbMinutiae);

    for (int i = 0; i < t->nbMinutiae; i++)
      fprintf(stream, "(x = %hu, y = %hu, orientation = %hhu, type = %hhu, quality = %hhu)\n", t->x[i], t->y[i], t->o[i], t->t[i], t->q[i]);
  }
  fprintf(stream, "\n");
}


// Private function definitions

/**
 * Reads the FORMAT_SIZE first bytes of the specified template file,
 * to determine its format.
 *
 * Returns:
 * UNKOWN in case of failure to read the bytes, or if the format is unknown.
 * the format of the file else.
 */
TemplateFormat checkFormat(FILE *f)
{
  const unsigned char ISO_FORMAT_BYTES[] = { 'F', 'M', 'R', 0, ' ', '2', '0', 0 };

  unsigned char formatBytes[FORMAT_SIZE];
  TemplateFormat format = UNKNOWN;

  if (fread(formatBytes, 1, FORMAT_SIZE, f) == FORMAT_SIZE)
  {
    if (memcmp(formatBytes, ISO_FORMAT_BYTES, FORMAT_SIZE) == 0)
      format = ISO2005;
  }

  return format;
}


/**
 * Reads an ISO 2005 formatted template file starting right after the format bytes,
 * and initializes the specified template object.
 *
 * t and f are supposed non NULL
 *
 * Returns:
 * 0 in case of success.
 * 1 in case of internal error (failed memory allocation), some allocated memory may not be freed
 * 4 if the template file is invalid.
 *
 */
int T_loadISO2005(T *t, FILE *f)
{
  unsigned char buffer[ISO2005_HEADER_SIZE];

  if (fread(buffer, 1, ISO2005_HEADER_SIZE, f) != ISO2005_HEADER_SIZE)
    return 4;

  // Record length (buffer[0-3])
  // padding zeros (buffer[4-5])
  t->width = (buffer[6] << 8) + buffer[7];
  t->height = (buffer[8] << 8) + buffer[9];
  t->hDensity = (buffer[10] << 8) + buffer[11];
  t->vDensity = (buffer[12] << 8) + buffer[13];
  // Number of fingerprint (buffer[14])
  // padding zero (buffer[15])
  // First finger position (buffer[16])
  // padding zero (buffer[17])
  t->quality = buffer[18];
  t->nbMinutiae = buffer[19];
  t->x = malloc(t->nbMinutiae * sizeof(short));
  t->y = malloc(t->nbMinutiae * sizeof(short));
  t->o = malloc(t->nbMinutiae * sizeof(char));
  t->t = malloc(t->nbMinutiae * sizeof(char));
  t->q = malloc(t->nbMinutiae * sizeof(char));
  if (!(t->x && t->y && t->o && t->t && t->q))
    return 1;

  for (int i = 0; i < t->nbMinutiae; i++)
  {
    if (fread(buffer, 1, ISO2005_MINUTIA_SIZE, f) != ISO2005_MINUTIA_SIZE)
    {
      free(t->x); free(t->y); free(t->o); free(t->t); free(t->q);
      return 4;
    }

    t->t[i] = (buffer[0] & 0xC0) >> 6;
    t->x[i] = ((buffer[0] & 0x3F) << 8) + buffer[1];
    t->y[i] = ((buffer[2] & 0x3F) << 8) + buffer[3];
    t->o[i] = buffer[4];
    t->q[i] = buffer[5];
  }

  return 0;
}
