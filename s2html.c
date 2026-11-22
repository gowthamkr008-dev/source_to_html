#include "s2html.h"

//control keywords
static  char *control_kw[] = {  "if", "else", "for", "while", "do", "switch", "case", "default",
                                "break", "continue", "return", "goto", NULL
                              };

// Storage class, type qualifiers, and related keywords
static  char *storage_kw[] = {"auto", "const", "enum", "extern", "inline", "register", "restrict", 
                              "sizeof", "static", "struct", "typedef", "union", "volatile", NULL
                            };

//in build  data types
static  char *type_kw[] = {"void", "char", "short", "int", "long", "float", "double", "signed",
                            "unsigned", "_Bool", "size_t", "FILE", "uint8_t", "uint16_t",
                            "uint32_t", "uint64_t", "int8_t", "int16_t", "int32_t", "int64_t", NULL
                          };

// Predefined constants and literals
static  char *const_kw[] = { "NULL", "EOF", "true", "false", NULL };

static int is_list(char *data,size_t len,char **list)
{
  
  for(int i = 0 ; list[i] != NULL;i++)
  {
    
    if(strlen(list[i]) == len && strncmp(list[i],data,len) ==0)
    {
      return 1;
    }
  }
  return 0;
}


int source_to_html(FILE *dest, char *data, size_t len)
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
      
      while(j < len && data[j] == ' ' || data[j] == '\t')j++;
      
      size_t nmstart = j;
      
      while( j < len && isalpha(data[j]) )j++;
      
      insert_data(dest,data,i,j,"preprocessor");
      
      int isinc = (j - nmstart == 7 && strncmp(data + nmstart,"include",7) == 0);
      
      line_start = 0;
      
      i = j;
      
      if(isinc)
      {
        while(j < len && isspace(data[j]))j++;
        
        insert_data(dest,data,i,j,NULL);
        if(data[i] == '<' || data[i] == '"')
        {
          char close = (data[i]=='<') ? '>' : '"';
          j = i + 1;
          
          while( j < len && (data[j] != close && data[j] != '\n' )) j++;
          if(j < len && data[j] == close) j++;
          insert_data(dest,data,i,j,"string");
          i = j;
        }
      }
      continue;
    }
    
    //=========== singleline comment ===============
    if(ch == '/' && i +1 <len && data[i+1] == '/'){
      j = i + 2;
      
      while(j < len && data[j] != '\n') j++;
      
      insert_data(dest,data,i ,j,"comment");
      
      i = j;
      line_start =0;
      
      continue;
    }
    
    //===========multiline comment======================
    if(ch == '/' && data[i+1] == '*')
    {
      j = i + 2;
      
      while(j < len && !(data[j] == '*' && data[j+1] =='/' ))j++;
      
      insert_data(dest,data,i,j+=2,"comment");
      
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
      insert_data(dest,data,i,i+1,NULL);
      i++;
      continue;
    }
    
    //===============numbers============================
    if(isdigit(ch) )
    {
      j = i + 1;
      
      while(j < len && (isalnum(data[j]) ) || data[j] =='.' || ( (data[j-1] == 'e' || data[j-1] == 'E' || data[j-1] == 'p' || data[j-1] == 'P'))) j++;
      
      insert_data(dest,data,i,j,"num");
      
      i = j;
      line_start = 0;
      continue; 
    }
    
    //============brackets===============
    if(strchr("[]{}()",ch) != NULL)
    { 
      insert_data(dest,data,i,i+1,"bracket");
      i++;
      line_start = 0;
      continue;
    }

      //=========string literal character literal ============
      if(ch == '"' || ch == '\''){
        j = i + 1;
        
        while(j < len && (data[j] != ch )) j++;
        
        if(  data[j] == '\'')j++;
        
        insert_data(dest,data,i,j+1,"string");
        i = j + 1;
        continue;
      }
      
      //===========functions / keywords / identifier ============
      if(isalpha(ch) || ch == '_')
      {
        j = i;
        size_t len_s = 0;
        
        while(j < len && (isalnum(data[j]))|| data[j] == '_' )
        {
          len_s++;
          j++;
        }
        
        char * cls = NULL;
        
        if(is_list(data + i,len_s,control_kw) ){
          cls = "controlkw";
        }else if(is_list(data + i,len_s,storage_kw)){
          cls ="storagekeyword";
        }else if(is_list(data + i,len_s,type_kw)){
          cls = "typekeyword";
        }else if(is_list(data + i,len_s,const_kw)){
          cls = "constantke";
        }else{
          size_t k = j;
          
          while(k < len && ((data[k] == ' ' || data[k] == '\t')))k++;
          
          if(k < len && data[k]== '('){
            cls = "function";
          }
        }
        
        insert_data(dest,&data[i],0,len_s,cls);
        i = j;
        continue;
      }
      
      //================operators=================
      if (strchr("+-*=<>%!|&^~?:", ch) != NULL)
      {
        insert_data(dest, data, i, i + 1, "operator");
        i++;
        line_start = 0;
        continue;
      }


      insert_data(dest,data,i,i + 1,NULL);
      line_start = 0;
      i++;
    }
    end_line(dest);
    return SUCCESS;
  }