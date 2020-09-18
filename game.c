#include <stdio.h>
#include "LPC17xx.H"                       
#include "GLCD.h"
#include "LED.h"
#include "ADC.h"
#include "KBD.h"

#define __FI        1                      /* Font index 16x24               */
#define __USE_LCD   0										/* Uncomment to use the LCD */

int check(int a, int b, int c){
	if (a == b && b == c)
		return a;
	else
		return 2;
}

int game(void){
char text[20];
uint32_t j;
uint32_t joystickInitg = 0;
int gscreen = 0;
int sum;
int x = 4;
int o = 4;
int grid[9] = {3,4,5,6,7,8,9,10,11};
int gridI[9] = {3,4,5,6,7,8,9,10,11};
int turn = 1;
int xscore =0,oscore=0;
int xscoreI =0,oscoreI=0;
int i;

					KBD_Init();
					sprintf(text,"X: %d | O: %d", xscore, oscore);
					#ifdef __USE_LCD
					GLCD_Clear(White);
					GLCD_SetBackColor(White);
					GLCD_SetTextColor(Black);
					GLCD_DisplayString(1, 0, __FI, (unsigned char*)text);
					GLCD_SetBackColor(Blue);
					GLCD_SetTextColor(Yellow);
					GLCD_DisplayChar(5, 2,  __FI, 'X');
					GLCD_Bargraph(0, 60, 90, 3, 1);
					GLCD_Bargraph(0, 97, 90, 3, 1);
					GLCD_Bargraph(0, 145, 90, 3, 1);
					GLCD_Bargraph(0, 195, 90, 3, 1);
					GLCD_Bargraph(25, 60, 3, 135, 2);
					GLCD_Bargraph(55, 60, 3, 135, 2);
					GLCD_Bargraph(90, 60, 3, 135, 2);
					#endif
					while(gscreen == 0 || gscreen == 1){				
						j = get_button();
						while (j != joystickInitg){
							
							if (gscreen == 0){ //Gaming Screen
								
								if (j == KBD_SELECT){ //make selection
									if (turn == 1){
										grid[x] = 1;
										turn = 0;
									}
									else if (turn == 0){
										grid[o] = 0;
										turn = 1;
									}
								}
								if (j == KBD_UP){ //Joystick pressed up
									if (turn == 1){
										x = x - 3;
										if (x == -3 || x == -2 || x == -1)
											x = 0;
									}
									if (turn == 0){
										o = o - 3;
										if (o == -3 || o == -2 || o == -1)
											o = 0;
									}	
								}
								if (j == KBD_DOWN){ //Joystick pressed down
									if (turn == 1){
										x = x + 3;
										if (x == 9 || x == 10 || x == 11)
											x = 8;
									}
									if (turn == 0){
										o = o + 3;
										if (o == 9 || o == 10 || o == 11)
											o = 8;
									}	
								}
								if (j == KBD_LEFT){ //Joystick pressed left
									if (turn == 1){
										x = x - 1;
										if (x == -1)
											x = 0;
									}
									if (turn == 0){
										o = o - 1;
										if (o == -1)
											o = 0;
									}	
								}
								if (j == KBD_RIGHT){ //Joystick pressed right
									if (turn == 1){
										x = x + 1;
										if (x == 9)
											x = 8;
									}
									if (turn == 0){
										o = o + 1;
										if (o == 9)
											o = 8;
									}	
								}
								
								//display
								sprintf(text,"X: %d | O: %d", xscore, oscore);
								#ifdef __USE_LCD
								GLCD_SetBackColor(White);
								GLCD_SetTextColor(Black);
								GLCD_DisplayString(1, 0, __FI, (unsigned char*)text);
								GLCD_DisplayChar(3, 0, __FI, ' ');
								GLCD_DisplayChar(3, 2, __FI, ' ');
								GLCD_DisplayChar(3, 4, __FI, ' ');
								GLCD_DisplayChar(5, 0, __FI, ' ');
								GLCD_DisplayChar(5, 2, __FI, ' ');
								GLCD_DisplayChar(5, 4, __FI, ' ');
								GLCD_DisplayChar(7, 0, __FI, ' ');
								GLCD_DisplayChar(7, 2, __FI, ' ');
								GLCD_DisplayChar(7, 4, __FI, ' ');
								#endif
								for (i=0; i<9; i++){
									if (i <= 2){ //first line
										if (grid[i] == 1){ 
											#ifdef __USE_LCD
											GLCD_SetBackColor(White);
											GLCD_SetTextColor(Black);
											GLCD_DisplayChar(3, 2*i,  __FI, 'X');
											#endif
										}
										if (grid[i] == 0){
											#ifdef __USE_LCD
											GLCD_SetBackColor(White);
											GLCD_SetTextColor(Black);
											GLCD_DisplayChar(3, 2*i,  __FI, 'O');
											#endif
										}
										if (x == i && turn == 1){ //if X pointer is at this position
										#ifdef __USE_LCD
										GLCD_SetBackColor(Blue);
										GLCD_SetTextColor(Yellow);
										GLCD_DisplayChar(3, 2*i,  __FI, 'X');
										#endif
										}
										if (o == i && turn == 0){ //if O pointer is at this position
										#ifdef __USE_LCD
										GLCD_SetBackColor(Blue);
										GLCD_SetTextColor(Yellow);
										GLCD_DisplayChar(3, 2*i,  __FI, 'O');
										#endif
										}
									}
									if (i >= 3 && i <= 5){ //second line
										if (grid[i] == 1){
											#ifdef __USE_LCD
											GLCD_SetBackColor(White);
											GLCD_SetTextColor(Black);
											GLCD_DisplayChar(5, (i-3)*2,  __FI, 'X');
											#endif
										}
										if (grid[i] == 0){
											#ifdef __USE_LCD
											GLCD_SetBackColor(White);
											GLCD_SetTextColor(Black);
											GLCD_DisplayChar(5, (i-3)*2,  __FI, 'O');
											#endif
										}
										if (x == i && turn == 1){ 
										#ifdef __USE_LCD
										GLCD_SetBackColor(Blue);
										GLCD_SetTextColor(Yellow);
										GLCD_DisplayChar(5, (i-3)*2,  __FI, 'X');
										#endif
										}
										if (o == i && turn == 0){ 
										#ifdef __USE_LCD
										GLCD_SetBackColor(Blue);
										GLCD_SetTextColor(Yellow);
										GLCD_DisplayChar(5, (i-3)*2,  __FI, 'O');
										#endif
										}
									}
									if (i >= 6){ //third line
										if (grid[i] == 1){
											#ifdef __USE_LCD
											GLCD_SetBackColor(White);
											GLCD_SetTextColor(Black);
											GLCD_DisplayChar(7, (i-6)*2,  __FI, 'X');
											#endif
										}
										if (grid[i] == 0){
											#ifdef __USE_LCD
											GLCD_SetBackColor(White);
											GLCD_SetTextColor(Black);
											GLCD_DisplayChar(7, (i-6)*2,  __FI, 'O');
											#endif
										}
										if (x == i && turn == 1){ 
										#ifdef __USE_LCD
										GLCD_SetBackColor(Blue);
										GLCD_SetTextColor(Yellow);
										GLCD_DisplayChar(7, (i-6)*2,  __FI, 'X');
										#endif
										}
										if (o == i && turn == 0){ 
										#ifdef __USE_LCD
										GLCD_SetBackColor(Blue);
										GLCD_SetTextColor(Yellow);
										GLCD_DisplayChar(7, (i-6)*2,  __FI, 'O');
										#endif
										}
									}
								}
								//compute score
								if (check(grid[0],grid[1],grid[2]) == 1 || check(grid[3],grid[4],grid[5]) == 1 || check(grid[6],grid[7],grid[8]) == 1){ //horizontal
										xscore++; //x wins
										gscreen = 1;
										sprintf(text,"X:%d  O:%d", xscore,oscore);
										#ifdef __USE_LCD
										GLCD_Clear(White);
										GLCD_DisplayString(1, 0, __FI, (unsigned char*)text);
										GLCD_DisplayString(3, 0, __FI, "Player 1 wins");
										GLCD_DisplayString(4, 0, __FI, "Press to exit");
										GLCD_DisplayString(5, 0, __FI, "Right to continue");
										#endif
								}
								if (check(grid[0],grid[1],grid[2]) == 0 || check(grid[3],grid[4],grid[5]) == 0 || check(grid[6],grid[7],grid[8]) == 0){
										oscore++; //o wins
										gscreen = 1;
										sprintf(text,"X:%d  O:%d", xscore,oscore);
										#ifdef __USE_LCD
										GLCD_Clear(White);
										GLCD_DisplayString(1, 0, __FI, (unsigned char*)text);
										GLCD_DisplayString(3, 0, __FI, "Player 2 wins");
										GLCD_DisplayString(4, 0, __FI, "Press to exit");
										GLCD_DisplayString(5, 0, __FI, "Right to continue");
										#endif
								}
								if (check(grid[0],grid[3],grid[6]) == 1 || check(grid[1],grid[4],grid[7]) == 1 || check(grid[2],grid[5],grid[8]) == 1){ //vertical
										xscore++; //x wins
										gscreen = 1;
										sprintf(text,"X:%d  O:%d", xscore,oscore);
										#ifdef __USE_LCD
										GLCD_Clear(White);
										GLCD_DisplayString(1, 0, __FI, (unsigned char*)text);
										GLCD_DisplayString(3, 0, __FI, "Player 1 wins");
										GLCD_DisplayString(4, 0, __FI, "Press to exit");
										GLCD_DisplayString(5, 0, __FI, "Right to continue");
										#endif
								}
								if (check(grid[0],grid[3],grid[6]) == 0 || check(grid[1],grid[4],grid[7]) == 0 || check(grid[2],grid[5],grid[8]) == 0){
										oscore++; //o wins
										gscreen = 1;
										sprintf(text,"X:%d  O:%d", xscore,oscore);
										#ifdef __USE_LCD
										GLCD_Clear(White);
										GLCD_DisplayString(1, 0, __FI, (unsigned char*)text);
										GLCD_DisplayString(3, 0, __FI, "Player 2 wins");
										GLCD_DisplayString(4, 0, __FI, "Press to exit");
										GLCD_DisplayString(5, 0, __FI, "Right to continue");
										#endif
								}
								if (check(grid[0],grid[4],grid[8]) == 1 || check(grid[2],grid[4],grid[6]) == 1){ //diagonal
										xscore++; //x wins
										gscreen = 1;
										sprintf(text,"X:%d  O:%d", xscore,oscore);
										#ifdef __USE_LCD
										GLCD_Clear(White);
										GLCD_DisplayString(1, 0, __FI, (unsigned char*)text);
										GLCD_DisplayString(3, 0, __FI, "Player 1 wins");
										GLCD_DisplayString(4, 0, __FI, "Press to exit");
										GLCD_DisplayString(5, 0, __FI, "Right to continue");
										#endif
								}
								if (check(grid[0],grid[4],grid[8]) == 0 || check(grid[2],grid[4],grid[6]) == 0){
										oscore++; //o wins
										gscreen = 1;
										sprintf(text,"X:%d  O:%d", xscore,oscore);
										#ifdef __USE_LCD
										GLCD_Clear(White);
										GLCD_DisplayString(1, 0, __FI, (unsigned char*)text);
										GLCD_DisplayString(3, 0, __FI, "Player 2 wins");
										GLCD_DisplayString(4, 0, __FI, "Press to exit");
										GLCD_DisplayString(5, 0, __FI, "Right to continue");
										#endif
								}
								
								for (i = 0; i < 9; i++){
									if (grid[i] == 1 || grid[i] == 0)
										sum += 1;		
								}
								if (sum == 9 && xscore == xscoreI && oscore == oscoreI){ //game draw
									gscreen = 1;
									sprintf(text,"X:%d  O:%d", xscore,oscore);
										#ifdef __USE_LCD
										GLCD_Clear(White);
										GLCD_DisplayString(1, 0, __FI, (unsigned char*)text);
										GLCD_DisplayString(3, 0, __FI, "Draw");
										GLCD_DisplayString(4, 0, __FI, "Press to exit");
										GLCD_DisplayString(5, 0, __FI, "Right to continue");
										#endif
								}
								sum = 0;
								xscoreI = xscore;
								oscoreI = oscore;
							}
							
							else if (gscreen == 1 ){ //second screen
								if (j == KBD_SELECT){
									gscreen = 2; //exit game
								}if (j == KBD_RIGHT){
									gscreen = 0; //continue game
									#ifdef __USE_LCD
									GLCD_Clear(White);
									GLCD_SetBackColor(Blue);
									GLCD_Bargraph(0, 60, 90, 3, 1);
									GLCD_Bargraph(0, 97, 90, 3, 1);
									GLCD_Bargraph(0, 145, 90, 3, 1);
									GLCD_Bargraph(0, 195, 90, 3, 1);
									GLCD_Bargraph(25, 60, 3, 135, 2);
									GLCD_Bargraph(55, 60, 3, 135, 2);
									GLCD_Bargraph(90, 60, 3, 135, 2);
									#endif
									for (i=0; i<9; i++)
										grid[i] = gridI[i];
								}
							}
					
					joystickInitg = j;
					}
				}
	 //end of game
	return 0;			
}
