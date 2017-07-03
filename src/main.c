#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fat16.h"
#include "commands.h"

/* prototypes */
void usage(char *);

void usage(char *executable){
    fprintf(stdout, "Usage:\n");
    fprintf(stdout, "\t%s, <fat16-img>\n", executable);
    fprintf(stdout, "\t       -h | --help for help\n");
    fprintf(stdout, "\tfat16-img needs to be a valid Fat16.\n\n");
    fprintf(stdout, "Author: Benjamin Mezger, 2017\n");
}

int main(int argc, char **argv){
    if (argc <= 1){
        usage(argv[0]);
        exit(1);
    }

    if (argc == 2 && (strcmp(argv[1], "-h") == 0 ||
                strcmp(argv[1], "--help") == 0)){
        usage(argv[0]);
        exit(0);
    }
    else if (argc >= 3 || argc >= 4){
        FILE *fp = fopen(argv[2], "rb");
        if (!fp){
            fprintf(stdout, "Could not open file %s\n", argv[2]);
            exit(1);
        }

        struct fat_bpb bpb;
        rfat(fp, &bpb);
        //if (!bpb){
        //    fprintf(stdout, "Could not read Bios parameter boot.\n");
        //    exit(1);
        //}
        char *command = argv[1];
        if (strcmp(command, "ls") == 0){
            ls(fp, &bpb);
        }
    }

    return 0;
}
