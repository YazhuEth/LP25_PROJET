//
// Created by flassabe on 14/10/22.
//

#include "configuration.h"

#include <getopt.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#include "utility.h"
//
// Created by flassabe on 14/10/22.
//

#include "configuration.h"

#include <getopt.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "utility.h"

/*!
 * @brief make_configuration makes the configuration from the program parameters. CLI parameters are applied after
 * file parameters. You shall keep two configuration sets: one with the default values updated by file reading (if
 * configuration file is used), a second one with CLI parameters, to overwrite the first one with its values if any.
 * @param base_configuration a pointer to the base configuration to be updated
 * @param argv the main argv
 * @param argc the main argc
 * @return the pointer to the updated base configuration
 */
configuration_t *make_configuration(configuration_t *base_configuration, char *argv[], int argc) {
    int opt;
    FILE *file;
    while((opt=getopt(argc,argv,":if:fdtovc"))!=-1){
        switch(opt){
            case 'f':
                file=fopen(optarg,"r");
                if(file!=NULL){
                    read_cfg_file(base_configuration,optarg);
                }
                break;
            case 'd':
                strcat(base_configuration->data_path,argv[optind]);
                break;
            case 't':
                strcat(base_configuration->temporary_directory,argv[optind]);
                break;
            case 'o':
                strcat(base_configuration->output_file,argv[optind]);
                break;
            case 'v':
                if(strstr(argv[optind],"on")!=NULL){ 
                    base_configuration->is_verbose=true;
                }
                break;
            case 'c':
                //base_configuration->cpu_core_multiplier=argv[optind];
                break;
        }
    }
    return base_configuration;
}

/*!
 * @brief skip_spaces advances a string pointer to the first non-space character
 * @param str the pointer to advance in a string
 * @return a pointer to the first non-space character in str
 */
char *skip_spaces(char *str) {
    char *skip=(char*)malloc(sizeof(char));
    str=strchr(str,' ');
    skip=strtok(str," ");
    str=skip;
    return str;
}

/*!
 * @brief check_equal looks for an optional sequence of spaces, an equal '=' sign, then a second optional sequence
 * of spaces
 * @param str the string to analyze
 * @return a pointer to the first non-space character after the =, NULL if no equal was found
 */
char *check_equal(char *str) {
    char *equal=strstr(str,"=");
    char *word=(char*)malloc(sizeof(char));
    if(!equal){
        return NULL;
    }
    str=strchr(str,'=');
    word=strtok(str,"=");
    str=word; //retour de tout ce qu'il suit
    return str;
}

/*!
 * @brief get_word extracts a word (a sequence of non-space characters) from the source
 * @param source the source string, where to find the word
 * @param target the target string, where to copy the word
 * @return a pointer to the character after the end of the extracted word
 */
char *get_word(char *source, char *target) {
    char head=target[0];
    char tail={target[strlen(target)-1]};
    char *word=strstr(source,target);
    if(word!=NULL){
        word=strchr(word,' ');
    }
    source=word;
    return source;
}

/*!
 * @brief read_cfg_file reads a configuration file (with key = value lines) and extracts all key/values for
 * configuring the program (data_path, output_file, temporary_directory, is_verbose, cpu_core_multiplier)
 * @param base_configuration a pointer to the configuration to update and return
 * @param path_to_cfg_file the path to the configuration file
 * @return a pointer to the base configuration after update, NULL is reading failed.
 */
configuration_t *read_cfg_file(configuration_t *base_configuration, char *path_to_cfg_file) {
    FILE *file=fopen(path_to_cfg_file,"r");
    char *d_path,*tmp_dir,*output,*verbose,*cpu;
    if(file!=NULL){
        while(fgetc(file)!=EOF){
            char buffer[500];
            fgets(buffer,500,file);        
            d_path=get_word(buffer,"ata_path");
            if(d_path!=NULL){
                d_path=check_equal(d_path);
                d_path=skip_spaces(d_path);
                strcpy(base_configuration->data_path,d_path);
            }
            buffer[500];
            tmp_dir=get_word(buffer,"emporary_directory");
            if(tmp_dir!=NULL){
                tmp_dir=check_equal(tmp_dir);
                tmp_dir=skip_spaces(tmp_dir);
                strcpy(base_configuration->temporary_directory,tmp_dir);
            }
            buffer[500];
            output=get_word(buffer,"utput_file");
            if(output!=NULL){
                output=check_equal(output);
                output=skip_spaces(output);
                strcpy(base_configuration->output_file,output);
            }
            buffer[500];
            verbose=get_word(buffer,"s_verbose");
            if(verbose!=NULL){
                verbose=check_equal(verbose);
                verbose=skip_spaces(verbose);
                if(strstr(verbose,"yes")!=NULL){ //marche pas
                    base_configuration->is_verbose=1;
                }
            }
            buffer[500];
            cpu=get_word(buffer,"pu_core_multiplier");
            if(cpu!=NULL){
                cpu=check_equal(cpu);
                cpu=skip_spaces(cpu);
                int cpu_number=atoi(cpu);
                base_configuration->cpu_core_multiplier=cpu_number;
                fclose(file);
            }
        }
    }else{
        perror("Failed opening file: ");
    }
    return base_configuration;
}

/*!
 * @brief display_configuration displays the content of a configuration
 * @param configuration a pointer to the configuration to print
 */
void display_configuration(configuration_t *configuration) {
    printf("Current configuration:\n");
    printf("\tData source: %s\n", configuration->data_path);
    printf("\tTemporary directory: %s\n", configuration->temporary_directory);
    printf("\tOutput file: %s\n", configuration->output_file);
    printf("\tVerbose mode is %s\n", configuration->is_verbose?"on":"off");
    printf("\tCPU multiplier is %d\n", configuration->cpu_core_multiplier);
    printf("\tProcess count is %d\n", configuration->process_count);
    printf("End configuration\n");
}

/*!
 * @brief is_configuration_valid tests a configuration to check if it is executable (i.e. data directory and temporary
 * directory both exist, and path to output file exists @see directory_exists and path_to_file_exists in utility.c)
 * @param configuration the configuration to be tested
 * @return true if configuration is valid, false else
 */
bool is_configuration_valid(configuration_t *configuration) {
    if(!directory_exists(configuration->data_path)){
        printf("data:%d %s\n",directory_exists(configuration->data_path),configuration->data_path);
        return false;
    }
    if(!directory_exists(configuration->temporary_directory)){
        printf("temp:%d\n",directory_exists(configuration->temporary_directory));
        return false;
    }
    if(!path_to_file_exists(configuration->output_file)){
        printf("output:%d\n",directory_exists(configuration->output_file));
        return false;
    }
    return true;
}
