#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

typedef struct {
    char name[21];
    int gold;
    int silver;
    int bronze;
} Country;

int count_elements(const Country *database) {
    /* Returns the amount of elements in the database, the
    last country, a.k.a. the dummy country, is included. */
    int i = 0;
    while (strcmp(database[i].name, "\0")) { i++; }
    return i+1; // There is always at least the dummy country.
}

int is_in_database(const Country *database, const char *srch) {
    // Returns 1 if country 'srch' in database.
    for (int i = 0; i < count_elements(database)-1; i++) {
        if (!strcmp(database[i].name, srch))
            return 1;
    }
    return 0;
}

int compare_results(const Country *a, const Country *b) {
    /* Returns 1 if country a has had a greater succes in the olympics,
    -1 if country b has had more succes, or 0 if the results are equal.
    The comparison works with the dummy country as well, the dummy country
    will lose the comparson every time since it has a negative medal count. */
    if (a->gold != b->gold) {
        if (a->gold > b->gold)
            return 1;
        else
            return -1;
    } else if (a->silver != b->silver) {
        if (a->silver > b->silver)
            return 1;
        else
            return -1;
    } else if (a->bronze != b->bronze) {
        if (a->bronze > b->bronze)
            return 1;
        else
            return -1;
    } else {
        return 0;
    }
}

Country *add_new_country(Country *database, const char *input) {

    if (strlen(input) > 23) {
        printf("Input too long, the country name should be 20 characters at max.\n");
        printf("Failed to add a new country to the database.\n");
        return database;
    }

    char new_country_name[21] = {0};
    int ret = sscanf(input, "%*s %s", new_country_name);
    if (ret < 1) {
        printf("Invalid input: %s", input);
        printf("Failed to add a new country to the database.\n");
        return database;
    } else if (is_in_database(database, new_country_name)) {
        printf("Country '%s' is already in the database!\n", new_country_name);
        return database;
    }

    int i = count_elements(database);
    database = realloc(database, (i+1)*sizeof(Country));

    // Move the dummy country to the last index in database.
    memset(&database[i], 0, sizeof(Country));
    memcpy(&database[i], &database[i-1], sizeof(Country));

    // The previously dummy country becomes the new country.
    memset(&database[i-1], 0, sizeof(Country));
    memcpy(database[i-1].name, new_country_name, 21);
    database[i-1].gold = 0;
    database[i-1].silver = 0;
    database[i-1].bronze = 0;

    printf("Added country '%s' to the database succesfully.\n", new_country_name);

    return database;
}

void update_medal_count(Country *database, const char *input) {

    int i = 0;
    char a = input[i];
    int count = -1;
    // Count the characters in the country name.
    while (a != '\0') {
        if (a == ' ') {
            if (count == -1) { count = 0; }
            else { break; }
        } else if (count != -1) {
            count++;
        }
        i++;
        a = input[i];
    } if (count > 20) {
        printf("Input too long, the country name should be 20 characters at max.\n");
        printf("Failed to update the medal count.\n");
        return;
    }

    char country_name[21] = {0};
    int golds;
    int silvers;
    int bronzes;

    // With correct input, garbage should be left unitialized.
    int ret = sscanf(input, "%*c %s %d %d %d", country_name, &golds, &silvers, &bronzes);
    if (ret < 4) {
        printf("Invalid input: %s", input);
        printf("Failed to update the medal count.\n");
        return;
    } else if (!is_in_database(database, country_name)) {
        printf("Country '%s' not in the database.\n", country_name);
        printf("Failed to update the medal count.\n");
        return;
    } else if (!golds && !silvers && !bronzes) {
        printf("There are no medals to add/reduct!\n");
        return;
    }

    i = 0;
    // Find the right index for country 'country_name'.
    while (strcmp(database[i].name, country_name)) { i++; }

    int modified = 0;
    database[i].gold += golds;
    if (database[i].gold < 0) {
        printf("%s's gold medal count can't be negative, gold medal count set to 0.\n", country_name);
        database[i].gold = 0;
        modified = 1;
    } database[i].silver += silvers;
    if (database[i].silver < 0) {
        printf("%s's silver medal count can't be negative, silver medal count set to 0.\n", country_name);
        database[i].silver = 0;
        modified = 1;
    } database[i].bronze += bronzes;
    if (database[i].bronze < 0) {
        printf("%s's bronze medal count can't be negative, bronze medal count set to 0.\n", country_name);
        database[i].bronze = 0;
        modified = 1;
    }

    /* Now that the medal count has been updated, the database might have to be reorganized.
    The country with the most success should be at index 0 and so on. */
    int j = 0;
    int rank_increases = 1;
    int is_better;
    while (j < count_elements(database)-1) {
        // Loop while the countries counting from the top have better results.
        is_better = compare_results(&database[j], &database[i]);
        if (is_better == 1) {
            // Country at index j is better than the updated country.
        } else if (is_better == 0) {
            // The results are equal.
            if (j == i) {
                // The country was just compared with itself.
                rank_increases = 0;
                if (compare_results(&database[j+1], &database[i]) != 1) {
                    /* If the country right below the updated country is doing worse
                    or has equal success, the rank remains unchanged. The algorithm is stable. */
                    break;
                }
            } else if (!rank_increases) {
                /* If the rank is already decreasing, don't bring the updated
                country any lower than necessary (mimicking stability). */
                break;
            }
        } else {
            // A suitable spot for the updated country has been found.
            break;
        }
        j++;
    }

    // Index i tells where the updated country is and index j where it should be.
    if (i == j) {
        if (modified) {
            printf("Updated %s's medal count with some exceptions.\n", country_name);
        } else {
            printf("Updated %s's medal count succesfully.\n", country_name);
        } return;
    }

    Country *to_move = malloc(sizeof(Country));
    memcpy(to_move, &database[i], sizeof(Country));
    int k = i;

    if (rank_increases) {
        // The countries from index j to index i are demoted one rank.
        while (k != j) {
            memcpy(&database[k], &database[k-1], sizeof(Country));
            k--;
        }
    } else {
        j--; /* The index j shouls be one smaller to the, since the
        countries from index i to index j are promoted one rank. */
        while (k != j) {
            memcpy(&database[k], &database[k+1], sizeof(Country));
            k++;
        }
    }

    memcpy(&database[j], to_move, sizeof(Country));
    free(to_move);

    if (modified) {
        printf("Updated %s's medal count with some exceptions.\n", country_name);
    } else {
        printf("Updated %s's medal count succesfully.\n", country_name);
    }
}

void print_results(const Country *database) {
    // Function 'update_medal_count' prioritizes the countries according to their success.
    if (count_elements(database) == 1) {
        // Only the dummy country.
        printf("Database empty!\n");
        return;
    }
    printf("|----------------------|-------------|---------------|---------------|\n");
    printf("| %-20s | %-11s | %-12s | %-12s |\n", "Country", "Gold medals", "Silver medals", "Bronze medals");
    printf("|----------------------|-------------|---------------|---------------|\n");
    for (int i = 0; i < count_elements(database)-1; i++) {
        printf("| %-20s | %-11d | %-13d | %-13d |\n", database[i].name, database[i].gold, database[i].silver, database[i].bronze);
    }
    printf("|----------------------|-------------|---------------|---------------|\n");
}

void write_results(const Country *database, const char *input) {

    if (strlen(input) > 23) {
        printf("Input too long, filename should be 20 characters at max.\n");
        printf("No results written to the file.\n");
        return;
    } else if (count_elements(database) == 1) {
        printf("No data in the database.\n");
        printf("No results written to the file.\n");
        return;
    }

    char filename[21] = {0};
    int ret = sscanf(input, "%*s %s", filename);
    if (ret < 1) {
        printf("Failed to extract the filename: %s\n", input);
        printf("No results written to the file.\n");
        return;
    }

    FILE *fp = fopen(filename, "w");
    if (!fp) {
        printf("Failed to open the file '%s'\n", filename);
        printf("No results written to the file.\n");
        return;
    }

    int num_of_elements = count_elements(database);
    // The first sizeof(int) bytes will tell how many countries are listed.
    ret = fwrite(&num_of_elements, sizeof(int), 1, fp);
    if (!ret) {
        printf("Something went wrong while writing to file '%s'.\n", filename);
        fclose(fp);
        return;
    }
    ret = fwrite(database, sizeof(Country), num_of_elements, fp);
    if (ret < num_of_elements) {
        printf("Something went wrong while writing to file '%s'.\n", filename);
        fclose(fp);
        return;
    }

    fclose(fp);
    printf("Data written to file '%s' succesfully!\n", filename);
}

Country *download_results(Country *database, const char *input) {

    if (strlen(input) > 23) {
        printf("Input too long, filename should be 20 characters at max.\n");
        printf("No results downloaded from the file.\n");
        return database;
    }

    char filename[21] = {0};
    int ret = sscanf(input, "%*s %s", filename);
    if (ret < 1) {
        printf("Failed to extract the filename: %s\n", input);
        printf("No results downloaded from the file.\n");
        return database;
    }

    FILE *fp = fopen(filename, "r");

    int count;
    /* The first sizeof(int) bits represent an integer that
    tells the amount of countries in the file (dummy included). */
    ret = fread(&count, sizeof(int), 1, fp);
    if (!ret) {
        printf("Something went wrong while reading file '%s'.\n", filename);
        printf("No results downloaded from the file.\n");
        fclose(fp);
        return database;
    } else if (count == 1) {
        printf("No data in file '%s'.\n", filename);
        printf("No results downloaded from the file.\n");
        return database;
    }

    // Create a copy first, if something goes wrong, the copy is returned.
    Country *copy = malloc(count_elements(database)*sizeof(Country));
    for (int i = 0; i < count_elements(database); i++) {
        memcpy(&copy[i], &database[i], sizeof(Country));
    }
    // A temporary placeholder for one country.
    Country *temp = malloc(sizeof(Country));

    database = realloc(database, count*sizeof(Country));
    for (int i = 0; i < count; i++) {
        ret = fread(temp, sizeof(Country), 1, fp);
        if (!ret) {
            printf("Something went wrong while reading file '%s'.\n", filename);
            printf("No results downloaded from the file.\n");
            fclose(fp);
            return copy;
        }
        memcpy(&database[i], temp, sizeof(Country));
    }

    free(temp);
    free(copy);
    fclose(fp);

    printf("A total of %d countries' results succesfully downloaded from file '%s'.\n", count-1, filename);

    return database;
}

int main(void) {

    char buffer[101];
    char option;
    char check;
    char *ptr;

    /* Initialize the database with a dummy country, this
    will mark the last element in the product array database */
    Country *database = malloc(sizeof(Country));
    memset(database, 0, sizeof(Country));
    database[0].gold = -1;
    database[0].silver = -1;
    database[0].bronze = -1;

    printf("\n|----------------------------------------------|\n");
    printf("| Welcome to the olympic games medal database! |\n");
    printf("|----------------------------------------------|\n");

    int go_on = 1;

    while (go_on) {
        // Keep looping until the command to quit (Q) is given.
        printf("\nChoose one of the following options:\n");
        printf("A 'country' -> Add a new country to the database.\n");
        printf("M 'country' 'new gold medals' 'new silver medals' 'new bronze medals' -> Update the medal count of a country.\n");
        printf("L -> Print the results.\n");
        printf("W 'filename' -> Write all the current data to a file.\n");
        printf("O 'filename' -> Download a file and overwrite the current data.\n");
        printf("Q -> Quit.\n");

        ptr = fgets(buffer, 101, stdin);
        if (ptr == NULL) {
            printf("The end of the input reached, program terminating.\n");
            break;
        }

        /* Option represents the character at index 0 in buffer.
        If the character at index 1 is not white space, the input is invalid. */
        sscanf(buffer, "%c", &option);
        sscanf((buffer+1), "%c", &check);
        if (!(isspace(check))) {
            // This leads to the default case, invalid input.
            option = 'X';
        }

        switch (option) {
            /* The first character typically decides which course of action is taken.
            The option is validated in the main function,
            the rest of the input is validated in other functions. */
            case 'A':
                // Try to add a new country to the database.
                database = add_new_country(database, buffer);
                break;
            case 'M':
                // Try to update the medal count of the chosen country.
                update_medal_count(database, buffer);
                break;
            case 'L':
                // Print the current information.
                print_results(database);
                break;
            case 'W':
                // Write the current information to a file.
                write_results(database, buffer);
                break;
            case 'O':
                // Download information from a file.
                database = download_results(database, buffer);
                break;
            case 'Q':
                // Quit the program.
                go_on = 0;
                printf("Program terminating.\n");
                break;
            default:
                /* Ask the user for another input, this can also happen due to the
                second character in the buffer being something else than white space. */
                printf("Invalid option given: %s", buffer);
                break;
        }
    }

    // Free the allocated memory.
    free(database);

    return 0;
}
