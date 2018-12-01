/*
 *
 * misc.c -- miscellaneous routines
 *
 */

#include "scrab.h"
#include "globals.h"

char spaces[81] =
"                                        \
                                        ";

void read_words() {
	FILE *fp;
	char inp[16], tinp[16];
	int i, tlen, rlen;
	long pt, wr;

#ifdef COMPRESSED_DICT
	fp = popen( DICT_FILE_RCMD, "r" );
#else
	fp = fopen( DICT_FILE, "r" );
#endif
	if( fp == NULL ) {
		move( 4, 0 );
		printw( "Error: could not find dictionary file '%s'\n", DICT_FILE );
		exit_window();
		exit( 1 );
	}
	printw( "Reading dictionary...\n" );
	refresh();
	pt = 0;
	wr = 0;
	tlen = 2;
	wlen[tlen] = wr;
	tinp[0] = '\0';
	while( fscanf( fp, "%s", inp ) != EOF ) {
		rlen = strlen( inp );
		if( rlen < 2 || rlen > 15 ) {
			move( 4, 0 );
			printw( "Error in dictionary file at word %s: words must be between 2 and 15\n", inp );
			printw( "characters long.\n" );
			exit_window();
			exit( 1 );
		}
		if( rlen < strlen( tinp ) ||
			( strcmp( tinp, inp ) > 0 && strlen( tinp ) == rlen ) ) {
			move( 4, 0 );
			printw( "Error in dictionary file at the words %s and %s.\n",
					  tinp, inp );
			printw( "The dictionary must be sorted from shortest words to longest, and\n" );
			printw( "alphabetically within each list of words of the same length." );
			exit_window();
			exit( 1 );
		}
		for( i = 0; i < rlen; i++ ) {
			if( inp[i] >= 'a' && inp[i] <= 'z' ) inp[i] = inp[i] - 'a' + 'A';
			else if( inp[i] < 'A' || inp[i] > 'Z' ) {
				move( 4, 0 );
				printw( "Error in dictionary file at word %s: words must contain alphabetic characters\n", inp );
				printw( "only.\n" );
				exit_window();
				exit( 1 );
			}
		}
		if( rlen > tlen ) {
			for( i = tlen + 1; i <= rlen; i++ ) wlen[i] = wr;
			tlen = rlen;
		}
		strcpy( &words[pt], inp );
		wptr[wr++] = pt;
		pt += rlen + 1;
		strcpy( tinp, inp );
	}
	words[pt] = '\0';
	wptr[wr] = pt;
	dict_size = pt;
	for( i = tlen + 1; i < 17; i++ ) wlen[i] = wr;
#ifdef COMPRESSED_DICT
	pclose( fp );
#else
	fclose( fp );
#endif
}

void clear_rect( y1, x1, y2, x2 )
  int y1, x1, y2, x2;
{
	int i;

	for( i = y1; i <= y2; i++ ) {
		move( i, x1 );
		printw( "%s", &spaces[79 - ( x2 - x1 )] );
	}
	move( y1, x1 );
}

void clear_prompt() {
	clear_rect( 19, 0, 23, 78 );
	move( 19, 0 );
}

void clear_turn() {
	clear_rect( 18, 0, 18, 79 );
	move( 18, 0 );
}

void press_return() {
	char c;

	move( 23, 0 );
	printw( "Press <RETURN> -> " );
	refresh();
	do {
		c = getch();
		if( c == 'q' || c == 'Q' ) game_abort = 1;
	} while( c != '\n' );
}

void print_board() {
	int i, j;

	clear_rect( BOARD_UY, BOARD_LX, BOARD_LY, BOARD_RX );
	for( i = 1; i < 16; i++ ) {
		for( j = 1; j < 16; j++ ) {
#ifdef DBLSIZE_BOARD
			move( BOARD_UY + i - 1, BOARD_LX + j * 2 - 2 );
#else
			move( BOARD_UY + i - 1, BOARD_LX + j - 1 );
#endif
			if( board[i][j] >= 'A' ) standout();
			if( ( i == b1y && j == b1x ) || ( i == b2y && j == b2x ) )
				addch( board[i][j] - 'A' + 'a' );
			else if( board[i][j] == CH_DW ) addch( 'o' );
			else addch( board[i][j] );
			if( board[i][j] >= 'A' ) standend();
		}
	}
	refresh();
}

void print_tiles() {
	int i, j;

	clear_rect( TILE_RACKS_Y, TILE_RACKS_X,
				  TILE_RACKS_Y + 7, TILE_RACKS_X + 19 );
	for( i = 0; i < players; i++ ) {
		move( TILE_RACKS_Y + i * 3, TILE_RACKS_X - 4 );
		if( plr_skills[i] != 0 ) printw( "(%d)", plr_skills[i] );
		move( TILE_RACKS_Y + i * 3, TILE_RACKS_X + 1 );
		if( i == human_player )
			printw( " You:" );
		else
			printw( "%s:", you[i] );
		for( j = 0; j < 7; j++ ) {
			addch( ' ' );
			if( plr_tiles[i][j] > 0 ) {
#ifndef PRINT_CPU_TILES
				if( i == human_player || game_done == players )
#endif
					addch( plr_tiles[i][j] );
#ifndef PRINT_CPU_TILES
				else addch( '-' );
#endif
			} else addch( ' ' );
		}
		move( TILE_RACKS_Y + i * 3 + 1, TILE_RACKS_X );
		printw( "Score: %3d", plr_scores[i] );
	}
	move( TILE_RACKS_Y + 12, TILE_RACKS_X - 2 );
	i = 0;
	if( b1x != 0 ) i++;
	if( b2x != 0 ) i++;
	printw( "Blank tiles placed: %d", i );
	if( b1x != 0 ) {
		move( TILE_RACKS_Y + 13, TILE_RACKS_X - 1 );
		printw( "Row %2d, Column %2d (%c)", b1y, b1x, board[b1y][b1x] );
	}
	if( b2x != 0 ) {
		move( TILE_RACKS_Y + 14, TILE_RACKS_X - 1 );
		printw( "Row %2d, Column %2d (%c)", b2y, b2x, board[b2y][b2x] );
	}
	move( TILE_RACKS_Y + human_player * 3, TILE_RACKS_X - 1 );
	refresh();
}

char draw_tile() {
	int i, tleft;
	int tile;

	tleft = 0;
	for( i = 0; i < 27; i++ )
		tleft += tiles_left[i];

	if( tleft == 0 ) return( 0 );
	tile = get_random() % tleft;
	for( i = 0;; i++ ) {
		if( tile - tiles_left[i] >= 0 )
			tile -= tiles_left[i];
		else
			break;
	}
	tiles_left[i]--;
	print_tiles_left();
	return( i == 26 ? CH_BL : i + 'A' );
}

void print_tiles_left() {
    int i, tleft;

    tleft = 0;
    for( i = 0; i < 27; i++ )
		tleft += tiles_left[i];

	move( TILES_LEFT_Y, TILES_LEFT_X );
	printw( "Tiles left to draw: %3d", tleft );
}

void print_passed_turns() {
	if( game_done == 0 ) clear_rect( 1, 50, 1, 79 );
	else {
		move( 1, 63 );
		printw( "Passed turns:  %d", game_done );
	}
	refresh();
}

void print_whos_move( plr )
  int plr;
{
    clear_turn();
	printw( "*** %s turn to play  ", your[plr] );
}

void print_version() {
	clear();
	printw( "*** SCRABBLE 1.31 ***\n\n" );
}

void screen_redraw() {
    print_version();
    print_info();
    print_board();
    print_tiles_left();
    print_passed_turns();
    print_tiles();
	print_whos_move( human_player );
}

char get_key() {
	char c;

	c = getch();
	if( c >= 'a' && c <= 'z' ) c = c - 'a' + 'A';
/*	if( c == '' )
		if( getch() == '[' ) {
			c = getch();
			if( c == 'A' ) c = 'K';
			else if( c == 'B' ) c = 'J';
			else if( c == 'C' ) c = 'L';
			else if( c == 'D' ) c = 'H';
			else c = '\0';
		} else c = '\0'; */
	return( c );
}

void seed_random() {
	long now, time();

	now = time( 0 );
	srandom( now % 65536 - 32768 );
}

long get_random() {
	return( (long) random() );
}
