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
  char dirPath[] = "./Data/validTemplates";
  struct timeval start, stop;
  gettimeofday(&start, NULL);

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
    }
  }
  closedir (dp);


  gettimeofday(&stop, NULL);
  printf("Scanning a dir of 1000 templates and load+free each of them: %.3f ms per template\n",
    (stop.tv_sec - start.tv_sec) + 1e-6 * (stop.tv_usec - start.tv_usec));
}

int main(int argc, char **argv)
{
  benchLoadFree();
  return EXIT_SUCCESS;
}
