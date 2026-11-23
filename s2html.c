#include "s2html.h"

//control keywords
static  char *control_kw[] = {  "if", "else", "for", "while", "do", "switch", "case", "default",
                                "break", "continue", "return", "goto",NULL
                              };

// Storage class, type qualifiers, and related keywords
static  char *storage_kw[] = {"auto", "const", "enum", "extern", "inline", "register", "restrict", 
                              "sizeof", "static", "struct", "typedef", "union", "volatile" ,NULL
                            };

//in build  data types
static  char *type_kw[] = {"void", "char", "short", "int", "long", "float", "double", "signed",
                            "unsigned", "_Bool", "size_t", "FILE", "uint8_t", "uint16_t",
                            "uint32_t", "uint64_t", "int8_t", "int16_t", "int32_t", "int64_t",NULL
                          };

// Predefined constants and literals
static  char *const_kw[] = { "NULL", "EOF", "true", "false",NULL };



//==========find what keyword in present==========
static int is_list(char *data,size_t len,char **list)
{
  
  for(int i = 0 ; list[i] != NULL;i++)                                                                  
  {
    
    if(strlen(list[i]) == len && strncmp(list[i],data,len) == 0)                   /*check the length if len true only do comparition */
    {
      return 1;                                                                    /*both length and string n compare is true return 1 */
    }
  }
  return 0;                                                                        /*word is not present in entire list refutn 0 is false */
}


int source_to_html(FILE *dest, char *data, size_t len)                              /*convert the c code to html */
{
  size_t i = 0,j; 
  unsigned int line_start = 1;
  
  start_line(dest);
  
  unsigned char ch ;
  
  while (i < len)                                                                  
  {
    ch = (unsigned)data[i];
    
    //=============preprocessor============
    if(ch == '#' && line_start)
    {
      j = i + 1;
      
      while(j < len && data[j] == ' ' || data[j] == '\t')j++;                             /*after # any space it check and added */
      
      size_t nmstart = j;
      
      while( j < len && isalpha(data[j]) )j++;                                           /*add the include , define like that this added */
      
      insert_data(dest,data,i,j,"preprocessor");                                         /*write a code for html as a preprocessor */
      
      int isinc = (j - nmstart == 7 && strncmp(data + nmstart,"include",7) == 0);       /*check this is include or not if include add the header file in below */
      
      line_start = 0;
      
      i = j;
      
      if(isinc)
      {
        while(j < len && isspace(data[j]))j++;                                          /*loop runc any space are there or not after include */
        
        insert_data(dest,data,i,j,NULL);                                                /*insert that spaces */
        if(data[i] == '<' || data[i] == '"')                                          
        {
          char close = (data[i]=='<') ? '>' : '"';
          j = i + 1;
          
          while( j < len && (data[j] != close && data[j] != '\n' )) j++;                  /*loop run until close tag or new line appear*/
          if(j < len && data[j] == close) j++;                                            /*loop will exiting by the closing tag increment ont the j*/
          insert_data(dest,data,i,j,"string");                                            /*insert a data as a string */
          i = j;
        }
      }
      continue;
    }
    
    //=========== singleline comment ===============
    if(ch == '/' && i +1 <len && data[i+1] == '/'){
      j = i + 2;
      
      while(j < len && data[j] != '\n') j++;                                                /*there is a single line comment the after // the entire content is commenting loop runs until the new line appear */
      
      insert_data(dest,data,i ,j,"comment");                                                /*insert a data to html file */
      
      i = j;
      line_start =0;
      
      continue;
    }
    
    //===========multiline comment======================
    if(ch == '/' && data[i + 1] == '*')                                              /*check is a multiline comment */
    {
      j = i + 2;
      
      while(j < len && !(data[j] == '*' && data[j+1] =='/' ))j++;                    /*loop runs until the  * /  characters appears in continous */
      
      insert_data(dest,data,i,j+=2,"comment");                                      /*inserta a data as comment */
      
      i = j;
      continue;
    }
    
    //================new line =======================
    if(ch == '\n'){
      insert_data(dest,data,i,i+1,NULL);
      i++;
      line_start = 1;
      continue;
    }
    
    //============space==========================
    if(isspace(ch))
    {
      insert_data(dest,data,i,i + 1,NULL);
      i++;
      continue;
    }
    
    //===============numbers============================
    if(isdigit(ch) )                                                             /*check the numbers is digit or not */
    {
      j = i + 1;
      
      while(j < len && (isalnum(data[j]) ) || data[j] =='.' || ( (data[j-1] == 'e' || data[j-1] == 'E' || data[j-1] == 'p' || data[j - 1] == 'P'))) j++;    /*loop runs the count number or sintific numbers  */
      
      insert_data(dest,data,i,j,"num");                                             /*insera a data as  a number */
      
      i = j;
      line_start = 0;
      continue; 
    }
    
    //============brackets===============
    if(strchr("[]{}()",ch) != NULL)                                               /*check the data as a brackets */
    { 
      insert_data(dest,data,i,i + 1,"bracket");                                   /*insert tha data */
      i++;
      line_start = 0;
      continue;
    }

      //=========string literal character literal ============
      if(ch == '"' || ch == '\''){                                                /*check the string or character literal */
        j = i + 1;
        
        while(j < len && (data[j] != ch )) j++;                                   /*loop runs until the any one char present ' or " */                                 
        
        if( data[j] == '\'')j++;
        
        insert_data(dest,data,i,j + 1,"string");                                  /*insert a data as a string */
        i = j + 1;
        continue;
      }
      
      //===========functions / keywords / variable  ============
      if(isalpha(ch) || ch == '_')                                                 /*check is is alpha or  _ */
      {
        j = i;
        size_t len_s = 0;
        
        while(j < len && (isalnum(data[j])) || data[j] == '_' )                   /* loop run until is not a  or special char like that  */
        {
          len_s++;                                                                /*count the length of the string */
          j++;
        }
        
        char * cls = NULL;
        
        if(is_list(data + i,len_s,control_kw) ){                                  /*check is a control keyword */
          cls = "controlkw";
        }else if(is_list(data + i,len_s,storage_kw)){                              /*check is a storage keyword */
          cls ="storagekeyword";
        }else if(is_list(data + i,len_s,type_kw)){                                 /*check is there is type keyword */
          cls = "typekeyword";
        }else if(is_list(data + i,len_s,const_kw)){                                /*check is there constant keyword */
          cls = "constantkw";
        }else{                                                                     /*not a any keyword consider as is function or variable */
          size_t k = j;                                                             /*here k no need to add the space and bracket in that variable class  */
          
          while(k < len && ((data[k] == ' ' || data[k] == '\t')))k++;              /*after words loop runs until the space in between the next word or brackets */
          
          if(k < len && data[k]== '('){                                            /*if that is bracket it variable is function*/
            cls = "function";
          }
        }
        
        insert_data(dest,&data[i],0,len_s,cls);
        i = j;
        continue;
      }
      
      //================operators=================
      if (strchr("+-*=<>%!|&^~?:", ch) != NULL)                                       /*check the operator is char or not */                  
      {
        insert_data(dest, data, i, i + 1, "operator");                                /*insert a data as a operator */
        i++;
        line_start = 0;
        continue;
      }


      insert_data(dest,data,i,i + 1,NULL);
      line_start = 0;
      i++;
    }
    end_line(dest);                                                                      /*everything is done close tha tag */
    return SUCCESS;                                                                       /*everything done succesfully return success */
  }