#include "hash_headers.h"

int main(int argc, char *argv[])
{
    int choice;
    hash_t database[hash_size];

    // Initialize all hash table entries
    for(int i = 0; i < hash_size; i++)
    {
        database[i].index = i;
        database[i].link = NULL;
    }

    // Display menu until the user chooses to exit
    do
    {
        printf("\n======================================\n");
        printf("\t INVERTED SEARCH\n");
        printf("======================================\n");
        printf("1. Create Database\n"
                "2. Display Databse\n"
                "3. Search Data\n"
                "4. Update Database\n"
                "5. Save Database\n"
                "6. Exit\n"
                "\nEnter your choice: \n");


        scanf("%d",&choice);

        // Perform the selected database operation
        switch (choice)
        {
            case 1:
                // Create database using the input files passed through command-line arguments
                if(create_database(argc,argv,database) == FAILURE)
                {
                    printf("Error: Couldn't able to create DATABASE\n");
                    break;
                }
                printf("Database created successfully\n");
                break;

            case 2:
                // Display all words and their file details
                display_database(database);
                break;

            case 3:
                // Search for a word in the database
                if(search_data(database) == FAILURE)
                {
                    printf("Word not found in database\n");
                    break;
                }
                break;

            case 4:
                // Load an existing database and add a new file
                if(update_database(database) == FAILURE)
                {
                    printf("Updation Failed\n");
                    break;
                }
                printf("Database updated successfully\n");
                break;

            case 5:
                // Save the current database into a backup file
                if(save_database(database) == FAILURE)
                {
                    printf("Couldn't able to save your database\n");
                    break;
                }
                printf("Database saved successfully\n");
                break;
            
            case 6:
                printf("Terminating Program\n");
                return FAILURE;

            default:
                printf("Enter Valid choice from 1 to 6\n");
                break;
        }
    } while (choice != 6);  
}