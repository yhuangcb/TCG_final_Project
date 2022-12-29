#include "MyAI.h"
#include<chrono>

// return the total possibility to choose a piece number
// e.g. 1XXXX6 = 5+5 = 10, X2XXX6 = 5+4 = 9 , X234XX=2+1+3=6
int mobility(bool exist[PIECE_NUM]){
    int count = 0;
    for(int i=0;i<PIECE_NUM;i++){
        if(exist[i]){
            count ++;
        }
        else{
            int j = i;
            while(j>0){
                j--;
                if(exist[j]){
                    count ++;
                    break;
                }
            }
            j = i;
            while(j<PIECE_NUM-1){
                j++;
                if(exist[j]){
                    count ++;
                    break;
                }
            }
        }
    }
    return count;
}

void get_next_position(char* position, int color, int p, int s, int e, char* new_position){
    MyAI temp;
    temp.Set_board(position);
    temp.Set_Color(color);
    temp.Make_move(p, s, e);
    strcpy(new_position, temp.position);
    //fprintf(stderr, "\nOriginal Position: %s\n", position);
    //fprintf(stderr, "\nNext Position: %s\n", new_position);
}

int color_reverse(int color){
    if(color == RED)return BLUE;
    else return RED;
}

const int MAX_NODE = 1000000;
const int MAX_CHILD = 18;
struct NODE{
    char position[25];
    int color;
    int dice;
    int p_id;
    int c_id[MAX_CHILD];
    int depth;
    double alpha;
    double beta;
    double eval_val;

}nodes[MAX_NODE];

#define parent(ptr) (nodes[ptr].p_id)       // id of ptr's parent
#define child(ptr, i) (nodes[ptr].c_id[i])  // the ith child of ptr
const int EXCEPT_TOTAL_STEP = 20;

struct myTimer{
    int total_time;
    std::chrono::time_point<std::chrono::system_clock> start;
    //std::chrono::duration<double> allowed_diff;
    double remain;
    double allowed_diff;
    int E_remain_step;
    bool working;
};
myTimer* mytimer;

void timer_init(myTimer* mt, int avail_time){
    mt->start = std::chrono::system_clock::now();
    mt->E_remain_step = EXCEPT_TOTAL_STEP;
    mt->total_time = avail_time * 1000;
    mt->remain = mt->total_time;
    mt->allowed_diff = mt->remain / mt->E_remain_step;
    mt->working = true;
}

void timer_step(myTimer* mt){
    if(mt->E_remain_step >= 3){
        mt->E_remain_step--;
    }
    mt->remain -= std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - mt->start).count();
    mt->start = std::chrono::system_clock::now();
    mt->allowed_diff = mt->remain / mt->E_remain_step;
}

bool time_permission(myTimer* mt){
    double current_diff = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - mt->start).count();
    if(current_diff >= mt->allowed_diff)return false;
    else return true;
}