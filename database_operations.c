#include "hash_headers.h"

// Validate input files and store valid filenames in the file linked list
int read_and_validate(int argc,char *argv[], file_SLL_t **f_head)
{ 
    // Traverse all filenames passed through command-line arguments
    for(int i = 1; i < argc; i++)
    {
        // Extract and validate the file extension
        char *ext = strchr(argv[i], '.');

        if(ext == NULL || strcmp(ext, ".txt") != 0)
        {
            printf("Error : Source file %d is not in .txt format\n",i);
            return FAILURE;
        }

        // Check whether the file exists
        FILE *fptr = fopen(argv[i], "r");
        if(fptr == NULL)
        {
            printf("Error : File does not exisit\n");
            return FAILURE;
        }

        // Check whether the file contains data
        if(!get_file_size(fptr))
        {
            printf("Error : File is empty\n");
            return FAILURE;
        }
        fclose(fptr);

        // Skip the file if it is already present in the file list
        if(search_duplicates(argv[i], f_head))
        {
            printf("Duplicate File Entry: %s\n",argv[i]);
            continue;
        }

        // Insert the validated filename into the file linked list
        if(insert_file(argv[i], f_head) == FAILURE)
        {
            printf("Couldn't able to store the File\n");
            return FAILURE;
        }
    }

    return SUCCESS;
}

// Create the inverted index database using the validated input files
int create_database(int argc, char *argv[], hash_t *database)
{
    if(argc < 2)
    {
        printf("Enter 1 or more .txt files\n");
        return FAILURE;
    }

    // Validate files and maintain their filenames in a linked list
    file_SLL_t *f_head = NULL;
    if(read_and_validate(argc, argv, &f_head) == FAILURE)
        return FAILURE;

    // Traverse the file list and read each file
    file_SLL_t *f_temp = f_head;
    while(f_temp != NULL)
    {
        FILE *fptr = fopen(f_temp->f_name, "r");

        if(fptr == NULL)
            return FAILURE;

        char word[50];

        // Read each word from the current file
        while(fscanf(fptr, "%49s", word) == 1)
        {
            // Calculate the hash index using the first character
            int index = hash_function(word);

            // Insert the word and its filename into the database
            if(insert_main_to_db(&database[index].link, word,
                                 f_temp->f_name) == FAILURE)
            {
                fclose(fptr);
                return FAILURE;
            }
        }

        fclose(fptr);
        f_temp = f_temp->f_to_f_link;
    }

    return SUCCESS;
}

// Display the complete inverted index database
void display_database(hash_t *database)
{
    printf("\n=============================================================================\n");
    printf("\t\t\tDATABASE\n");
    printf("=============================================================================\n");
    printf("%-8s %-15s %-12s %-25s %-12s\n",
           "Index", "Word", "File Count", "File Name", "Word Count");
    printf("=============================================================================\n");

    // Traverse all hash table indexes
    for(int i = 0; i < 28; i++)
    {
        main_node_t *m_temp = database[i].link;

        if(m_temp == NULL)
            continue;

        // Traverse the main-node list at the current index
        while(m_temp != NULL)
        {
            // Reset the flag for every main node
            int first = 1;
            sub_node_t *s_temp = m_temp->main_to_sub_link;

            // Display all files associated with the current word
            while(s_temp != NULL)
            {
                if(first)
                {
                    // Display the main-node details along with the first sub-node
                    printf("%-8d %-15s %-12d %-25s %-12d\n",
                           i, m_temp->word, m_temp->f_count,
                           s_temp->f_name, s_temp->w_count);
                    first = 0;
                }
                else
                {
                    // Display remaining filenames under the same word
                    printf("%-8s %-15s %-12s %-25s %-12d\n",
                           "", "", "", s_temp->f_name, s_temp->w_count);
                }

                s_temp = s_temp->sub_to_sub_link;
            }

            m_temp = m_temp->main_to_main_link;
        }
    }

    printf("\n=============================================================================\n");
}

// Search for a given word in the database
int search_data(hash_t *database)
{
    char word[50];
    printf("Enter the word to search: \n");
    scanf("%49s",word);

    // Calculate the hash index for the search word
    int index = hash_function(word);
    main_node_t *m_temp = database[index].link;

    int found = 0;

    // Traverse the main-node list at the calculated index
    while(m_temp != NULL)
    {
        if(strcmp(m_temp->word,word) == 0)
        {
            // Display the search result header
            printf("\n=============================================================================\n");
            printf("%-8s %-15s %-12s %-25s %-12s\n",
                   "Index", "Word", "File Count", "File Name", "Word Count");
            printf("\n=============================================================================\n");

            sub_node_t *s_temp = m_temp->main_to_sub_link;
            int first = 1;
            found = 1;

            // Display all files containing the searched word
            while(s_temp != NULL)
            {
                if(first)
                {
                    printf("%-8d %-15s %-12d %-25s %-12d\n",
                           index, m_temp->word, m_temp->f_count,
                           s_temp->f_name, s_temp->w_count);
                    first = 0;
                }
                else
                {
                    printf("%-8s %-15s %-12s %-25s %-12d\n",
                           "", "", "", s_temp->f_name, s_temp->w_count);
                }

                s_temp = s_temp->sub_to_sub_link;
            }
        }

        // Stop searching once the word is found
        if(found)
        {
            printf("\n=============================================================================\n");
            return SUCCESS;  
        }

        m_temp = m_temp->main_to_main_link;
    }

    if(!found)
        return FAILURE;
}

// Save the current database into an encoded backup file
int save_database(hash_t *database)
{
    char filename[50];
    printf("Enter backup filename:\n");
    scanf("%45s",filename);

    // Append the .txt extension to the backup filename
    strcat(filename,".txt");
    FILE *fptr = fopen(filename,"w");
    if(fptr == NULL)
    {
        printf("Unable to create backup file\n");
        return FAILURE;
    }

    // Write the backup file header
    fprintf(fptr,"%s","# INVERTED SEARCH DATABASE\n");

    // Traverse the complete hash table
    for(int i = 0; i < 28; i++)
    {
        main_node_t *m_temp = database[i].link;
        while (m_temp != NULL)
        {
            sub_node_t *s_temp = m_temp->main_to_sub_link;
            while(s_temp != NULL)
            {
                char data[200];
                char encoded[400];

                // Prepare the database record using semicolon separators
                sprintf(data, "%d;%s;%d;%s;%d",
                        i,
                        m_temp->word,
                        m_temp->f_count,
                        s_temp->f_name,
                        s_temp->w_count);

                // Apply XOR encoding and convert the result into hexadecimal
                int len = strlen(data);
                encode_decode_data(data,len);
                encode_hex(data,encoded,len);

                // Write the encoded record into the backup file
                fprintf(fptr,"%s\n",encoded);

                s_temp = s_temp->sub_to_sub_link;
            }
            m_temp = m_temp->main_to_main_link;
        }
    } 

    fclose(fptr);
    return SUCCESS;
}

// Load the previously saved database from a backup file
int load_database(hash_t *database)
{
    char filename[50];
    printf("Enter your backup filename:\n");
    scanf("%45s", filename);

    // Append the .txt extension
    strcat(filename, ".txt");

    FILE *fptr = fopen(filename,"r");

    if(fptr == NULL)
    {
        printf("Unable to open backup file\n");
        return FAILURE;
    }

    char line[500];
    char decoded[250];

    // Validate the backup file header
    if(fgets(line,sizeof(line),fptr) == NULL || strcmp(line, "# INVERTED SEARCH DATABASE\n") != 0)
    {
        printf("Invalid backup file\n");
        fclose(fptr);
        return FAILURE;
    }

    // Read each encoded record from the backup file
    while(fgets(line, sizeof(line), fptr) != NULL)
    {
        line[strcspn(line, "\n")] = '\0';

        int len;

        // Convert the hexadecimal record back into bytes
        if(decode_hex(line,decoded, &len) == FAILURE)
        {
            printf("Invalid encoded record\n");
            fclose(fptr);
            return FAILURE;
        }

        // Decode the original data using XOR
        len = strlen(decoded);

        encode_decode_data(decoded, len);

        // Extract individual fields from the decoded record
        char *index_str = strtok(decoded,";");
        char *word = strtok(NULL, ";");
        char *f_count = strtok(NULL,";");
        char *file_name = strtok(NULL,";");
        char *w_count = strtok(NULL,";");

        // Validate the extracted record fields
        if(!index_str || !word || !f_count || !file_name || !w_count)
        {
            printf("Invalid database record\n");
            fclose(fptr);
            return FAILURE;
        }

        // Validate the stored hash index
        int index = atoi(index_str);

        if(index < 0 || index >= hash_size || hash_function(word) != index)
        {
            printf("Invalid databse index\n");
            fclose(fptr);
            return FAILURE;
        }

        // Search for the word in the corresponding hash bucket
        main_node_t *m_temp = database[index].link;

        while(m_temp != NULL && strcmp(m_temp->word, word) != 0)
        {
            m_temp = m_temp->main_to_main_link;
        }

        // Create a new main node if the word does not exist
        if(m_temp == NULL)
        {
            m_temp = malloc(sizeof(main_node_t));

            if(m_temp == NULL)
            {
                fclose(fptr);
                return FAILURE;
            }

            strcpy(m_temp->word,word);
            m_temp->f_count = atoi(f_count);
            m_temp->main_to_sub_link = NULL;

            // Insert the new main node at the beginning of the bucket
            m_temp->main_to_main_link = database[index].link;
            database[index].link = m_temp;
        }

        // Check whether the filename already exists under this word
        sub_node_t *s_temp = m_temp->main_to_sub_link;

        while(s_temp != NULL)
        {
            if(strcmp(s_temp->f_name, file_name) == 0)
            {
                // Update the occurrence count if the filename already exists
                s_temp->w_count = atoi(w_count);
                break;
            }

            s_temp = s_temp->sub_to_sub_link;
        }

        // Insert a new sub-node if the filename is not already present
        if(s_temp == NULL)
        {
            sub_node_t *s_new = malloc(sizeof(sub_node_t));

            if(s_new == NULL)
            {
                fclose(fptr);
                return FAILURE;
            }

            strcpy(s_new->f_name, file_name);
            s_new->w_count = atoi(w_count);

            // Insert the sub-node at the beginning of the sub-node list
            s_new->sub_to_sub_link = m_temp->main_to_sub_link;
            m_temp->main_to_sub_link = s_new;
        }
    }

    fclose(fptr);

    printf("Database loaded successfully\n");
    return SUCCESS;
}

// Load an existing database and add a new source file to it
int update_database(hash_t *database)
{
    // Load the existing database from the backup file
    if(load_database(database) == FAILURE)
    {
        printf("Loading Database Failed\n");
        return FAILURE;
    }

    char new_filename[50];
    printf("Enter a new filename for updation (without mentioning extn):\n");
    scanf("%45s",new_filename);

    // Append the .txt extension to the new filename
    strcat(new_filename,".txt");

    // Check whether the new filename already exists in the database
    if(search_duplicate_filename(database, new_filename) == SUCCESS)
    {
        printf("Duplicate file found\n");
        return FAILURE;
    }

    // Open and validate the new source file
    FILE *fptr = fopen(new_filename, "r");
    if(fptr == NULL)
    {
        printf("Error : File does not exisit\n");
        return FAILURE;
    }

    // Check whether the new source file is empty
    if(!get_file_size(fptr))
    {
        printf("Error : File is empty\n");
        return FAILURE;
    }

    char word[50];

    // Read each word and insert it into the existing database
    while(fscanf(fptr, "%49s", word) == 1)
    {
        int index = hash_function(word);

        if(insert_main_to_db(&database[index].link, word,
                             new_filename) == FAILURE)
        {
            fclose(fptr);
            return FAILURE;
        }
    }

    fclose(fptr);

    return SUCCESS;
}