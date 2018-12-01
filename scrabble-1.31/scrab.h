/*
 *
 * scrab.h -- constants and structures
 *
 */

#include <curses.h>
#include <string.h>
#include <malloc.h>
#ifdef  BSD
#include <sys/time.h>
#else   /* USG */
#define index   strchr
#define srandom srand48
#define random  lrand48
long srand48();
long lrand48();
#endif  /* USG */

#define SKILL_LEVELS	5
#define SKILL_EXPERT	( SKILL_LEVELS + 1 )

#define CH_EM			'.'
#define CH_DL			'+'
#define CH_TL			'#'
#define CH_DW			'0'
#define CH_TW			'@'
#define CH_BL			'*'
#define CH_IT			'_'

#define INFO_Y			2
#define INFO_X			0
#define TILE_RACKS_Y	2
#define TILE_RACKS_X	59
#define TILES_LEFT_Y	0
#define TILES_LEFT_X	57

#ifdef  DBLSIZE_BOARD
#define BOARD_UY		2
#define BOARD_LY		16
#define BOARD_LX		25
#define BOARD_RX		53
#else
#define BOARD_UY		2
#define BOARD_LY		16
#define BOARD_LX		32
#define BOARD_RX		46
#endif

#ifdef  COMPRESSED_DICT
#define DICT_FILE_RCMD	"compress -d < scrabbledict"
#define DICT_FILE_WCMD  "compress > scrabbledict"
#endif
#ifndef DICT_FILE
#define DICT_FILE		"scrabbledict"
#endif

struct another_word {
	int startx;
	int starty;
	int dir;
	int length;
	int blankloc;
	int score;
	char letters[16];
};

struct plrlet {
	int x;
	int y;
	char letter;
	char blankchar;
	char oldlet;
	int tilepos;
};

typedef struct cm {
	struct cm *next;
	struct plrlet newlet[7];
	int placed;
	int dir;
	int score;
} compmove;

#define CM_SIZE		( sizeof( compmove ) )

typedef struct sc {
	struct sc *next;
	int score;
	int num;
} scoreent;

#ifndef SC_SIZE
#define SC_SIZE		( sizeof( scoreent ) )
#endif

struct let_distrib {
	int tiles;
	int points;
};
