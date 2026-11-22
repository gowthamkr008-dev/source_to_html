#include  "s2html.h"

int main(int argc,char *argv[])                                                        /*input given throw the command line argument */
{
  char *fname_src,*fname_dest;                                                        
  
  if(cla_validation(argc,argv,&fname_src,&fname_dest) == FAILURE )                    /*verify the commend line argument user give proper or not */
  {
    return FAILURE;
  }
  
  FILE *src;
  FILE *dest;
  size_t len;
  char *srcdata;
  
  if(read_file(&src,&len,fname_src,&srcdata) == FAILURE)                             /*  read the source file from given user input */
  {                        
    return FAILURE;
  }
  
  dest = fopen(fname_dest,"w");                                                    /* open the destination file in the write mode  */
  
  if(dest == NULL)                                                                 /*validate file is open or not */
  {
    perror("Error ");
    return FAILURE;
  }
  

  write_header(dest, "Source to HTML", "C - Project",fname_src);                     /* write_header writes the start of the HTML page (CSS, title and the opening card) before the code lines.   */
  
  if(source_to_html(dest,srcdata,len) == FAILURE){                                   /*source_to_html converts C source code into coloured, line-numbered HTML.*/
    return FAILURE;
  }
  
  fprintf(dest,"</div>\n</div>\n</body>\n</html>\n");                                         /*after writing the htl and css code need to close the tags */
 fprintf(dest,"</div>\n<footer class=\"footer\">&copy; 2026 Source to HTML | C - Project | Made by Gowtham K R</footer>\n</body>\n</html>\n");
  fclose(dest);                                                                       /*close the destination file */
  printf("Output file created : %s\n",fname_dest);                                    /*print the output file */
  
  free(fname_dest);                                                                  /*delete the before allocated memory for filename  */
  free(srcdata);                                                                     /*delete the data has inreaded files datas*/

  return 0;
}