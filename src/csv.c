#include <stdio.h>

#include "csv.h"

int csv_create(){
    printf("Creating csv file\n");
    return 0;
}

int csv_append_line(char * filename, char *line){
    FILE *csv_file = NULL;
    csv_file = fopen(filename, "a");
    if (csv_file == NULL)
    {
        printf("Error! can't open log file.");
        return -1;
    }

    fprintf(csv_file, "%s\n" ,line);
    fclose(csv_file);
    
    return 0;
}
