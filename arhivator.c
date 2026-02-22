#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct
{

  char file_name[100];
  char file_mode[8];
  char oid[8];
  char gid[8];
  char file_size[12];
  char last_mod[12];
  char checksum[8];
  char typeflag;
  char link_file_name[100];
  char magic[6];
  char version[2];
  char ownerU_name[32];
  char ownerG_name[32];
  char device_major_nr[8];
  char device_minor_nr[8];
  char filename_prefix[155];
  char padding[12];
  
} HEADER;

unsigned int get_chksum(HEADER *h)
{
  
  unsigned int chksum = 0;
  unsigned char *p = (unsigned char *)h;
  
  for(; p < (unsigned char *)h+512; p++)
    chksum += *p;
  
  return chksum;
}

HEADER get_data(char *file, int size)
{
  HEADER h_file = {0};

  strcpy(h_file.file_name, file);
  if(strlen(h_file.file_name) < 99)
    {
      for(int i = strlen(h_file.file_name)+1; i < 100; i++)
	h_file.file_name[i] = '\0';
    }

  char nr_octal[12];
  sprintf(nr_octal, "%o", size);
  if(strlen(nr_octal) == 11)
    strcpy(h_file.file_size, nr_octal);
  else
    {
      int dif = 11 - strlen(nr_octal);
      for(int i = 0; i < dif; i++)
	  h_file.file_size[i] = '0';
      h_file.file_size[dif] = '\0';
      strcat(h_file.file_size, nr_octal);
    }

  strcpy(h_file.file_mode, "0000644");
  strcpy(h_file.oid, "0000000");
  strcpy(h_file.gid, "0000000");
  strcpy(h_file.last_mod, "00000000000");
  h_file.typeflag = '0';
  for(int i = 0; i < 100; i++)
    {
      h_file.link_file_name[i] = '\0';
    }
  strcpy(h_file.magic, "ustar");
  strcpy(h_file.version, "00");
  strcpy(h_file.ownerU_name, "user");
  strcpy(h_file.ownerG_name, "group");
  strcpy(h_file.device_major_nr, "0000000");
  strcpy(h_file.device_minor_nr, "0000000");
  for(int i = 0 ; i < 155; i++)
    {
      h_file.filename_prefix[i] = '\0';
    }

  for(int i = 0; i < 8; i++)
    h_file.checksum[i] = ' ';
  
  char sum[7];
  sprintf(sum, "%o",get_chksum(&h_file));
  if(strlen(sum) < 6)
    {
      int dif = 6 - strlen(sum);
      for(int i = 0; i < dif; i++)
	h_file.checksum[i] = '0';
      h_file.checksum[dif] = '\0';
      strcat(h_file.checksum, sum);
    }
  else
    strcpy(h_file.checksum, sum);
  h_file.checksum[7] = ' ';

  for(int i = 0; i < 12; i++)
    h_file.padding[i] = '\0';
  
  return h_file;
}

void write_header(FILE *out, HEADER h)
{
  fwrite(h.file_name, 100, 1, out);
  fwrite(h.file_mode, 8, 1, out);
  fwrite(h.oid, 8, 1, out);
  fwrite(h.gid, 8, 1, out);
  fwrite(h.file_size, 12, 1, out);
  fwrite(h.last_mod, 12, 1, out);
  fwrite(h.checksum, 8, 1, out);
  fwrite(&h.typeflag, 1, 1, out);
  fwrite(h.link_file_name, 100, 1, out);
  fwrite(h.magic, 6, 1, out);
  fwrite(h.version, 2, 1, out);
  fwrite(h.ownerU_name, 32, 1, out);
  fwrite(h.ownerG_name, 32, 1, out);
  fwrite(h.device_major_nr, 8, 1, out);
  fwrite(h.device_minor_nr, 8, 1, out);
  fwrite(h.filename_prefix, 155, 1, out);
  fwrite(h.padding, 12, 1, out);
}

int read_content(FILE *in, char **content)
{
  
  char *aux;
  char c;
  int k = 0;
  
  while(fread(&c, sizeof(c), 1, in))
    {
      k++;
      aux = (char*)realloc(*content, k*sizeof(char));
      if(aux == NULL)
	{
	  perror("Memorie insuficienta\n");
	  free(*content);
	  exit(-1);
	}

      *content = aux;
      (*content)[k-1] = c;
    }

    
  return k;
}

void add_padding(FILE *file, int file_size)
{
  int padding = 0;
  int multiple = 512;

  if(file_size == 0)
    padding = 0;
  else if(file_size < multiple)
    padding = multiple - file_size;
  else
    {
      while(multiple < file_size)
	multiple += 512;
      padding = multiple - file_size;
    }

  char buffer[512] = {0};
  fwrite(buffer, 1, padding, file);
  
}

int main(int argc, char **argv)
{

  if(argc < 3)
    {
      printf("eroare la argumente\n");
      exit(-1);
    }
  
  FILE *out;
  if((out = fopen(argv[1], "wb")) == NULL)
    {
      perror("eroare la deschidere fisier out\n");
      exit(-1);
    }
  

  int nr_files = argc - 2;
  FILE **in = malloc(nr_files * sizeof(FILE*));

  for(int i = 0; i < nr_files; i++)
    {
      if((in[i] = fopen(argv[2+i], "rb")) == NULL)
	{
	  perror("eroare la deschidere fisier de intrare\n");
	  exit(-1);
	}
      
      char *content = malloc(1);
      if(content == NULL)
	{
	  perror("memorie insuficienta\n");
	  free(content);
	  exit(-1);
	}
      
      int size = (read_content(in[i], &content));
      
      HEADER header = get_data(argv[2+i], size);

      write_header(out, header);
      
      fwrite(content, size, 1, out);
      add_padding(out, size);
      free(content);
      
      if(fclose(in[i]) != 0)
	{
	  perror("eroare la inchidere fisier de intrare %d\n");
	  exit(-1);
	}
    }

  char buffer[512] = {0};
  fwrite(buffer, 1, 512, out);
  fwrite(buffer, 1, 512, out); 
  free(in);
  if(fclose(out) != 0)
    {
      perror("eroare la inchidere fisier out\n");
      exit(-1);
    }

  return 0;
}
