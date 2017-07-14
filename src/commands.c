#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "commands.h"
#include "fat16.h"
#include "support.h"

off_t fsize(const char *filename){
    struct stat st;
    if (stat(filename, &st) == 0)
        return st.st_size;
    return -1;
}

struct fat_dir find(struct fat_dir *dirs, char *filename, struct fat_bpb *bpb){
    struct fat_dir curdir;
    int dirs_len = sizeof(struct fat_dir) * bpb->possible_rentries;
    int i;

    for (i=0; i < dirs_len; i++){
        if (strcmp((char *) dirs[i].name, filename) == 0){
            curdir = dirs[i];
            break;
        }
    }
    return curdir;
}

struct fat_dir *ls(FILE *fp, struct fat_bpb *bpb){
    int i;
    struct fat_dir *dirs = malloc(sizeof (struct fat_dir) * bpb->possible_rentries);

    for (i=0; i < bpb->possible_rentries; i++){
        uint32_t offset = bpb_froot_addr(bpb) + i * 32;
        read_bytes(fp, offset, &dirs[i], sizeof(dirs[i]));
    }
    return dirs;
}

int write_dir(FILE *fp, char *fname, struct fat_dir *dir){
    char* name = padding(fname);
    strcpy((char *) dir->name, (char *) name);
    if (fwrite(dir, 1, sizeof(struct fat_dir), fp) <= 0)
        return -1;
    return 0;
}

int write_data(FILE *fp, char *fname, struct fat_dir *dir, struct fat_bpb *bpb){

    FILE *localf = fopen(fname, "r");
    int c;

    while ((c = fgetc(localf)) != EOF){
        if (fputc(c, fp) != c)
            return -1;
    }
    return 0;
}

int wipe(FILE *fp, struct fat_dir *dir, struct fat_bpb *bpb){
    int start_offset = bpb_froot_addr(bpb) + (bpb->bytes_p_sect * \
            dir->starting_cluster);
    int limit_offset = start_offset + dir->file_size;

    while (start_offset <= limit_offset){
        fseek(fp, ++start_offset, SEEK_SET);
        if(fputc(0x0, fp) != 0x0)
            return 01;
    }
    return 0;
}

void mv(FILE *fp, char *filename, struct fat_bpb *bpb){
    if (access(filename, F_OK) < 0)
        return;
    struct fat_dir *dirs = ls(fp, bpb); // find empty place to store file.
    int dirs_len = sizeof (struct fat_dir) * bpb->possible_rentries;

    int i;
    uint32_t data_addrs = bpb_froot_addr(bpb);
    struct fat_dir *curdir;

    for (i=0; i < dirs_len; i++){
        data_addrs += bpb->bytes_p_sect;
        if (dirs[i].name[0] == '\0'){
            curdir = &dirs[i]; /* found the first free directory entry */
            curdir->starting_cluster = i + 1;
            break;
        }
        else if (dirs[i].name[0] == DIR_FREE_ENTRY){
            curdir = &dirs[i];
            wipe(fp, curdir, bpb);
            return;
        }
    }

    int dir_addr = bpb_froot_addr(bpb) + i * 32;
    fseek(fp, dir_addr, SEEK_SET);

    off_t filesize = fsize(filename);
    if (filesize > 0){
        curdir->file_size = filesize;
    }
    else {
        return;
    }

    if (write_dir(fp, filename, curdir) < 0)
        return;

    fseek(fp, data_addrs, SEEK_SET);
    if (write_data(fp, filename, curdir, bpb) < 0)
        return;

    free(dirs);
}

void rm(FILE *fp, char *filename, struct fat_bpb *bpb){
    struct fat_dir *dirs = ls(fp, bpb);
    struct fat_dir curdir = find(dirs, filename, bpb);

    curdir.attr = DIR_FREE_ENTRY; /* set deleted flag */
    curdir.name[0] = DIR_FREE_ENTRY; /* set deleted flag */

    int dir_addr = (bpb_froot_addr(bpb) + curdir.starting_cluster * 32) -
        sizeof(struct fat_dir); /* move backwards */

    fseek(fp, dir_addr, SEEK_SET);
    if (fwrite(&curdir, 1, sizeof(struct fat_dir *), fp) != sizeof(struct fat_dir *))
        return;
    // wipe(fp, &curdir, bpb);
}

void cp(FILE *fp, char *filename, struct fat_bpb *bpb){
    ;; /* TODO */
}

