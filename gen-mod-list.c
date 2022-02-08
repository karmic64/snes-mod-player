#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>


char innambuf[512] = "mods/";
char outnambuf[512] = "cmods/";


int main()
{
  FILE *f = fopen("mod-list.asm","wb");
  if (!f)
  {
    printf("outfile open error: %s\n", strerror(errno));
    return EXIT_FAILURE;
  }
  fputs("mod_list = [",f);
  
  DIR *dir = opendir("mods");
  if (!dir)
  {
    printf("dir open error: %s\n", strerror(errno));
    goto fail;
  }
  struct dirent *de = NULL;
  while ((de = readdir(dir)))
  {
    char *name = de->d_name;
    size_t namlen = strlen(name);
    memcpy(innambuf+5,name,namlen+1);
    
    struct stat st;
    if (stat(innambuf,&st))
    {
      printf("%s stat error: %s\n", innambuf,strerror(errno));
      goto fail;
    }
    if (!S_ISREG(st.st_mode)) continue;
    
    memcpy(outnambuf+6,name,namlen);
    memcpy(outnambuf+6+namlen,".cmod",6);
    
    char modname[20];
    FILE *inf = fopen(innambuf,"rb");
    if (!inf)
    {
      printf("%s open error: %s\n", innambuf,strerror(errno));
      goto fail;
    }
    fread(modname,1,20,inf);
    fclose(inf);
    
    fprintf(f, "[binary(\"%s\"),\"%.20s\"],", outnambuf, (modname[0]=='\0') ? name : modname);
  }
  closedir(dir);
  
  
  fseek(f,-1,SEEK_CUR);
  fputc(']',f);
  fclose(f);
  return EXIT_SUCCESS;
  
fail:
  if (dir) closedir(dir);
  fclose(f);
  remove("mod-list.asm");
  return EXIT_FAILURE;
}