/*
 *
 * globals.h -- external variables
 *
 */

extern char board[16][16];

extern char words[900000];
extern long wptr[90000];
extern long wlen[90000];
extern long dict_size;
extern int dict_changed;

extern char plr_tiles[4][7];
extern int plr_scores[4];
extern int plr_skills[4];
extern int mean_pcts[SKILL_LEVELS];
extern int tiles_left[27];
extern int players;

extern char your[4][7];
extern char you[4][5];
extern int human_player;

extern int b1x, b1y, b2x, b2y;

extern compmove *chead;

extern int game_done;
extern int game_abort;

extern struct another_word new_words[8];

extern struct let_distrib letters[27];

extern int normal_dist[40];
extern int normal_sum;

#ifdef  COMPILE_GCC
#define QX(A) A
#else
#define QX(A) ()
#endif

/* curses & other stuff */

int wrefresh();
int endwin();
int stty();
int wmove();
int printw();
int wgetch();
int waddch();
int fclose();
int wstandout();
int wstandend();
int wclear();

/* cmove.c */

void computer_move QX((int));
void init_cmove QX((int));
int first_move QX((int));
void pick_word QX((int));
int cpu_move QX((int));
void try_tiles QX((int, int, int, int));
void eval_score QX((char *, char *, int, int, int, int));

/* misc.c */

void read_words QX((void));
void clear_rect QX((int, int, int, int));
void clear_prompt QX((void));
void clear_turn QX((void));
void press_return QX((void));
void print_board QX((void));
void print_tiles QX((void));
char draw_tile QX((void));
void print_tiles_left QX((void));
void print_passed_turns QX((void));
void print_whos_move QX((int));
void print_version QX((void));
void screen_redraw QX((void));
char get_key QX((void));
void seed_random QX((void));
long get_random QX((void));

/* pmove.c */

void player_move QX((void));
int place_tiles QX((int, int));
void show_instructions QX((void));
int rearrange_tiles QX((void));
void disp_instr QX((void));
int draw_new_tiles QX((void));

/* score.c */

int find_new_words QX((struct plrlet *, int, int, int));
long bin_search QX((char *));
void display_words QX((int, struct plrlet *, int, int));
void add_dict QX((char *));

/* smain.c */

void set_up_window QX((void));
void exit_window QX((void));
void init_board QX((void));
void print_info QX((void));
void init_tiles QX((void));
void get_num_players QX((void));
void end_game QX((void));
int main QX((void));
