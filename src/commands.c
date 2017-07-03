#include "commands.h"
#include "fat16.h"

void ls(FILE *fp, struct fat_bpb *bpb){
    int i;
    for (i=0; i < bpb->possible_rentries; i++){
        struct fat_dir dir;
        uint32_t offset = bpb_froot_addr(bpb) + i * 32;
        read_bytes(fp, offset, &dir, sizeof(dir));

        if (dir.name[0] == 0)
            break;
        else if (dir.name[0] == 0xE5)
            continue;
        fprintf(stdout, "\n%.*s", (int) (sizeof(dir.name) / sizeof(char)), dir.name);
    }
    fprintf(stdout, "\n");
}

void mv(FILE *fp, char *filename, char *dst, struct fat_bpb *bpb){
    ;; // TODO
}

void verbose(struct fat_bpb *bios_pb){
    int i;

    fprintf(stdout, "Bios parameter block:\n");
    fprintf(stdout, "Jump instruction: ");

    for (i=0; i < 3; i++){
        fprintf(stdout, "%hhX ", bios_pb->jmp_instruction[i]);
    }
    fprintf(stdout, "\n");

    fprintf(stdout, "OEM ID: %s\n", bios_pb->oem_id);
    fprintf(stdout, "Bytes per sector: %d\n", bios_pb->bytes_p_sect);
    fprintf(stdout, "Sector per cluster: %d\n", bios_pb->sector_p_clust);
    fprintf(stdout, "Reserved sector: %d\n", bios_pb->reserved_sect);
    fprintf(stdout, "Number of FAT copies: %d\n", bios_pb->n_fat);
    fprintf(stdout, "Number of possible entries: %d\n", bios_pb->possible_rentries);
    fprintf(stdout, "Small number of sectors: %d\n", bios_pb->snumber_sect);
    fprintf(stdout, "Media descriptor: %hhX\n", bios_pb->media_desc);
    fprintf(stdout, "Sector per fat: %d\n", bios_pb->sect_per_fat);

    fprintf(stdout, "Sector per track: %d\n", bios_pb->sect_per_track);
    fprintf(stdout, "Number of heads: %d\n", bios_pb->number_of_heads);

    fprintf(stdout, "FAT Address: 0x%x\n", bpb_faddress(bios_pb));
    fprintf(stdout, "Root Address: 0x%x\n", bpb_froot_addr(bios_pb));
    fprintf(stdout, "Data Address: 0x%x\n", bpb_fdata_addr(bios_pb));
}

