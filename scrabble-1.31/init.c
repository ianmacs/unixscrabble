/*
 *
 * init.c -- static declarations
 *
 */

#include "scrab.h"

char board[16][16];

char words[900000];
long wptr[90000];
long wlen[17];
long dict_size;
int dict_changed;

char plr_tiles[4][7];
int plr_scores[4];
int plr_skills[4];
int mean_pcts[SKILL_LEVELS] = { 10, 20, 35, 50, 70 };
int tiles_left[27];
int players;

char your[4][7];
char you[4][5];
int human_player;

int b1x, b1y, b2x, b2y;

compmove *chead;

int game_done;
int game_abort;

struct another_word new_words[8];

struct let_distrib letters[27] = {
   {9,  1},  {2,  3},  {2,  3},  {4,  2}, {12,  1},  {2,  4},
   {3,  2},  {2,  4},  {9,  1},  {1,  8},  {1,  5},  {4,  1},
   {2,  3},  {6,  1},  {8,  1},  {2,  3},  {1, 10},  {6,  1},
   {4,  1},  {6,  1},  {4,  1},  {2,  4},  {2,  4},  {1,  8},
   {2,  4},  {1, 10},  {2,  0} };

int normal_dist[40] = { 1000, 995, 980, 956, 923, 882, 835, 783,
	726, 667, 607, 546, 487, 430, 375, 325, 278, 236, 198, 164,
	135, 110, 89, 71, 56, 44, 34, 26, 20, 15, 11, 8, 6, 4, 3, 2,
	2, 1, 1, 0 };
int normal_sum;
