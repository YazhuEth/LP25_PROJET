//
// Created by flassabe on 26/10/22.
//

#include "analysis.h"

#include <dirent.h>
#include <stddef.h>
#include <unistd.h>
#include <libgen.h>
#include <string.h>
#include <fcntl.h>
#include <ctype.h>
#include <stdlib.h>
#include <sys/file.h>

#include "utility.h"


/*!
 * @brief parse_dir parses a directory to find all files in it and its subdirs (recursive analysis of root directory)
 * All files must be output with their full path into the output file.
 * @param path the path to the object directory
 * @param output_file a pointer to an already opened file
 */
void parse_dir(char *path, FILE *output_file)
{

    DIR *dir = opendir(path);
    struct dirent *entry;
    char temp[1024];

    if (dir)
    {
        entry = readdir(dir);
        while (entry != NULL)
        {
            if (strcmp(entry->d_name, ".") && strcmp(entry->d_name, ".."))
            {
                if (entry->d_type == DT_DIR)
                {
                    strcpy(temp, path);
                    strcat(temp, "/");
                    strcat(temp, entry->d_name);
                    parse_dir(temp, output_file);
                }
                if (entry->d_type == DT_REG)
                {
                    fprintf(output_file, "%s/%s\n", path, entry->d_name);
                }
            }
            entry = readdir(dir);
        }
        closedir(dir);
    }
}

/*!
 * @brief clear_recipient_list clears all recipients in a recipients list
 * @param list the list to be cleared
 */
void clear_recipient_list(simple_recipient_t *list)
{
    while (list != NULL)
    {

        int i = 0;
        while (i < STR_MAX_LEN)
        {
            list->email[i] = '\0';
            i++;
        }
        list = list->next;
    }
}

/*!
 * @brief add_recipient_to_list adds a recipient to a recipients list (as a pointer to a recipient)
 * @param recipient_email the string containing the e-mail to add
 * @param list the list to add the e-mail to
 * @return a pointer to the new recipient (to update the list with)
 */
simple_recipient_t *add_recipient_to_list(char *recipient_email, simple_recipient_t *list)
{
    simple_recipient_t *temp = (simple_recipient_t *)malloc(sizeof(simple_recipient_t));
    strcpy(temp->email, recipient_email);
    temp->next = list;
    return temp;
}

/*!
 * @brief extract_emails extracts all the e-mails from a buffer and put the e-mails into a recipients list
 * @param buffer the buffer containing one or more e-mails
 * @param list the resulting list
 * @return the updated list
 */
simple_recipient_t *extract_emails(char *buffer, simple_recipient_t *list)
{
    // 1. Check parameters
    // 2. Go through buffer and extract e-mails
    // 3. Add each e-mail to list
    // 4. Return list
}

/*!
 * @brief extract_e_mail extracts an e-mail from a buffer
 * @param buffer the buffer containing the e-mail
 * @param destination the buffer into which the e-mail is copied
 */
void extract_e_mail(char buffer[], char destination[])
{
    strcpy(destination, buffer);
}

// Used to track status in e-mail (for multi lines To, Cc, and Bcc fields)
typedef enum
{
    IN_DEST_FIELD,
    OUT_OF_DEST_FIELD
} read_status_t;

/*!
 * @brief parse_file parses mail file at filepath location and writes the result to
 * file whose location is on path output
 * @param filepath name of the e-mail file to analyze
 * @param output path to output file
 * Uses previous utility functions: extract_email, extract_emails, add_recipient_to_list,
 * and clear_recipient_list
 */
void parse_file(char *filepath, char *output)
{
    // 1. Check parameters
    // 2. Go through e-mail and extract From: address into a buffer
    // 3. Extract recipients (To, Cc, Bcc fields) and put it to a recipients list.
    // 4. Lock output file
    // 5. Write to output file according to project instructions
    // 6. Unlock file
    // 7. Close file
    // 8. Clear all allocated resources

    FILE *f_in = fopen(filepath, "r");
    FILE *f_out = fopen(output, "a");

    char from[1024];
    char buffer[STR_MAX_LEN];
    simple_recipient_t *list_mails = NULL;

    if (f_in == NULL || f_out == NULL)
    {
        printf("erreur\n");
        exit(1);
    }

    fgets(from, 7, f_in);
    while (strcmp(from, "From: ") != 0)
    {
        fgets(from, 7, f_in);
    }
    fgets(from, STR_MAX_LEN - 1, f_in);
    printf("%s", from);

    fgets(buffer, STR_MAX_LEN - 1, f_in);

    char d[] = " ,\n\t";
    char *p = strtok(buffer, d);

    while (strcmp(p, "Subject:") != 0)
    {
        while (p != NULL)
        {
            if (strcmp(p, "To:") == 0 || strcmp(p, "Cc:") == 0 || strcmp(p, "Bcc:") == 0 || strlen(p) < 2)
            {
                p = strtok(NULL, d);
            }
            else
            {
                /// retire les ,
                char *adr_retour_ligne;
                adr_retour_ligne = strpbrk(p, ", "); /* Recherche de l'adresse d'un \n dans la variable chaine */
                if (adr_retour_ligne != NULL)        /* Adresse trouvée ? */
                {
                    *adr_retour_ligne = 0; /* Remplacement du caractère par un octet nul (fin de chaîne en C) */
                }

                /// retire les tabulations
                char *adr_tab = strpbrk(p, "\t"); /* Recherche de l'adresse d'un \n dans la variable chaine */
                if (adr_tab != NULL)              /* Adresse trouvée ? */
                {
                    *adr_tab = "."; /* Remplacement du caractère par un octet nul (fin de chaîne en C) */
                }

                printf("%s\n", p);
                list_mails = add_recipient_to_list(p, list_mails);
                p = strtok(NULL, d);
            }
        }

        fgets(buffer, STR_MAX_LEN - 1, f_in);
        p = strtok(buffer, d);
    }

    list_mails = add_recipient_to_list(from, list_mails);
    simple_recipient_t *Q = list_mails;

    flock(f_out, LOCK_EX);

    while (Q != NULL)
    {
        
    char *adr_retour_ligne;
    adr_retour_ligne = strpbrk(Q->email, "\r"); /* Recherche de l'adresse d'un \n dans la variable chaine */
    if (adr_retour_ligne != NULL)           /* Adresse trouvée ? */
    {
        *adr_retour_ligne = 0; /* Remplacement du caractère par un octet nul (fin de chaîne en C) */
    }

        fprintf(f_out, "%s ", Q->email);
        Q = Q->next;
    }

    fprintf(f_out,"\n");

    flock(f_out, LOCK_UN);

    fclose(f_in);
    clear_recipient_list(list_mails);
}

/*!
 * @brief process_directory goes recursively into directory pointed by its task parameter object_directory
 * and lists all of its files (with complete path) into the file defined by task parameter temporary_directory/name of
 * object directory
 * @param task the task to execute: it is a directory_task_t that shall be cast from task pointer
 * Use parse_dir.
 */
void process_directory(char *object_directory, char *temp_files, char *user)
{
    char adresse_f_user[500];
    strcpy(adresse_f_user, temp_files);
    strcat(adresse_f_user, "/");
    strcat(adresse_f_user, user);

    FILE *fichier = fopen(adresse_f_user, "w");

    if (fichier)
    {
        parse_dir(object_directory, fichier);
    }

    fclose(fichier);
}

/*!
 * @brief process_file processes one e-mail file.
 * @param task a file_task_t as a pointer to a task (you shall cast it to the proper type)
 * Uses parse_file
 */

void process_file(char *object_file, char *temporary_directory)
{
    // 1. Check parameters
    // 2. Build full path to all parameters
    // 3. Call parse_file

    char file_temp[1024];
    strcpy(file_temp, temporary_directory);
    strcat(file_temp, "/output");

    parse_file(object_file, file_temp);
}
