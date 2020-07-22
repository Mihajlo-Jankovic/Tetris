/* Made by Mihajlo Jankovic - 2020 <jankovic.mihajlo99@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify it.
 * Since I am currently the first year student, I have been working on this 
 * project for the purpose of practice and also for fun.
 * 
 * Thank you for visiting and playing!
 * 
 * Enjoy!
 */
 
#include<ncurses.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#define ROW 21 // Numer of rows
#define COL 10 // Number of columns
#define X 25 // Starting coordinate of tetris field (X)
#define Y 12// Starting coordinate of tetris field (Y) 
#define START_ROW 1 // Starting row where figures appear
#define START_SPEED 480 // Movement speed
#define SHAPE_BLOCKS_MAX 4 // Each figure consists of 4 blocks
#define SCOREBOARD_MAX 5 // Max number of names in scoreboard table
#define MAX_SCORE 999999 // Max score player can reach
#define GIGA 1000000000 // 10^9
#define MICRO 0.000001 //10^-6
#define SPACE 32
#define ENTER_CR 13
#define ENTER_LF 10
#define ESC 27
#define EMPTY 0  // Empety space in matrix

typedef int playfield[ROW][COL];

struct player
{
	char name[7];
	int score;
};

// struct timespec // From time.h library
// {                                                                                     
//     time_t   tv_sec;        /* seconds */                                                             
//     long     tv_nsec;       /* nanoseconds */                                                         
// }; 

void matrix_initialize(playfield matrix) // Initialize entire matrix to EMPTY value
{
	int i,j;
	
	for(i = 0; i < ROW; i++)
	{
		for(j = 0; j < COL; j++)
		{
			matrix[i][j] = EMPTY;
		}
	}
}

void matrix_print(playfield matrix, WINDOW *win_main)
{
	int i,j;
	
	for(i = 0; i < ROW; i++)
	{
		for(j = 0; j < COL; j++)
		{
			if(matrix[i][j] == 0) mvwaddch(win_main, i, j, ' ');
			else mvwaddch(win_main, i, j, '#' | COLOR_PAIR(matrix[i][j]));	
		}
	}
	
	wrefresh(win_main);
}

void delay(int milli_seconds)
{  
    int time = 1000 * milli_seconds;
    
    clock_t start_time = clock(); 
     
    while (clock() < start_time + time);  // looping till required time is not achieved 
}

void color(int ind) // Color schemes
{
	init_pair(8, COLOR_MAGENTA, COLOR_BLACK); 
	init_pair(9, COLOR_WHITE, COLOR_BLACK);
	init_pair(10, COLOR_YELLOW, COLOR_BLACK);
	init_pair(11, COLOR_BLACK, COLOR_WHITE);
	init_pair(12, COLOR_WHITE, COLOR_GREEN);
	init_pair(13, COLOR_GREEN, COLOR_BLACK);
	
	if(ind == 1) // ASCII RICH
	{
		init_pair(1, COLOR_CYAN , COLOR_BLACK); // I
		init_pair(2, COLOR_WHITE , COLOR_BLACK); // O
		init_pair(3, COLOR_MAGENTA , COLOR_BLACK); // T
		init_pair(4, COLOR_YELLOW , COLOR_BLACK); // L
		init_pair(5, COLOR_BLUE , COLOR_BLACK); // J
		init_pair(6, COLOR_RED , COLOR_BLACK); // Z
		init_pair(7, COLOR_GREEN , COLOR_BLACK); // S
	}
	
	else if(ind == 0) // SHAPE ORIG
	{
		init_pair(1, COLOR_CYAN, COLOR_CYAN); // I
		init_pair(2, COLOR_WHITE, COLOR_WHITE); // O
		init_pair(3, COLOR_MAGENTA, COLOR_MAGENTA); // T
		init_pair(4, COLOR_YELLOW , COLOR_YELLOW); // L
		init_pair(5, COLOR_BLUE , COLOR_BLUE); // J
		init_pair(6, COLOR_RED , COLOR_RED); // Z
		init_pair(7, COLOR_GREEN , COLOR_GREEN); // S
	}
	
	else
	{
		for(int i = 1; i < 8; i++)
		{
			init_pair(i, COLOR_WHITE, COLOR_BLACK); // ASCII ORIG
		}
	}
}

int shape_generator(playfield matrix, int i, int j, int *rand_num, int **shape_len, int **shape_hight)
{
	switch(*rand_num)
	{
		case 1: // I
			matrix[i][j] = 1;
			matrix[i][j+1] = 1;
			matrix[i][j+2] = 1;
			matrix[i][j+3] = 1;
			**shape_len = 4;
			**shape_hight = 1;
			
			break;
		case 2: // O
			matrix[i][j] = 2;
			matrix[i+1][j] = 2;
			matrix[i][j+1] = 2;
			matrix[i+1][j+1] = 2;
			**shape_len = 2;
			**shape_hight = 2;
			
			break; 
		case 3: // T
			matrix[i][j] = 3;
			matrix[i][j+1] = 3;
			matrix[i][j+2] = 3;
			matrix[i+1][j+1] = 3;
			**shape_len = 3;
			**shape_hight = 2;
			
			break; 
		case 4: // L
			matrix[i][j] = 4;
			matrix[i+1][j] = 4;
			matrix[i][j+1] = 4;
			matrix[i][j+2] = 4;
			**shape_len = 3;
			**shape_hight = 2;
			
			break;
		case 5: // J
			matrix[i][j] = 5;
			matrix[i][j+1] = 5;
			matrix[i][j+2] = 5;
			matrix[i+1][j+2] = 5;
			**shape_len = 3;
			**shape_hight = 2;
			
			break;
		case 6: // Z
			matrix[i][j] = 6;
			matrix[i][j+1] = 6;
			matrix[i+1][j+1] = 6;
			matrix[i+1][j+2] = 6;
			**shape_len = 3;
			**shape_hight = 2;
			
			break; 
		case 7: // S
			matrix[i+1][j] = 7;
			matrix[i+1][j+1] = 7;
			matrix[i][j+1] = 7;
			matrix[i][j+2] = 7;
			**shape_len = 3;
			**shape_hight = 2;
			
			break;
	}
}

void win_next(int temp[][10], int *rand_num, int shape_len, int shape_hight)
{
	WINDOW * win_next_border = newwin(6,17,Y+3,X+COL+3);
	WINDOW * win_next = subwin(win_next_border,4,15,Y+4,X+COL+4);
	
	wborder(win_next_border, 0, 0, 0, 0, 0, 0, 0, 0); 
	
	keypad(win_next, TRUE);
	nodelay(win_next, TRUE);
	
	wbkgd(win_next_border, COLOR_PAIR(8));
	
	if(shape_len)
	{
		int i,j;
		
		for(i = 1; i < shape_hight+1; i++)
		{
			for(j = 6; j < shape_len+6; j++)
			{
				if(temp[i-1][j-6] == 0) mvwaddch(win_next, i, j, ' ');
				else mvwaddch(win_next, i, j, '#' | COLOR_PAIR(temp[i-1][j-6]));	
			}
		}
	}
	
	wrefresh(win_next);
	wrefresh(win_next_border);
}

int generate_shapes(playfield matrix, int *start_pos, int *rand_num, int *shape_len, int *shape_hight)
{
	srand(time(0));
	
	static int rand_num_ind = 1; // Only first time indicator
	static int rand_num_next; // Next figure number
	const int lower_num = 1, upper_num = 7; // possible shape values in between, also including 1 and 7
	const int lower_start = 0, upper_start = COL-4; // possible shape start position (column)
	int shape_len_next, shape_hight_next;
	int temp[2][10], *shape_len_temp, *shape_hight_temp; // PROVERI ZA VELICINU 10!!!!!!!!
	int game_over_status = 0; // End game status
	int tmp_game_over[2][4], game_over_ind = 0;
	int i = 0, br = 0;
	
	shape_len_temp = &shape_len_next;
	shape_hight_temp = &shape_hight_next;
	
	for(int j = 0; j < 4; j++)
	{
		temp[0][j] = EMPTY;
		temp[1][j] = EMPTY;
	}
	
	if(rand_num_ind) { rand_num_next = (rand() % (upper_num - lower_num + 1)) + lower_num; rand_num_ind = 0; } // Only first time (Generate figure)
	
	*rand_num = rand_num_next;
	rand_num_next = (rand() % (upper_num - lower_num + 1)) + lower_num; // Generate next figure value
	*start_pos = (rand() % (upper_start - lower_start + 1)) + lower_start; // Generate shape start position
	
	for(int j = *start_pos; j < *start_pos+4; j++) 
	{
		tmp_game_over[0][i] = matrix[START_ROW][j];
		tmp_game_over[1][i] = matrix[START_ROW+1][j];
		
		if(matrix[START_ROW][j] != EMPTY) { matrix[START_ROW][j] = EMPTY; br++; }
		if(matrix[START_ROW+1][j] != EMPTY) { matrix[START_ROW+1][j] = EMPTY; br++; }
		
		i++;
	}
	
	if(br) game_over_ind = 1;
	
	shape_generator(matrix, START_ROW, *start_pos, rand_num, &shape_len, &shape_hight);
	
	shape_generator(temp, 0, 0, &rand_num_next, &shape_len_temp, &shape_hight_temp);
	win_next(temp,rand_num,shape_len_next,shape_hight_next); // print next figure
	
	if(game_over_ind)
	{
		br = 0;
		i = 0;
		for(int j = *start_pos; j < *start_pos+*shape_len; j++) 
		{
			if(matrix[START_ROW+1][j] != EMPTY && tmp_game_over[1][i] != EMPTY) br++;
			else if(matrix[START_ROW+1][j] == EMPTY && tmp_game_over[1][i] != EMPTY) matrix[START_ROW+1][j] = tmp_game_over[1][i];
			
			if(matrix[START_ROW][j] != EMPTY && tmp_game_over[0][i] != EMPTY) br++;
			else if(matrix[START_ROW][j] == EMPTY && tmp_game_over[0][i] != EMPTY) matrix[START_ROW][j] = tmp_game_over[0][i];
			i++;
		}
		
		if(*shape_len < 4) // Check if game is over
		{
			for(int j = *start_pos+*shape_len; j < *start_pos+4; j++)
			{
				if(matrix[START_ROW+1][j] == EMPTY && tmp_game_over[1][i] != EMPTY) matrix[START_ROW+1][j] = tmp_game_over[1][i];
				if(matrix[START_ROW][j] == EMPTY && tmp_game_over[0][i] != EMPTY) matrix[START_ROW][j] = tmp_game_over[0][i];
				i++;
			}
		}
		
		if(br) game_over_status = 1;
	}
	
	return game_over_status;
}

int rotate_detection(playfield matrix, int rand_num, int start_pos, int current_row, int shape_state, int shape_len, int shape_hight, int *state_ind, int *temp1_shape, int *temp2_shape)
{
	int i,j,br = 0,ind = 0;
	
	switch(shape_state)
	{
		case 1:
			ind = 0;
			for(i = current_row; i < current_row+shape_len; i++)
			{
				if(temp1_shape[ind] != EMPTY && matrix[i][start_pos] == EMPTY) br++;
				if(temp2_shape[ind] != EMPTY && matrix[i][start_pos+1] == EMPTY) br++; 
				
				ind++;
			}
			
			switch(*state_ind)
			{
				case 1:
					if(rand_num == 1 || rand_num == 5) { if(br == 3) return 1; }
					else if(br == 2) return 1;
					return 0;
					break;
					
				case 2:
					if(rand_num == 1 || rand_num == 4) { if(br == 3) return 1; }
					else if(br == 2) return 1;
					return 0;
					break;
			}
			break;
			
			case 2:
				if(start_pos+shape_len-1 >= COL) return 0;
			
				ind = 0;
				for(i = start_pos; i < start_pos+shape_len; i++)
				{
					if(temp1_shape[ind] != EMPTY && matrix[current_row][i] == EMPTY) br++;
					if(temp2_shape[ind] != EMPTY && matrix[current_row+1][i] == EMPTY) br++;
					
					ind++;
				}
				
				switch(*state_ind)
				{
					case 1:
						if(rand_num == 6 || rand_num == 7) { if(br == 2) return 1; }
						else if(rand_num == 3) { if(br == 1) return 1; }
						else if(br == 3) return 1;
						return 0;
						break;
						
					case 2:
						if(rand_num == 1) { if(br == 3) return 1; }
						else if(br == 2) return 1;
						return 0;
						break;
				}
				break;
	}
}

void rotate_shape(playfield matrix, int rand_num, int *start_pos, int current_row, int *shape_state, int *shape_len, int *shape_hight,int *state_ind, int *temp1_shape, int *temp2_shape, int *temp1, int *temp2)
{
	int i,j,ind = 0;
	
	switch(*shape_state)
	{
		case 1:	
				for(i = *start_pos; i < *start_pos+*shape_len; i++)
				{
					if(temp1[ind] != EMPTY) matrix[current_row][i] = EMPTY;
					if(temp2[ind] != EMPTY) matrix[current_row+1][i] = EMPTY;
					ind++;
				}
				
				ind = 0;
				for(i = current_row; i < current_row+*shape_len; i++)
				{
					if(temp1_shape[ind] != EMPTY) matrix[i][*start_pos] = rand_num;
					if(temp2_shape[ind] != EMPTY) matrix[i][*start_pos+1] = rand_num;
					ind++;
				}
				
				*shape_state = 2;
			break;
			
		case 2:
				for(i = current_row; i < current_row+*shape_len; i++)
				{
					if(temp1[ind] != EMPTY) matrix[i][*start_pos] = EMPTY;
					if(temp2[ind] != EMPTY) matrix[i][*start_pos+1] = EMPTY;
					ind++;
				}
				
				ind = *shape_len-1;
				for(i = *start_pos+*shape_len-1; i >= *start_pos; i--)
				{
					if(temp1_shape[ind] != EMPTY) matrix[current_row][i] = rand_num;
					if(temp2_shape[ind] != EMPTY) matrix[current_row+1][i] = rand_num;
					ind--;
				}
				
				if(*state_ind == 1) *state_ind = 2;
				else *state_ind = 1;
				
				*shape_state = 1;
			break;	
	}
}

int move_detect(playfield matrix, int side, int rand_num, int start_pos, int current_row, int shape_state, int shape_len, int shape_hight, int *temp1_shape, int *temp2_shape)
{
	int i, br = 0, ind = 0, ind_below = 0; // ind_below -> indicates when active figure is positioned under another block on the playfield
	
	switch(side)
	{
		case 1: //Left
			switch(shape_state)
			{
				case 1:
					for(i = start_pos; i < start_pos+shape_len; i++)
					{
						if(temp1_shape[ind] != EMPTY)
						{
							if(matrix[current_row][i-1] == EMPTY) br++;
							if(i == start_pos && matrix[current_row-1][i-1] != EMPTY) ind_below = 1;
						}
						
						if(temp2_shape[ind] != EMPTY)
						{
							if(matrix[current_row+1][i-1] == EMPTY) br++;
							if(i == start_pos && temp1_shape[ind] == EMPTY && matrix[current_row][i-1] != EMPTY) ind_below = 1;
						}
						
						ind++;
					}
					
					if(br == shape_hight && ind_below == 1) return -1;
					
					if(rand_num == 1)
					{
						if(br == 1) return 1;
					}
					
					else if(br == shape_hight) 
					{
						return 1;
					}
					
					return 0;
					break;
					
				case 2:
					for(i = current_row; i < current_row+shape_len; i++)
					{
						if(temp1_shape[ind] != EMPTY)
						{
							if(matrix[i][start_pos-1] == EMPTY) br++;
							if(matrix[i-1][start_pos-1] != EMPTY) ind_below = 1;
						}
						
						if(temp2_shape[ind] != EMPTY)
						{
							if(matrix[i][start_pos] == EMPTY) br++;
						}
						
						ind++;
					}
					
					if(br == shape_len && ind_below == 1) return -1;
					else if(br == shape_len) return 1;
					else return 0;
					break;
			}
			break;
			
		case 2: // Right
			switch(shape_state)
			{
				case 1:
					ind = shape_len-1;
					
					for(i = start_pos+shape_len-1; i >= start_pos; i--)
					{
						if(temp1_shape[ind] != EMPTY)
						{
							if(matrix[current_row][i+1] == EMPTY) br++;
							if(i == start_pos+shape_len-1 && matrix[current_row-1][i+1] != EMPTY) ind_below = 1;
						}
						
						if(temp2_shape[ind] != EMPTY)
						{
							if(matrix[current_row+1][i+1] == EMPTY) br++;
							if(i == start_pos+shape_len-1 && temp1_shape[ind] == EMPTY && matrix[current_row][i+1] != EMPTY) ind_below = 1;
						}
						
						ind--;
					}
					
					if(br == shape_hight && ind_below == 1) return -1;
					
					if(rand_num == 1)
					{
						if(br == 1) return 1;
					}
					
					else if(br == shape_hight) 
					{
						return 1;
					}
					
					return 0;
					break;
					
				case 2:
					for(i = current_row; i < current_row+shape_len; i++)
					{
						if(temp1_shape[ind] != EMPTY)
						{
							if(matrix[i][start_pos+1] == EMPTY) br++;
						}
						
						if(temp2_shape[ind] != EMPTY)
						{
							if(matrix[i][start_pos+2] == EMPTY) br++;
							if(matrix[i-1][start_pos+2] != EMPTY) ind_below = 1;
						}
						
						ind++;
					}
					
					if(br == shape_len && ind_below == 1) return -1;
					
					if(br == shape_len) return 1;
					else return 0;
					break;
			}
			break;
	}
}

int move_shape(playfield matrix, int side, int rand_num, int *start_pos, int *current_row, int *shape_state, int *shape_len, int *shape_hight, int *temp1_shape, int *temp2_shape)
{
	int i,j,move_det,ind = 0;
	
	move_det = move_detect(matrix,side,rand_num,*start_pos,*current_row,*shape_state,*shape_len,*shape_hight,temp1_shape,temp2_shape);
	if(move_det == 0) return move_det;
	
	switch(side)
	{
		case 1: // Left
			switch(*shape_state)
			{
				case 1:
					for(i = *start_pos; i < *start_pos+*shape_len; i++)
					{
						if(temp1_shape[ind] != EMPTY)
						{
							matrix[*current_row][i-1] = matrix[*current_row][i];
							matrix[*current_row][i] = EMPTY;
						}
						
						if(temp2_shape[ind] != EMPTY)
						{
							matrix[*current_row+1][i-1] = matrix[*current_row+1][i];
							matrix[*current_row+1][i] = EMPTY;
						}
						
						ind++;
					}
					
					*start_pos = *start_pos-1;
					break;
					
				case 2:
					for(i = *current_row; i < *current_row+*shape_len; i++)
					{
						if(temp1_shape[ind] != EMPTY)
						{
							matrix[i][*start_pos-1] = matrix[i][*start_pos];
							matrix[i][*start_pos] = EMPTY; 
						}
						
						if(temp2_shape[ind] != EMPTY)
						{
							matrix[i][*start_pos] = matrix[i][*start_pos+1];
							matrix[i][*start_pos+1] = EMPTY; 
						}
						
						ind++;
					}
					
					*start_pos = *start_pos-1;
					break;
			}
			break;
			
		case 2: // Right
			switch(*shape_state)
			{
				case 1:
					if(*start_pos+*shape_len >= COL) return 0;
					ind = *shape_len-1;
					
					for(i = *start_pos+*shape_len-1; i >= *start_pos ; i--)
					{
						if(temp1_shape[ind] != EMPTY)
						{
							matrix[*current_row][i+1] = matrix[*current_row][i];
							matrix[*current_row][i] = EMPTY;
						}
						
						if(temp2_shape[ind] != EMPTY)
						{
							matrix[*current_row+1][i+1] = matrix[*current_row+1][i];
							matrix[*current_row+1][i] = EMPTY;
						}
						
						ind--;
					}
					
					*start_pos = *start_pos+1;
					break;
					
				case 2:
					if(*start_pos+*shape_hight >= COL) return 0;
					
					for(i = *current_row; i < *current_row+*shape_len; i++)
					{
						if(temp2_shape[ind] != EMPTY)
						{
							matrix[i][*start_pos+2] = matrix[i][*start_pos+1];
							matrix[i][*start_pos+1] = EMPTY; 
						}
						
						if(temp1_shape[ind] != EMPTY)
						{
							matrix[i][*start_pos+1] = matrix[i][*start_pos];
							matrix[i][*start_pos] = EMPTY; 
						}
						
						ind++;
					}
					
					*start_pos = *start_pos+1;
					break;
			}	
			break;
	}
	
	return move_det;
}

int movement_detect(playfield matrix, int start_pos, int current_row, int shape_state, int shape_len, int shape_hight, int *temp1_shape, int *temp2_shape)
{
	int i, ind = 0, br = 0;
	
	switch(shape_state)
	{
		case 1:
			for(i = start_pos; i < start_pos+shape_len; i++)
			{
				if(temp2_shape[ind] != EMPTY)
				{
					if(matrix[current_row+shape_hight][i] == EMPTY) br++;
				}
				
				else if(matrix[current_row+1][i] == EMPTY) br++;
				
				ind++;
			}
			
			if(br == shape_len) return 1;
			else return 0;
			break;
			
		case 2:
			ind = shape_len-1;
			
			if(temp1_shape[ind] != EMPTY && temp2_shape[ind] != EMPTY)
			{
				if(matrix[current_row+shape_len][start_pos] == EMPTY) br++;
				if(matrix[current_row+shape_len][start_pos+1] == EMPTY) br++;
			}
			
			else
			{
				if(temp1_shape[ind] != EMPTY)
				{
					if(matrix[current_row+shape_len][start_pos] == EMPTY) br++;
					
					for(i = current_row+shape_len; i > current_row; i--)
					{
						if(temp2_shape[ind] != EMPTY)
						{
							if(matrix[i][start_pos+1] == EMPTY) br++;
							break;
						}
						
						else ind--;
					}
				}
				
				else if(temp2_shape[ind] != EMPTY)
				{
					if(matrix[current_row+shape_len][start_pos+1] == EMPTY) br++;
					
					for(i = current_row+shape_len; i > current_row; i--)
					{
						if(temp1_shape[ind] != EMPTY)
						{
							if(matrix[i][start_pos] == EMPTY) br++;
							break;
						}
						
						else ind--;
					}
				}
			}
			
			if(br == shape_hight) return 1;
			else return 0;
			break;
	}		
}

int movement(playfield matrix, int *start_pos, int current_row, int *shape_state, int *shape_len, int *shape_hight, int rand_num, int *temp1_shape, int *temp2_shape)
{
	int i,j,movement_det,ind = 0;
	
	movement_det = movement_detect(matrix,*start_pos,current_row,*shape_state,*shape_len,*shape_hight,temp1_shape,temp2_shape);
	if((*shape_state == 1 && current_row+*shape_hight == ROW) || (*shape_state == 2 && current_row+*shape_len == ROW) || movement_det == 0) return 0; // Stop if active figure hits the playfield bottom
	
	switch(*shape_state)
	{
		case 1:
			for(j = *start_pos; j < *start_pos+*shape_len; j++)
			{
				if(temp2_shape[ind] != EMPTY)
				{
					matrix[current_row+*shape_hight][j] = matrix[current_row+*shape_hight-1][j];
					if(temp1_shape[ind] == EMPTY) matrix[current_row+*shape_hight-1][j] = EMPTY;
				}
				
				if(temp1_shape[ind] != EMPTY)
				{
					matrix[current_row+1][j] = matrix[current_row][j];
					matrix[current_row][j] = EMPTY;
				}
				ind++;
			}	
			break;
			
		case 2:
			ind = *shape_len-1;
			
			for(i = current_row+*shape_len; i > current_row; i--)
			{
				if(temp2_shape[ind] != EMPTY)
				{
					matrix[i][*start_pos+1] = matrix[i-1][*start_pos+1];
					if(temp2_shape[ind-1] == EMPTY) matrix[i-1][*start_pos+1] = EMPTY;
				}
				
				if(temp1_shape[ind] != EMPTY)
				{
					matrix[i][*start_pos] = matrix[i-1][*start_pos];
					if(temp1_shape[ind-1] == EMPTY) matrix[i-1][*start_pos] = EMPTY;
					
				}
				ind--;
			}
			
			if(temp2_shape[0] != EMPTY) matrix[current_row][*start_pos+1] = EMPTY;
			if(temp1_shape[0] != EMPTY) matrix[current_row][*start_pos] = EMPTY;
			break;
	}
	
	return movement_det;
}

int temp_shape_generate(int *temp1_shape, int *temp2_shape, playfield matrix, int current_row, int start_pos, int shape_len)
{
	int i,ind = 0, count = 0;
	
	for(i = start_pos; i < start_pos+shape_len; i++)
	{
		if(matrix[current_row][i] != EMPTY) count++; 
		if(matrix[current_row+1][i] != EMPTY) count++;
		
		temp1_shape[ind] = matrix[current_row][i];
		temp2_shape[ind] = matrix[current_row+1][i];
		ind++; 
	}
	
	if(count != SHAPE_BLOCKS_MAX) return 0; // when matrix is filled up to the last_row this indicates when to stop
	
	return 1;
}

void temp_shape_rotate(int *temp1_shape, int *temp2_shape, int shape_len, int temp_shape_rotate_ind)
{
	int i; int tmp;
	
	switch(temp_shape_rotate_ind)
	{
		case 1:
			for(i = 0; i < shape_len; i++)
			{
				tmp = temp1_shape[i];
				temp1_shape[i] = temp2_shape[i];
				temp2_shape[i] = tmp;
			}
			break;
			
		case 2:
			tmp = temp1_shape[shape_len-1];
			temp1_shape[shape_len-1] = temp1_shape[0];
			temp1_shape[0] = tmp;
			
			tmp = temp2_shape[shape_len-1];
			temp2_shape[shape_len-1] = temp2_shape[0];
			temp2_shape[0] = tmp;
			break;
	}
}

void temp_rotate(int *temp1, int *temp2, int *temp1_shape , int *temp2_shape, int shape_len)
{
	for(int i = 0; i < shape_len; i++)
	{
		temp1[i] = temp1_shape[i];
		temp2[i] = temp2_shape[i];
	}
}

int check_this_line(playfield matrix, int line)
{
	int j,br = 0;
	 
	for(j = 0; j < COL; j++)
	{
		if(matrix[line][j] != EMPTY) br++;
	}
	
	return br;
}

void clear_line(playfield matrix, int lower_limit, int upper_limit)
{
	int i,j;
	
	for(i = lower_limit; i > upper_limit; i--)
	{
		for(j = 0; j < COL; j++) matrix[i][j] = EMPTY;
	}
}

void move_lines(playfield matrix, int lower_limit, int upper_limit)
{
	int j,br;
	
	br = check_this_line(matrix,upper_limit);

	while(br != 0)
	{
		for(j = 0; j < COL; j++)
		{
			matrix[lower_limit][j] = matrix[upper_limit][j];
			matrix[upper_limit][j] = EMPTY;
		}
		
		lower_limit--; upper_limit--;
		br = check_this_line(matrix,upper_limit);
	}	
}

int score_num(int level, int gap)
{
	int score;
	
	switch(gap)
	{
		case 1:
			score = 40*(level+1);
			break;
			
		case 2:
			score = 100*(level+1);
			break;
			
		case 3:
			score = 300*(level+1);
			break;
			
		case 4:
			score = 1200*(level+1);
			break;
	}
	
	return score;
}

void line_checker(playfield matrix, int rand_num, int shape_len, int shape_hight, int shape_state, int current_row, int level, int *lines, int *score, WINDOW *border, WINDOW *win)
{
	int i,len,br = 0,ind = 0;
	int lower_limit = -1, upper_limit = -1;
	
	if(shape_state == 1) len = shape_hight;
	else len = shape_len;
	
	for(i = current_row+len-1; i >= current_row-1; i--)
	{	
		br = check_this_line(matrix,i); // if line is full br == COL
		
		if(lower_limit == -1 && br == COL) 
		{
			lower_limit = i;
			upper_limit = i-1;
			ind = 1;
			continue;
		}
			
		if(br != COL && ind == 1)
		{	
			int gap;
			
			upper_limit = i;
			gap = lower_limit-upper_limit;
			
			*lines += gap; //Line counter
			*score += score_num(level,gap); // Score counter
			
			clear_line(matrix,lower_limit,upper_limit);
			matrix_print(matrix,win);
			delay(200);
			
			move_lines(matrix,lower_limit,upper_limit); i++;
			delay(200);
			
			lower_limit = -1;
			upper_limit = -1;
			ind = 0;
		} 
	}
}

void win_pause(int flag)
{
	WINDOW * win_pause_border = newwin(5,ROW-7,Y+9,X-1);
	WINDOW * win_pause = subwin(win_pause_border,3,ROW-9,Y+10,X);
	
	wborder(win_pause_border, 0, 0, 0, 0, 0, 0, 0, 0); 
	
	wbkgd(win_pause_border, COLOR_PAIR(8));
	wbkgd(win_pause, COLOR_PAIR(11));
	
	wattron(win_pause,A_BLINK);
	mvwprintw(win_pause,1,3,"PAUSED");
	
	if(flag)
	{
		wrefresh(win_pause_border);
		wrefresh(win_pause);
	}
	
	else
	{
		wclear(win_pause_border);
		wclear(win_pause);
		wrefresh(win_pause_border);
		wrefresh(win_pause);	
	}
}

void win_game_over(int flag, char string[])
{
	WINDOW * win_game_over_border = newwin(5,ROW-7,Y+9,X-1);
	WINDOW * win_game_over = subwin(win_game_over_border,3,ROW-9,Y+10,X);
	
	wborder(win_game_over_border, 0, 0, 0, 0, 0, 0, 0, 0); 
	
	wbkgd(win_game_over_border, COLOR_PAIR(8));
	wbkgd(win_game_over, COLOR_PAIR(11));
	
	wattron(win_game_over,A_BLINK);
	mvwprintw(win_game_over,1,1,string);
	
	if(flag)
	{
		wrefresh(win_game_over_border);
		wrefresh(win_game_over);
	}
	
	else
	{
		wclear(win_game_over_border);
		wclear(win_game_over);
		wrefresh(win_game_over_border);
		wrefresh(win_game_over);	
	}
}

int read_players(struct player players[])
{
	FILE *f = fopen("tetris_scoreboard.txt","r");
	
	if(f == NULL)
	{
		return 1;	
	}
	
	int i = 0;
	while(!feof(f))
	{
		fscanf(f,"%s ", players[i].name);
		fscanf(f,"%d", &players[i].score);
		i++;
	}
	
	fclose(f);
	
	return 0;
}

void write_players(struct player players[])
{
	FILE *f = fopen("tetris_scoreboard.txt","w");
	
	int i = 0;
	while(players[i].score != -1 && i != SCOREBOARD_MAX)
	{
		fprintf(f,"%s ", players[i].name);
		fprintf(f,"%d", players[i].score);
		fprintf(f,"\n");
		i++;
	}
	
	fclose(f);
}

void sort_players_arr(struct player players[], int score, char tmp[])
{
	for(int i = 0; i < SCOREBOARD_MAX; i++)
	{
		if(players[i].score == -1)
		{
			strcpy(players[i].name,tmp);
			players[i].score = score;
			break;
		}
		
		else if(players[i].score < score)
		{
			for(int j = SCOREBOARD_MAX-2; j >= i; j--)
			{
				players[j+1] = players[j];
			}
			
			strcpy(players[i].name,tmp);
			players[i].score = score;
			break;
		}
	}
}

void win_enter_name(struct player players[], int score, int flag)
{
	WINDOW * win_enter_name_border = newwin(6,ROW-11,Y+9,X+1);
	WINDOW * win_enter_name = subwin(win_enter_name_border,4,ROW-13,Y+10,X+2);
	
	wborder(win_enter_name_border, 0, 0, 0, 0, 0, 0, 0, 0); 
	
	wbkgd(win_enter_name_border, COLOR_PAIR(8));
	wbkgd(win_enter_name, COLOR_PAIR(11));
	
	wrefresh(win_enter_name_border);
	mvwprintw(win_enter_name,1,1,":Name:");
	
	if(flag)
	{
		char tmp[10];
		
		echo(); curs_set(1);
		
		wmove(win_enter_name,3,1);
		wgetstr(win_enter_name,tmp);
		
		sort_players_arr(players,score,tmp);
		write_players(players);
		
		curs_set(0); delay(200); noecho();
	}
	
	else
	{
		wclear(win_enter_name_border);
		wclear(win_enter_name);
		wrefresh(win_enter_name_border);
		wrefresh(win_enter_name);	
	}
}

void main_menu_print(WINDOW *win_esc_menu, char string[], int s_start, int s_end, int x)
{
	int n = 10;
	int i,ind = 0;
	
	for(i = 1; i < n+1; i++) 
	{
		if(i < s_start || i > s_end) mvwaddch(win_esc_menu, x, i, ' ' | COLOR_PAIR(9));
		else { mvwaddch(win_esc_menu, x, i, string[ind] | COLOR_PAIR(9)); ind++; }	
	}
}

int win_skin_menu(WINDOW *win_border, WINDOW *win)
{
	WINDOW * win_esc_menu_border = newwin(8,ROW-7,Y+7,X-1);
	WINDOW * win_esc_menu = subwin(win_esc_menu_border,6,ROW-9,Y+8,X);
	wborder(win_esc_menu_border, 0, 0, 0, 0, 0, 0, 0, 0); 
	
	wbkgd(win_esc_menu_border, COLOR_PAIR(8));
	wbkgd(win_esc_menu, COLOR_PAIR(11));
	
	wrefresh(win_esc_menu_border);
	wrefresh(win_esc_menu);
	
	int c = 0,flag = 0;
	
	while(c != ESC)
	{
		c = wgetch(win);
		
		if(c == KEY_DOWN && flag < 3 ) flag++;
		else if(c == KEY_UP && flag > 0) flag--;
		
		if(flag == 0)
		{
			wbkgd(win_esc_menu, COLOR_PAIR(11));
			main_menu_print(win_esc_menu,"ASCII ORIG", 1,10,1);
			mvwprintw(win_esc_menu,2,1,"ASCII RICH");
			mvwprintw(win_esc_menu,3,1,"SHAPE ORIG");
			mvwprintw(win_esc_menu,4,4,"BACK");
		}
		
		if(flag == 1)
		{
			wbkgd(win_esc_menu, COLOR_PAIR(11));
			mvwprintw(win_esc_menu,1,1,"ASCII ORIG");
			main_menu_print(win_esc_menu,"ASCII RICH", 1,10,2);
			mvwprintw(win_esc_menu,3,1,"SHAPE ORIG");
			mvwprintw(win_esc_menu,4,4,"BACK");
		}
		
		if(flag == 2)
		{
			wbkgd(win_esc_menu, COLOR_PAIR(11));
			mvwprintw(win_esc_menu,1,1,"ASCII ORIG");
			mvwprintw(win_esc_menu,2,1,"ASCII RICH");
			main_menu_print(win_esc_menu,"SHAPE ORIG", 1,10,3);
			mvwprintw(win_esc_menu,4,4,"BACK");
		}
		
		if(flag == 3)
		{
			wbkgd(win_esc_menu, COLOR_PAIR(11));
			mvwprintw(win_esc_menu,1,1,"ASCII ORIG");
			mvwprintw(win_esc_menu,2,1,"ASCII RICH");
			mvwprintw(win_esc_menu,3,1,"SHAPE ORIG");
			main_menu_print(win_esc_menu,"BACK", 4,7,4);
		}
		
		if(c == ENTER_CR || c == ENTER_LF)
		{
			if(flag == 0) color(2);
			if(flag == 1) color(1);
			if(flag == 2) color(0);
			if(flag == 3) 
			{
				wclear(win_esc_menu_border);
				wclear(win_esc_menu);
				break;
			}
		}
		
		wrefresh(win_esc_menu_border);
		wrefresh(win_esc_menu);
	}
}

int win_main_menu(WINDOW *win_border, WINDOW *win, int ind)
{
	WINDOW * win_main_menu_border = newwin(8,ROW-7,Y+7,X-1);
	WINDOW * win_main_menu = subwin(win_main_menu_border,6,ROW-9,Y+8,X);
	wborder(win_main_menu_border, 0, 0, 0, 0, 0, 0, 0, 0); 
	
	wbkgd(win_main_menu_border, COLOR_PAIR(8));
	wbkgd(win_main_menu, COLOR_PAIR(11));
	
	int c = 0,flag = 0;
	
	while(c != ESC || ind) // ind -> not to begin game when pressing esc in main menu
	{
		c = wgetch(win);
		
		if(c == KEY_DOWN && flag < 2 ) flag++;
		else if(c == KEY_UP && flag > 0) flag--;
		
		if(flag == 0)
		{
			wbkgd(win_main_menu, COLOR_PAIR(11));
			if(ind) main_menu_print(win_main_menu,"START GAME", 1,10,1);
			else main_menu_print(win_main_menu,"RESUME", 3,8,1);
			mvwprintw(win_main_menu,2,2,"SET SKIN");
			if(ind) mvwprintw(win_main_menu,4,4,"EXIT");
			else mvwprintw(win_main_menu,4,4,"MENU");
		}
		
		if(flag == 1)
		{
			wbkgd(win_main_menu, COLOR_PAIR(11));
			if(ind) mvwprintw(win_main_menu,1,1,"START GAME");
			else mvwprintw(win_main_menu,1,3,"RESUME");
			main_menu_print(win_main_menu,"SET SKIN", 2,9,2);
			if(ind) mvwprintw(win_main_menu,4,4,"EXIT");
			else mvwprintw(win_main_menu,4,4,"MENU");
		}
		
		if(flag == 2)
		{
			wbkgd(win_main_menu, COLOR_PAIR(11));
			if(ind) mvwprintw(win_main_menu,1,1,"START GAME");
			else mvwprintw(win_main_menu,1,3,"RESUME");
			mvwprintw(win_main_menu,2,2,"SET SKIN");
			if(ind) main_menu_print(win_main_menu,"EXIT", 4,7,4);
			else main_menu_print(win_main_menu,"MENU", 4,7,4);
		}
		
		if(c == ENTER_CR || c == ENTER_LF)
		{
			if(flag == 0) break;
			if(flag == 1)
			{
				win_skin_menu(win_border,win);
				wborder(win_border, 0, 0, 0, 0, 0, 0, 0, 0);
			}
			if(flag == 2) return 0;
		}
		
		wrefresh(win_main_menu_border);
		wrefresh(win_main_menu);
	}
	
	wclear(win_main_menu_border);
	wclear(win_main_menu);
	wrefresh(win_main_menu_border);
	wrefresh(win_main_menu);
	
	wborder(win_border, 0, 0, 0, 0, 0, 0, 0, 0); 
	wrefresh(win_border);
	
	return 1;
}

void print_stats_num(WINDOW *win_stats, int color, int num, int x, int y)
{
	int i = 0;
	char tmp[10];
	
	if(num)
	{
		while(num)
		{
			tmp[i++] = '0' + num%10;
			num /= 10;
		}
	}
	
	else tmp[i++] = '0';
	
	tmp[i--] = '\0';
	
	while(i >= 0)
	{
		mvwaddch(win_stats, x, y, tmp[i] | COLOR_PAIR(color));	
		y++; i--;
	}
}

void win_stats(int score, int level, int lines)
{
	WINDOW * win_stats_border = newwin(9,17,Y+3,X-18); // newwin(9, 17, Y+14, X+COL+3)
	WINDOW * win_stats = subwin(win_stats_border,7,15,Y+4,X-17);  
	wborder(win_stats_border, 0, 0, 0, 0, 0, 0, 0, 0);
	
	keypad(win_stats, TRUE);
	nodelay(win_stats, TRUE);
	
	wbkgd(win_stats, COLOR_PAIR(9));
	wbkgd(win_stats_border, COLOR_PAIR(8));
	
	wrefresh(win_stats_border);	
	
	mvwprintw(win_stats, 1, 1, "Score: "); mvwaddch(win_stats, 1, 8, '0' | COLOR_PAIR(10)); print_stats_num(win_stats, 10, score, 1, 8);
	mvwprintw(win_stats, 3, 1, "Level: "); mvwaddch(win_stats, 3, 8, '0' | COLOR_PAIR(10)); print_stats_num(win_stats, 10, level, 3, 8);
	mvwprintw(win_stats, 5, 1, "Lines: "); mvwaddch(win_stats, 5, 8, '0' | COLOR_PAIR(10)); print_stats_num(win_stats, 10, lines, 5, 8);
	wrefresh(win_stats);
}

int tetris_main(playfield matrix, WINDOW *border, WINDOW *win, struct player players[])
{
	static int state_ind,temp_shape_rotate_ind; // must be initialized to 1 at the beginning of each round because of rotate_detection() function
	static int lines = 0, level = 0, score = 0, speed = START_SPEED;
	static int win_menu_ind = 1;
	int i, c = 1, flag = 1, end_game_ind;
	int rand_num, start_pos, shape_len = 0, shape_hight; 
	int shape_state = 1, current_row = START_ROW; // shape_state (one of four possible states of rotation) -> horizontal = 1, vertical = 2
	int side; // Side (arrow) -> left = 1, right = 2
	int temp1_shape[4],temp2_shape[4];
	int game_over_status;
	
	state_ind = 1;
	temp_shape_rotate_ind = 1;
	
	if(win_menu_ind)
	{
		if(win_main_menu(border,win,1) == 0) return 0;
		win_menu_ind = 0;
	}
	
	game_over_status = generate_shapes(matrix,&start_pos,&rand_num,&shape_len,&shape_hight);
	flag = temp_shape_generate(temp1_shape,temp2_shape,matrix,current_row,start_pos,shape_len);
	
	matrix_print(matrix,win);
	
	if(flag == 0) delay(speed); // when the matrix is filled up to the last_row and temp_shape_generate try to collect pieces from the field for current figure flag becomes 0 but this delay allows player to see last generated shape and not to show the next one immediately
	
	if(game_over_status) // Game Over
	{	
		if(score >= MAX_SCORE) // If maximum (999999) score is reached
		{
			score = MAX_SCORE;
			matrix_print(matrix,win);
			
			win_game_over(1,"IMPOSSIBLE");
			delay(5000);
			win_game_over(0,"IMPOSSIBLE");
		}
		
		else
		{
			win_game_over(1,"GAME OVER!");
			delay(5000);
			win_game_over(0,"GAME OVER!");
		}
		
		wborder(border, 0, 0, 0, 0, 0, 0, 0, 0); 
		wrefresh(border);
		
		while(c != ERR) // Collect all characters before entering the player name
		{
			c = wgetch(win);
		}
		
		win_enter_name(players,score,1);
		win_enter_name(players,0,0);
		
		wborder(border, 0, 0, 0, 0, 0, 0, 0, 0); 
		wrefresh(border);
		
		lines = 0;
		level = 0;
		score = 0;
		speed = START_SPEED;
		win_stats(score,level,lines);
		win_menu_ind = 1;
		
		return -1;
	}
	
	struct timespec start,end; // struct timespec from <time.h> library
	double time = 0;
	
	while(current_row != ROW-1 && flag == 1)
	{
		double interval; // Time interval between start and end of the while loop
		int ind = 0;
		
		clock_gettime(CLOCK_REALTIME,&start);
		
		c = wgetch(win);
		
		if(c == KEY_UP && rand_num != 2) // rand_num != 2 because O figure does not rotate
		{
			int temp1[4],temp2[4];
			int rotate_det;
			
			temp_rotate(temp1,temp2,temp1_shape,temp2_shape,shape_len); //Does the same as the lower temp_rotate, it is only needed in advance to check the conditions
			if(rand_num != 1) temp_shape_rotate(temp1,temp2,shape_len,temp_shape_rotate_ind);
			
			rotate_det = rotate_detection(matrix,rand_num,start_pos,current_row,shape_state,shape_len,shape_hight,&state_ind,temp1,temp2);
			
			if(rotate_det)
			{	
				temp_rotate(temp1,temp2,temp1_shape,temp2_shape,shape_len);
				
				if(rand_num != 1) temp_shape_rotate(temp1_shape,temp2_shape,shape_len,temp_shape_rotate_ind);
				rotate_shape(matrix,rand_num,&start_pos,current_row,&shape_state,&shape_len,&shape_hight,&state_ind,temp1_shape,temp2_shape,temp1,temp2);
				
				if(temp_shape_rotate_ind == 1) temp_shape_rotate_ind = 2;
				else temp_shape_rotate_ind = 1;
			}
		}
		
		if(c == SPACE) // Drop figure
		{
			while(flag && current_row != ROW-1)
			{
				flag = movement(matrix,&start_pos,current_row,&shape_state,&shape_len,&shape_hight,rand_num,temp1_shape,temp2_shape);
				if(current_row == ROW-1 || flag != 1) break;
				current_row++;
			}
		}
		
		if(c == KEY_LEFT) // Move left
		{
			side = 1;
			if(start_pos-1 >= 0) ind = move_shape(matrix,side,rand_num,&start_pos,&current_row,&shape_state,&shape_len,&shape_hight,temp1_shape,temp2_shape); //PROVERI DA LI JE POTREBNO DA VRACA NESTOOO!!!!!!!
		}
		
		if(c == KEY_RIGHT) // Move right
		{
			side = 2;
			ind = move_shape(matrix,side,rand_num,&start_pos,&current_row,&shape_state,&shape_len,&shape_hight,temp1_shape,temp2_shape);
		}
		
		if(c == KEY_DOWN) // Skip one field
		{
			flag = movement(matrix,&start_pos,current_row,&shape_state,&shape_len,&shape_hight,rand_num,temp1_shape,temp2_shape);
			if(flag != 1) break;
			
			current_row++; 
		}
		
		if(c == 'p' || c == 'P') // Pause game
		{
			win_pause(1);
			c = 0;
			
			while(c != 'p' && c != 'P')
			{	
				c = wgetch(win);
			}
			
			win_pause(0);
			wborder(border, 0, 0, 0, 0, 0, 0, 0, 0); 
			wrefresh(border);
		}
		
		if(c == ESC) // Esc menu
		{
			if(win_main_menu(border,win,0) == 0)
			{
				lines = 0;
				level = 0;
				score = 0;
				speed = START_SPEED;
				win_stats(score,level,lines);
				win_menu_ind = 1;
				
				return -1;
			}
		}
		
		clock_gettime(CLOCK_REALTIME,&end);
	
		interval = end.tv_nsec - start.tv_nsec;

		if (start.tv_nsec > end.tv_nsec) // clock underflow
		{  
			interval += GIGA; 
		}
		
		interval *= MICRO*10;
		time += interval;
		
		if(time > speed)
		{
			flag = movement(matrix,&start_pos,current_row,&shape_state,&shape_len,&shape_hight,rand_num,temp1_shape,temp2_shape);
			if(flag == 0) break;
			
			current_row++; time = 0;
		}
			
		matrix_print(matrix,win);
	}
	
	line_checker(matrix,rand_num,shape_len,shape_hight,shape_state,current_row,level,&lines,&score,border,win);
	if((level+1)*10 < lines) // Level checker
	{ 
		level++; 
		if(speed > 30) speed = speed - 50; // Decrease speed if the next level is reached
		else if(speed > 10) speed = speed - 10;
	} 
	
	win_stats(score,level,lines);
	
	return 1;
}

void win_tetris(WINDOW * win_border, WINDOW * win_main)
{
	wborder(win_border, 0, 0, 0, 0, 0, 0, 0, 0);
	
	keypad(win_main, TRUE);
	nodelay(win_main, TRUE);
	cbreak();
	
	wmove(win_main,0,0);
	
    wclear(win_main);
    wrefresh(win_border);
}

void win_help(void)
{
	WINDOW * win_help_border = newwin(11, 17, Y+12, X+COL+3);
	WINDOW * win_help = subwin(win_help_border,9,15,Y+13,X+COL+4);
	
	wborder(win_help_border, 0, 0, 0, 0, 0, 0, 0, 0);
	
	wbkgd(win_help_border, COLOR_PAIR(8));
	wbkgd(win_help, COLOR_PAIR(9));
	
	mvwprintw(win_help, 1, 1, "Right"); mvwprintw(win_help, 1, 9, "r_arr");
	mvwprintw(win_help, 2, 1, "Left"); mvwprintw(win_help, 2, 9, "l_arr");
	mvwprintw(win_help, 3, 1, "Rotate"); mvwprintw(win_help, 3, 9, "u_arr");
	mvwprintw(win_help, 4, 1, "Skip"); mvwprintw(win_help, 4, 9, "d_arr");
	mvwprintw(win_help, 5, 1, "Pause"); mvwprintw(win_help, 5, 9, "p_key");
	mvwprintw(win_help, 6, 1, "Drop"); mvwprintw(win_help, 6, 9, "space");
	mvwprintw(win_help, 7, 1, "Menu"); mvwprintw(win_help, 7, 9, "esc");
	
	wrefresh(win_help_border);
	wrefresh(win_help);
}

int print_name(WINDOW *win_scoreboard, char *name, int x, int y)
{
	int len;
	
	len = strlen(name)+2;
	
	for(int j = 2; j < len; j++)
	{
		mvwaddch(win_scoreboard, x, j, name[j-2] | COLOR_PAIR(13));
	}
	
	mvwaddch(win_scoreboard, x, len, '|' | COLOR_PAIR(9));
	
	return len+1;
}

void win_scoreboard(struct player players[])
{
	WINDOW * win_scoreboard_border = newwin(11,17,Y+15,X-18);
	WINDOW * win_scoreboard = subwin(win_scoreboard_border,9,15,Y+16,X-17);
	
	wborder(win_scoreboard_border, 0, 0, 0, 0, 0, 0, 0, 0); 
	
	wbkgd(win_scoreboard_border, COLOR_PAIR(8));
	wbkgd(win_scoreboard, COLOR_PAIR(9));
	
	int x = 0;
	for(int i = 0; i < SCOREBOARD_MAX; i++)
	{
		if(players[i].score != -1)
		{
			mvwaddch(win_scoreboard, x, 0, '1'+i | COLOR_PAIR(9)); mvwaddch(win_scoreboard, x, 1, ')' | COLOR_PAIR(9)); 
			int len = print_name(win_scoreboard,players[i].name,x,8-strlen(players[i].name)); print_stats_num(win_scoreboard,13, players[i].score, x, len);
			x+=2;
		}
	}
	
	wrefresh(win_scoreboard_border);
	wrefresh(win_scoreboard);
}

void win_names_print(WINDOW *win_border, WINDOW *win, int color1, int color2, char string[], int x, int y)
{
	wborder(win_border, 0, 0, 0, 0, 0, 0, 0, 0); 
	
	wbkgd(win_border, COLOR_PAIR(color1));
	wbkgd(win, COLOR_PAIR(color2));
	
	mvwprintw(win, x, y, string);
	
	wrefresh(win_border);
	wrefresh(win);
}

void win_names(void)
{
	WINDOW * win_help_name_border = newwin(3,17,Y+9,X+COL+3);
	WINDOW * win_help_name = subwin(win_help_name_border,1,15,Y+10,X+COL+4);
	win_names_print(win_help_name_border,win_help_name,8,9,"HELP?",0,5);
	
	WINDOW * win_next_name_border = newwin(3,17,Y,X+COL+3);
	WINDOW * win_next_name = subwin(win_next_name_border,1,15,Y+1,X+COL+4);
	win_names_print(win_next_name_border,win_next_name,8,9,"NEXT FIGURE",0,2);
	
	WINDOW * win_scoreboard_name_border = newwin(3,17,Y+12,X-18);
	WINDOW * win_scoreboard_name = subwin(win_scoreboard_name_border,1,15,Y+13,X-17);
	win_names_print(win_scoreboard_name_border,win_scoreboard_name,8,9,"HIGH SCORES",0,2);
	
	WINDOW * win_stats_name_border = newwin(3,17,Y,X-18);
	WINDOW * win_stats_name = subwin(win_stats_name_border,1,15,Y+1,X-17);
	win_names_print(win_stats_name_border,win_stats_name,8,9,"STATS",0,5);
	
	WINDOW * win_dev_border = newwin(3,ROW+9,Y+ROW+2,X);
	WINDOW * win_dev = subwin(win_dev_border,1,ROW+7,Y+ROW+3,X+1);
	win_names_print(win_dev_border,win_dev,8,9,"Dev: Mihajlo Jankovic",0,1);
}

void header_char(WINDOW * win_header, int ind, int n, int c, int x, int y)
{
	for(int i = 0; i < n; i++)
	{
		if(ind) mvwaddch(win_header, y, x+i, '#' | COLOR_PAIR(c));
		else mvwaddch(win_header, y+i, x, '#' | COLOR_PAIR(c));
	}
}

void win_header(void)
{
	int x = 10, y =1 , len = 3, hight = 4, space = 2;
	
	WINDOW * win_header_border = newwin(9,ROW+27,Y-9,X-18);
	WINDOW * win_header = subwin(win_header_border,7,ROW+25,Y-8,X-17);
	
	wborder(win_header_border, 0, 0, 0, 0, 0, 0, 0, 0); 
	
	wbkgd(win_header_border, COLOR_PAIR(8));
	wbkgd(win_header, COLOR_PAIR(9));
	
	header_char(win_header,1,len,6,x,y); // T
	header_char(win_header,0,hight,6,x+1,y+1);
	
	x += len+space;
	header_char(win_header,1,len,5,x,y); // E
	header_char(win_header,0,hight-1,5,x,y+1);
	header_char(win_header,1,len-2,5,x+1,y+hight/2);
	header_char(win_header,1,len,5,x,y+hight);
	
	x += len+space;
	header_char(win_header,1,len,4,x,y); // T
	header_char(win_header,0,hight,4,x+1,y+1);
	
	x += len+space;
	header_char(win_header,1,len,7,x,y); // R
	header_char(win_header,0,hight,7,x,y+1);
	header_char(win_header,0,len,7,x+len-1,y+1);
	header_char(win_header,1,len,7,x,y+hight/2);
	
	x += len+space;
	header_char(win_header,0,hight+1,1,x,y); // I
	
	x += space+1;
	header_char(win_header,1,len,3,x,y); // S
	header_char(win_header,1,len,3,x,y+hight/2);
	header_char(win_header,1,len,3,x,y+hight);
	header_char(win_header,0,1,3,x,y+1);
	header_char(win_header,0,1,3,x+len-1,y+3);
	
	wrefresh(win_header_border);
	wrefresh(win_header);	
}

void arr_players_init(struct player players[])
{
	for(int i = 0; i < SCOREBOARD_MAX; i++) players[i].score = -1;
}

int main()
{
	initscr(); //Begin of stdsrt win
	start_color(); // Default color of main(stdscr) window is COLOR_BLACK
	//use_default_colors(); // Not to change backround color of the main(stdscr) window
	noecho();
	curs_set(0);
	
	WINDOW * win_border = newwin(ROW+2, COL+2, Y, X);
	WINDOW * win_main = subwin(win_border,ROW,COL,Y+1,X+1);
	
	struct player players[SCOREBOARD_MAX]; //Players array
    arr_players_init(players);
	read_players(players);
	
	win_tetris(win_border, win_main);
	win_stats(0,0,0);
    win_names();
    win_help();
    win_next(0,0,0,0);
    win_header();
    
	int flag = 1, color_ind = 0;
	playfield matrix; //char matrix[ROW][COL];
	
	color(color_ind);
	matrix_initialize(matrix); // Initialize entire matrix to EMPTY value

	while(flag)
	{
		win_scoreboard(players);
		
		if(flag == -1) 
		{
			win_next(0,0,0,0);
			wclear(win_main);
			matrix_initialize(matrix); // Initialize entire matrix to EMPTY value
		}
		
		flag = tetris_main(matrix,win_border,win_main,players);
	}
	
	endwin(); //End of stdsrt win
}
