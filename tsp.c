/** @file     tsp.c
 *  @brief    Traveling Salesman Problem.
 *  @author   Marcelo Pinto (xmrcl0@gmail.com)
 *  @date     09/12/2017
 *  @version  0.2
 */

#include "tsp.h"
#include "print.h"
#include "utils.h"

void
help (void)
{
  printf ("usage: tsp [-h] [-n <ITER>] -m <MODE> -f <FILE>\n");
  printf ("Find best path to Traveling Salesman Problem using Monte Carlo Method\n\n");
  printf ("Options:\n");
  printf ("  -n <ITER>    Number of paths to simulate\n");
  printf ("  -m <MODE>    Exibition mode 0, 1 or 2 (silent = 0)\n");
  printf ("  -f <FILE>    Cities coordinates file\n");
  printf ("  -h           Show this help message and exit\n\n");
  printf ("Example:\n");
  printf ("  tsp -n 5 -m 0 -f data/grid04_xy.txt   # Simulates 5 paths for 4 cities data file\n");
}


void
distance_matrix (float ***coord, float ***distance, int num_city)
{
  int i, j, nrows, ncols;

  ncols = num_city;
  nrows = num_city;

  *distance = (float **) malloc (nrows * sizeof (float *));
  for (i = 0; i < nrows; i++)
    (*distance)[i] = (float *) malloc (ncols * sizeof (float));

  for (i = 0; i < num_city; i++)
    for (j = 0; j < num_city; j++)
      (*distance)[i][j] = sqrt (pow ((*coord)[i][0] - (*coord)[j][0], 2) + pow ((*coord)[i][1] - (*coord)[j][1], 2));
}


void
create_path (int num_city, int **coord)
{
  int i;

  (*coord) = (int *) malloc ((num_city + 1) * sizeof (int));

  randperm (num_city, *coord);
  (*coord)[num_city] = (*coord)[0];
}


float
measure_path (float ***distance, int num_city, int **path)
{
  int i;
  float l = 0;

  for (i = 0; i < num_city; i++)
    l = l + (*distance)[(*path)[i]][(*path)[i + 1]];
  return l;
}


int
read_file (char *file, float ***array)
{
  int i, j, nrows = 0, ncols = 2;
  char c;
  char *line = NULL;
  size_t len=0;
  FILE *fp;

  fp = fopen (file, "r");
  if (fp == NULL)
    return 0;

  while ((getline(&line, &len, fp) != -1))
  { 
    if (!is_coordinate (line))
      return -1;
    nrows++;
  }
  free(line);

  // Allocate memory for coordinates matrix 
  *array = (float **) malloc (nrows * sizeof (float *));
  for (i = 0; i < nrows; i++)
    (*array)[i] = (float *) malloc (ncols * sizeof (float));

  // Read coordinates from file to coordinates matrix
  fseek (fp, 0, SEEK_SET);
  for (i = 0; i < nrows; i++)
    for (j = 0; j < ncols; j++)
      if (!fscanf (fp, "%f", &(*array)[i][j]))
	break;
  fclose (fp);

  return nrows;
}


/*
 * Main routine
 */
int
main (int argc, char **argv)
{
  char c;
  long double i, num_iter;
  int num_cities, nflag = 0, mflag = 0, fflag = 0, mode;
  float **coord, **distance;
  int *path, *min_path;
  float len = 0, min_len = FLT_MAX;
  FILE *file;


  // Read and parse command line arguments
  opterr = 0;
  while ((c = getopt (argc, argv, "n:m:f:h::")) != -1)
    switch (c)
    {
    case 'n':
      nflag = 1;
      if (!is_integer (optarg))
      {
	      fprintf (stderr, "%s: error: number of simulations must be an integer\n", argv[0]);
	      exit (EXIT_FAILURE);
      }
      else
	      num_iter = strtold (optarg, NULL);
      break;
    case 'm':
      mflag = 1;
      if (!is_positive_number (optarg))
      {
	      fprintf (stderr, "%s: error: invalid mode, choose 0, 1 or 2\n", argv[0]);
	      exit (EXIT_FAILURE);
      }
      else
	      mode = atoi (optarg);
      if (mode > 2)
      {
	      fprintf (stderr, "%s: error: invalid mode, choose 0, 1 or 2\n", argv[0]);
	      exit (EXIT_FAILURE);
      }
      break;
    case 'f':
      fflag = 1;
      num_cities = read_file (optarg, &coord);
      if (num_cities == 0)
      {
	      fprintf (stderr, "%s: error: no such file or directory\n", argv[0]);
	      exit (EXIT_FAILURE);
      }
      else
      if (num_cities == -1)
      {
	      fprintf (stderr, "%s: error: incompatible data file\n", argv[0]);
	      exit (EXIT_FAILURE);
      }
      break;
    case 'h':
      help ();
      exit (EXIT_SUCCESS);
      break;
    case '?':
      fprintf (stderr, "%s: error: invalid option\n", argv[0]);
      return 1;
    default:
      fprintf (stderr, "usage: tsp [-h] [-n <ITER>] -m <MODE> -f <FILE>\n");
      abort ();
    }

  for (i = optind; i < argc; i++)
  {
    fprintf (stderr, "%s: error: too many or too few arguments\n", argv[0]);
    exit (EXIT_FAILURE);
  }

  if (num_iter + 1 < num_iter)
  {
    fprintf (stderr, "%s: error: number of simulations must be less than %Lf \n", argv[0], LDBL_MAX);
    exit (EXIT_FAILURE);
  }

  // Check if obrigatory argumets were given
  if (nflag == 0 || mflag == 0 || fflag == 0)
  {
    fprintf (stderr, "%s: error: too few parameters\n", argv[0]);
    fprintf (stderr, "usage: tsp [-h] [-n <ITER>] -m <MODE> -f <FILE>\n");
    exit (EXIT_FAILURE);
  }

  // Create a new seed
  srand (time (NULL));

  // Create distance matrix
  distance_matrix (&coord, &distance, num_cities);

  // Simulates n round trips
  if (!mode == 0)
    printf ("POSSIBLE PATHS:\n");
  for (i = 0; i < num_iter; i++)
  {
    create_path (num_cities, &path);
    len = measure_path (&distance, num_cities, &path);
    if (!mode == 0)
      print_path (&distance, &path, num_cities, len, mode);

    if (len < min_len)
    {
      min_len = len;
      array_copy (&path, &min_path, num_cities);
    }
    free (path);
  }
  if (!mode == 0)
    printf ("\n");

  // Print report 
  print_repo (coord, distance, min_path, num_cities, min_len, num_iter, mode);

  free (min_path);
  free (coord);
  free (distance);

  return 0;
}
