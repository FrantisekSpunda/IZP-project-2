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

void cmd_help();
void cmd_test();
void cmd_rpath();
void cmd_lpath();
void cmd_shortest();

int main(int argc, char **argv)
{

  if (argc > 2)
  {
    printf("\033[0;31mToo few arguments.\033[0m\n");
  }

  if (strcmp(argv[1], "--help") == 0)
    cmd_help();
  else if (strcmp(argv[1], "--test") == 0)
    cmd_test();
  else if (strcmp(argv[1], "--rpath") == 0)
    cmd_rpath();
  else if (strcmp(argv[1], "--lpath") == 0)
    cmd_lpath();
  else if (strcmp(argv[1], "--shortest") == 0)
    cmd_shortest();
  else
    printf("\033[0;31mInvalid argument.\033[0m\n");

  return 0;
}

void cmd_help()
{
  printf(" You can use these commands:\n"
         "\t\033[0;32m--help\033[0m\t\tShows this help\n"
         "\t\033[0;32m--test\033[0m\t\tTest labirint format\n"
         "\t\033[0;32m--rpath\033[0m\n"
         "\t\033[0;32m--lpath\033[0m\n"
         "\t\033[0;32m--shortest\033[0m");
}

/**
 * @brief Test format if file with map of labirint has right format
 *
 */
void cmd_test()
{
  if (0)
    printf("\n \033[0;32mValid\033[0m\n");
  else
    printf("\n \033[0;31mInvalid\033[0m\n");
}

/**
 * @brief
 *
 */
void cmd_rpath() {}

/**
 * @brief
 *
 */
void cmd_lpath() {}

/**
 * @brief
 *
 */
void cmd_shortest() {}
