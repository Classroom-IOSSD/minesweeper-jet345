#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "conio.h"
#define MAX 10

// background color
#define KNRM  "\x1B[0m"
#define BRED  "\x1B[41m"
#define BGRN  "\x1B[42m"
#define BYEL  "\x1B[43m"
#define BBLU  "\x1B[44m"
#define BMAG  "\x1B[45m"
#define BCYN  "\x1B[46m"
#define BWHT  "\x1B[47m"

// text color
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"

typedef enum {false, true} bool;
void new_game();
void flag_mode();
void end_of_game();
void check_mode();

// global variables
// game table
unsigned char table_array[MAX][MAX];
// location of cursor
int x = 0, y = 0;
// flag: input mode = 0, flag mode = 1, check mode = 2
int game_mode = 0;

char Cursor_direction;
int Numberof_Mines; // the number of the remaining mines
int i, j, r, c, value, rows[8], columns[8];
char direction;
int first;
char* argv[];
bool Is_valid_rows_and_column;

/*This is a recursive function which uncovers blank cells while they are adjacent*/
int uncover_blank_cell(int row, int col) {
	int value, rows[8], columns[8], i;
	bool is_valid;
	if (table_array[row][col] != 0)
		return 0; // error

	table_array[row][col] += 10; // uncover current cell

								 // Get position of adjacent cells of current cell
	rows[0] = row - 1;
	columns[0] = col + 1;
	rows[1] = row;
	columns[1] = col + 1;
	rows[2] = row + 1;
	columns[2] = col + 1;
	rows[3] = row - 1;
	columns[3] = col;
	rows[4] = row + 1;
	columns[4] = col;
	rows[5] = row - 1;
	columns[5] = col - 1;
	rows[6] = row;
	columns[6] = col - 1;
	rows[7] = row + 1;
	columns[7] = col - 1;

	for (i = 0; i < 8; i++) {
		value = table_array[rows[i]][columns[i]];
		is_valid = (rows[i] >= 0 && rows[i] < MAX) && (columns[i] >= 0 && columns[i] < MAX);
		if (is_valid) {	// to prevent negative index and out of bounds
			if (value > 0 && value <= 8)
				table_array[rows[i]][columns[i]] += 10;										// it is a cell with 1-8 number so we need to uncover
			else if (value == 0)
				uncover_blank_cell(rows[i], columns[i]);
		}

	}

	return 1; // success!
}


void print_table() {
	// clear screen
	system("clear");

	int i, j, value;
	for (i = 0; i < MAX; i++) {
		for (j = 0; j < MAX; j++) {
			if (x == j && y == i) {
				if (game_mode == 1) {
					printf("|%sF%s", BMAG, KNRM);
					continue;
				}
				else if (game_mode == 2) {
					printf("|%sC%s", BMAG, KNRM);
					continue;
				}

			}
			value = table_array[i][j];

			if ((value >= 0 && value <= 8) || value == 0 || value == 99)
				printf("|X");
			else if (value == 10) // clean area
				printf("|%s%d%s", KCYN, value - 10, KNRM);
			else if (value == 11) // the number of near mine is 1
				printf("|%s%d%s", KYEL, value - 10, KNRM);
			else if (value > 11 && value <= 18) // the number of near mine is greater than 1
				printf("|%s%d%s", KRED, value - 10, KNRM);
			else if ((value >= 20 && value <= 28) || value == 100)
				printf("|%sF%s", KGRN, KNRM);
			else
				printf("ERROR"); // test purposes

		}
		printf("|\n");
	}

	printf("cell values: 'X' unknown, '%s0%s' no mines close, '1-8' number of near mines, '%sF%s' flag in cell\n", KCYN, KNRM, KGRN, KNRM);
	if (game_mode == 0) {
		printf("f (put/remove Flag in cell), c (Check cell), n (New game), q (Exit game): ");
	}
	else if (game_mode == 1) {
		printf("Enter (select to put/remove Flag in cell), q (Exit selection): ");
	}
	else if (game_mode == 2) {
		printf("Enter (select to check cell), q (Exit selection): ");
	}


}

/**
	@breif°ÔÀÓ ÁŸ·áœÃ
*/
void end_of_game() {
	game_mode = 0;
	print_table();
	printf("\nGAME OVER\n");

	if (Numberof_Mines == 0)
		printf("you won!!!!\n");

	else
		printf("BOOM! you LOOSE!\n");

	do {
		printf("Are you sure to exit? (y or n)? ");
		Cursor_direction = getch();
		putchar('\n');
		if (Cursor_direction == 'y' || Cursor_direction == 'Y') {
			break;
		}
		else if (Cursor_direction == 'n' || Cursor_direction == 'N') {
			new_game(first, argv);
		}
		printf("Please answer y or n\n");
	} while (1);
	printf("See you next time!\n");

	fflush(stdin);
}

/**
	@breif check mode ÁøÀÔ
*/
void check_mode() {
	game_mode = 2;

	bool UP = (direction == '8');
	bool DOWN = (direction == '2');
	bool LEFT = (direction == '4');
	bool RIGHT = (direction == '6');
	
	do {
		print_table();
		direction = getch();

		// arrow direction
		if (UP) {
			y = (MAX + --y) % MAX;
		}
		else if (DOWN) {
			y = ++y % MAX;
		}
		else if (LEFT) {
			x = (MAX + --x) % MAX;
		}
		else if (RIGHT) {
			x = ++x % MAX;
		}
		else if (direction == 'f' || direction == 'F') {
			flag_mode();
		}

		else if (direction == '\n') {
			value = table_array[y][x];
			if (value == 0)						// blank case
				uncover_blank_cell(y, x);
			else if (value == 99)				// mine case
				end_of_game();
			else if (value > 0 && value <= 8)	// number case (the next cell is a mine)
				table_array[y][x] += 10;
		}
	} while (direction != 'q' && direction != 'Q');
	game_mode = 0;
}
/**
	@breif flag mode ÁøÀÔ
*/
void flag_mode() {
	game_mode = 1;
	do {
		print_table();
		direction = getch();
		// arrow direction
		if (direction == '8') {
			// up
			y = (MAX + --y) % MAX;
		}
		else if (direction == '2') {
			// down
			y = ++y % MAX;
		}
		else if (direction == '4') {
			x = (MAX + --x) % MAX;
		}
		else if (direction == '6') {
			x = ++x % MAX;
		}
		else if (direction == 'c' || direction == 'C') {
			check_mode();
		}
		else if (direction == '\n') {
			value = table_array[y][x];

			if (value == 99) {				// mine case
				table_array[y][x] += 1;
				Numberof_Mines -= 1;				// mine found
			}
			else if (value >= 0 && value <= 8) {	// number of mines case (the next cell is a mine)
				table_array[y][x] += 20;
			}
			else if (value >= 20 && value <= 28) {
				table_array[y][x] -= 20;
			}

			if (Numberof_Mines == 0)
				break;
		}
	} while (direction != 'q' && direction != 'Q');
	game_mode = 0;
}

/**
	@breif °ÔÀÓ œÃÀÛ
*/
void new_game() {

	Numberof_Mines = 10;

	// random seed
	srand(time(NULL));

	// setting cursor
	x = 0;
	y = 0;

	// set all cells to 0
	for (i = 0; i < 10; i++)
		for (j = 0; j < 10; j++)
			table_array[i][j] = 0;

	for (i = 0; i < Numberof_Mines; i++) {
		/* initialize random seed: */

		// It generates a integer in the range 0 to 9
		r = rand() % 10;
		c = rand() % 10;

		// put mines
		if (table_array[r][c] != 99) {
			table_array[r][c] = 99;

			// Get position of adjacent cells of current cell
			rows[0] = r - 1;
			columns[0] = c + 1;
			rows[1] = r;
			columns[1] = c + 1;
			rows[2] = r + 1;
			columns[2] = c + 1;
			rows[3] = r - 1;
			columns[3] = c;
			rows[4] = r + 1;
			columns[4] = c;
			rows[5] = r - 1;
			columns[5] = c - 1;
			rows[6] = r;
			columns[6] = c - 1;
			rows[7] = r + 1;
			columns[7] = c - 1;

			for (j = 0; j < 8; j++) {
				value = table_array[rows[j]][columns[j]];
				Is_valid_rows_and_column = (rows[j] >= 0 && rows[j] < MAX) && (columns[j] >= 0 && columns[j] < MAX);

				if (Is_valid_rows_and_column) {	// to prevent negative index and out of bounds
					if (value != 99)			// to prevent remove mines
						table_array[rows[j]][columns[j]] += 1;		// sums 1 to each adjacent cell
				}
			}
		}
		else {							// to make sure that there are the properly number of mines in table
			i--;
			continue;
		}
	}

	while (Numberof_Mines != 0) {			// when Numberof_Mines becomes 0 you will win the game
		print_table();
		Cursor_direction = getch();
		// cursor direction

		switch (Cursor_direction) {
			// flag
		case 'f':
		case 'F':
			flag_mode();

			// check cell
		case 'c':
		case 'C':
			check_mode();

			// jump to a new game
		case 'n':
		case 'N':
			new_game();
			// exit
		case 'q':
		case 'Q':
			end_of_game();

		default:
			break;
		}
	}
}


int main(int argc, char *argv[]) {
	new_game();

	return 0;
}


