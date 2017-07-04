#ifndef COMMANDS_H
#define COMMANDS_H

#include "fat16.h"

/* list files in fat_bpb */
struct fat_dir *ls(FILE *, struct fat_bpb *);

/* move file from source to destination */
void mv(FILE *, char *, char *, struct fat_bpb *);

#endif
