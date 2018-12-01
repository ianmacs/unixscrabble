/*
 *
 * smain.c -- main program
 *
 */

#include "scrab.h"
#include "globals.h"


void set_up_window() {
	WINDOW *screen;

	screen = initscr();
	refresh();
	if( screen->_maxx < 80 || screen->_maxy < 24 ) {
		endwin();
		fprintf( stderr, "Need at least a 24x80 screen\n" );
		exit( 1 );
	}
	cbreak();
	noecho();
}

void exit_window() {
	char c;

	move( 23, 0 );
	printw( "Press any key to end? " );
	refresh();
	c = getch();
	endwin();
}

void init_board() {
	int i, j;

	for( i = 1; i < 16; i++ )
		for( j = 1; j < 16; j++ )
			board[i][j] = CH_EM;
	board[1][1] = CH_TW;
	board[8][1] = CH_TW;
	board[8][8] = CH_DW;
	for( i = 2; i < 6; i++ ) {
		board[i][i] = CH_DW;
	}
	board[2][6] = CH_TL;
	board[6][2] = CH_TL;
	board[6][6] = CH_TL;
	board[1][4] = CH_DL;
	board[4][1] = CH_DL;
	board[3][7] = CH_DL;
	board[7][3] = CH_DL;
	board[8][4] = CH_DL;
	board[7][7] = CH_DL;
	for( i = 1; i < 9; i++ )
		for( j = 1; j < 8; j++ ) {
			board[16-j][i] = board[i][j];
			board[16-i][16-j] = board[i][j];
			board[j][16-i] = board[i][j];
		}
}

void print_info() {
	move( INFO_Y    , INFO_X );
	printw( "Special symbols:" );
	move( INFO_Y + 1, INFO_X + 2 );
	printw( "%c: Double letter score", CH_DL );
	move( INFO_Y + 2, INFO_X + 2 );
	printw( "%c: Triple letter score", CH_TL );
	move( INFO_Y + 3, INFO_X + 2 );
	printw( "%c: Double word score", 'o' );
	move( INFO_Y + 4, INFO_X + 2 );
	printw( "%c: Triple word score", CH_TW );
	move( INFO_Y + 5, INFO_X + 2 );
	printw( "%c: Blank tile", CH_BL );
	move( INFO_Y + 7, INFO_X );
	printw( "Tile distribs & values:" );
	move( INFO_Y + 8, INFO_X + 2 );
	printw( "9A1  2H4  8O1  2V4" );
	move( INFO_Y + 9, INFO_X + 2 );
	printw( "2B3  9I1  2P3  2W4" );
	move( INFO_Y +10, INFO_X + 2 );
	printw( "2C3  1J8  1Q10 1X8" );
	move( INFO_Y +11, INFO_X + 2 );
	printw( "4D2  1K5  6R1  2Y4" );
	move( INFO_Y +12, INFO_X + 1 );
	printw( "12E1  4L1  4S1  1Z10" );
	move( INFO_Y +13, INFO_X + 2 );
	printw( "2F4  2M3  6T1  2%c0", CH_BL );
	move( INFO_Y +14, INFO_X + 2 );
	printw( "3G2  6N1  4U1" );
	refresh();
}

void init_tiles() {
	int i, j;

	for( i = 0; i < 27; i++ )
		tiles_left[i] = letters[i].tiles;
	for( i = 0; i < 4; i++ )
		for( j = 0; j < 7; j++ )
			plr_tiles[i][j] = 0;
	b1x = 0;
	b2x = 0;
	b1y = 0;
	b2y = 0;
	normal_sum = 0;
	for( i = 0; i < 40; i++	) normal_sum += normal_dist[i];
}

void get_num_players() {
	char c;
	int i, j;
	char ft[4], cs[7];

	clear_prompt();
	printw( "  Number of computer players (1-3)? " );
	refresh();
	for( ;; ) {
		c = getch();
		if( c >= '1' && c <= '3' ) break;
	}
	players = c - '0' + 1;
	for( ;; ) {
		init_tiles();
		for( i = 0; i < players; i++ ) ft[i] = draw_tile();
		for( i = 1; i < players; i++ )
			if( ft[0] == ft[i] ) break;
		if( i != players ) continue;
		move( 20, 0 );
		printw( "  All players draw a tile.  You: %c", ft[0] );
		for( i = 1; i < players; i++ ) {
			printw( "  Player %d: %c", i + 1, ft[i] );
			if( ft[i] == CH_BL ) ft[i] = 'A' - 1;
		}
		j = players;
		for( i = 1; i < players; i++ )
			if( ft[0] < ft[i] ) j--;
		if( j == 1 ) strcpy( cs, "first" );
		if( j == 2 ) strcpy( cs, "second" );
		if( j == 3 ) strcpy( cs, "third" );
		if( j == 4 ) strcpy( cs, "fourth" );
		printw( "\n  You will play %s.", cs );
		human_player = j - 1;
		print_tiles_left();
		press_return();
		clear_prompt();
		refresh();
		break;
	}

	init_tiles();
	j = '1';
	for( i = 0; i < 4; i++ ) {
		if( i == human_player ) {
			strcpy( &(your[i][0]), "Your" );
			strcpy( &(you[i][0]), "You" );
		} else {
			strcpy( &(your[i][0]), "CPU 's" );
			strcpy( &(you[i][0]), "CPU " );
			your[i][3] = j;
			you[i][3] = j++;
		}
		plr_skills[i] = 0;
	}

	for( i = 0; i < players; i++ ) {
		print_tiles();
		if( i == human_player ) continue;
		clear_prompt();
		printw( "  Select skill level 1-%d for %s, where 1 is beginner, %d is advanced, and\n", SKILL_EXPERT, you[i], SKILL_LEVELS );
		printw( "  %d is expert. -> ", SKILL_EXPERT );
		refresh();
		for( ;; ) {
			c = getch();
			if( c >= '1' && c <= '6' ) break;
		}
		plr_skills[i] = c - '0';
	}

	for( i = 0; i < players; i++ ) {
		plr_scores[i] = 0;
		for( j = 0; j < 7; j++ )
			plr_tiles[i][j] = draw_tile();
	}
	print_tiles();
}

void end_game() {
	int i, j;
	int adj[4], wbs, wb[4], was, wa[4];
	long p1;
	FILE *fp;
	char c;

	clear_turn();
	printw( "*** GAME OVER\n" );
	print_tiles();
	clear_prompt();
	printw( "  Here are the score adjustments: " );
	wbs = -1;
	for( i = 0; i < players; i++ )
		if( plr_scores[i] > wbs ) wbs = plr_scores[i];
	for( i = 0; i < players; i++ ) {
		if( plr_scores[i] == wbs ) wb[i] = 1;
		else wb[i] = 0;
	}
	for( i = 0; i < players; i++ ) {
		adj[i] = 1;
		for( j = 0; j < 7; j++ ) {
			c = plr_tiles[i][j];
			if( c != 0 ) {
				if( adj[i] == 1 ) adj[i] = 0;
				if( c != CH_BL ) adj[i] -= letters[c - 'A'].points;
			}
		}
	}
	for( j = 0; j < players; j++ )
		if( adj[j] == 1 ) break;
	if( j < players ) {
		adj[j] = 0;
		for( i = 0; i < players; i++ )
			if( i != j ) adj[j] -= adj[i];
	}
	for( i = 0; i < players; i++ ) {
		move( 19 + i, 36 );
		if( i == human_player ) addch( ' ' );
		printw( "%s: %d", you[i], adj[i] );
		plr_scores[i] += adj[i];
	}
	was = -1;
	for( i = 0; i < players; i++ )
		if( plr_scores[i] > was ) was = plr_scores[i];
	for( i = 0; i < players; i++ ) {
		if( plr_scores[i] == was ) wa[i] = 1;
		else wa[i] = 0;
	}
	press_return();
	print_tiles();
	clear_prompt();
	move( 20, 0 );
	was = 0;
	for( i = 0; i < players; i++ ) was += wa[i];
	wbs = 0;
	for( i = 0; i < players; i++ ) wbs += wb[i];
	if( was == 1 || wbs == 1 ) {
		if( was == 1 ) {
			for( i = 0; i < players; i++ )
				if( wa[i] == 1 ) break;
		} else {
			for( i = 0; i < players; i++ )
				if( wb[i] == 1 ) break;
		}
		printw( "  And the winner is:  %s!", you[i] );
	} else {
		printw( "  It was a tie between:  " );
		wbs = 1;
		for( i = 0; i < players; i++ )
			if( wa[i] == 1 ) {
				if( wbs < was - 1 ) printw( ", " );
				else {
					if( was > 2 ) printw( ", and " );
					else printw( " and " );
				}
				printw( "%s", you[i] );
				wbs++;
			}
		printw( "!" );
	}
	refresh();
	if( dict_changed == 1 ) {
		press_return();
		clear_prompt();
		printw( "  The computer's dictionary was modified this game.\n" );
		printw( "  Would you like to save the new dictionary (y/n)? " );
		refresh();
		do {
			c = get_key();
		} while( c != 'Y' && c != 'N' );
		if( c == 'Y' ) {
			printw( "\n\n  Writing new dictionary..." );
			refresh();
#ifdef COMPRESSED_DICT
			fp = popen( DICT_FILE_WCMD, "w" );
#else
			fp = fopen( DICT_FILE, "w+" );
#endif
			if( fp == NULL ) {
				printw( "error writing new dictionary!\n" );
				exit_window();
				exit( 1 );
			}
			for( p1 = 0; p1 < wlen[16]; p1++ )
				fprintf( fp, "%s\n", &(words[wptr[p1]]) );
#ifdef COMPRESSED_DICT
			pclose( fp );
#else
			fclose( fp );
#endif
		}
	}
}

int main() {
	int plr;
	int i;

	set_up_window();
	print_version();

	read_words();
	dict_changed = 0;

	print_version();

	init_board();
	seed_random();
	print_board();
	print_info();
	get_num_players();

	game_done = 0;
	game_abort = 0;
	do {
		for( plr = 0; plr < players; plr++ ) {
			print_whos_move( plr );
			if( plr == human_player ) {
				player_move();
			} else {
				computer_move( plr );
			}
			if( game_abort != 0 ) break;
			print_passed_turns();
			if( game_done == players ) break;
			for( i = 0; i < 7; i++ )
				if( plr_tiles[plr][i] != 0 ) break;
			if( i == 7 ) {
				game_done = players;
				break;
			}
		}
	} while( game_done < players && game_abort == 0 );

	if( game_abort == 0 ) end_game();
	exit_window();
	return( 0 );
}
