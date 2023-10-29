/**
 * @file main.c
 * @author Frantisek Spunda
 * @date 2023-29-10
 * @brief Second project for subject IZP in BC1 
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include "main.h"

#define FILE_LINE_LENGTH 100
#define CMD_C 5

typedef unsigned char cell;

typedef struct
{
  int rows;
  int cols;
  cell *cells;
} Map;

typedef struct
{
  char *name;
  int argc;
  bool (*function)(int argc, char **argv);
} Command;

bool arg_test(int argc, int need);
bool map_load(Map *map, char *filename);

bool cmd_help(int argc, char **argv);
bool cmd_test(int argc, char **argv);
bool cmd_rpath(int argc, char **argv);
bool cmd_lpath(int argc, char **argv);
bool cmd_shortest(int argc, char **argv);
cell map_get_cell(Map *map, int row, int col);
bool isborder(Map *map, int r, int c, cell border);
int find_way(Map *map, int r, int c, cell entry, int leftright);

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
    if (strcmp(argv[1], commands[i].name) == 0 && arg_test(argc, commands[i].argc))
      return commands[i].function(argc, argv);
    else if (i == CMD_C)
      fprintf(stderr, "\033[0;31mInvalid arguments.\033[0m\n");
  }

  return 0;
}

bool cmd_help(int argc, char **argv)
{
  printf(" You can use these commands:\n"
         "\t\033[0;32m--help\033[0m\t\tShows this help\n"
         "\t\033[0;32m--test\033[0m\t\tTest labirint format\n"
         "\t\033[0;32m--rpath\033[0m\n"
         "\t\033[0;32m--lpath\033[0m\n"
         "\t\033[0;32m--shortest\033[0m");

  return 0;
}

bool cmd_test(int argc, char **argv)
{
  Map map;
  bool load_error = map_load(&map, argv[2]);
  printf("%s\n", load_error ? "Invalid" : "Valid");
  return load_error;
}

bool cmd_rpath(int argc, char **argv)
{
  Map map;
  bool load_error = map_load(&map, argv[4]);
  if (load_error)
  {
    fprintf(stderr, "Error while loading map");
    return true;
  }

  // int start_row = atoi(argv[2]) - 1;
  // int start_col = atoi(argv[3]) - 1;

  find_way(&map, 5, 0, 1, 1);

  return 0;
}

int start_border(Map *map, int r, int c, int leftright)
{

  return 1;
};

int find_way(Map *map, int r, int c, cell entry, int leftright)
{
  for (int i = 1; i < 4; i++)
  {
    int cell_type = (r + c) % 2 == 0; // TRUE border on bottom / FALSE border on top)

    cell border = leftright ^ cell_type ? ((entry << 3) >> i) : entry << i;

    if (border > 4)
      border >>= 3;

    // entry = 1                entry = 2               entry = 4
    // i = 1, border = 4 (2)    i = 1, border = 1 (4)   i = 1, border = 2 (1)
    // i = 2, border = 2 (4)    i = 2, border = 4 (1)   i = 2, border = 1 (2)
    // i = 3, border = 1 (1)    i = 3, border = 2 (2)   i = 3, border = 4 (4)

    bool is_border = isborder(map, r, c, border);

    if (!is_border)
    {
      printf("%i, %i\n", r + 1, c + 1);

      // border = 1, new_entry = 2
      // border = 2, new_entry = 1
      // border = 4, new_entry = 4
      cell new_entry = border == 4 ? border : 3 ^ border;

      int new_c = border == 4 ? c : (border == 1 ? c - 1 : c + 1);
      int new_r = border != 4 ? r : (cell_type ? r - 1 : r + 1);

      if (new_r < map->rows && new_r > -1 && new_c < map->cols && new_c > -1)
        find_way(map, new_r, new_c, new_entry, leftright);
      else
        printf("No way!\n");

      break;
    }
  }
}

bool cmd_lpath(int argc, char **argv)
{
}

bool cmd_shortest(int argc, char **argv)
{
}

/**
 * @brief Checks if there is border of given type.
 *
 * @param map pointer to map
 * @param r row of cell
 * @param c column of cell
 * @param border type of border - `L` - left, `R` - right, `T` - top, `B` - bottom (`T` and `B` has same effect)
 * @return boolean - true if border, false if not
 */
bool isborder(Map *map, int r, int c, cell border)
{
  if (border == 1 || border == 2 || border == 4)
    return border & map_get_cell(map, r, c);
  else
  {
    fprintf(stderr, "Invalid border type.\n");
    return true;
  }
}

/**
 * @brief Load map from file
 *
 * @param map
 * @param filename
 * @return 1 if error, 0 if success
 */
bool map_load(Map *map, char *filename)
{
  FILE *file = fopen(filename, "r");

  if (file != NULL)
  {
    char line[FILE_LINE_LENGTH];
    int line_index = 0;

    while (fgets(line, FILE_LINE_LENGTH, file) != NULL)
    {
      if (line_index == 0)
      {
        char *token = strtok(line, " ");
        map->rows = atoi(token);
        token = strtok(NULL, " ");
        map->cols = atoi(token);
        map->cells = malloc(map->rows * map->cols);
      }
      else
      {
        int found = 0;
        for (int c = 0; line[c]; c++)
        {
          if (line[c] >= '0' && line[c] <= '7')
          {
            map->cells[(line_index - 1) * map->cols + found] = line[c] - '0';
            found++;
          }
          else if (line[c] != ' ' && line[c] != '\n' && line[c] != '\t' && line[c] != '\r' && line[c] != '\0')
          {
            fprintf(stderr, "Invalid character: %c\n", line[c]);
            return true;
          }
        }
      }

      line_index++;
    }

    fclose(file);
  }
  else
  {
    fprintf(stderr, "\033[0;31mInvalid file.\033[0m\n");
    return true;
  }

  return false;
}

/**
 * @brief Get cell value.
 *
 * @param map
 * @param row
 * @param col
 * @return cell
 */
cell map_get_cell(Map *map, int row, int col)
{
  return map->cells[(row * map->cols) + col];
}

/**
 * @brief Test if there are right number of arguments.
 *
 * @param argc
 * @param need
 * @return int
 */
bool arg_test(int argc, int need)
{
  int res = argc - need;
  if (res != 0)
  {
    printf("\033[0;31mToo %s arguments.\033[0m\n", res > 0 ? "many" : "few");
    return false;
  }
  else
    return true;
}