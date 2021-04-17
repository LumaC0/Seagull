#include <stdlib.h>
#include <unistd.h>
#include <locale.h>
#include <ncurses.h>
#include <math.h>

#define DEBUG_S(R) mvprintw(R,0, "made it to %d\n", R); \
				   getch();

#define MAX_POINTS 100

/* ############################################################################# */

typedef struct {short y, x;} Point;

typedef struct {
    char *pattern_type;
	int n_points;
	int xsize;
	int ysize;
	Point points[MAX_POINTS];
} INIT_PATTERN;

INIT_PATTERN patterns[]= {
	{.pattern_type = "Oscillator",
	 .n_points = 6,
	 .xsize = 4, /* aiming to roughly center the pattern coordinates */
	 .ysize = 2, 
	 .points = {{5,3}, {5,4}, {5,5},
				{6,4}, {6,5}, {6,6}}
	},
	{.pattern_type = "Random",
	 .n_points = 100,
	 .xsize = 100,
     .ysize = 100,
	 .points = { {84, 84},{98, 98},{92, 92},
				 {38, 38},{88, 88},{15, 15},
				 {80, 80}, {5, 5}, {83, 83},
				 {79, 79}, {17, 17},{70, 70},
				 {33, 33},{78, 78},{6, 6},
				 {7, 7},{52, 52},{34, 34},
				 {11, 11},{68, 68},{46, 46},
				 {47, 47},{44, 44},{72, 72},
				 {87, 87},{36, 36},{94, 94},
				 {61, 61},{76, 76},{25, 25},
				 {77, 77},{29, 29},{1, 1},
				 {28, 28},{65, 65},{56, 56},
				 {2, 2},{58, 58},{45, 45},
				 {73, 73},{64, 64},{54, 54},
				 {8, 8},{23, 23},{20, 20},
				 {60, 60},{90, 90},{24, 24},
				 {55, 55},{21, 21},{57, 57},
				 {40, 40},{75, 75},{59, 59},
				 {18, 18},{48, 48},{42, 42},
				 {67, 67},{41, 41},{19, 19},
				 {93, 93},{22, 22},{91, 91},
				 {50, 50},{16, 16},{26, 26},
				 {43, 43},{35, 35},{27, 27},
				 {62, 62},{32, 32},{31, 31},
				 {49, 49},{10, 10},{12, 12},
				 {74, 74},{96, 96},{37, 37},
				 {13, 13},{3, 3},{97, 97},
				 {0, 0},{9, 9},{63, 63},
				 {53, 53},{86, 86},{85, 85},
				 {39, 39},{69, 69},{71, 71},
				 {66, 66},{82, 82},{81, 81},
				 {4, 4},{99, 99},{30, 30},
				 {14, 14},{51, 51},{95, 95},{89, 89}}
	}
};

#ifdef RAND_MAX
#undef RAND_MAX
#define RAND_MAX sizeof(patterns)/sizeof(INIT_PATTERN);
#endif

/* 
 * struct array of initial patters
 * todo: populate with more initial patterns 
 * todo: move pattern creation to stand alone file
 *
 */

/* ############################################################################# */

struct dimension {
	int height, width;
	int curY, curX;
	int xoffset, yoffset;
	char *form_type;
};
/* window dimesions along with the name of the
 * initial pattern for labeling on screen
 */

typedef int SCRN_STATE;
/* To be initialized as array and hold corresponding screen values.
 * Simulation will change values in these NxN arrays
 * and will push to window
 */

typedef struct {
	WINDOW* win;
	SCRN_STATE* scr;
	struct dimension* dim;
} SCRN;
/* portable window implimentation with all referenceable values
 * membership does not include pattern struct
 * keeping init pattern and screen exclusive
 */

void init(void);
/* initialize ncurses */

void terminate(SCRN *);
/* terminate ncurses, free allocated memory, clear screen, cleanup */

SCRN* create_window(WINDOW *, INIT_PATTERN *);
/* create initial ncurses window.
 * todo: dynamically add upto 6 windows
 * recalculate a constant window dimension
 */  

void update_window(SCRN *);
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
	int generations = 100;

	if (argc > 1)
	    if ((generations = atoi(argv[1])) == 0) {
			printf("%s: Looking for an integer\n", argv[1]);
			return -1;
		}

	init();	
	
	WINDOW *win;

	/* "Random" initialization pattern */
	INIT_PATTERN chosen1 = patterns[1];
	
	SCRN* scr1 = create_window(win, &chosen1);
	int	timecheck; 	

	while (generations--) {
		update_window(scr1);
		sleep(1);	
		//getch();
	}

	getch();
	terminate(scr1);
	return 0;
}


#define G_RATIO ((1 + sqrt(5))/2)
#define FOR_(d, len) for (int d = 0; d < len; d++)

#include <wchar.h>
wchar_t middot = 0x00B7;	 	
wchar_t alive = '#';

/* TODO: initialize with patter
 */

SCRN* create_window(WINDOW *win, INIT_PATTERN *chosen1)
{

	struct dimension *dim = malloc(sizeof(*dim));	
	
	dim->height = LINES / G_RATIO;
	dim->width = COLS / G_RATIO;
	dim->curY = (LINES/2) - (dim->height/2); 
	dim->curX = (COLS/2) - (dim->width/2); 
	
	SCRN_STATE *scr_arr = malloc(sizeof(*scr_arr) * dim->height * dim->width);

	win = newwin(dim->height, dim->width, dim->curY, dim->curX);

	/* 0x25A2 is a white boardered square */
	/* middot to all values */
	FOR_(y, dim->height) {
		FOR_(x, dim->width) {
			mvwprintw(win, y, x,"%lc", middot);
			*(scr_arr + y*dim->height + x) = middot;
		}
	}
	/* add pattern */
	FOR_(p, chosen1->n_points) {
		int y = (dim->height/2) - (chosen1->points[p].y%dim->height);
		int x = (dim->width/2) - (chosen1->points[p].x%dim->width);
		wattron(win, COLOR_PAIR(1));
		mvwprintw(win, y, x,"%lc", alive );
		wattroff(win, COLOR_PAIR(1));
		*(scr_arr + y*dim->height + x) = alive;
		}


	box(win, 0, 0);
	wrefresh(win);

	SCRN *scr1 = malloc(sizeof(*scr1) + sizeof(dim) + sizeof(scr_arr));

	scr1->win = win;
	scr1->dim = dim;
	scr1->scr = scr_arr;
	getch();
	return scr1;
}

/* TODO: update pattern
 */

void update_window(SCRN* scr1)
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

            int n_score = 0; /* Moore neighborhood score for each pixel */
			int *point = (scr1->scr + y*h + x);

           FOR_(i,8) {
				int xadd, yadd, *tmpoint;
                if ((y+(yadd = moore_n[i][1])>=1 && y+moore_n[i][1]<h-1) && 
					(x+(xadd = moore_n[i][0])>=1 && x+moore_n[i][0]<w-1)) {

					tmpoint = (scr1->scr + (y+yadd)*h + (x+xadd));
                    n_score += (*tmpoint == alive) ? 1:0;
				}
			}

			*point =
             		((*point == alive) && (n_score>1 && n_score<4)) || 
					((*point == middot) && n_score==3)    		  ?   alive: middot;
      }
	}
	/* updating window with values from scr representation 1D-array */
	wchar_t status;
	FOR_(y, h) {
		FOR_(x, w) {
			if ((status = *(scr1->scr + y*h + x)) == alive)
				wattron(scr1->win, COLOR_PAIR(1));
			mvwprintw(scr1->win, y, x,"%lc", status);
			wattroff(scr1->win, COLOR_PAIR(1));
		}
	}

	box(scr1->win, 0, 0);
	wrefresh(scr1->win);
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
	init_pair(1, COLOR_CYAN, COLOR_BLACK);
	refresh();
	/* create initial subwindow */	
}

void terminate(SCRN *scr1)
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
