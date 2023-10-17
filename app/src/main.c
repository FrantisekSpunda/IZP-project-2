/**
 * @file main.c
 * @author Frantisek Spunda
 * @date 2023-17-10
 * @brief Second project for subject IZP in BC1 
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"

#define FILE_LINE_LENGTH 100
#define CMD_C 5

typedef struct
{
  int rows;
  int cols;
  unsigned char *cells;
} Map;

typedef struct
{
  char **name;
  int argc;
  void (*function)(int argc, char **argv);
} Command;

int arg_test(int argc, int need);
void map_load(Map *map, char *filename);

void cmd_help(int argc, char **argv);
void cmd_test(int argc, char **argv);
void cmd_rpath(int argc, char **argv);
void cmd_lpath(int argc, char **argv);
void cmd_shortest(int argc, char **argv);

int main(int argc, char **argv)
{

  Command commands[CMD_C] = {
      {"--help", 2, cmd_help},
      {"--test", 3, cmd_test},
      {"--rpath", 5, cmd_rpath},
      {"--lpath", 5, cmd_rpath},
      {"--shortest", 5, cmd_shortest},
  };

  // Call function by argument
  for (int i = 0; i < CMD_C; i++)
  {
    if (strcmp(argv[1], commands[i].name) == 0 && arg_test(argc, commands[i].argc) == 0)
    {
      commands[i].function(argc, argv);
      break;
    }
    else if (i == CMD_C)
      printf("\033[0;31mInvalid argument.\033[0m\n");
  }

  return 0;
}

void cmd_help(int argc, char **argv)
{
  printf(" You can use these commands:\n"
         "\t\033[0;32m--help\033[0m\t\tShows this help\n"
         "\t\033[0;32m--test\033[0m\t\tTest labirint format\n"
         "\t\033[0;32m--rpath\033[0m\n"
         "\t\033[0;32m--lpath\033[0m\n"
         "\t\033[0;32m--shortest\033[0m");
}

void cmd_test(int argc, char **argv)
{
  // Map map;
  // map_load(argc, argv);

  if (0)
    printf("\n \033[0;32mValid\033[0m\n");
  else
    printf("\n \033[0;31mInvalid\033[0m\n");
}

void cmd_rpath(int argc, char **argv)
{
}

void cmd_lpath(int argc, char **argv)
{
}

void cmd_shortest(int argc, char **argv) {}

void map_load(Map *map, char *filename)
{
  FILE *file = fopen(filename, "r");
  char line[FILE_LINE_LENGTH];
  int i = 0;

  while (fgets(line, FILE_LINE_LENGTH, file) != NULL)
  {
    if (i == 0)
    {
      char *token = strtok(line, " ");
      map->rows = atoi(token);
      token = strtok(NULL, " ");
      map->cols = atoi(token);
      map->cells = malloc(map->rows * map->cols);
    }

    i++;
  }

  printf("Rows: %d\n", map->rows);
  printf("Cols: %d\n", map->cols);
  printf("Cells: %d\n", map->cells);
}

int arg_test(int argc, int need)
{
  if (argc == need)
    printf("\033[0;31mToo few arguments.\033[0m\n");
}