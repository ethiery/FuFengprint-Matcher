#ifndef MTM_H
#define MTM_H

#include "pair.h"

/**
* The local Minutia Tensor Matrix of a set of minutia pairs is defined as follow.
* For any couple of minutia pairs (M1,M2), (M3,M4) .
*
* MTM((M1,M2), (M3,M4)) =
*     - 0            if M1 = M3 but M2 != M4 (M1 cannot be matched with both M2 and M4)
*     - sim(M1, M2)  if M1=M2 and M1'=M2' (i.e on the diagonal)
*     - gamma * comp((M1, M2), (M3, M4)) else
*
* where compatibility of two pairs is defined as
* comp((M1, M2), (M3, M4)) = exp(- beta * ||vec(M1M3) - vec(M2M4)||1)
*
* where beta and gamma are two manually set positive parameters.
*/

/**
 * Computes the MTMscore of the specified minutia pairs
 */
float MTMscore(int nbPairs, Pair *pairs, LMTS *l1, LMTS *l2, 
               float beta, float gamma, float thresh, int maxNbIters, int totalNbPairs);


#endif
