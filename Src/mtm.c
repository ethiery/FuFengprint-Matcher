#include <math.h>

// #include "cblas.h"
#include "mtm.h"
#include "mkl_types.h"
#include "mkl_spblas.h"
#include <mkl.h>

#define ALIGNMENT 32



/**
 * Computes the MTMscore of the specified minutia pairs
 */
float MTMscore(int nbPairs, Pair *pairs, LMTS *l1, LMTS *l2, 
               float beta, float gamma, float thresh, int maxNbIters, int totalNbPairs)
{
  if (nbPairs <= 0) return 0;
  if (nbPairs == 1) return pairs[0].sim / totalNbPairs;

  float thresh2 = - log(1e-4) / beta;
  int d;
  int dR[nbPairs];
  unsigned char dA[nbPairs];
  unsigned char dO[nbPairs];
  unsigned char d1, d2, d3;
  Pair *p;

//   char mtmUplo = 'u';
//   MKL_INT mtmNnz = 0;
//   MKL_INT mtmSize = nbPairs;
//   float *mtmVal = mkl_malloc(nbPairs * nbPairs * sizeof(float), ALIGNMENT);
//   MKL_INT *mtmCol = mkl_malloc(nbPairs * nbPairs * sizeof(MKL_INT), ALIGNMENT);
//   MKL_INT *mtmRow = mkl_malloc(nbPairs * nbPairs * sizeof(MKL_INT), ALIGNMENT);
  float *mtm = mkl_calloc(nbPairs * nbPairs, sizeof(float), ALIGNMENT);
  float *v = mkl_malloc(nbPairs * sizeof(float), ALIGNMENT);
  float *oldV = mkl_malloc(nbPairs * sizeof(float), ALIGNMENT);
  float *swap;

  for (int i = 0; i < nbPairs; i++)
  {
    p = pairs + i;
    dR[i] = (int)l2->r[p->m2] - (int)l1->r[p->m1];
    dA[i] = l2->a[p->m2] - l1->a[p->m1];
    dO[i] = l2->o[p->m2] - l1->o[p->m1];
  }

  for (int i = 0; i < nbPairs; i++)
  {
//     mtmVal[mtmNnz] = v[i] = pairs[i].sim;
//     mtmCol[mtmNnz] = mtmRow[mtmNnz] = i;
//     mtmNnz++;
    mtm[i * nbPairs + i] = pairs[i].sim;
    for (int j = i+1; j < nbPairs; j++)
    {
      if (pairs[i].m1 != pairs[j].m1 && pairs[i].m2 != pairs[j].m2)
      {
        d1 = (dR[i] < dR[j]) ? dR[j] - dR[i] : dR[i] - dR[j];
        d2 = dA[j] - dA[i];
        d2 = (d2 > 128) ? 256 - d2 : d2;
        d3 = dO[j] - dO[i];
        d3 = (d3 > 128) ? 256 - d3 : d3;
        d = d1 + d2 + d3;
        if (d < thresh2)
        {
//           mtmVal[mtmNnz] = gamma * expf(- beta * d);
//           mtmCol[mtmNnz] = j;
//           mtmRow[mtmNnz] = i;
//           mtmNnz++;
          mtm[j * nbPairs + i] = gamma * expf(- beta * d);
        }
      }
    }
  }

  cblas_sscal(nbPairs, 1/cblas_sasum(nbPairs, v, 1), v, 1);


  int iter = 0;
  float norm = 0, prevNorm; 
  do {
    swap = oldV; oldV = v; v = swap; prevNorm = norm;// oldV = V
//     mkl_cspblas_scoosymv(&mtmUplo, &mtmSize, mtmVal, mtmRow, mtmCol, &mtmNnz, oldV, v);
    cblas_ssymv(CblasColMajor, CblasUpper, nbPairs, 1, mtm, nbPairs, oldV, 1, 0, v, 1); // v = MTM*oldV
    norm = cblas_snrm2(nbPairs, v, 1);
    cblas_sscal(nbPairs, 1/norm, v, 1); // v = v / ||v||2
    iter++;
  } 
  while (fabsf(norm - prevNorm) > 1e-3 && iter < maxNbIters); // continue until ||oldV|| < 1e-4
  
//   mkl_free(mtmVal);
//   mkl_free(mtmCol);
//   mkl_free(mtmRow);
  mkl_free(mtm);
  mkl_free(oldV);

  // Discretizes
  int iMax, nbMatches = 0;
  float score = 0;
  while (1)
  {
    // Choose best minutia pair
    iMax = cblas_isamax(nbPairs, v, 1);
    if (v[iMax] < 1e-4) break;
    // Adds it to the count, and its score to the global score
    nbMatches++;
    score += pairs[iMax].sim;
    // Invalidate all incompatible pairs (amongst which the one with a common minutia)
    for (int i = 0; i < nbPairs; i++)
    {
      if (pairs[i].m1 == pairs[iMax].m1 || pairs[i].m2 == pairs[iMax].m2)
        v[i] = 0;
    }
  }

  mkl_free(v);
  mkl_thread_free_buffers();
  // if (score * nbMatches * nbMatches / totalNbPairs > 0.1)
  //   printf("%.3f\n", score * nbMatches * nbMatches / totalNbPairs);
  return score * nbMatches * nbMatches / totalNbPairs;
}
