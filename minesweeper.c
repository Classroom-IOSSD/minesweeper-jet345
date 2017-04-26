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

 #define BIT(x) (0x01 << (x)) 
 #define BITMASK_SET(x,y) ((x) |= (y))
 #define BITMASK_CLEAR(x,y) ((x) &= (~(y)))
 #define BITMASK_FLIP(x,y) ((x) ^= (y))
 #define BITMASK_CHECK(x,y) (((x) & (y)) == (y))

const unsigned int FLAG_MASK = BIT(4);
const unsigned int UNCOVERED_MASK = BIT(5);
const unsigned int MINE_MASK = BIT(6);
const unsigned int NUM_8 = BIT(3);
const unsigned int NUM_4 = BIT(2);
const unsigned int NUM_2 = BIT(1);
const unsigned int NUM_1 = BIT(0);

// global variables
// game table
unsigned char table_array[MAX][MAX];
unsigned int km_table_array[MAX][MAX];
// location of cursor
int x=0, y=0;
// flag: input mode = 0, flag mode = 1, check mode = 2
int game_mode=0;

_Bool has_mine(unsigned int cell){
	return BITMASK_CHECK(cell, MINE_MASK);
}

_Bool is_uncovered(unsigned int cell){
	return BITMASK_CHECK(cell, UNCOVERED_MASK);
}

_Bool is_flagged(unsigned int cell){
	return BITMASK_CHECK(cell, FLAG_MASK);
}
//--------------------------------------------//
void put_mine(unsigned int cell){
	BITMASK_SET(cell, MINE_MASK);
}

void put_uncovered(unsigned int cell){
	BITMASK_SET(cell, UNCOVERED_MASK);
}
void put_flagged(unsigned int cell){
	BITMASK_SET(cell, FLAG_MASK);
}
//--------------------------------------------//
unsigned int num_mines(unsigned int cell){
	//check true or false
	unsigned int result = 0;
	if(BITMASK_CHECK(cell, NUM_8))
		result+=8;
	if(BITMASK_CHECK(cell, NUM_4))
		result+=4;
	if(BITMASK_CHECK(cell, NUM_2))
		result+=2;
	if(BITMASK_CHECK(cell, NUM_1))
		result+=1;

	return result;
}

int uncover_blank_cell(int row, int col) {
	int value, rows[8], columns[8], i;
	unsigned int km_value;

	if(table_array[row][col] != 0)
        return 0; // error

    table_array[row][col] += 10; // uncover current cell
    put_uncovered(km_table_array[row][col]);
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

    for(i = 0; i < 8; i++) {
    	value = table_array[rows[i]][columns[i]];
    	km_value = km_table_array[rows[i]][columns[i]];

        if( (rows[i] >= 0 && rows[i] < MAX) && (columns[i] >= 0 && columns[i] < MAX) ) {	// to prevent negative index and out of bounds
        	if(value > 0 && value <= 8){
        		table_array[rows[i]][columns[i]] += 10;
        	}
        	if(value == 0)
        		uncover_blank_cell(rows[i], columns[i]);

        	if(num_mines(km_value)==0){
        		put_uncovered(km_table_array[rows[i]][columns[i]]);
        	}
        	if(num_mines(km_value) > 0){
        		uncover_blank_cell(rows[i], columns[i]);
        	}
        }

    }

    return 1; // success!
}

void print_table() {
    // clear screen
	system("clear");

	int i, j, value;
	unsigned int km_value;
	for(i = 0; i < MAX; i++) {
		for(j = 0; j < MAX; j++) {
			if(x == j && y == i) {
				if(game_mode == 1) {
					printf("|%sF%s",BMAG,KNRM);
					continue;
				} else if(game_mode == 2) {
					printf("|%sC%s",BMAG,KNRM);
					continue;
				}

			}
			value = table_array[i][j];
			km_value = km_table_array[i][j];

			/*if((value >= 0 && value <= 8) || value == 99)
				printf("|X");
            else if(value == 10) // clean area
            	printf("|%s%d%s",KCYN, value - 10,KNRM);            	
            else if(value == 11) // the number of near mine is 1
            	printf("|%s%d%s",KYEL, value - 10,KNRM);
            else if(value > 11 && value <= 18) // the number of near mine is greater than 1
            	printf("|%s%d%s",KRED, value - 10,KNRM);
            else if((value >= 20 && value <= 28) || value == 100)
            	printf("|%sF%s",KGRN,KNRM);
            else
                printf("ERROR"); // test purposes*/

            if(is_uncovered(km_value)){
            	if(num_mines(km_value) == 0)
            		printf("|%s%d%s",KCYN, value - 10,KNRM);
            	else if(num_mines(km_value) == 1)
            		printf("|%s%d%s",KYEL, value - 10,KNRM);
            	else
            		printf("|%s%d%s",KRED, value - 10,KNRM);
            }
            else if(!is_uncovered(km_value)){
            	
            	if (is_flagged(km_value))
            		printf("|%sF%s",KGRN,KNRM);
            	else
            		printf("|X");
            }
        }
        printf("|\n");
    }

        printf("cell values: 'X' unknown, '%s0%s' no mines close, '1-8' number of near mines, '%sF%s' flag in cell\n",KCYN,KNRM,KGRN,KNRM);
        if(game_mode == 0) {
        	printf("f (put/remove Flag in cell), c (Check cell), n (New game), q (Exit game): ");
        } else if(game_mode == 1) {
        	printf("Enter (select to put/remove Flag in cell), q (Exit selection): ");
        } else if(game_mode == 2) {
        	printf("Enter (select to check cell), q (Exit selection): ");
        }
}

void print_real_table(){
    	printf("\n");
    	int i,j;
    	for(i=0; i<MAX ; i++){
    		for(j=0; j<MAX ; j++){
    			printf(" %d ",table_array[i][j]);
    		}
    		printf("\n");
    	}
}

int main(int argc, char *argv[]) {

    char ch;
    int nMines; // the number of the remaining mines
    int i,j,r,c,value, rows[8], columns[8];
    unsigned int km_value;

    new_game:
    // the number of mines
    nMines = 10;
    if(argc == 2) {
    	nMines = atoi(argv[1]);
    }
    srand (time(NULL));						// random seed
    // setting cursor
    x = 0;
    y = 0;
    // set all cells to 0
    for(i = 0; i < 10; i++)
    	for(j = 0; j < 10; j++){
    		table_array[i][j] = 0;
            km_table_array[i][j]=0; //initialization
        }

        for(i = 0; i < nMines; i++) {
        /* initialize random seed: */

        r = rand() % 10;					// it generates a integer in the range 0 to 9
        c = rand() % 10;

        // put mines
        if(table_array[r][c] != 99) {
        	//MINE MASK 
        	
        	table_array[r][c] = 99;
        	put_mine(km_table_array[r][c]);

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

        	for(j = 0; j < 8; j++) {
        		value = table_array[rows[j]][columns[j]];
        		km_value = km_table_array[rows[j]][columns[j]];

                if( (rows[j] >= 0 && rows[j] < MAX) && (columns[j] >= 0 && columns[j] < MAX) ) {	// to prevent negative index and out of bounds
                	if(value != 99){
                		table_array[rows[j]][columns[j]] += 1;                    	
                	}
                	if(has_mine(km_value)){
                		km_table_array[rows[j]][columns[j]] += 1; 
                	}
                }
            }

        } else {							// to make sure that there are the properly number of mines in table
        	i--;
        	continue;
        }
    }

    //
    while(nMines != 0) {			// when nMines becomes 0 you will win the game
    	print_table();
    	print_real_table();
    	ch = getch();
        // cursor direction
    	char direction;
    	switch (ch) {

        // flag
    		case 'f':
    		case 'F':


    		flag_mode:
    		game_mode = 1;
    		do {
    			print_table();
    			direction = getch();
                // arrow direction
    			if(direction == '8') {
                    // up
    				y = (MAX + --y) % MAX;
    			} else if(direction == '2') {
                    // down
    				y = ++y % MAX;
    			} else if(direction == '4') {
    				x = (MAX + --x) % MAX;
    			} else if(direction == '6') {
    				x = ++x % MAX;
    			} else if(direction == 'c' || direction == 'C') {
    				goto check_mode;
    			} else if(direction == '\n') {
    				value = table_array[y][x];

                    if (value == 99) {				// mine case
                    	table_array[y][x] += 1;
                        nMines -= 1;				// mine found
                    } else if(value >= 0 && value <= 8) {	// number of mines case (the next cell is a mine)
                    	table_array[y][x] += 20;
                    } else if(value >= 20 && value <= 28) {
                    	table_array[y][x] -= 20;
                    }

                    if(nMines == 0)
                    	break;
                }
            } while (direction != 'q' && direction != 'Q');
            game_mode = 0;

            break;

        // check cell
            case 'c':
            case 'C':

            check_mode:
            game_mode = 2;
            do {
            	print_table();
            	direction = getch();

                // arrow direction
            	if(direction == '8') {
                    // up
            		y = (MAX + --y) % MAX;
            	} else if(direction == '2') {
                    // down
            		y = ++y % MAX;
            	} else if(direction == '4') {
            		x = (MAX + --x) % MAX;
            	} else if(direction == '6') {
            		x = ++x % MAX;
            	} else if(direction == 'f' || direction == 'F') {
            		goto flag_mode;
            	}

            	else if(direction == '\n') {
            		value = table_array[y][x];
                    if(value == 0)						// blank case
                    	uncover_blank_cell(y, x);
                    else if(value == 99)				// mine case
                    	goto end_of_game;
                    else if(value > 0 && value <= 8)	// number case (the next cell is a mine)
                    	table_array[y][x] += 10;

                    //	break;
                }
            } while (direction != 'q' && direction != 'Q');
            game_mode = 0;

            break;



        // jump to a new game
            case 'n':
            case 'N':
            goto new_game;
            break;

        // exit
            case 'q':
            case 'Q':
            goto end_of_game;


            default:
            break;
        }


        end_of_game:
        game_mode = 0;
        print_table();
        printf("\nGAME OVER\n");

        if(nMines == 0)
        	printf("you won!!!!\n");

        else
        	printf("BOOM! you LOOSE!\n");

        do {
        	printf("Are you sure to exit? (y or n)? ");
        	ch = getch();
        	putchar('\n');
        	if(ch == 'y' || ch == 'Y') {
        		break;
        	} else if(ch == 'n' || ch == 'N') {
        		goto new_game;
        	}
        	printf("Please answer y or n\n");
        } while(1);
        printf("See you next time!\n");

        fflush(stdin);    

    }
	return 0;    
}

