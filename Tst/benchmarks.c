#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <dirent.h>

#include "lmts.h"
#include "template.h"

void benchLoadFree()
{
  printf("For accurate measurements, don't forget to disable powersave:\n"
    "sudo cpufreq-set -g performance\n\n");

  char dirPath[] = "./Data/validTemplates";
  struct timeval start, stop;
  gettimeofday(&start, NULL);

  int nbTemplates = 0;
  T t;
  char filePath[300];
  int len;
  DIR *dp = opendir (dirPath);
  assert (dp != NULL);
  struct dirent *ep;

  int r = 1.0;

  while ((ep = readdir(dp)))
  {
    len = strlen(ep->d_name);
    if (strcmp(ep->d_name + (len-4), ".fmr") == 0)
    {
      snprintf(filePath, 300, "%s/%s", dirPath, ep->d_name);
      T_load(&t, filePath);
      float distances[t.nbMinutiae * t.nbMinutiae];
      int nbNeighbours[t.nbMinutiae];
      T_computeDistances(&t, distances, r, nbNeighbours);
      LMTS lmts[t.nbMinutiae];
      LMTS_buildAll(lmts, &t, r, distances, nbNeighbours);
      LMTS_free(t.nbMinutiae, lmts);
      T_free(&t);
      nbTemplates += 1;
    }
  }
  closedir (dp);


  gettimeofday(&stop, NULL);
  double ms = 1e3 * (stop.tv_sec - start.tv_sec) + 1e-3 * (stop.tv_usec - start.tv_usec);
  printf("Scanning a dir of %d templates and load+free each of them: %.3f ms per template\n",
    nbTemplates, ms / nbTemplates);
}

int main(int argc, char **argv)
{
  benchLoadFree();
  return EXIT_SUCCESS;
}
