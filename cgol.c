#include <stdlib.h>
#include <unistd.h>
#include <wchar.h>
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
/* 
 * struct array of initial patters
 * todo: populate with more initial patterns 
 * todo: move pattern creation to stand alone file
 */
INIT_PATTERN patterns[]= {
	{.pattern_type = "Oscillator",
	 .n_points = 6,
	 .points = {{5,3}, {5,4}, {5,5},
				{6,4}, {6,5}, {6,6}}
	}
};


struct win_dim {
	int height, width;
	int curY, curX;
	int xoffset, yoffset;
	char *form_type;
};

typedef int SCR_STATE;
/* To be initialized as array and hold corresponding screen values.
 * Simulation will change values in these NxN arrays
 * and will push to window
 */

void init(void);
/* initialize ncurses */

void terminate(WINDOW *, SCR_STATE *);
/* terminate ncurses, free allocated memory, clear screen, cleanup */

SCR_STATE* create_window(WINDOW *);
/* create initial ncurses window.
 * todo: dynamically add upto 6 windows
 * recalculate a constant window dimension
 */  

void update_window(WINDOW *, SCR_STATE *);
/* Evolution will be created in this function
 * nested loops will update grid
 * evolutions are determined by generations arg
 */

/*
 * Focus on getting a initial pattern oscillating on screen
 * single window.
 * todo: interative point placement !!
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
	SCR_STATE *scr1;

	scr1 = create_window(win);
	/*
	while (generations--) {
		update_window(win, scr1);
		sleep(1);	
	}
	*/
	getch();
	terminate(win, scr1);
	return 0;
}

#define G_RATIO ((1 + sqrt(5))/2)
#define FOR_(d, len) for (int d = 0; d < len; d++)

SCR_STATE* create_window(WINDOW *win)
{
	int height = LINES / G_RATIO;
	int width = COLS / G_RATIO;
	int curY = (LINES/2) - (height/2); 
	int curX = (COLS/2) - (width/2); 

	win = newwin(height, width, curY, curX);
	/* 0x25A2 is a white boardered square */
	wchar_t middot = 0x00B7;	 	
	SCR_STATE *scr1 = malloc(sizeof(scr1) * height * width);

	FOR_(y, height) {
		FOR_(x, width) {
			mvwprintw(win, y, x,"%lc", middot);
			*(scr1 + y*width + x) = middot;
		}
	}

	box(win, 0, 0);
	wrefresh(win);

	return scr1;
}
/*
void update_window(WINDOW* win, SCR_STATE)
{
	
}
*/
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

void terminate(WINDOW *win, SCR_STATE *scr1)
{
	clear();
	delwin(win);
	free(scr1);
	refresh();
	endwin();
}
