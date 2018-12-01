/*
 *
 * pmove.c -- player move routines
 *
 */

#include "scrab.h"
#include "globals.h"


void player_move() {
	char c;
	int xpos, ypos, done;

	xpos = 1;
	ypos = 1;
	for( ;; ) {
		clear_prompt();
		printw( "  Use the 'hjkl' keys to move the cursor, and control-L to refresh the screen.\n" );
		printw( "  Press <RETURN> on an empty square to start placing tiles on the board.\n" );
		printw( "  Press <SPACE> to rearrange your tiles.\n" );
		printw( "  Press '.' to use your turn to draw new tiles.\n" );
		refresh();
		done = 0;
		do {
#ifdef DBLSIZE_BOARD
			move( BOARD_UY + ypos - 1, BOARD_LX + xpos * 2 - 2 );
#else
			move( BOARD_UY + ypos - 1, BOARD_LX + xpos - 1 );
#endif
			refresh();
			c = get_key();
			switch( c ) {
			  case 'H':
				if( xpos > 1 ) xpos--;
				break;
			  case 'L':
				if( xpos < 15 ) xpos++;
				break;
			  case 'J':
				if( ypos < 15 ) ypos++;
				break;
			  case 'K':
				if( ypos > 1 ) ypos--;
				break;
			  case '\n':
				if( board[ypos][xpos] < 'A' ) done = place_tiles( xpos, ypos );
				break;
			  case ' ':
				done = rearrange_tiles();
				break;
			  case '.':
				done = draw_new_tiles();
				break;
			  case '':
				screen_redraw();
				done = -1;
				break;
			}
		} while( done == 0 );
		if( done == 1 ) break;
	}
	if( c == '.' ) game_done++;
	else game_done = 0;
}

int place_tiles( xpos, ypos )
  int xpos, ypos;
{
	char c;
	int xinc, yinc;
	int i, j, t1, t2, ct1, ct2, tile;
	struct plrlet newlet[7];
	int placed;
	int err, xt, yt;

	ct1 = 0;
	ct2 = 0;
	clear_prompt();
	printw( "  Which direction would you like to place your tiles:" );
	printw( " horizontally or\n  vertically (h/v)? " );
	refresh();
	c = get_key();
	if( c != 'H' && c != 'V' ) return( -1 );
	if( c == 'H' ) {
		xinc = 1;
		yinc = 0;
	} else {
		xinc = 0;
		yinc = 1;
	}
	show_instructions();
	placed = 0;
	for( ;; ) {
#ifdef DBLSIZE_BOARD
		move( BOARD_UY + ypos - 1, BOARD_LX + xpos * 2 - 2 );
#else
		move( BOARD_UY + ypos - 1, BOARD_LX + xpos - 1 );
#endif
		refresh();
		c = get_key();
		if( c == '-' || c == 127 || c == 8 ) {
			if( placed == 0 ) break;
			--placed;
			do {
				xpos -= xinc;
				ypos -= yinc;
			} while( board[ypos][xpos] >= 'A' );
#ifdef DBLSIZE_BOARD
			move( BOARD_UY + ypos - 1, BOARD_LX + xpos * 2 - 2 );
#else
			move( BOARD_UY + ypos - 1, BOARD_LX + xpos - 1 );
#endif
			addch( newlet[placed].oldlet );
			plr_tiles[human_player][newlet[placed].tilepos]
			  = newlet[placed].letter;
			print_tiles();
		} else if( c == 27 ) {
			while( placed-- > 0 ) {
				do {
					xpos -= xinc;
					ypos -= yinc;
				} while( board[ypos][xpos] >= 'A' );
#ifdef DBLSIZE_BOARD
				move( BOARD_UY + ypos - 1, BOARD_LX + xpos * 2 - 2 );
#else
				move( BOARD_UY + ypos - 1, BOARD_LX + xpos - 1 );
#endif
				addch( newlet[placed].oldlet );
				plr_tiles[human_player][newlet[placed].tilepos]
				  = newlet[placed].letter;
			}
			print_tiles();
			break;
		} else if( c == '\n' ) {
			if( placed == 0 ) break;
			clear_prompt();
			err = 0;
			if( board[8][8] == CH_DW ) {
				if( placed < 2 ) {
					printw( "  On the first move, you must place at least two tiles!\n" );
					err = 1;
				} else {
					for( i = 0; i < placed; i++ )
						if( newlet[i].x != 8 || newlet[i].y != 8 ) err++;
					if( err == placed )
						printw( "  On the first move, you must place a tile on the centre square!\n" ); 
					else err = 0;
				}
			} else {
				for( i = 0; i < placed; i++ ) {
					xt = newlet[i].x;
					yt = newlet[i].y;
					if( board[yt][xt - 1] >= 'A' || board[yt][xt + 1] >= 'A'
					  || board[yt - 1][xt] >= 'A' || board[yt + 1][xt] >= 'A' )
						break;
				}
				if( i == placed ) {
					printw( "  You must add to part of the existing puzzle!\n" );
					err = 1;
				}
			}
			if( err == 0 ) {
				clear_prompt();
				for( tile = 0, t1 = -1, t2 = -1, i = 0; i < placed; i++ )
					if( newlet[i].letter == CH_BL ) {
						tile++;
						if( t1 == -1 ) {
							t1 = i;
							ct1 = newlet[i].blankchar;
						} else if( t2 == -1 ) {
							t2 = i;
							ct2 = newlet[i].blankchar;
						}
					}
				for( i = 0, xt = 2; i < tile; i++ ) {
					if( i == 0 ) j = t1;
					else if( i == 1 ) j = t2;
					if( newlet[j].blankchar == CH_BL ) {
						clear_rect( 20, 0, 20, 79 );
						printw( "  What letter do you want the " );
						if( tile == 2 && i == 0 ) printw( "first " );
						else if( tile == 2 ) printw( "second " );
						printw( "blank tile to be? " );
						refresh();
						do {
							c = get_key();
						} while( c < 'A' || c > 'Z' );
						newlet[j].blankchar = c;
					}
					move( 22, xt );
					if( tile == 2 && i == 0 ) printw( "First b" );
					else if( tile == 2 ) printw( "Second b" );
					else printw( "B" );
					printw( "lank tile is a" );
					if( index( "AEFHILMNORSX", newlet[j].blankchar ) != 0 ) {
						printw( "n" );
						xt++;
					}
					printw( " %c.", newlet[j].blankchar );
					xt += 26;
				}
				clear_rect( 20, 0, 20, 79 );
				printw( "  Type <RETURN> to end your turn.  Otherwise, type 'n'. -> " );
				refresh();
				do {
					c = getch();
				} while( c != '\n' && c != 'N' && c != 'n' );
				if( c == '\n' ) break;
				if( t1 != -1 ) newlet[t1].blankchar = ct1;
				if( t2 != -1 ) newlet[t2].blankchar = ct2;
			} else press_return();
			show_instructions();
		} else {
			if( xpos > 15 || ypos > 15 ) continue;
			tile = -1;
			for( i = 0; i < 7; i++ ) {
				if( c == plr_tiles[human_player][i] ) tile = i;
				else if( plr_tiles[human_player][i] == CH_BL && tile == -1 )
					tile = i;
			}
			if( tile == -1 ) continue;
			if( plr_tiles[human_player][tile] == CH_BL )
				newlet[placed].blankchar = c;
			newlet[placed].letter = plr_tiles[human_player][tile];
			plr_tiles[human_player][tile] = 0;
			newlet[placed].x = xpos;
			newlet[placed].y = ypos;
			newlet[placed].oldlet = board[ypos][xpos];
			if( newlet[placed].oldlet == CH_DW ) newlet[placed].oldlet = 'o';
			newlet[placed].tilepos = tile;
			addch( newlet[placed].letter );
			do {
				xpos += xinc;
				ypos += yinc;
			} while( board[ypos][xpos] >= 'A' && xpos <= 15 && ypos <= 15 );
			print_tiles();
			placed++;
		}
	}
	if( placed <= 0 ) return( -1 );
	err = find_new_words( newlet, placed, xinc, yinc );
	display_words( human_player, newlet, err, placed );
	return( 1 );
}

void show_instructions() {
	clear_prompt();
	printw( "  Type the letter to be placed in each square, and '%c' for a blank tile.\n", CH_BL );
	printw( "  If you type a letter which you do not have, and you have a blank tile, then\n" );
	printw( "  it will be placed for you automatically.\n" );
	printw( "  Press '-' to backspace, <ESC> to cancel, and <RETURN> when done.\n" );
	refresh();
}

int rearrange_tiles() {
	char c;
	int i, j;

	clear_prompt();
	printw( "  Each letter you type will be moved to the end of your rack.\n" );
	printw( "  Type '%c' for a blank tile, and <RETURN> when you are done.\n",
		  CH_BL );
	refresh();
	print_tiles();
	do {
		c = get_key();
		for( i = 0; i < 7; i++ ) {
			if( c == plr_tiles[human_player][i] ) {
				for( j = i; j < 6; j++ )
					plr_tiles[human_player][j] = plr_tiles[human_player][j + 1];
				plr_tiles[human_player][6] = c;
				print_tiles();
				break;
			}
		}
	} while( c != '\n' );
	return( -1 );
}

void disp_instr() {
	clear_rect( 19, 0, 20, 79 );
	printw( "  Type the tiles that you wish to discard, using '%c' for a blank tile.\n", CH_BL );
	printw( "  Press '-' to backspace, <ESC> to cancel, and <RETURN> when done.\n" );
	refresh();
}

int draw_new_tiles() {
	char c, tile[7];
	int i, j, k, plr;
	int used[7];

	clear_prompt();
	disp_instr();
	plr = human_player;
	j = 0;
	for( i = 0; i < 7; i++ ) {
		used[i] = 0;
		tile[i] = plr_tiles[plr][i];
	}
	for( ;; ) {
		clear_rect( 22, 0, 22, 79 );
		printw( "  Tiles to discard:" );
		for( i = 0; i < 7; i++ )
			if( used[i] != 0 ) printw( " %c", tile[i] );
		print_tiles();
		c = get_key();
		if( c == '\n' ) {
			if( j != 0 ) {
				clear_rect( 19, 0, 20, 79 );
				printw( "  Type <RETURN> to confirm your selection.  Otherwise, type 'n'. -> " );
			} else {
				clear_rect( 19, 0, 20, 79 );
				printw( "  Type <RETURN> to pass your turn.  Otherwise, type 'n'. -> " );
			}
			refresh();
			do {
				c = getch();
			} while( c != '\n' && c != 'N' && c != 'n' );
			if( c == '\n' ) break;
			disp_instr();
		} else if( c == '-' || c == 127 || c == 8 ) {
			if( j > 0 ) {
				for( i = 0; i < 7; i++ )
					if( used[i] == j ) {
						used[i] = 0;
						plr_tiles[plr][i] = tile[i];
						j--;
						break;
					}
			} else {
				j = -1;
				break;
			}
		} else if( c == 27 ) {
			for( i = 0; i < 7; i++ )
				if( used[i] != 0 ) plr_tiles[plr][i] = tile[i];
			print_tiles();
			j = -1;
			break;
		} else {
			for( i = 0; i < 7; i++ ) {
				if( c == plr_tiles[plr][i] ) {
					j++;
					used[i] = j;
					plr_tiles[plr][i] = 0;
					break;
				}
			}
		}
	}
	if( j == -1 ) return( -1 );
	for( i = 0; i < 7; i++ )
		if( used[i] != 0 ) {
			k = tile[i] - 'A';
			if( tile[i] == CH_BL ) k = 26;
			tiles_left[k]++;
		}
	for( i = 0; i < 7; i++ )
		if( used[i] != 0 ) plr_tiles[plr][i] = draw_tile();
	print_tiles();
	return( 1 );
}
