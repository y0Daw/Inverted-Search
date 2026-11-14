#include "hash_headers.h"

// Calculate the hash index based on the first character of the word
int hash_function(char *word)
{
    int index;
    if(isalpha(word[0]))
        index = tolower(word[0]) - 97;
    else if(isdigit(word[0]))
        index = 26;
    else
        index = 27;

    return index;
}

// Find the size of the given file
int get_file_size(FILE *fptr)
{
    long file_size;

    // Move the file pointer to the end of the file.
    fseek(fptr, 0, SEEK_END);

    // Get the current position, which represents the file size.
    file_size = ftell(fptr);
    rewind(fptr);

    return file_size;
}

// Insert a filename into the file linked list
int insert_file(char *str,file_SLL_t** head)
{
    file_SLL_t *new = malloc(sizeof(file_SLL_t));
    if(new == NULL)
        return FAILURE;

    if(*head == NULL)
    {
        strcpy(new->f_name, str);
        new->f_to_f_link = *head;
        *head = new;

        return SUCCESS;
    }
    else
    {
        // Traverse to the last node
        file_SLL_t *temp = *head;
        while(temp->f_to_f_link != NULL)
        {
            temp = temp->f_to_f_link;
        }
        strcpy(new->f_name, str);
        temp->f_to_f_link = new;
        new->f_to_f_link = NULL;

        return SUCCESS;
    }
}

// Check whether the filename is already present in the file linked list
int search_duplicates(char *str,file_SLL_t **head)
{
    file_SLL_t *temp = *head;
    while(temp != NULL)
    {
        if(!strcmp(temp->f_name, str))
            return SUCCESS;

        temp = temp->f_to_f_link;
    }
    return FAILURE; 
}

// Encode or decode data using XOR operation
void encode_decode_data(char *data, int len)
{
    for(int i = 0; i < len; i++)
    {
        data[i] ^= XOR_KEY;
    }
}

// Convert data into hexadecimal format
void encode_hex(char *data, char *encoded,int len)
{
  for(int i = 0; i < len; i++)
  {
    sprintf(encoded + (i * 2), "%02x", (unsigned char)data[i]);
  }
  encoded[len * 2] = '\0';
}

// Convert hexadecimal data back into its original byte format
int decode_hex(char *encoded, char *decoded, int *len)
{
    int i;
    int hex_len = strlen(encoded);

    // Check whether the hexadecimal string has an even number of characters
    if(hex_len % 2 != 0)
        return FAILURE;

    *len = hex_len / 2;

    // Convert every pair of hexadecimal characters into one byte
    for(i = 0; i < *len; i++)
    {
        unsigned int value;
        if(sscanf(encoded + (i * 2), "%2x", &value) != 1)
            return FAILURE;
        decoded[i] = (char)value;
    }

    decoded[*len] = '\0';
    return SUCCESS;
}

// Check whether the filename already exists in the database
int search_duplicate_filename(hash_t *database, char *filename)
{
    // Traverse all hash table indexes
    for(int i = 0; i < hash_size; i++)
    {
        main_node_t *m_temp = database[i].link;

        while(m_temp != NULL)
        {
            sub_node_t *s_temp = m_temp->main_to_sub_link;
            while(s_temp != NULL)
            {
                // Compare the given filename with existing filenames
                if(strcmp(s_temp->f_name, filename) == 0)
                    return SUCCESS;

                s_temp = s_temp->sub_to_sub_link;
            }
            m_temp = m_temp->main_to_main_link;
        }
    }
    return FAILURE;
}

// Insert a word into the database or update its existing entry
int insert_main_to_db(main_node_t **m_head, char *word, char *f_name)
{
    // Allocate memory for a main node and a sub-node
    main_node_t *m_new = malloc(sizeof(main_node_t));
    sub_node_t *s_new = malloc(sizeof(sub_node_t));
    if(!m_new || !s_new)
    {
        free(m_new);
        free(s_new);
        return FAILURE;
    }

    // Create the first main node when the list is empty
    if(*m_head == NULL)
    {   
        // Initialize main node
        m_new->f_count = 1;
        strcpy(m_new->word, word);   
        m_new->main_to_sub_link = s_new;
        m_new->main_to_main_link = NULL;

        // Initialize sub-node
        strcpy(s_new->f_name, f_name);
        s_new->w_count = 1;
        s_new->sub_to_sub_link = NULL;

        *m_head = m_new;
        return SUCCESS;
    }

    // Traverse the main-node list to find the word
    main_node_t *m_temp = *m_head;
    main_node_t *m_prev = NULL;
    while(m_temp != NULL)
    {
        if(!strcmp(m_temp->word, word))
        {
            // Word found: check whether the filename already exists
            sub_node_t *s_temp = m_temp->main_to_sub_link;
            sub_node_t *s_prev = NULL;
            while(s_temp != NULL)
            {
                if(!strcmp(s_temp->f_name,f_name))
                {
                    // Same word in the same file: increment occurrence count
                    s_temp->w_count++;

                    free(m_new);
                    free(s_new);
                    return SUCCESS;
                }
                s_prev = s_temp;
                s_temp = s_temp->sub_to_sub_link;
            }

            // Word exists, but filename is new: add a sub-node
            s_prev->sub_to_sub_link = s_new;
            m_temp->f_count++;

            strcpy(s_new->f_name,f_name);
            s_new->w_count = 1;
            s_new->sub_to_sub_link = NULL;

            return SUCCESS;
        }
        m_prev = m_temp;
        m_temp = m_temp->main_to_main_link;
    }

    // Word not found: insert a new main node at the end
    m_prev->main_to_main_link = m_new;

    m_new->f_count = 1;
    strcpy(m_new->word, word);
    m_new->main_to_sub_link = s_new;
    m_new->main_to_main_link = NULL;

    // Initialize the corresponding sub-node
    strcpy(s_new->f_name, f_name);
    s_new->w_count = 1;
    s_new->sub_to_sub_link = NULL;

    return SUCCESS;
}