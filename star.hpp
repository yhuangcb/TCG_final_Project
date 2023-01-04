#include "MyAI.h"
#include "utils.hpp"
#include "table.hpp"

const int DIS_VAL[6] = {100, 7, 3, 1, 0, 0};
const int PNUM_VAL[7] = {-100, 1, 2, 3, 4, 5, 6};

const double V_MAX = 30.0;
const double V_MIN = -30.0;

// return a number that indicates how good/bad the current position is for player pov
// considering piece number, mobility, distance to the goal,,etc
double MyAI::EvalBoard(){
    int R_piece_num = this->red_piece_num;
    int B_piece_num = this->blue_piece_num;
    int R_mobility = mobility(this->red_exist) - 4;
    int B_mobility = mobility(this->blue_exist) - 4;
    int R_distance = 5;
    int B_distance = 5;
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

void pick_ab_with_depth(char position[25], int color, int depth, int dice, int* piece, int* start_point, int* end_point);
void pick_ab_iterative(char position[25], int color, int dice, int* piece, int* start_point, int* end_point);

//double Star_0_F_3_0(char position[25], double alpha, double beta, int depth);
//double F_3_0(char position[25], int color, double alpha, double beta, int depth);
double G_3_0(char position[25], int color, double alpha, double beta, int depth);
double G_3_1(char position[25], int color, double alpha, double beta, int depth);
double G_3_1_H(char position[25], int color, double alpha, double beta, int depth);

void pick_ab_iterative(char position[25], int color, int dice, int* p, int* s, int* e){
    // time constraint to be implement
    int depth = 1;
    int piece, start_point, end_point;
    fprintf(stderr, "\nallowed diff: %f, expect steps: %d\n", mytimer.allowed_diff, mytimer.E_remain_step);

    while(time_permission(&mytimer) && depth < 16){
        pick_ab_with_depth(position, color, depth, dice, &piece, &start_point, &end_point);
        depth++;
    }
    *p = piece;
    *s = start_point;
    *e = end_point;
    
    fprintf(stderr, "\nCount: %d, Collision: %d\n", myHashTable->count, myHashTable->collision);
    timer_step(&mytimer);
}


void pick_ab_with_depth(char position[25], int color, int depth, int dice, int* piece, int* start_point, int* end_point){
    double eval = -999;
    double temp_eval;
    MyAI tempAI;
    tempAI.Set_board(position);
    tempAI.Set_Color(color);
    tempAI.Set_Dice(dice);
    int result[100];
    int b = tempAI.get_legal_move(result);
    for(int i=0;i<b;i++){
        char next_position[25];
        get_next_position(position, color, result[i*3], result[i*3+1], result[i*3+2], next_position); 
        temp_eval = -1 * G_3_1_H(next_position, color_reverse(color),-30, 30, depth);
        //fprintf(stderr, "\nNext Position: %s\n", next_position);
        //fprintf(stderr, "\ntemp_EVAL: %f\n", temp_eval);
        if(eval < temp_eval){
            eval = temp_eval;
            *piece = result[i*3];
            *start_point = result[i*3+1];
            *end_point = result[i*3+2];
        }
    }
    fprintf(stderr, "\nAB_EVAL: %f\n", eval);
}

/*
double F_3_0(char position[25], int color, double alpha, double beta, int depth){
    // Init
    MyAI node_ai;
    int result[100];
    node_ai.Set_board(position);
    node_ai.Set_Color(color);
    if(depth == 0){
        // reach depth restriction
        return node_ai.EvalBoard();
    }

    // general case
    double vsum = 0;
    for(int d=0;d<PIECE_NUM;d++){
        node_ai.Set_Dice(d+1);
        int b = node_ai.get_legal_move(result);
        double m = -999;
        double t;

        if(b == 0){
            // terminal node
            return node_ai.EvalBoard();
        }
        // dive deeper
        for(int i=0;i<b;i++){
            char next_position[25];
            get_next_position(position, color_reverse(color), result[i*3], result[i*3+1], result[i*3+2], next_position); 
            t = G_3_0(next_position, color,-999, 999, depth-1);
            if(t > m) m = t;
        }
        vsum += m;
    }
    return vsum / PIECE_NUM;
}
*/

double G_3_0(char position[25], int color, double alpha, double beta, int depth){
    // Init
    MyAI node_ai;
    int result[100];
    node_ai.Set_board(position);
    node_ai.Set_Color(color);
    if(depth == 0){
        // reach depth restriction
        return node_ai.EvalBoard();
    }

    // general case
    double vsum = 0;
    for(int d=0;d<PIECE_NUM;d++){
        node_ai.Set_Dice(d+1);
        int b = node_ai.get_legal_move(result);
        double m = -999;
        double t;

        if(b == 0){
            // terminal node
            return node_ai.EvalBoard();
        }
        // dive deeper
        for(int i=0;i<b;i++){
            char next_position[25];
            get_next_position(position, color, result[i*3], result[i*3+1], result[i*3+2], next_position); 
            t = -1 * G_3_0(next_position, color_reverse(color),-999, 999, depth-1);
            if(t > m) m = t;
        }
        vsum += m;
    }
    return vsum / PIECE_NUM;
}

double G_3_1(char position[25], int color, double alpha, double beta, int depth){
    // Init
    MyAI node_ai;
    int result[100];
    node_ai.Set_board(position);
    node_ai.Set_Color(color);
    if(depth == 0){
        // reach depth restriction
        return node_ai.EvalBoard();
    }

    // general case
    double vsum = 0;
    double A_[PIECE_NUM+1] = {0};
    double B_[PIECE_NUM+1] = {0};
    double M_[PIECE_NUM+1] = {0};
    double m_[PIECE_NUM+1] = {0};
    A_[0] = ((alpha - V_MAX) * PIECE_NUM) + V_MAX;
    B_[0] = ((beta - V_MIN) * PIECE_NUM) + V_MIN;
    M_[0] = V_MAX;
    m_[0] = V_MIN;

    for(int d=0;d<PIECE_NUM;d++){
        node_ai.Set_Dice(d+1);
        int b = node_ai.get_legal_move(result);
        double m = -999;
        double t;

        if(b == 0){
            // terminal node
            return node_ai.EvalBoard();
        }
        // dive deeper
        for(int i=0;i<b;i++){
            char next_position[25];
            get_next_position(position, color, result[i*3], result[i*3+1], result[i*3+2], next_position); 
            t = -1 * G_3_1(next_position, color_reverse(color),std::max(V_MIN, A_[d]), std::min(B_[d],V_MAX), depth-1);
            if(t > m){
                m = t;
            }
        }
        m_[d+1] = m_[d] + ((m - V_MIN) / (double)PIECE_NUM);
        M_[d+1] = M_[d] + ((m - V_MAX) / (double)PIECE_NUM);
        if(m >= B_[d]){
            //fprintf(stderr, "\nFAILED HIGH\n");
            return m_[d+1];
        } 
        if(m <= A_[d]){
            //fprintf(stderr, "\nFAILED LOW\n");
            return M_[d+1];
        } 
        vsum += m;
        A_[d+1] = A_[d] + V_MAX - m;
        B_[d+1] = B_[d] + V_MIN - m;
    }
    return vsum / PIECE_NUM;
}

double G_3_1_H(char position[25], int color, double alpha, double beta, int depth){
    // Init
    MyAI node_ai;
    int result[100];
    node_ai.Set_board(position);
    node_ai.Set_Color(color);
    if(depth == 0){
        // reach depth restriction
        ht_insert(myHashTable, node_ai.board_bit, depth, node_ai.EvalBoard(), color);
        //fprintf(stderr, "\nCOUNT: %d\n", myHashTable->count);
        return node_ai.EvalBoard();
    }

    
    // Check Hash Table
    double hash_value;
    if(ht_search(myHashTable, node_ai.board_bit, depth, &hash_value, color)){
        return hash_value;
    }
    //fprintf(stderr, "\nDEBUG\n");
    // general case
    double vsum = 0;
    double A_[PIECE_NUM+1] = {0};
    double B_[PIECE_NUM+1] = {0};
    double M_[PIECE_NUM+1] = {0};
    double m_[PIECE_NUM+1] = {0};
    A_[0] = ((alpha - V_MAX) * PIECE_NUM) + V_MAX;
    B_[0] = ((beta - V_MIN) * PIECE_NUM) + V_MIN;
    M_[0] = V_MAX;
    m_[0] = V_MIN;


    for(int d=0;d<PIECE_NUM;d++){
        node_ai.Set_Dice(d+1);
        int b = node_ai.get_legal_move(result);
        double m = -999;
        double t;

        if(b == 0){
            // terminal node
            ht_insert(myHashTable, node_ai.board_bit, depth, node_ai.EvalBoard(), color);
            return node_ai.EvalBoard();
        }

        
        // dive deeper
        for(int i=0;i<b;i++){
            char next_position[25];
            get_next_position(position, color, result[i*3], result[i*3+1], result[i*3+2], next_position); 
            t = -1 * G_3_1_H(next_position, color_reverse(color),std::max(V_MIN, A_[d]), std::min(B_[d],V_MAX), depth-1);
            if(t > m){
                m = t;
            }
        }
        m_[d+1] = m_[d] + ((m - V_MIN) / (double)PIECE_NUM);
        M_[d+1] = M_[d] + ((m - V_MAX) / (double)PIECE_NUM);
        if(m >= B_[d]){
            //fprintf(stderr, "\nFAILED HIGH\n");
            ht_insert(myHashTable, node_ai.board_bit, depth, m_[d+1], color);
            return m_[d+1];
        } 
        if(m <= A_[d]){
            //fprintf(stderr, "\nFAILED LOW\n");
            ht_insert(myHashTable, node_ai.board_bit, depth, M_[d+1], color);
            return M_[d+1];
        } 
        vsum += m;
        A_[d+1] = A_[d] + V_MAX - m;
        B_[d+1] = B_[d] + V_MIN - m;
    }
    ht_insert(myHashTable, node_ai.board_bit, depth, vsum/PIECE_NUM, color);
    return vsum / PIECE_NUM;
}

