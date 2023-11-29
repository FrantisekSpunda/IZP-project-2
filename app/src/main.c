/**
 * @file main.c
 * @author Frantisek Spunda
 * @date 2023-29-11
 * @brief Project 2 for subject IZP in BC1
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define FILE_LINE_LENGTH 101

typedef enum
{
  LEFT = 1,
  RIGHT = 2,
  TOP_BOTTOM = 4,
  BORDER_SIZE = 3,
} border_t;

typedef enum
{
  LEFT_HAND,
  RIGHT_HAND
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

// Functions for commands that can be called with flag --<command> ...
bool cmd_help(char **argv);
bool cmd_test(char **argv);
bool cmd_rpath(char **argv);
bool cmd_lpath(char **argv);

bool map_test(map_t *map);
bool map_load(map_t *map, char *filename);
bool isborder(map_t *map, int r, int c, border_t border);
bool find_hand_rule(map_t *map, int r, int c, border_t border, hand_rule_t leftright);
cell_t map_get_cell(map_t *map, int r, int c);
border_t start_border(map_t *map, int r, int c, hand_rule_t leftright);
border_t next_border(int r, int c, border_t border, int next, hand_rule_t leftright);
void map_free();
bool cell_type(int r, int c);

int main(int argc, char **argv)
{
  command_t commands[] = {
      {"--help", 2, cmd_help},
      {"--test", 3, cmd_test},
      {"--rpath", 5, cmd_rpath},
      {"--lpath", 5, cmd_lpath},
  };

  if (argc == 1)
  {
    cmd_help(argv);
    return 0;
  }

  // Call function by passed argument
  for (int i = 0; i < (int)(sizeof(commands) / sizeof(command_t)); i++)
  {
    if (strcmp(argv[1], commands[i].name) == 0)
    {

      int res = argc - commands[i].argc;
      if (res)
      {
        printf("Too %s arguments.\n", res > 0 ? "many" : "few");
        return false;
      }
      return commands[i].function(argv);
    }
  }

  printf("Unknown command \"%s\".\n", argv[1]);
  return false;
}

bool cmd_help(char **argv)
{
  if (argv)
    printf(" You can use these commands:\n"
           "\t\033[0;32m--help\033[0m\t\tShows this message\n\n"
           "\t\033[0;32m--test\033[0m\t\tTest map format for maze\n"
           "\t\t\tArguments: --test <file with map>\n\n"
           "\t\033[0;32m--rpath\033[0m\t\tFind path in maze with right hand rule\n"
           "\t\t\tArguments: --rpath <row> <column> <file with map>\n\n"
           "\t\033[0;32m--lpath\033[0m\t\tFind path in maze with left hand rule\n"
           "\t\t\tArguments: --lpath <row> <column> <file with map>\n");

  return 0;
}

bool cmd_test(char **argv)
{
  map_t map;
  if (map_load(&map, argv[2]))
    printf("Error while loading map\n");
  else
  {
    printf("%s\n", map_test(&map) ? "Invalid" : "Valid");
    map_free(&map);
  }

  return false;
}

bool cmd_rpath(char **argv)
{
  map_t map;
  if (map_load(&map, argv[4]))
  {
    printf("Error while loading map\n");
    return false;
  }
  if (map_test(&map))
  {
    printf("Invalid map\n");
    map_free(&map);
    return false;
  }

  int first_r = atoi(argv[2]) - 1;
  int first_c = atoi(argv[3]) - 1;
  border_t border = start_border(&map, first_r, first_c, RIGHT_HAND);
  if (border)
    find_hand_rule(&map, first_r, first_c, border, RIGHT_HAND);
  map_free(&map);

  return false;
}

bool cmd_lpath(char **argv)
{
  map_t map;
  if (map_load(&map, argv[4]))
  {
    printf("Error while loading map\n");
    return false;
  }
  if (map_test(&map))
  {
    printf("Invalid map\n");
    map_free(&map);
    return false;
  }

  int first_r = atoi(argv[2]) - 1;
  int first_c = atoi(argv[3]) - 1;
  border_t border = start_border(&map, first_r, first_c, LEFT_HAND);
  if (border)
    find_hand_rule(&map, first_r, first_c, border, LEFT_HAND);
  map_free(&map);

  return false;
}

/**
 * @brief Returns first border we enter the maze.
 *
 * @param map pointer of map
 * @param r row
 * @param c column
 * @param leftright use left/right hand rule
 * @return border_t or 0 in error status
 */
border_t start_border(map_t *map, int r, int c, hand_rule_t leftright)
{
  if ((r == map->rows - 1 && !cell_type(r, c)) && !isborder(map, r, c, TOP_BOTTOM))
  {
    if (!((c == 0 && !leftright && !isborder(map, r, c, LEFT)) || (c + 1 == map->cols && leftright && !isborder(map, r, c, RIGHT))))
      return TOP_BOTTOM;
  }
  if (r == 0 && cell_type(r, c) && !isborder(map, r, c, TOP_BOTTOM))
  {
    if (!((c == 0 && leftright && !isborder(map, r, c, LEFT)) || (c + 1 == map->cols && !leftright && !isborder(map, r, c, RIGHT))))
      return TOP_BOTTOM;
  }
  if (c == 0 && !isborder(map, r, c, LEFT))
    return LEFT;
  if (c == map->cols - 1 && !isborder(map, r, c, RIGHT))
    return RIGHT;

  printf("Cannot enter to the maze from this position.\n");
  return 0;
};

/**
 * @brief Find way in maze with left/right hand rule
 *
 * @param map pointer of map
 * @param r row
 * @param c column
 * @param border border we enter to cell
 * @param leftright use left/right hand rule
 * @return `true` or `false` if we found way
 */
bool find_hand_rule(map_t *map, int r, int c, border_t border, hand_rule_t leftright)
{
  if (r >= map->rows || r < 0 || c >= map->cols || c < 0)
    return true;

  for (int i = 1; i <= BORDER_SIZE; i++)
  {
    border_t new_border = next_border(r, c, border, i, leftright);

    if (!isborder(map, r, c, new_border))
    {
      printf("%i,%i\n", r + 1, c + 1);
      return find_hand_rule(
          map,
          new_border != TOP_BOTTOM ? r : (cell_type(r, c) ? r - 1 : r + 1),    // new row
          new_border == TOP_BOTTOM ? c : (new_border == LEFT ? c - 1 : c + 1), // new column
          new_border == TOP_BOTTOM ? new_border : new_border ^ 0b011,          // new border
          leftright);
    }
  }

  return false;
}

/**
 * @brief Get type of cell. Cell can be triangle of type 🔺or🔻
 *
 * @param r row
 * @param c column
 * @return `true` - even OR `false` - odd
 */
bool cell_type(int r, int c)
{
  return !((r + c) & 1);
}

border_t next_border(int r, int c, border_t border, int next, hand_rule_t leftright)
{
  int new_border = leftright ^ cell_type(r, c) ? ((border << BORDER_SIZE) >> next) : border << next;

  return new_border > TOP_BOTTOM ? new_border >> BORDER_SIZE : new_border;
}

/**
 * @brief Checks if there is border of given type.
 * @return `true` if there is border, `false` if not
 */
bool isborder(map_t *map, int r, int c, border_t border)
{
  return !!(border & map_get_cell(map, r, c));
}

/**
 * @brief Test map, if cells next to each other are valid.
 *
 * @param map
 * @return `true` if error or `false` if success
 */
bool map_test(map_t *map)
{
  bool error = false;
  for (int r = 0; !error && r < map->rows; r++)
  {
    for (int c = 0; !error && c < map->cols - cell_type(r, c); c++)
    {
      if (c + 1 < map->cols)
        if (((map_get_cell(map, r, c) >> 1) ^ map_get_cell(map, r, c + 1)) & 0b001)
          error = true;

      if (!cell_type(r, c) && r + 1 < map->rows)
        if ((map_get_cell(map, r, c) ^ map_get_cell(map, r + 1, c)) & 0b100)
          error = true;
    }
  }

  return error;
}

/**
 * @brief Load map from file.
 *
 * @param map
 * @param filename
 * @return `true` if error, `false` if success
 */
bool map_load(map_t *map, char *filename)
{
  bool error = false;
  FILE *file = fopen(filename, "r");

  if (file == NULL)
    error = true;

  char line[FILE_LINE_LENGTH];
  int line_index = -1;

  while (!error && fgets(line, FILE_LINE_LENGTH, file) != NULL)
  {
    // Get size of maze from first line. For example 6 7 (6 rows, 7 columns)
    if (line_index == -1)
    {
      if (sscanf(line, "%d %d", &map->rows, &map->cols) != 2)
        error = true;

      map->cells = malloc(map->rows * map->cols * sizeof(cell_t));
      if (map->cells == NULL)
        error = true;
    }
    else
    {
      int found = 0;
      for (int c = 0; !error && line[c]; c++)
      {
        if (line[c] >= '0' && line[c] <= '7')
        {
          map->cells[line_index * map->cols + found] = line[c] - '0';
          found++;
        }
        else if (line[c] != ' ' && line[c] != '\n' && line[c] != '\t' && line[c] != '\r' && line[c] != '\0')
          error = true;
      }

      if (found != map->cols)
        error = true;
    }

    line_index++;
  }

  if (!error && line_index != map->rows)
    error = true;

  if (error)
    map_free(map);
  else
    fclose(file);

  return error;
}

/**
 * @brief Free memory allocated for map cells
 *
 * @param map pointer of map
 */
void map_free(map_t *map)
{
  free(map->cells);
}

/**
 * @brief Get cell value in map
 *
 * @param map pointer of map
 * @param r row
 * @param c column
 * @return cell_t
 */
cell_t map_get_cell(map_t *map, int r, int c)
{
  return map->cells[r * map->cols + c];
}