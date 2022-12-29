#ifndef MYHASH
#define MYHASH

#include<bitset>
#include<random>
#include "MyAI.h"

using namespace std;


const int K = PIECE_NUM * 2;
const int R = BOARD_SIZE * BOARD_SIZE;
const int HASH_KEY_LENGTH = 64;
const int HASH_INDEX_LENGTH = 30;

std::bitset<HASH_KEY_LENGTH> RN_TABLE[K * R];

template< size_t size>
typename std::bitset<size> random_bitset(){
    typename std::bitset<size> bits;
    random_device rd;
    mt19937 gen(rd());
    bernoulli_distribution d(0.5);
    for(int n=0;n<size;n++){
        bits[n] = d(gen);
    }
    return bits;
}

void initialize_RN_TABLE(){
    for(int i=0;i<K*R;i++){
        RN_TABLE[i] = random_bitset<HASH_KEY_LENGTH>();
    }
}

void test_RN_TABLE(){
    initialize_RN_TABLE();
    for(int i=0;i<K*R;i++){
        fprintf(stderr, "\n%s\n", RN_TABLE[i].to_string().c_str());
    }
}

// initialize board's hash
void MyAI::board2bitset(){
    bitset<HASH_KEY_LENGTH> bb(0);
    int temp = 0;
    for(int p=0;p<R;p++){ //0~24
        int p_row = p / BOARD_SIZE;
        int p_col = p % BOARD_SIZE;
        temp = this->board[p_row][p_col]; // 1~6 & 7~12
        if(temp != 0){
            //fprintf(stderr, "\n%s\n", bb.to_string().c_str());
            //fprintf(stderr, "\n%d, %d, %d\n", p, K, temp);
            //fprintf(stderr, "\n%s\n", RN_TABLE[p * K + temp - 1].to_string().c_str());
            bb ^= RN_TABLE[p * K + temp - 1];
        }
    }
    this->board_bit = bb;
}

/*
// incrementally calculate board's hash
void MyAI::bitset_refresh(const int piece, const int start_point, const int end_point){
    int start_row = start_point / BOARD_SIZE;
	int start_col = start_point % BOARD_SIZE;
	int end_row = end_point / BOARD_SIZE;
	int end_col = end_point % BOARD_SIZE;
}
*/

typedef struct Ht_item Ht_item;
struct Ht_item{
    bitset<HASH_KEY_LENGTH> key;
    int depth;
    double value;
    // int exact;
};

typedef struct HashTable HashTable;
struct HashTable{
    Ht_item** items;
    int size;
    int count;
    int collision;
};
HashTable* myHashTable;

Ht_item* create_item(bitset<HASH_KEY_LENGTH> key, int depth, double value){
    Ht_item* item = (Ht_item*) malloc(sizeof(Ht_item));
    item->key = key;
    item->depth = depth;
    item->value = value;
    return item;
}

HashTable* create_table(int size){
    fprintf(stderr, "\nStart Creating Table\n");
    HashTable* table = (HashTable*) malloc(sizeof(HashTable));
    table->size = size;
    table->count = 0;
    table->collision = 0;
    table->items = (Ht_item**) calloc(table->size, sizeof(Ht_item*));
    for(int i=0; i<table->size;i++){
        table->items[i] = NULL;
        //if(i%100 == 0)fprintf(stderr, "\n%d\n", i);
    }
    fprintf(stderr, "\nTable Created\n");
    return table;
}

void free_item(Ht_item* item){
    free(item);
}
void free_table(HashTable* table){
    for(int i=0;i<table->size;i++){
        Ht_item* item = table->items[i];
        if(item != NULL){
            free_item(item);
        }
    }
    free(table->items);
    free(table);
}

int hash_function(bitset<HASH_KEY_LENGTH> key){
    return (int)((key >> (HASH_KEY_LENGTH - HASH_INDEX_LENGTH)).to_ulong());
}

void ht_insert(HashTable* table, bitset<HASH_KEY_LENGTH> key, int depth, double value, int color){
    // record eval in BLUE pov
    if(color == RED){
        value *= -1;
    }
    //
    Ht_item* item = create_item(key, depth, value);
    int index = hash_function(key);
    Ht_item* current_item = table->items[index];
    if(current_item == NULL){
        // key does not exist
        if(table->count == table->size){
            fprintf(stderr, "\nHash Table is FULL\n");
            free_item(item);
            return;
        }
        table->items[index] = item;
        table->count++;
    }
    else{
        // Case 1, update if deeper
        if(current_item->key == key){
            if(current_item->depth < depth){
                current_item->depth = depth;
                current_item->value = value;
            }
        }
        // Case 2, collision
        else{
            //fprintf(stderr, "\nCollision Occurred !!\n");
            // Still replace
            if(current_item->depth < depth){
                current_item->depth = depth;
                current_item->value = value;
            }
            table->collision++;
        }
        return;
    }
}

bool ht_search(HashTable* table, bitset<HASH_KEY_LENGTH> key, int depth, double* value, int color){
    int index = hash_function(key);
    //fprintf(stderr, "\nindex: %lu\n", index);
    Ht_item* item = table->items[index];
    if(item == NULL) return false; // Not Found
    if(item->depth < depth) return false; // Shallow

    // HIT
    // remember the value is in BLUE pov
    if(color == RED){
        *value = item->value * -1 ;
    }
    else{
        *value = item->value; 
    }
    
    return true;
}

#endif