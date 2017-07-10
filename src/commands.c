#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include "commands.h"
#include "fat16.h"


struct fat_dir *find(struct fat_dir *dirs, char *filename, struct fat_bpb *bpb){
    struct fat_dir *curdir = malloc(sizeof(struct fat_dir));
    int dirs_len = sizeof(struct fat_dir) * bpb->possible_rentries;
    int i;

    for (i=0; i < dirs_len; i++){
        if (strcmp((char *) dirs[i].name, filename) == 0){
            curdir = &dirs[0];
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
    strncpy((char *) dir->name, (char *) fname, sizeof(fname));

    if (fwrite(dir, 1, sizeof(struct fat_dir), fp) <= 0)
        return -1;
    return 0;
}

int write_data(FILE *fp, char *fname, struct fat_dir *dir, struct fat_bpb *bpb){

    FILE *localf = fopen(fname, "r");
    int *c;
    int start_offset = bpb_froot_addr(bpb) + (bpb->bytes_p_sect * \
            dir->first_clust_low);

    while ((*c = fgetc(localf)) != EOF){
        fseek(fp, start_offset, SEEK_SET);
        if (fwrite((char *) c, 1, sizeof(c), fp) != sizeof(c))
            return -1;
        start_offset += 1;
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
            curdir->first_clust_low = i + 1;
            break;
        }
    }
    int dir_addr = bpb_froot_addr(bpb) + i * 32;
    fseek(fp, dir_addr, SEEK_SET);

    if (write_dir(fp, filename, curdir) < 0)
        return;
    if (write_data(fp, filename, curdir, bpb) < 0)
        return;

    free(dirs);
}

void rm(FILE *fp, char *filename, struct fat_bpb *bpb){
    struct fat_dir *dirs = ls(fp, bpb);
    struct fat_dir *curdir = find(dirs, filename, bpb);
    if (!curdir)
        return;

    curdir->attr = DIR_FREE_ENTRY; /* set deleted flag */
    curdir->name[0] = DIR_FREE_ENTRY; /* set deleted flag */

    int dir_addr = bpb_froot_addr(bpb) + curdir->first_clust_low * 32;
    fseek(fp, dir_addr, SEEK_SET);
    if (fwrite(curdir, 1, sizeof(struct fat_dir *), fp) != sizeof(struct fat_dir *))
        return;

    free(dirs);
    free(curdir);
}

void cp(FILE *fp, char *filename, struct fat_bpb *bpb){
    ;; /* TODO */
}

