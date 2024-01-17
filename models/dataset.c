/*
 * dataset.c
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

#include "main.h"
#include "dataset.h"
#include "utils.h"

// global variables
int num_dataset_entries;
struct st_dataset aDataset[MAX_DATASET_ENTRIES];

void fn_process_label_line(char *line, size_t len, int label_id) {
  char item[200];
  char name[200];
  int offset = 0;

  // first we get the label name
  offset = fn_get_item_line_space(line, len, offset, name);
  int d = num_dataset_entries;
  aDataset[d].label_name = (char *)malloc(sizeof(char) * (strlen(name)+1));
  strcpy(aDataset[d].label_name, name);
  // description
  aDataset[d].label_description = (char *)malloc(sizeof(char) * (strlen(line)-strlen(name)+2));
  strcpy(aDataset[d].label_description, &line[strlen(name)]);
  // label id
  aDataset[d].label_id = label_id;

  // now we search all the files for that label
  // first we compute how many files exist for this label
  int num_files = 0;
  DIR *_d;
  struct dirent *dir;
  char dirlabel[250];
  sprintf(dirlabel, "%s%s", data_directory, aDataset[d].label_name);
  _d = opendir(dirlabel);
  if (_d) {
    while ((dir = readdir(_d)) != NULL) {
      if (strstr(dir->d_name, ".jpg") || strstr(dir->d_name, ".JPEG")) num_files++;
    }
    closedir(_d);
  }
  aDataset[d].num_files = num_files;
  aDataset[d].filenames = (char **)malloc(sizeof(char *) * num_files);
  // now we store every file (by searching again)
  _d = opendir(dirlabel);
  int f = 0;
  if (_d) {
    while ((dir = readdir(_d)) != NULL) {
      if (strstr(dir->d_name, ".jpg") || strstr(dir->d_name, ".JPEG")) {
	char filename[350];
	sprintf(filename, "%s/%s", dirlabel, dir->d_name);
	aDataset[d].filenames[f] = (char*)malloc(sizeof(char) * (strlen(filename)+1));
	strcpy(aDataset[d].filenames[f], filename);
	f++;
      }
    }
    closedir(_d);
  }
  num_dataset_entries++;
}

/*
 * load_dataset()
 *
 * loads the labels file and the filename for every image found
 *
 */
void load_dataset() {

  FILE *fd;
  char *line = NULL;
  size_t len = 0;
  ssize_t read;
  int label_id = 0;

  if ((fd = fopen(labels_file, "r"))==NULL) {printf("Error, dataset labels file not found\n"); exit(1);}

  num_dataset_entries = 0;

  while ((read = getline(&line, &len, fd)) != -1) fn_process_label_line(line, len, label_id++);
  fclose(fd);
  if (line) free(line); 

  if (verbose && (verbose_level>=2)) printf("  completed (dataset entries: %d)\n", num_dataset_entries);
}
