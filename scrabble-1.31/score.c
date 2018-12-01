/*
 *
 * score.c -- find score for a turn
 *
 */

#include "scrab.h"
#include "globals.h"


int find_new_words( newlet, placed, xinc, yinc )
  struct plrlet *newlet;
  int placed, xinc, yinc;
{
	struct plrlet *currlet;
	int i, w, j;
	int dx, dy;
	int x, y;
	int score, mult, tsc, cptr, blloc;
	char c, ch;

	w = 0;
	dx = 1 - xinc;
	dy = 1 - yinc;
	for( i = 0; i <= placed; i++ ) {
		if( i < placed ) {
			currlet = &(newlet[i]);
			x = currlet->x;
			y = currlet->y;
		} else {
			x = newlet[0].x;
			y = newlet[0].y;
			dx = xinc;
			dy = yinc;
		}
		if( ( y - dy > 0 && x - dx > 0 && board[y - dy][x - dx] >= 'A' )
		  || ( y + dy < 16 && x + dx < 16 && board[y + dy][x + dx] >= 'A' )
		  || i == placed ) {
			while( board[y - dy][x - dx] >= 'A' && x - dx > 0 && y - dy > 0 ) {
				x -= dx;
				y -= dy;
			}
			new_words[w].startx = x;
			new_words[w].starty = y;
			new_words[w].dir = dy;
			score = 0;
			mult = 1;
			cptr = 0;
			blloc = -1;
			for( ;; ) {
				c = board[y][x];
				if( c >= 'A' ) {
					if( ( x != b1x || y != b1y ) && ( x != b2x || y != b2y ) )
						score += letters[c - 'A'].points;
					new_words[w].letters[cptr++] = c;
				} else {
					for( j = 0; j < placed; j++ )
						if( x == newlet[j].x && y == newlet[j].y )
							break;
					if( j == placed ) break;
					if( blloc == -1 ) blloc = cptr;
					else blloc = -2;
					ch = newlet[j].letter;
					if( ch == CH_BL ) {
						tsc = 0;
						new_words[w].letters[cptr++] = newlet[j].blankchar;
					} else {
						tsc = letters[ch - 'A'].points;
						new_words[w].letters[cptr++] = ch;
					}
					if( c == CH_DL ) tsc *= 2;
					else if( c == CH_TL ) tsc *= 3;
					else if( c == CH_DW ) mult *= 2;
					else if( c == CH_TW ) mult *= 3;
					score += tsc;
				}
				x += dx;
				y += dy;
				if( x > 15 || y > 15 ) break;
			}
			score *= mult;
			if( placed == 7 && i == placed ) score += 50;
			new_words[w].length = cptr;
			new_words[w].letters[cptr] = '\0';
			new_words[w].blankloc = blloc;
			new_words[w].score = score;
			if( i != placed || cptr != 1 ) w++;
		}
	}
	return( w );
}

long bin_search( word )
  char *word;
{
	long f, l, m;
	int found;

	f = wlen[strlen( word )];
	l = wlen[strlen( word ) + 1] - 1;
	if( f > l ) return( 0 );
	found = 0;
	do {
		m = ( f + l ) / 2;
		if( strcmp( word, &(words[wptr[m]]) ) > 0 )
			f = m + 1;
		else if( strcmp( word, &(words[wptr[m]]) ) < 0 )
			l = m - 1;
		else {
			found = 1;
			f = m;
			l = m;
		}
	} while( f < l );
	if( strcmp( word, &(words[wptr[f]]) ) == 0 
		|| strcmp( word, &(words[wptr[l]]) ) == 0 ) found = 1;
	return( found );
}

void display_words( plr, newlet, nwords, placed )
  int plr, nwords, placed;
  struct plrlet *newlet;
{
	int i, j, xloc, score;
	char c, tile, *word;
	int x, y;
	int illword, ind[8];
	int shift;

	tile = 0;
	clear_turn();
	clear_prompt();
	move( 18, 0 );
	printw( "  %s made the following words:\n", you[plr] );
	refresh();
	score = 0;
	illword = 0;
	printw( "    " );
	xloc = 4;
	for( i = 0; i < nwords; i++ ) {
		word = &(new_words[i].letters[0]);
		if( strlen( word ) + xloc + 2 > 79 ) {
			printw( "\n    " );
			xloc = 4;
		}
		printw( "%s  ", word );
		score += new_words[i].score;
		xloc += strlen( word ) + 2;
		if( bin_search( word ) == 0 ) {
			ind[illword++] = i;
		}
	}
	move( 21, 0 );
	if( illword != 0 ) {
		if( plr != human_player ) {
			printw( "*** Fatal error in display_words()\n" );
			exit_window();
			exit( 1 );
		}
		for( i = 0; i < illword; i++ ) {
			clear_rect( 21, 0, 22, 79 );
			j = ind[i];
			printw( "  %s is not in the computer's dictionary.\n",
			  &(new_words[j].letters[0]) );
			printw( "  Is it a valid Scrabble word (y/n)? " );
			refresh();
			do {
				c = get_key();
			} while( c != 'Y' && c != 'N' );
			if( c == 'N' ) break;
			clear_rect( 21, 0, 22, 79 );
			printw( "  Inserting %s into the dictionary...",
			  &(new_words[j].letters[0]) );
			refresh();
			add_dict( &(new_words[j].letters[0]) );
			dict_changed = 1;
		}
		clear_rect( 21, 0, 22, 79 );
		if( i == illword ) {
			illword = 0;
		} else {
			printw( "  For placing an invalid word, you score zero this round.\n" );
			printw( "  You remove your tiles from the board.\n" );
			for( j = 0; j < placed; j++ )
				plr_tiles[plr][newlet[j].tilepos] = newlet[j].letter;
		}
	}
	if( illword == 0 ) {
		if( placed == 7 )
			printw( "  %s used all seven tiles!", you[plr] );
		printw( "  %s score this round is %3d.\n", your[plr], score );
		plr_scores[plr] += score;
		for( i = 0; i < placed; i++ ) {
			x = newlet[i].x;
			y = newlet[i].y;
			tile = newlet[i].letter;
			if( tile == CH_BL ) {
				tile = newlet[i].blankchar;
				if( b1x == 0 ) {
					b1x = x;
					b1y = y;
				} else {
					b2x = x;
					b2y = y;
				}
			}
			board[y][x] = tile;
		}
		for( i = 0, shift = 0; i < 7; i++ ) {
			if( plr_tiles[plr][i] == 0 ) shift++;
			else if( shift > 0 ) {
				plr_tiles[plr][i - shift] = plr_tiles[plr][i];
				plr_tiles[plr][i] = 0;
			}
		}
		for( i = 7 - shift; i < 7; i++ ) {
			tile = draw_tile();
			if( tile == 0 ) break;
			plr_tiles[plr][i] = tile;
		}
		move( 22, 0 );
		if( tile == 0 && i == 7 - shift) {
			if( plr != human_player )
				printw( "  There are no tiles left for %s to draw.\n", you[plr] );
			else
				printw( "  There are no tiles left for you to draw.\n" );
		} else {
			if( tile == 0 ) {
				printw( "  %s can only draw ", you[plr] );
				shift = i - ( 7 - shift );
			} else {
				printw( "  %s dra%s ", you[plr],
				  ( ( plr == human_player ) ? "w" : "ws" ) );
			}
			printw( "%d til%s.\n", shift, ( shift == 1 ? "e" : "es" ) );
		}
	}
	print_board();
	print_tiles();
	press_return();
}

void add_dict( word )
  char *word;
{
	int i, l;
	long p1, p2;

	l = strlen( word );
	for( p1 = wlen[l];
		  p1 < wlen[l + 1] && strcmp( word, &(words[wptr[p1]]) ) > 0;
		  p1++ );
	for( p2 = dict_size; p2 >= wptr[p1]; p2-- ) words[p2 + l + 1] = words[p2];
	for( p2 = wlen[16] + 1; p2 > p1; p2-- ) wptr[p2] = wptr[p2 - 1] + l + 1;
	for( i = l + 1; i < 17; i++ ) wlen[i]++;
	strcpy( &(words[wptr[p1]]), word );
	dict_size += l + 1;
/*	printw( "\n  " );
	for( p2 = ( p1 - 2 >= 0 ? p1 - 2 : 0 ); p2 < p1 + 3 && p2 <= wlen[16]; p2++ )
		printw( "%s  ", &(words[wptr[p2]]) );
	printw( "%s  %s", &(words[wptr[wlen[8]]]), &(words[wptr[wlen[16] - 1]]) );
	press_return(); */
}
