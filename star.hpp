#include "MyAI.h"
#include "utils.hpp"

const int DIS_VAL[5] = {100, 7, 3, 1, 0};
const int PNUM_VAL[7] = {-100, 1, 2, 3, 4, 5, 6};

// return a number that indicates how good/bad the current position is for player pov
// considering piece number, mobility, distance to the goal,,etc
double MyAI::EvalBoard(){
    int R_piece_num = this->red_piece_num;
    int B_piece_num = this->blue_piece_num;
    int R_mobility = mobility(this->red_exist) - 4;
    int B_mobility = mobility(this->blue_exist) - 4;
    int R_distance = 99;
    int B_distance = 99;
    // 
    for(int i=0;i<PIECE_NUM;i++){
        if(this->red_exist[i]){
            int temp_d = this->Dis2Goal(i+1+PIECE_NUM);
            if(temp_d<R_distance)R_distance=temp_d;
        }
        if(this->blue_exist[i]){
            int temp_d = this->Dis2Goal(i+1);
            if(temp_d<B_distance)B_distance=temp_d;
        }
    }
    double W1 = 0.1;
    double W2 = 0.3;
    double W3 = 1.0;

    //double R_score = R_piece_num*W1+R_mobility*W2+(5 - R_distance)*W3;
    //double B_score = B_piece_num*W1+B_mobility*W2+(5 - B_distance)*W3;
    //double R_score = R_piece_num*W1+R_mobility*W2+DIS_VAL[R_distance]*W3;
    //double B_score = B_piece_num*W1+B_mobility*W2+DIS_VAL[B_distance]*W3;
    double R_score = PNUM_VAL[R_piece_num]*W1+R_mobility*W2+DIS_VAL[R_distance]*W3;
    double B_score = PNUM_VAL[B_piece_num]*W1+B_mobility*W2+DIS_VAL[B_distance]*W3;


    if(this->color == RED)return R_score-B_score;
    else return B_score-R_score;
}

int MyAI::Dis2Goal(int piece){
    int color;
    int loc_i, loc_j;
    if(piece<=PIECE_NUM)color=BLUE;
    else color=RED;
    for(int i = 0; i < BOARD_SIZE; i++)
	{
		for(int j = 0; j < BOARD_SIZE; j++)
		{
			if(this->board[i][j] == piece)
			{
				loc_i = i;
                loc_j = j;
			}
		}
	}
    if(color == RED){
        loc_i = BOARD_SIZE - 1 - loc_i;
        loc_j = BOARD_SIZE - 1 - loc_j;
    }
    if(loc_i<loc_j)loc_i=loc_j;
    return loc_i;
}


void random_move(int legal_moves[100], int move_count, int* piece, int* start_point, int* end_point){
    int rand_move = rand() % move_count;
    *piece = legal_moves[rand_move * 3];
	*start_point = legal_moves[rand_move * 3 + 1];
	*end_point = legal_moves[rand_move * 3 + 2];
}

void pick_max_eval(MyAI* myai, int legal_moves[100], int move_count, int* piece, int* start_point, int* end_point){
    char original_pos[32];
    strcpy(original_pos, myai->position);
    int p, s, e;
    MyAI tempAI;
    double eval = -99.0;
    double temp_eval;
    int color = myai->Get_Color();

    for(int i=0;i<move_count;i++){
        tempAI.Set_board(original_pos);
        tempAI.Set_Color(color);
        p = legal_moves[i * 3];
        s = legal_moves[i * 3 + 1];
        e = legal_moves[i * 3 + 2];
        tempAI.Make_move(p,s,e);
        temp_eval = tempAI.EvalBoard();
        if(temp_eval>eval){
            *piece = p;
            *start_point = s;
            *end_point = e;
            eval = temp_eval;
        }
    }
}