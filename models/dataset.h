/*
 * dataset.h
 *
 */

#ifndef _DATASET_H
#define _DATASET_H

#define MAX_DATASET_ENTRIES 20000

struct st_dataset {
  char *label_name;
  char *label_description;
  int   label_id;
  int   num_files;
  char **filenames;
};

// global variables
extern int num_dataset_entries;
extern struct st_dataset aDataset[MAX_DATASET_ENTRIES];

// function prototypes
void load_dataset();

#endif
