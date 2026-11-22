#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define SUCCESS 1
#define FAILURE -1

int cla_validation(int argc,char *argv[],char **src,char **dest);
int read_file(FILE ** src,size_t * len,char *fname,char **data);
int source_to_html(FILE *dest,char *data,size_t len);

// write_header(dest, "Source to HTML", "Syntax-highlighted C source",fname_src);


void write_header(FILE *dest_fp,char *title,char *subtitle,char *fname);
void start_line(FILE *dest);
void end_line(FILE *dest);

void insert_data(FILE *dest ,char *data,size_t start,size_t end,char *class);