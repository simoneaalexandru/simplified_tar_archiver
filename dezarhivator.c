#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int read_content(FILE *file, char **file_content)
{
  *file_content = malloc(1);
  int k = 0;
  char c;
  
  while(fread(&c, 1, 1, file))
    {
   
      k++;
      char *aux = (char*)realloc(*file_content, k);
      if(aux == NULL)
	{
	  perror("memorie insuficienta\n");
	  free(*file_content);
	  exit(-1);
	}

      *file_content = aux;
      (*file_content)[k-1] = c;
      if(c == '\0')
	break;
    }
  
  return k;
}

int main(int argc, char **argv)
{
  if(argc != 2)
    {
      printf("error at arguments!\n");
      exit(-1);
    }
  
  FILE *archive;

  if((archive = fopen(argv[1], "rb")) == NULL)
    {
      perror("error at opening archive file!\n");
      exit(-1);
    }

  char file_name[100];
  char *file_content;
  int i = 1;
  while(1)
    {
      fread(file_name, 100, 1, archive);
      
      if(strlen(file_name) == 0)
	break;

      FILE *out;

      if((out = fopen(file_name, "wb")) == NULL)
	{
	  perror("error at opening output file\n");
	  exit(-1);
	}
      
      fseek(archive, 412, SEEK_CUR);
      int size = read_content(archive, &file_content);
      fwrite(file_content, size, 1, out);
      free(file_content);
      
      fseek(archive, i*1024, SEEK_SET);
      i++;

      if(fclose(out) != 0)
	{
	  perror("error at closing the output file\n");
	  exit(-1);
	}
    }

  if(fclose(archive) != 0)
    {
      perror("error at closing the archive file!\n");
      exit(-1);
    }

  return 0;
}
