/**
 * @file main.c
 * @author Frantisek Spunda
 * @date 2023-19-11
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

#define FILE_LINE_LENGTH 100
#define CMD_C 5

typedef enum border_enum
{
  NONE,
  LEFT,
  RIGTH,
  BORDER_SIZE,
  TOP_BOTTOM
} border_t;

typedef enum hand_rule_enum
{
  LEFT_HAND,
  RIGTH_HAND
} hand_rule_t;

typedef unsigned char cell_t;

typedef struct
{
  int rows;
  int cols;
  cell_t *cells;
} map_t;

typedef struct
{
  char *name;
  int argc;
  bool (*function)(char **argv);
} command_t;

bool cmd_help(char **argv);
bool cmd_test(char **argv);
bool cmd_rpath(char **argv);
bool cmd_lpath(char **argv);
// bool cmd_shortest(char **argv);

bool map_test(map_t *map);
bool map_load(map_t *map, char *filename);
bool isborder(map_t *map, int r, int c, border_t border);
bool find_hand_rule(map_t *map, int r, int c, border_t border, hand_rule_t leftright);
cell_t map_get_cell(map_t *map, int row, int col);
border_t start_border(map_t *map, int r, int c, hand_rule_t leftright);
border_t next_border(int r, int c, border_t border, int next, hand_rule_t leftright);

bool arg_test(int argc, int need);
bool cell_type(int r, int c);

int main(int argc, char **argv)
{

  command_t commands[CMD_C] = {
      {"--help", 2, cmd_help},
      {"--test", 3, cmd_test},
      {"--rpath", 5, cmd_rpath},
      {"--lpath", 5, cmd_lpath},
      // {"--shortest", 5, cmd_shortest},
  };

  // Call function by argument
  for (int i = 0; i < CMD_C; i++)
  {
    if (strcmp(argv[1], commands[i].name) == 0 && arg_test(argc, commands[i].argc))
      return commands[i].function(argv);
    else if (i == CMD_C)
      fprintf(stderr, "\033[0;31mInvalid arguments.\033[0m\n");
  }

  return 0;
}

bool cmd_help(char **argv)
{
  if (argv)
    printf(" You can use these commands:\n"
           "\t\033[0;32m--help\033[0m\t\tShows this help\n"
           "\t\033[0;32m--test\033[0m\t\tTest labirint format\n"
           "\t\033[0;32m--rpath\033[0m\n"
           "\t\033[0;32m--lpath\033[0m\n"
           "\t\033[0;32m--shortest\033[0m");

  return 0;
}

bool cmd_test(char **argv)
{
  map_t map;
  bool error = map_load(&map, argv[2]);
  if (!error)
    error = map_test(&map);

  printf("%s\n", error ? "Invalid" : "Valid");
  return error;
}

bool cmd_rpath(char **argv)
{
  map_t map;
  if (map_load(&map, argv[4]))
  {
    fprintf(stderr, "Error while loading map");
    return true;
  }

  int first_r = atoi(argv[2]) - 1;
  int first_c = atoi(argv[3]) - 1;
  border_t border = start_border(&map, first_r, first_c, 1);
  find_hand_rule(&map, first_r, first_c, border, 1);

  return 0;
}

bool cmd_lpath(char **argv)
{
  map_t map;
  if (map_load(&map, argv[4]))
  {
    fprintf(stderr, "Error while loading map");
    return true;
  }

  int first_r = atoi(argv[2]) - 1;
  int first_c = atoi(argv[3]) - 1;
  border_t border = start_border(&map, first_r, first_c, 0);
  find_hand_rule(&map, first_r, first_c, border, 0);

  return 0;
}

border_t start_border(map_t *map, int r, int c, hand_rule_t leftright)
{
  if ((r == map->rows - 1 && (map->rows - 1) % 2) && !isborder(map, r, c, TOP_BOTTOM))
  {
    if (!((c == 0 && leftright == LEFT_HAND && !isborder(map, r, c, LEFT)) || (c + 1 == map->cols && leftright == RIGTH_HAND && !isborder(map, r, c, RIGTH))))
      return TOP_BOTTOM;
  }
  if (r == 0 && !isborder(map, r, c, TOP_BOTTOM))
  {
    if (!((c == 0 && leftright == RIGTH_HAND && !isborder(map, r, c, LEFT)) || (c + 1 == map->cols && leftright == LEFT_HAND && !isborder(map, r, c, RIGTH))))
      return TOP_BOTTOM;
  }
  if (c == 0 && !isborder(map, r, c, LEFT))
    return LEFT;
  if (c == map->cols - 1 && !isborder(map, r, c, RIGTH))
    return RIGTH;

  return NONE;
};

bool find_hand_rule(map_t *map, int r, int c, border_t border, hand_rule_t leftright)
{
  if (border != NONE)
    for (int i = 1; i <= BORDER_SIZE; i++)
    {
      border_t new_border = next_border(r, c, border, i, leftright);

      if (!isborder(map, r, c, new_border))
      {
        printf("%i,%i\n", r + 1, c + 1);

        int new_c = new_border == TOP_BOTTOM ? c : (new_border == LEFT ? c - 1 : c + 1);
        int new_r = new_border != TOP_BOTTOM ? r : (cell_type(r, c) ? r - 1 : r + 1);
        if (new_border != TOP_BOTTOM)
          new_border ^= 0b011;

        if (new_r < map->rows && new_r > -1 && new_c < map->cols && new_c > -1)
          find_hand_rule(map, new_r, new_c, new_border, leftright);
        return true;
      }
    }

  return false;
}

bool cell_type(int r, int c)
{
  return (r + c) % 2 == 0;
}

border_t next_border(int r, int c, border_t border, int next, hand_rule_t leftright)
{
  int new_border = leftright ^ cell_type(r, c) ? ((border << BORDER_SIZE) >> next) : border << next;

  if (new_border > TOP_BOTTOM)
    new_border >>= BORDER_SIZE;

  return new_border;
}

/**
 * @brief Checks if there is border of given type.
 */
bool isborder(map_t *map, int r, int c, border_t border)
{
  if (border == LEFT || border == RIGTH || border == TOP_BOTTOM)
    return border & map_get_cell(map, r, c);

  fprintf(stderr, "Invalid border type.\n");
  return true;
}

bool map_test(map_t *map)
{
  bool error = false;
  for (int row = 0; row < map->rows; row++)
  {
    for (int col = 0; col < ((row + col) % 2 ? map->cols : map->cols - 1); col++)
    {

      if (col + 1 < map->cols)
      {
        cell_t curr = map_get_cell(map, row, col);
        cell_t on_rigth = map_get_cell(map, row, col + 1);
        if (((curr >> 1) ^ on_rigth) & 0b001)
        {
          error = true;
          break;
        }
      }

      if ((row + col) % 2 && row + 1 < map->rows)
      {
        cell_t curr = map_get_cell(map, row, col);
        cell_t on_bottom = map_get_cell(map, row + 1, col);

        if ((curr ^ on_bottom) & 0b100)
        {
          error = true;
          break;
        }
      }
    }

    if (error)
      break;
  }

  return error;
}

/**
 * @brief Load map from file
 *
 * @param map
 * @param filename
 * @return true if error, false if success
 */
bool map_load(map_t *map, char *filename)
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
            return true;
        }
        if (found != map->cols)
          return true;
      }

      line_index++;
    }

    fclose(file);

    if (line_index - 1 != map->rows)
      return true;

    return false;
  }

  return true;
}

/**
 * @brief Get value of cell in map.
 */
cell_t map_get_cell(map_t *map, int row, int col)
{
  return map->cells[(row * map->cols) + col];
}

/**
 * @brief Test if there are right number of arguments.
 */
bool arg_test(int argc, int need)
{
  int res = argc - need;
  if (res != 0)
  {
    printf("\033[0;31mToo %s arguments.\033[0m\n", res > 0 ? "many" : "few");
    return false;
  }
  return true;
}