#ifndef COMMANDS_H
#define COMMANDS_H

#include "fat16.h"

/* list files in fat_bpb */
struct fat_dir *ls(FILE *, struct fat_bpb *);

/* write content to directory */
int write_dir (FILE *, char *, struct fat_dir *);

/* write file content to fat directory */
int write_data(FILE *, char *, struct fat_dir *, struct fat_bpb *);

/* move file from source to destination */
void mv(FILE *, char *, struct fat_bpb *);

#endif
