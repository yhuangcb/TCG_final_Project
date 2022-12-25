#include "MyAI.h"

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

void get_next_position(char position[25], int color, int p, int s, int e, char new_position[25]){
    MyAI temp;
    temp.Set_board(position);
    temp.Set_Color(color);
    temp.Make_move(p, s, e);
    strcpy(new_position, temp.position);
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