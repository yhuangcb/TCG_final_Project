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