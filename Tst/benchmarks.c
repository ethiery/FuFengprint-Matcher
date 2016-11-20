#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <dirent.h>

#include "lmts.h"
#include "template.h"
#include "pair.h"

void benchLoadFree()
{
  printf("For accurate measurements, don't forget to disable powersave:\n"
    "sudo cpufreq-set -g performance\n\n");

  struct timeval start, stop;
  gettimeofday(&start, NULL);


  int nbMatches = 0;
  float r = 0.3;

  char dirPath[] = "./Data/validTemplates";
  DIR *probeDir = opendir (dirPath);
  assert(probeDir != NULL);
  struct dirent *probeDirEntry;
  T probe;
  char probeFilePath[300];

  while ((probeDirEntry = readdir(probeDir)) && nbMatches < 10000)
  {
    // Preparing probe
    if (strcmp(probeDirEntry->d_name + (strlen(probeDirEntry->d_name)-4), ".fmr") == 0)
    {
      snprintf(probeFilePath, 300, "%s/%s", dirPath, probeDirEntry->d_name);
      T_load(&probe, probeFilePath);

      float probeDist[probe.nbMinutiae * probe.nbMinutiae];
      int probeNbNeighb[probe.nbMinutiae];
      LMTS probeLmts[probe.nbMinutiae];
      T_computeDistances(&probe, probeDist, r, probeNbNeighb);
      LMTS_buildAll(probeLmts, &probe, r, probeDist, probeNbNeighb);

      // Goes through candidates one by one
      DIR *candidateDir = opendir (dirPath);
      assert(candidateDir != NULL);
      struct dirent *candidateDirEntry;
      T candidate;
      char candidateFilePath[300];

      while ((candidateDirEntry = readdir(candidateDir)))
      {
        // Preparing candidate
        if (strcmp(candidateDirEntry->d_name + (strlen(candidateDirEntry->d_name)-4), ".fmr") == 0)
        {
          snprintf(candidateFilePath, 300, "%s/%s", dirPath, candidateDirEntry->d_name);
          T_load(&candidate, candidateFilePath);

          float candidateDist[candidate.nbMinutiae * candidate.nbMinutiae];
          int candidateNbNeighb[candidate.nbMinutiae];
          LMTS candidateLmts[candidate.nbMinutiae];
          T_computeDistances(&candidate, candidateDist, r, candidateNbNeighb);
          LMTS_buildAll(candidateLmts, &candidate, r, candidateDist, candidateNbNeighb);

          // Match
          Pair pairs[probe.nbMinutiae * candidate.nbMinutiae];
          int nbPairs;
          for (int c1 = 0; c1 < probe.nbMinutiae; c1++)
            for (int c2 = 0; c2 < candidate.nbMinutiae; c2++)
            {
                Pair_buildAll(pairs, &nbPairs, probeLmts+c1, candidateLmts+c2, 0.1, 1e-3);
            }


          LMTS_free(candidate.nbMinutiae, candidateLmts);
          T_free(&candidate);

          nbMatches++;
        }
      }

      closedir(candidateDir);
      LMTS_free(probe.nbMinutiae, probeLmts);
      T_free(&probe);
    }
  }
  closedir(probeDir);

  gettimeofday(&stop, NULL);
  double ms = 1e3 * (stop.tv_sec - start.tv_sec) + 1e-3 * (stop.tv_usec - start.tv_usec);
  printf("Performing %d matches: %.3f ms per match\n",
    nbMatches, ms / nbMatches);
}

int main(int argc, char **argv)
{
  benchLoadFree();
  return EXIT_SUCCESS;
}
