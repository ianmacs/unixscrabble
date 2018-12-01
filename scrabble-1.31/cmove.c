/*
 *
 * cmove.c -- computer move algorithm
 *
 */

#include "scrab.h"
#include "globals.h"

int b1, b2, ntiles;
int tilev[7];
struct plrlet newlet[7];

void computer_move( plr )
  int plr;
{
	int retcode;

	clear_prompt();
	printw( "\n  Searching board...\n" );
	refresh();
	init_cmove( plr );
	if( board[8][8] < 'A' ) {
		retcode = first_move( plr );
	} else {
		retcode = cpu_move( plr );
	}
	if( retcode == -1 ) {
		move( 21, 0 );
		printw( "  CPU couldn't move\n" );
		press_return();
		game_done++;
	} else game_done = 0;
}

void init_cmove( plr )
  int plr;
{
	int i;

	b1 = -1;
	b2 = -1;
	for( i = 0; i < 7; i++ ) {
		if( plr_tiles[plr][i] == CH_BL ) {
			if( b1 < 0 ) b1 = i;
			else b2 = i;
		}
	}
	ntiles = 0;
	for( i = 0; i < 7; i++ ) {
		if( plr_tiles[plr][i] != 0 ) {
			tilev[i] = plr_tiles[plr][i] - 'A';
			if( plr_tiles[plr][i] == CH_BL ) tilev[i] = 26;
			ntiles++;
		} else tilev[i] = 27;
	}
	chead = NULL;
}

int first_move( plr )
  int plr;
{
	long ind, eind;
	int used[7];
	char tword[16], dummy[8], ch;
	int i, j, le;
	int xinc, yinc;
	struct plrlet *currlet;

	for( ind = wlen[2], eind = wlen[8]; ind < eind; ind++ ) {
		strcpy( tword, &(words[wptr[ind]]) );
		le = strlen( tword );
		for( i = 0; i < 7; i++ ) used[i] = 0;
		for( i = 0; i < le; i++ ) {
			ch = tword[i];
			for( j = 0; j < 7; j++ ) {
				if( ch == plr_tiles[plr][j] && used[j] == 0 ) {
					used[j] = 1;
					break;
				}
			}
			if( j == 7 ) {
				if( b1 < 0 ) break;
				if( used[b1] == 1 ) {
					if( b2 < 0 ) break;
					else used[b2] = 1;
				} else used[b1] = 1;
			}
		}
		if( i != le ) continue;
		for( i = 0; i < le; i++ ) dummy[i] = '*';
		dummy[le] = '\0';
		for( xinc = 1, yinc = 0; xinc >= 0; xinc--, yinc++ ) {
			for( i = 9 - le; i < 9; i++ ) {
				for( j = 0; j < le; j++ ) {
					currlet = &(newlet[j]);
					currlet->x = 8;
					currlet->y = 8;
					if( yinc == 0 ) currlet->x = i + j;
					else currlet->y = i + j;
				}
				eval_score( tword, dummy, le, le, 1, yinc );
			}
		}
	}
	if( chead == NULL ) return( -1 );
	pick_word( plr );
	return( 1 );
}

void pick_word( plr )
  int plr;
{
	int i, j, n, w, dir;
	int sd4, mean, pn, bar;
	compmove *moveptr, *tptr, *uptr;
	scoreent *schead, *rptr, *sptr;

	if( plr_skills[plr] == SKILL_EXPERT ) moveptr = chead;
	else {
/*		move( 18, 0 );
		printw( "Score distributions: " ); */
		tptr = chead;
		i = -1;
		j = 0;
		n = 0;
		schead = NULL;
		for( ;; ) {
			if( tptr != NULL && tptr->score == i ) j++;
			else {
				if( i != -1 ) {
/*					printw( "%d-%d,", i, j ); */
					sptr = (scoreent *) malloc( SC_SIZE );
					sptr->next = NULL;
					sptr->score = i;
					sptr->num = j;
					if( schead == NULL ) schead = sptr;
					else rptr->next = sptr;
					rptr = sptr;
				}
				if( tptr != NULL ) {
					i = tptr->score;
					j = 1;
					n++;
				}
			}
			if( tptr == NULL ) break;
			tptr = tptr->next;
		}
		sd4 = ( n * 40 ) / 200;
		mean = ( n * mean_pcts[SKILL_LEVELS - plr_skills[plr]] + 50 ) / 100;
		pn = get_random() % 2;
		bar = get_random() % normal_sum;
		for( i = 0; i < 40; i++ ) {
			if( bar - normal_dist[i] > 0 ) bar -= normal_dist[i];
			else break;
		}
		if( pn == 0 ) mean += ( i * sd4 ) / 40;
		else mean -= ( i * sd4 ) / 40;
		j = sd4 / 40;
		if( j > 1 )
			while( ( bar - normal_dist[i] / j ) > 0 ) {
				bar -= normal_dist[i] / j;
				if( pn == 0 ) mean++;
				else mean--;
			}
		if( mean < 0 ) mean = 0;
		else if( mean > n - 1 ) mean = n - 1;

/*		move( 21, 0 );
		printw( "  %d choices; %d mean, %d dir, %d bar.", n, mean, pn, bar );
		printw( "  Final choice: %d.", mean );
		press_return(); */

		rptr = schead;
		while( mean-- > 0 ) rptr = rptr->next;
		i = rptr->score;
		j = get_random() % rptr->num;
		moveptr = chead;
		while( moveptr->score != i ) moveptr = moveptr->next;
		while( j-- > 0 ) moveptr = moveptr->next;
		rptr = schead;
		while( rptr != NULL ) {
			sptr = rptr->next;
			free( (scoreent *) rptr );
			rptr = sptr;
		}
	}

	for( i = 0; i < moveptr->placed; i++ )
		plr_tiles[plr][moveptr->newlet[i].tilepos] = 0;
	dir = moveptr->dir;
	w = find_new_words( moveptr->newlet, moveptr->placed, 1 - dir, dir );
	display_words( plr, moveptr->newlet, w, moveptr->placed );
	tptr = chead;
	while( tptr != NULL ) {
		uptr = tptr->next;
		free( (compmove *) tptr );
		tptr = uptr;
	}
}

int cpu_move( plr )
  int plr;
{
	int sx, sy, dir, xinc, yinc;
	int x, y;
	int blank, tile, oldbl;

	for( sy = 1; sy < 16; sy++ ) {
		for( sx = 1; sx < 16; sx++ ) {
			for( dir = 0; dir < 2; dir++ ) {
				xinc = 1 - dir;
				yinc = dir;
				x = sx;
				y = sy;
				blank = 0;
				tile = 0;
				oldbl = 0;
				if( x - xinc > 0 && y - yinc > 0 )
					if( board[y - yinc][x - xinc] >= 'A' ) continue;
				while( x < 16 && y < 16 && blank < ntiles ) {
					if( board[y][x] < 'A' ) blank++;
					if( board[y][x] >= 'A' ||
						( x - xinc > 0 && y - yinc > 0 &&
						  board[y - yinc][x - xinc] >= 'A' ) ||
						( x + xinc < 16 && y + yinc < 16 &&
						  board[y + yinc][x + xinc] >= 'A' ) ||
						( x - yinc > 0 && y - xinc > 0 &&
						  board[y - xinc][x - yinc] >= 'A' ) ||
						( x + yinc < 16 && y + xinc < 16 &&
						  board[y + xinc][x + yinc] >= 'A' ) )
						tile++;
					if( blank > oldbl && tile > 0 ) {
						oldbl = blank;
#ifdef DBLSIZE_BOARD
						move( BOARD_UY + sy - 1, BOARD_LX + sx * 2 - 2 );
#else
						move( BOARD_UY + sy - 1, BOARD_LX + sx - 1 );
#endif
						refresh();
						try_tiles( sx, sy, blank, dir );
					}
					x += xinc; y += yinc;
				}
			}
		}
	}
	if( chead == NULL ) return( -1 );
	pick_word( plr );
	return( 1 );
}

void try_tiles( sx, sy, placed, dir )
  int sx, sy, placed, dir;
{
	int xinc, yinc, x, y;
	int i, w, c;
	int let[26], tlet[26], bt, tbt;
	int used[7], tileind[7], index[7], stack[7], blankloc[7];
	int level, len;
	long f, ef;
	char tword[16], ttword[16], dword[16];

/*	clear_rect( 20, 0, 21, 79 );
	printw( "  try_tiles() called with (%2d,%2d), %d tiles, dir %d",
	  sy, sx, placed, dir ); */

	xinc = 1 - dir;
	yinc = dir;
	x = sx;
	y = sy;
	for( i = 0; i < placed; i++ ) {
		while( board[y][x] >= 'A' ) {
			x += xinc;
			y += yinc;
		}
		newlet[i].x = x;
		newlet[i].y = y;
		newlet[i].letter = CH_BL;
		newlet[i].blankchar = '*';
		x += xinc;
		y += yinc;
	}
	w = find_new_words( newlet, placed, xinc, yinc );
	for( i = 0; i < w - 1; i++ ) {
		stack[i] = 0;
		blankloc[i] = new_words[i].blankloc;
		if( dir == 0 )
			index[i] = new_words[i].startx - new_words[w - 1].startx;
		else
			index[i] = new_words[i].starty - new_words[w - 1].starty;
	}
	for( i = 0; i < 26; i++ ) let[i] = -1;
	bt = 0;
	for( i = 0; i < 7; i++ ) {
		used[i] = 0;
		if( tilev[i] < 26 ) {
			let[tilev[i]]++;
			if( let[tilev[i]] == 0 ) let[tilev[i]]++;
		} else if( tilev[i] == 26 ) bt++;
	}
	level = 0;
	strcpy( tword, new_words[w - 1].letters );
	strcpy( ttword, new_words[w - 1].letters );
	len = strlen( tword );
	do {
		if( level != w - 1 ) {
			if( level < w - 1 ) {
				c = stack[level];
				if( let[c] > 0 || bt > 0 ) {
					new_words[level].letters[blankloc[level]] = c + 'A';
					f = bin_search( new_words[level].letters );
				} else f = 0;
			} else {
				level = w - 2;
				if( level < 0 ) break;
				c = stack[level];
				if( let[c] >= 0 ) let[c]++;
				else bt++;
				used[tileind[level]] = 0;
				f = 0;
			}
			if( f == 0 ) {
				for( ;; ) {
					stack[level]++;
					if( stack[level] == 26 ) {
						level--;
						if( level >= 0 ) {
							c = stack[level];
							if( let[c] >= 0 ) let[c]++;
							else bt++;
							used[tileind[level]] = 0;
							continue;
						}
					}
					break;
				}
			} else {
				tword[index[level]] = c + 'A';
				if( let[c] > 0 ) let[c]--;
				else {
					bt--;
					c = 26;
				}
				for( i = 0; i < 7; i++ ) {
					if( tilev[i] == c && used[i] == 0 ) {
						used[i] = 1;
						tileind[level] = i;
						break;
					}
				}
				level++;
				if( level < w - 1 ) stack[level] = 0;
			}
		} else {

/*			clear_rect( 20, 0, 21, 79 );
			printw( "  (%2d,%2d)  Tileind[]:", sy, sx );
			for( i = 0; i < w - 1; i++ )
				printw( " %d", tileind[i] );
			printw( "  Stack[]:" );
			for( i = 0; i < w - 1; i++ )
				printw( " %2d", stack[i] );
			printw( "  Used[]:" );
			for( i = 0; i < 7; i++ )
				printw( " %d", used[i] );
			printw( "  Words: %d\n", w );
			for( i = 0; i < w - 1; i++ )
				printw( "  %s", new_words[i].letters );
			printw( "  %s", tword );
			press_return(); */

			for( f = wlen[len], ef = wlen[len + 1]; f < ef; f++ ) {
				strcpy( dword, &(words[wptr[f]]) );
				for( i = 0; i < len; i++ )
					if( tword[i] >= 'A' && tword[i] != dword[i] ) break;
				if( i < len ) continue;
				for( i = 0; i < 26; i++ ) tlet[i] = let[i];
				tbt = bt;
				for( i = 0; i < len; i++ ) {
					if( tword[i] >= 'A' ) continue;
					c = dword[i] - 'A';
					if( tlet[c] <= 0 && tbt == 0 ) break;
					if( tlet[c] > 0 ) tlet[c]--;
					else tbt--;
				}
				if( i < len ) continue;
				eval_score( dword, ttword, len, placed, w, dir );
			}
			level = w;
		}
	} while( level >= 0 );

}

void eval_score( dword, tword, len, placed, w, dir )
  char *dword, *tword;
  int len, placed, w, dir;
{
	int xinc, yinc;
	int i, j, k;
	int tused[7];
	char c;
	int score;
	compmove *newptr, *ptr1, *ptr2;

	xinc = 1 - dir;
	yinc = dir;
	for( i = 0; i < 7; i++ ) tused[i] = 0;
	for( i = 0, k = 0; i < len; i++ ) {
		if( tword[i] >= 'A' ) continue;
		c = dword[i];
		for( j = 0; j < 7; j++ ) {
			if( c == tilev[j] + 'A' && tused[j] == 0 ) {
				tused[j] = 1;
				break;
			}
		}
		if( j == 7 ) {
			if( b1 < 0 ) break;
			if( tused[b1] == 1 ) {
				if( b2 < 0 ) break;
				else {
					j = b2;
					tused[b2] = 1;
				}
			} else {
				j = b1;
				tused[b1] = 1;
			}
		}
		if( j == 7 ) {
			move( 20, 0 );
			printw( "*** Fatal error in eval_score()" );
			exit_window();
			exit( 1 );
		}
		if( j != b1 && j != b2 ) newlet[k].letter = c;
		else {
			newlet[k].letter = CH_BL;
			newlet[k].blankchar = c;
		}
		newlet[k++].tilepos = j;
	}
	i = find_new_words( newlet, placed, xinc, yinc );
	score = 0;
	for( i = 0; i < w; i++ ) score += new_words[i].score;
	newptr = (compmove *) malloc( CM_SIZE );
	if( newptr == NULL ) {
		move( 20, 0 );
		printw( "*** Fatal error: no memory left in eval_score()" );
		exit_window();
		exit( 1 );
	}
	newptr->next = NULL;
	newptr->score = score;
	newptr->placed = placed;
	newptr->dir = dir;
	for( i = 0; i < placed; i++ ) {
		newptr->newlet[i].x = newlet[i].x;
		newptr->newlet[i].y = newlet[i].y;
		newptr->newlet[i].letter = newlet[i].letter;
		newptr->newlet[i].blankchar = newlet[i].blankchar;
		newptr->newlet[i].tilepos = newlet[i].tilepos;
	}
	if( chead == NULL ) chead = newptr;
	else if( score > chead->score ) {
		newptr->next = chead;
		chead = newptr;
	} else {
		ptr1 = chead;
		while( ptr1 != NULL && score <= ptr1->score ) {
			ptr2 = ptr1;
			ptr1 = ptr1->next;
		}
		newptr->next = ptr1;
		ptr2->next = newptr;
	}

/*	clear_rect( 21, 0, 21, 79 );
	printw( "  (%2d,%2d) %d Words:", newlet[0].y, newlet[0].x, dir );
	for( i = 0; i < w - 1; i++ )
		printw( "  %s %d", new_words[i].letters, new_words[i].score );
	printw( "  %s %d; Score: %3d", new_words[w - 1].letters,
		new_words[w - 1].score, score );
	refresh(); */
}
