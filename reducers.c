//
// Created by flassabe on 26/10/22.
//

#include "reducers.h"

#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "global_defs.h"
#include "utility.h"

/*!
 * @brief add_source_to_list adds an e-mail to the sources list. If the e-mail already exists, do not add it.
 * @param list the list to update
 * @param source_email the e-mail to add as a string
 * @return a pointer to the updated beginning of the list
 */
sender_t *add_source_to_list(sender_t *list, char *source_email) {
    return NULL;
}

/*!
 * @brief clear_sources_list clears the list of e-mail sources (therefore clearing the recipients of each source)
 * @param list a pointer to the list to clear
 */
void clear_sources_list(sender_t *list) {
}

/*!
 * @brief find_source_in_list looks for an e-mail address in the sources list and returns a pointer to it.
 * @param list the list to look into for the e-mail
 * @param source_email the e-mail as a string to look for
 * @return a pointer to the matching source, NULL if none exists
 */
sender_t *find_source_in_list(sender_t *list, char *source_email) {
    return NULL;
}

/*!
 * @brief add_recipient_to_source adds or updates a recipient in the recipients list of a source. It looks for
 * the e-mail in the recipients list: if it is found, its occurrences is incremented, else a new recipient is created
 * with its occurrences = to 1.
 * @param source a pointer to the source to add/update the recipient to
 * @param recipient_email the recipient e-mail to add/update as a string
 */
void add_recipient_to_source(sender_t *source, char *recipient_email) {
}



/*!
 * @brief files_list_reducer is the first reducer. It uses concatenates all temporary files from the first step into
 * a single file. Don't forget to sync filesystem before leaving the function.
 * @param data_source the data source directory (its directories have the same names as the temp files to concatenate)
 * @param temp_files the temporary files directory, where to read files to be concatenated
 * @param output_file path to the output file (default name is step1_output, but we'll keep it as a parameter).
 */
void files_list_reducer(char *data_source, char *temp_files, char *output_file) {
      DIR *dir = opendir(temp_files);
    FILE *output = fopen(output_file, "w+");
    struct dirent *entry ;
    char add[500];
    char buffer[500];
    int compteur=0;
    if(output){
        if(dir){
            entry = readdir(dir);
            while(entry != NULL){
                if(entry->d_type==DT_REG){
                    strcpy(add, temp_files);
                    strcat(add, "/");
                    strcat(add, entry->d_name);
                    FILE *fichier = fopen(add, "r");
                    if (fichier){
                        while(fgets(buffer,499,fichier) != NULL){
                            fprintf(output, "%s" ,buffer);
                            compteur++;
                        }
                    }
                    fclose(fichier);
                    remove(add);
                }
                entry=readdir(dir);
            }
        }
        closedir(dir);
    }
    fclose(output);

    printf("nb ligne saisie : %d \n", compteur);
}


/*!
 * @brief files_reducer opens the second temporary output file (default step2_output) and collates all sender/recipient
 * information as defined in the project instructions. Stores data in a double level linked list (list of source e-mails
 * containing each a list of recipients with their occurrences).
 * @param temp_file path to temp output file
 * @param output_file final output file to be written by your function
 */
void files_reducer(char *temp_file, char *output_file) {
}
