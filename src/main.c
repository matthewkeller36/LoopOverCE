/*
 *--------------------------------------
 * Program Name:
 * Author:
 * License:
 * Description:
 *--------------------------------------
*/

/* Keep these headers */
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <tice.h>

/* Standard headers (recommended) */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <graphx.h>
#include <keypadc.h>
#include <fileioc.h>

#include "key_helper.h"

/* Put your function prototypes here */
void setBoard();
void printBoard(int pos);
void move(int pos, int times, bool vert);
bool checkBoard();
void shuffle();
void homescreen();
void animate(int pos, bool vert, bool four);
void drawBottom();
void runGame();
/* Put all your globals here */

int board[5][5], colors[5][5], key, prevKey;
int baseBoard[5][5] = {{1,2,3,4,5},{6,7,8,9,10},{11,12,13,14,15},{16,17,18,19,20},{21,22,23,24,25}};
int baseColors[5][5] = {{224,200,176,112,16},{193,161,146,91,26},{227,196,141,85,20},{197,102,46,54,29},{231,135,15,23,31}};
bool selected = false, showNums = true, home = true, quit = false, play = false, shuffled;
float elapsed, hTime = 0.0;
uint24_t hMoves = 0, totalMoves;

extern uint16_t *kb_Falling;

void main(void) {
    ti_var_t file;
    srand(rtc_Time());
    gfx_Begin();
    //init_Keys();

    file = ti_Open("HLOOP","r");
    if(file){
        ti_Read(&hMoves,sizeof(uint24_t),1, file);
        ti_Read(&hTime,sizeof(float),1, file);
    }
    gfx_FillScreen(0);
    gfx_SetDrawBuffer();
    gfx_SwapDraw();
    gfx_FillScreen(0);
    while (!quit && !(kb_Data[6] & kb_Clear)){
        totalMoves = 0;
        shuffled = false;
        timer_Control = TIMER1_DISABLE;
        setBoard();
        homescreen();
        if(!quit){
            shuffle();
            runGame();
        }
    }
    delay(500);
    ti_CloseAll();
    file = ti_Open("HLOOP","w");
    if(file){
        ti_Write(&hMoves,sizeof(uint24_t),1, file);
        ti_Write(&hTime,sizeof(float),1, file);
    }
    ti_SetArchiveStatus(true, file);
    gfx_End();
    //end_kb();
    prgm_CleanUp();

}

void runGame(){
    int curPos = 22;
    timer_Control = TIMER1_ENABLE | TIMER1_32K | TIMER1_UP;
    while(!checkBoard() && !(kb_Falling[6] & kb_Clear) && !quit){
        //scan_kb();
        if(selected && kb_Falling[7]){
            move(curPos,((kb_Falling[7] & kb_Up) || (kb_Falling[7] & kb_Left) ? 1 : 4), (kb_Falling[7] & kb_Up) || (kb_Falling[7] & kb_Down));
        }else{
            if(kb_Falling[7] & kb_Up)curPos = ((curPos-10) < 0 ? curPos+40 : curPos-10);
            if(kb_Falling[7] & kb_Down)curPos = ((curPos+10)/10 == 5 ? curPos-40 : curPos+10);
            if(kb_Falling[7] & kb_Left)curPos = ((curPos-1)%10 == 9 || (curPos-1)%10 < 0  ? curPos+4 : curPos-1);
            if(kb_Falling[7] & kb_Right)curPos = ((curPos+1)%10 == 5 ? curPos - 4 : curPos+1);
            if(kb_Falling[1] & kb_2nd)selected = !selected;
        }
        printBoard(curPos);
        drawBottom();
        gfx_SwapDraw();
        gfx_BlitScreen();

    }
    if(checkBoard() && (hMoves > totalMoves || hMoves < 5)){hMoves = totalMoves;}
    if(checkBoard() && (elapsed < hTime || hTime < 5)){hTime = elapsed;}
}

void homescreen(){
    ti_var_t file;
    int opt = 0, offset = 0;
    home = true;
    play = false;
    timer_1_Counter = 0.0;
    elapsed = 0.0;
    gfx_SetColor(255);
    gfx_FillRectangle(216,6,88,196);
    gfx_SetTextScale(2,2);
    gfx_PrintStringXY("Loop",228,8);
    gfx_PrintStringXY("Over",228,24);
    gfx_SetTextScale(1,1);
    gfx_PrintStringXY("matkeller19",220,40);
    gfx_PrintStringXY("Play",244,61);
    gfx_PrintStringXY("Toggle",236,88);
    gfx_PrintStringXY("Numbers",232,98);
    gfx_PrintStringXY("Reset",240,122);
    gfx_PrintStringXY("Time",244,132);
    gfx_PrintStringXY("Reset",240,154);
    gfx_PrintStringXY("Moves",240,164);
    gfx_PrintStringXY("Exit",244,189);
    while(!quit && !play){
        offset = 0;
        //scan_kb();
        if(kb_Falling[7] & kb_Up) opt = (opt + 4) % 5;
        if(kb_Falling[7] & kb_Down) opt = (opt + 1) % 5;
        if(kb_Falling[6] & kb_Clear) quit = true;
        if(kb_Falling[1] & kb_2nd) switch(opt){
            case 0:
            play = true;
            break;
            case 1:
            showNums = !showNums;
            break;
            case 2:
            hTime = 0;
            ti_CloseAll();
            file = ti_Open("HLOOP","w");
            if(file){
                ti_Seek(3, 0, file);
                ti_Write(&hTime,sizeof(uint24_t),sizeof(hTime)/sizeof(uint24_t), file);
            }
            ti_Close(file);
            break;
            case 3:
            hMoves = 0;
            ti_CloseAll();
            file = ti_Open("HLOOP","w");
            if(file){
                ti_Write(&hMoves,sizeof(uint24_t),sizeof(hMoves)/sizeof(uint24_t), file);
            }
            ti_Close(file);
            break;
            case 4:
            quit = true;
        }
        printBoard(0);
        gfx_SetColor(255);
        gfx_FillRectangle(216,60,12,140);
        gfx_FillRectangle(292,60,12,140);
        if(!play){
            gfx_PrintStringXY(">",220,61+32 * opt);
            gfx_PrintStringXY("<",294,61+ 32 * opt);
        }
        drawBottom();
        gfx_SwapDraw();
        gfx_BlitScreen();
    }
    gfx_SwapDraw();
    gfx_BlitScreen();
    home = false;
    selected = false;
}

void setBoard(){
    int i, j;
    for(i = 0; i < 5; i++){
        for(j = 0; j < 5; j++){
            board[i][j] = baseBoard[i][j];
            colors[i][j] = baseColors[j][i];
        }
    }
}
void shuffle(){
    int i, randomPos;
    for(i = 0; i < 500; i++){
        randomPos = (rand()%4)*10+(rand()%4);
        move(randomPos,rand()%4+1,rand()%2==1);
    }
    totalMoves-=500;
    shuffled = true;
}
void move(int pos, int times, bool vert){
    int i, j, temp, colorsTemp;
    for(j = 0; j < times; j++){
        temp = (vert ? board[0][pos%10] : board[pos/10][0]);//y,x
        colorsTemp = (vert ? colors[pos%10][0] : colors[0][pos/10]);//y,x
        for(i = 0; i < 4; i++){
            if(vert){
                board[i][pos%10] = board[i+1][pos%10];
                colors[pos%10][i] = colors[pos%10][i+1];
            }else{
                board[pos/10][i] = board[pos/10][i+1];
                colors[i][pos/10] = colors[i+1][pos/10];
            }
        }
        if(vert){
            board[4][pos%10] = temp;
            colors[pos%10][4] = colorsTemp;
        }
        else{
            board[pos/10][4] = temp;
            colors[4][pos/10] = colorsTemp;
        }
    }
    if(shuffled){animate(pos,vert, times==4);}
    totalMoves++;
}
bool checkBoard(){
    int i, j;
    for(i = 0; i < 5; i++){
        for(j = 0; j < 5; j++){
            if(board[i][j] != 5*i+j+1){
                return false;
            }
        }
    }
    return true;
}
void printBoard(int pos){
    int i, j;
    gfx_SetColor(0);
    gfx_FillRectangle(0,0,206,206);
    for(i = 0; i < 5; i++){
        for(j = 0; j < 5; j++){
            gfx_SetColor(colors[j][i]);
            gfx_FillRectangle(40*j+4,40*i+4,38,38);
            if(!home){
                if(selected){
                    gfx_SetColor(0xE5);
                    gfx_HorizLine(4,pos/10*40+3,198);
                    gfx_HorizLine(4,pos/10*40+42,198);
                    gfx_VertLine(pos%10*40+3,4,198);
                    gfx_VertLine(pos%10*40+42,4,198);
                }
                gfx_SetColor(0xFF);
                gfx_Rectangle(pos%10*40+3,pos/10*40+3,40,40);
            }
        }
    }
    if(showNums){
        for(i = 0; i < 5; i++){
            for(j = 0; j < 5; j++){
                gfx_SetTextXY(i*40+16,18+j*40);
                gfx_PrintInt(board[j][i],2);

            }
        }
    }
}

void animate(int pos, bool vert, bool four){
    int i;
    gfx_SetColor(0);
    gfx_Rectangle(pos%10*40+3,pos/10*40+3,41,41);
    for(i = 0; i < 8; i++){
        drawBottom();
        if(vert){
            gfx_SetClipRegion(40*(pos%10)+2,4,40*(pos%10)+42,202);
            if(!four){gfx_ShiftUp(8);}
            gfx_ShiftDown(4);
            gfx_SetColor(colors[pos%10][(four ? 0 : 4)]);
            gfx_FillRectangle(40*(pos%10)+4,(four ? 4 : 200),38,8);
            gfx_SetColor(0);
            gfx_FillRectangle(0,(four ? 202 : 0),202,4);
        }
        else{
            gfx_SetClipRegion(4,40*(pos/10)+2,200,40*(pos/10)+42);
            if(!four){gfx_ShiftLeft(8);}
            gfx_ShiftRight(4);
            gfx_SetColor(colors[(four ? 0 : 4)][pos%10]);
            gfx_FillRectangle((four ? 4 : 200),40*(pos%10)+4,2,8);
            gfx_SetColor(0);
            gfx_FillRectangle((four ? 202 : 0),0,4,202);
        }
        gfx_SetClipRegion(0,0,480,600);
    gfx_SwapDraw();
    gfx_BlitScreen();
    }
    gfx_SetClipRegion(0,0,480,600);
    gfx_SwapDraw();
    gfx_BlitScreen();
}

void drawBottom(){
    real_t elapsed_real, bestTime_real;
    char str[10];
    gfx_SetColor(255);
    gfx_FillRectangle(4,206,198,30);
    gfx_SetTextFGColor(0);
    gfx_SetTextTransparentColor(254);
    gfx_PrintStringXY("Current:",8,210);
    gfx_SetTextXY(76,210);
    gfx_PrintInt(totalMoves,3);
    gfx_PrintStringXY("Best: ",8,222);
    gfx_SetTextXY(76,222);
    gfx_PrintUInt(hMoves,3);
    elapsed = (float)atomic_load_increasing_32(&timer_1_Counter) / 32768;
    elapsed_real = os_FloatToReal(elapsed <= 0.001f ? 0.0f : elapsed);
    os_RealToStr(str, &elapsed_real, 8, 1, 2);
    gfx_PrintStringXY(str,120,210);
    bestTime_real = os_FloatToReal(hTime <= 0.001f ? 0.0f : hTime);
    os_RealToStr(str, &bestTime_real, 8, 1, 2);
    gfx_PrintStringXY(str,120,222);
    gfx_SetTextTransparentColor(255);
}
