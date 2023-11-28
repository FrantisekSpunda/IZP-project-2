/**
 * @file main.c
 * @author Frantisek Spunda
 * @date 2023-28-11
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

typedef enum border_enum
{
  NONE,
  LEFT,
  RIGHT,
  BORDER_SIZE,
  TOP_BOTTOM
} border_t;

typedef enum hand_rule_enum
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

bool cmd_help(char **argv);
bool cmd_test(char **argv);
bool cmd_rpath(char **argv);
bool cmd_lpath(char **argv);
// bool cmd_shortest(char **argv);

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
      // {"--shortest", 5, cmd_shortest},
  };

  // Call function by passed argument
  int cmd_c = sizeof(commands) / sizeof(command_t);
  for (int i = 0; i < cmd_c; i++)
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

    if (i + 1 == cmd_c)
      fprintf(stderr, "Invalid arguments.\n");
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
           "\t\033[0;32m--shortest\033[0m\n");

  return 0;
}

bool cmd_test(char **argv)
{
  map_t map;
  bool error = map_load(&map, argv[2]) || map_test(&map);
  map_free(&map);
  printf("%s\n", error ? "Invalid" : "Valid");
  return error;
}

bool cmd_rpath(char **argv)
{
  map_t map;
  if (map_load(&map, argv[4]))
  {
    fprintf(stderr, "Error while loading map\n");
    map_free(&map);
    return true;
  }
  if (map_test(&map))
  {
    fprintf(stderr, "Invalid map\n");
    map_free(&map);
    return true;
  }

  int first_r = atoi(argv[2]) - 1;
  int first_c = atoi(argv[3]) - 1;
  border_t border = start_border(&map, first_r, first_c, 1);
  find_hand_rule(&map, first_r, first_c, border, 1);
  map_free(&map);

  return 0;
}

bool cmd_lpath(char **argv)
{
  map_t map;
  if (map_load(&map, argv[4]))
  {
    fprintf(stderr, "Error while loading map\n");
    map_free(&map);
    return true;
  }
  if (map_test(&map))
  {
    fprintf(stderr, "Invalid map\n");
    map_free(&map);
    return true;
  }

  int first_r = atoi(argv[2]) - 1;
  int first_c = atoi(argv[3]) - 1;
  border_t border = start_border(&map, first_r, first_c, 0);
  find_hand_rule(&map, first_r, first_c, border, 0);
  map_free(&map);

  return 0;
}

/**
 * @brief Returns first border we enter the maze.
 *
 * @param map pointer of map
 * @param r row
 * @param c column
 * @param leftright use left/right hand rule
 * @return border_t
 */
border_t start_border(map_t *map, int r, int c, hand_rule_t leftright)
{
  if ((r == map->rows - 1 && !cell_type(r, c)) && !isborder(map, r, c, TOP_BOTTOM)) // TODO shit
  {
    if (!((c == 0 && leftright == LEFT_HAND && !isborder(map, r, c, LEFT)) || (c + 1 == map->cols && leftright == RIGHT_HAND && !isborder(map, r, c, RIGHT))))
      return TOP_BOTTOM;
  }
  if (r == 0 && cell_type(r, c) && !isborder(map, r, c, TOP_BOTTOM))
  {
    if (!((c == 0 && leftright == RIGHT_HAND && !isborder(map, r, c, LEFT)) || (c + 1 == map->cols && leftright == LEFT_HAND && !isborder(map, r, c, RIGHT))))
      return TOP_BOTTOM;
  }
  if (c == 0 && !isborder(map, r, c, LEFT))
    return LEFT;
  if (c == map->cols - 1 && !isborder(map, r, c, RIGHT))
    return RIGHT;

  printf("It is not possible to enter the maze from passed cell\n");
  return NONE;
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

  if (border != NONE)
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
  for (int r = 0; r < map->rows; r++)
  {
    for (int c = 0; c < map->cols - cell_type(r, c); c++)
    {

      if (c + 1 < map->cols)
      {
        if (((map_get_cell(map, r, c) >> 1) ^ map_get_cell(map, r, c + 1)) & 0b001)
        {
          error = true;
          break;
        }
      }

      if (!cell_type(r, c) && r + 1 < map->rows)
      {
        if ((map_get_cell(map, r, c) ^ map_get_cell(map, r + 1, c)) & 0b100)
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
 * @brief Load map from file.
 *
 * @param map
 * @param filename
 * @return `true` if error, `false` if success
 */
bool map_load(map_t *map, char *filename)
{
  FILE *file = fopen(filename, "r");

  if (file == NULL)
    return true;

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
      if (map->cells == NULL)
        return true;
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
          map_free(map);
          return true;
        }
      }
      if (found != map->cols)
      {
        map_free(map);
        return true;
      }
    }

    line_index++;
  }

  fclose(file);

  if (line_index - 1 != map->rows)
  {
    map_free(map);
    return true;
  }

  return false;
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