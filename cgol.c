#include <stdlib.h>
#include <unistd.h>
#include <locale.h>
#include <ncurses.h>
#include <math.h>

#define DEBUG_S(R) mvprintw(R,0, "made it to %d\n", R)

#define MAX_POINTS 100

typedef struct {short y, x;} Point;

typedef struct {
    char *pattern_type;
	int n_points;
	Point points[MAX_POINTS];
} INIT_PATTERN;

INIT_PATTERN patterns[]= {
	{.pattern_type = "Oscillator",
	 .n_points = 6,
	 .points = {{5,3}, {5,4}, {5,5},
				{6,4}, {6,5}, {6,6}}
	}
};
/* 
 * struct array of initial patters
 * todo: populate with more initial patterns 
 * todo: move pattern creation to stand alone file
 */


struct dimension {
	int height, width;
	int curY, curX;
	int xoffset, yoffset;
	char *form_type;
};
/* window dimesions along with the name of the
 * initial pattern for labeling on screen
 */

typedef int SCR_STATE;
/* To be initialized as array and hold corresponding screen values.
 * Simulation will change values in these NxN arrays
 * and will push to window
 */

typedef struct {
	WINDOW* win;
	SCR_STATE* scr;
	struct dimension* dim;
} SCR;
/* portable window implimentation with all referenceable values
 * membership does not include pattern struct
 * keeping init pattern and screen exclusive
 */

void init(void);
/* initialize ncurses */

void terminate(SCR *);
/* terminate ncurses, free allocated memory, clear screen, cleanup */

SCR* create_window(WINDOW *);
/* create initial ncurses window.
 * todo: dynamically add upto 6 windows
 * recalculate a constant window dimension
 */  

void update_window(SCR *);
/* Evolution will be created in this function
 * nested loops will update grid
 * evolutions are determined by generations arg
 */

/*
 * Focus on getting a initial pattern oscillating on screen
 * single window.
 * todo: interactive point placement !!
 */
int main(int argc, char *argv[])
{
	int generations = 10;

	if (argc > 1)
	    if ((generations = atoi(argv[1])) == 0) {
			printf("%s: Looking for an integer\n", argv[1]);
			return -1;
		}

	init();	
	
	WINDOW *win;

	SCR* scr1 = create_window(win);
	int	timecheck; 	

	while (generations--) {
		update_window(scr1);
		if ((timecheck = usleep(1)) == 0);
			break;	
	}

	getch();
	terminate(scr1);
	return 0;
}


#define G_RATIO ((1 + sqrt(5))/2)
#define FOR_(d, len) for (int d = 0; d < len; d++)

#include <wchar.h>
wchar_t middot = 0x00B7;	 	

/* TODO: initialize with patter
 */

SCR* create_window(WINDOW *win)
{

	struct dimension *dim = malloc(sizeof(*dim));	
	
	dim->height = LINES / G_RATIO;
	dim->width = COLS / G_RATIO;
	dim->curY = (LINES/2) - (dim->height/2); 
	dim->curX = (COLS/2) - (dim->width/2); 
	
	SCR_STATE *scr_arr = malloc(sizeof(*scr_arr) * dim->height * dim->width);

	win = newwin(dim->height, dim->width, dim->curY, dim->curX);

	/* 0x25A2 is a white boardered square */

	FOR_(y, dim->height) {
		FOR_(x, dim->width) {
			mvwprintw(win, y, x,"%lc", middot);
			*(scr_arr + y*dim->width + x) = middot;
		}
	}

	box(win, 0, 0);
	wrefresh(win);

	SCR *scr1 = malloc(sizeof(*scr1) + sizeof(dim) + sizeof(scr_arr));

	scr1->win = win;
	scr1->dim = dim;
	scr1->scr = scr_arr;

	return scr1;
}

/* TODO: update pattern
 */

void update_window(SCR* scr1)
{
	int h = scr1->dim->height;	
	int w = scr1->dim->width;	

 	int moore_n [][2] = {
			{-1,1},
			{-1,0},
			{-1,-1},
			{0, 1},
			{0,-1},
			{1,1},
			{1, 0},
			{1,-1}
    };

    FOR_(y, h) {
		FOR_(x, w) {

            int n_score = 0;
			int *point = (scr1->scr + y*scr1->dim->width + x);

           FOR_(i,8) {
                if ((y+moore_n[i][1]>=1 && y+moore_n[i][1]<h-1) && 
					(x+moore_n[i][0]>=1 && x+moore_n[i][0]<w-1))

                    n_score += (*(point+moore_n[i][1]*scr1->dim->width + moore_n[i][0]) == middot) ? 1:0;
			}

			*point =
             		((*point == '#') && (n_score>1 && n_score<4)) || 
					((*point == middot) && n_score==3)    		  ?   '#': middot;
      }
	}
}

/* initialize ncurses, colors and first window */
void init()
{
	setlocale(LC_ALL, "");
	initscr();
	cbreak();
	noecho;
	keypad(stdscr, TRUE);
	/* nodelay(stdscr, TRUE);	// force getch to be a non-blocking call */
	
	/* add terminal color initialization */
	start_color();
	init_pair(1, COLOR_CYAN, COLOR_WHITE);
	refresh();
	/* create initial subwindow */	
}

void terminate(SCR *scr1)
{
	clear();
	refresh();
	delwin(scr1->win);
	endwin();

	//free(scr1->win);
	free(scr1->dim);
	free(scr1->scr);
	free(scr1);
	
}
