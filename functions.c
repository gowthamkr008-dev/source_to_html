#include "s2html.h"

int cla_validation(int argc,char *argv[],char **src,char **dest)                 /*check the command line argumenr if user gives proper argument or not */
{
  if(argc < 2)                                                                  /*argument count must more than 1 if not invalid argument  */
  {
    printf("Invalid argument \n");
    printf("./a.out <inputfilename.c>  <optionalfile>  optional is output filename\n");   /*usage message */
    return FAILURE; 
  }
  
  *src =  strchr(argv[1],'.');                                     /*check the argument extension present or not */
  
  if(*src == NULL)                                                  /*if str chr return NULL invalid */
  {
    fprintf(stderr,"invalid file %s\n",argv[1]);
    return FAILURE;
  }
  
  if(strcmp(*src,".c")== 0)                                        /*compare the full extension if present or not */
  {
    *src = argv[1];
  }
  
  
  if(argc >= 3)                                                /*if argument count is equal to or greater than 3 user given the output file name */
  {
  
    *dest = strrchr(argv[2],'.');                              /*check the extension present or not */
    
    if( *dest != NULL && (strcmp(*dest,".html") == 0) )         /*extension present asign to the destination fname */
    {
      *dest = malloc( (strlen(argv[2]) + 1) * sizeof(char) );      /*allocate a memory for copy the argv[2]*/
      
      if(*dest == NULL)
      {
        printf("Allocation failure\n");
        return FAILURE;
      }
      strcpy(*dest,argv[2]);
    }else{                                                             /*extension is not a  .html */
      
      *dest = malloc( (strlen(argv[2]) + 6) * sizeof(char) );          /*allocate a space */
      
      if(*dest == NULL)
      {
        printf("Allocation failure\n");
        return FAILURE;
      }

      strcpy(*dest,argv[2]);                                            /*add the file name with extension*/
      strcat(*dest,".html");
    }
  }else{                                                                /*argument count is not a 3*/
    *dest = malloc( (strlen("output.html") + 1) * sizeof(char) );   /* allocate a memory */
    
    if(*dest == NULL)
    {
      printf("Allocation failure\n");
      return FAILURE;
    }
    strcpy(*dest ,"output.html");                                   /*copy the data to the destination*/
  }
  return SUCCESS;
}


int read_file(FILE ** src,size_t * len,char *fname,char **data)     /*read the file from the file */
{
  
  *src = fopen(fname,"r");                                          /*open the file in read mode */
  
  if(*src == NULL)
  {
    perror("error ");
    return FAILURE;
  }
  
  fseek(*src,0,SEEK_END);                                          /*move the pointer to end */
  *len = ftell(*src);                                              /*using the ftell check the number of character */
  
  if(*len < 1)                                                     /*the character is less than 1 the file is empty*/
  {
    printf("File empty\n");
    fclose(*src);
    return FAILURE;
  }
  
  rewind(*src);                                                   /*move the file pointer to the first position */
  *data = malloc( (*len + 1) * sizeof(char));                     /*asign a memory for a data */
  if(*data == NULL)
  {
    perror("Error ");
    fclose(*src);
    return FAILURE;
  }
  
  size_t val = fread(*data,1,*len,*src);                         /*read from the file in one time */
  
  (*data)[*len]= '\0';
  fclose(*src);                                                 /*cloe the file */
  return SUCCESS;
}


void write_header(FILE *dest_fp,char *title,char *subtitle,char *fname)
{
  fputs("<!DOCTYPE html>\n<html lang=\"en\">\n<head>\n"
        "<meta charset=\"UTF-8\">\n"
        "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n", dest_fp);
        
        fprintf(dest_fp, "<title>%s</title>\n", fname);
        
        fputs("<style>\n"
          "body{margin:0;padding:40px 0;background:#0f1534;color:#d4d4d4;font-family:'Segoe UI',Arial,sans-serif;}\n"
          ".page{max-width:1280px;margin:0 auto;padding:0 20px;}\n"
          "h1 { background-clip: text;font-size: 70px; color: #59f7c2; text-shadow: 0 5px 5px #4b81ff, 0 0 20px #7aa2ff, 0 0 40px #7aa2ff;text-align: center; }\n"
          ".sub{  margin:0 0 30px;color:#9da5b4;font-size:35px;text-align: center;font-weight: 500;}\n"
          ".card{background:#1f1f1f;border:1px solid #7489b6;border-radius:14px;padding:30px 25px;overflow-x:auto;}\n"
          ".code{font-family:Consolas,'Courier New',monospace;font-size:14px;}\n"
          ".l{white-space:pre;line-height:1.6;min-height:1.6em;}\n"
          ".n{display:inline-block;width:4ch;margin-right:2ch;text-align:right;color:#5c6678;user-select:none;}\n"
          ".n{display:inline-block;width:4ch;margin-right:2ch;text-align:right;color:#5c6678;user-select:none;}\n"
          ".preprocessor{color: #7c3aed;}\n"                                            /*   preprocessor    */
          ".string{color: #e972f4; }\n"                                                    /*   string          */             
          ".comment{color: #5647fc;font-style:italic;}\n"                                  /*   comment         */
          ".num{color : #f97407;}\n"                                                      /*   numbers         */
          ".bracket{color: #ebad66;}\n"                                                     /*   {}[]() bracket  */
          ".storagekeyword{color:#d8a0f0;font-weight:bold;}\n"                                 /*   storage keyword */
          ".typekeyword{color : #6efffd;font-weight:bold;}\n"                               /*   typekeyword     */         
          ".controlkw{color : #ff76f6;font-weight:bold;}\n"                                     /*   control keyword */   
          ".constantkw{color:#60a5fa;}\n"                                                        /*   constant        */     
          ".function{color:#f9ff90;}\n"                                                           /*  function        */           
          ".operator{color:#94a3b8;}\n"                                                           /*   operator       */
          ".footer { color: #b8b8b8; clear: both; margin-top: 40px;text-align:center;font-size: 18px; }\n"
                                                              
       ".footer { clear: both; margin-top: 40px;text-align: center; }\n"                                                                   
       "</style>\n</head>\n<body>\n<div class=\"page\">\n", dest_fp);
       
       fprintf(dest_fp, "<h1>%s</h1>\n", title);
       fprintf(dest_fp, "<p class=\"sub\">%s</p>\n", subtitle);
       fputs("<div class=\"card\">\n<div class=\"code\">\n", dest_fp);
}


unsigned int line = 0;
void start_line(FILE *dest)
{
    fprintf(dest, "<div class=\"l\"><span class=\"n\">%u</span>", ++line);
}

void end_line(FILE *dest)
{
    fputs("</div>\n", dest);
}


//=============HTML entities (like `&lt;` or `&copy;`) display reserved or special characters as text instead of code.=====
static void entities(FILE *dest,char c)
{
  
  switch (c)
  {
    case '<':
      fprintf(dest,"&lt;");
      break;
    case '>':
      fprintf(dest,"&gt;");
      break;
    case '&':
      fprintf(dest,"&amp;");
      break;
    case '"':
      fprintf(dest,"&quot;");
      break;
    case '\'':
      fprintf(dest,"&apos;");
      break;
    default:
      fprintf(dest,"%c",c);
      break;
    }
  }
  


  
void insert_data(FILE *dest,  char *data, size_t start, size_t end, char *class)
{
  
  if(class != NULL)
  {
    fprintf(dest, "<span class=\"%s\">", class);
  }
  
  for (size_t k = start; k < end; k++)
  {
    if (data[k] == '\n')
    {
      if (class != NULL) 
        fputs("</span>", dest);   
            
        end_line(dest);
        start_line(dest);
        
        if (class != NULL)
          fprintf(dest, "<span class=\"%s\">", class);  
        }else{
          entities(dest, data[k]);
        }
      }
      
      if (class != NULL)
        fputs("</span>", dest);
}

