#ifndef HASH
#define HASH
#define XOR_KEY 9

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>

#define SUCCESS 1
#define FAILURE 0
#define hash_size 28

// Linked list to maintain the list of input filenames
typedef struct node{
    char f_name[50];
    struct node* f_to_f_link;
}file_SLL_t;

// Sub-node stores the filename and word occurrence count
typedef struct sub{
    int w_count;
    char f_name[50];
    struct sub* sub_to_sub_link;
}sub_node_t;

// Main node stores a unique word and the number of files containing it
typedef struct main{
    int f_count;
    char word[50];
    sub_node_t* main_to_sub_link;
    struct main* main_to_main_link;
}main_node_t;

// Hash table structure containing the bucket index and main-node pointer
typedef struct hashing{
    int index;
    main_node_t* link;
}hash_t;


// Function declarations

// File handling and validation
int get_file_size(FILE *fptr);
int insert_file(char *str,file_SLL_t** f_head);
int search_duplicates(char *str,file_SLL_t ** f_head);
int search_duplicate_filename(hash_t *database, char *filename);
int read_and_validate(int argc, char *argv[], file_SLL_t **head);

// Hashing and database insertion
int hash_function(char *word);
int insert_main_to_db(main_node_t **m_head,char *word, char *f_name);
int insert_subnode(hash_t *database,char *word);

// Encoding and decoding backup data
void encode_decode_data(char *data, int len);
void encode_hex(char *data, char *encoded, int len);
int decode_hex(char *encoded, char *decoded, int *len);

// Database operations
int create_database(int argc, char *argv[], hash_t *database);
void display_database(hash_t *database);
int search_data(hash_t *database);
int save_database(hash_t *database);
int load_database(hash_t *database);
int update_database(hash_t *database);

#endif