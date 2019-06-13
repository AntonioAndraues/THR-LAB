#include "raylib.h"
#include <dirent.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>

#define bufferSize 1
const int screenWidth = 800;
const int screenHeight = 450;
const char bufferInterface[255];

typedef struct {
  char argumento1[255], argumento2[255];
  long int porcentagem;
  int stop;
  int isDir;
  long int porcentagem_total;
} copy_struc;
long int findSize(const char *file_name) {
  struct stat st; /*declare stat variable*/

  /*get the size using stat()*/
  struct stat stbuf;
  stat( file_name, &stbuf);
  if(isDirectory(file_name)){
    return 0;
  }
  return stbuf.st_size;
  // if (stat(file_name, &st) == 0){
  //   return (st.st_size);
  // }
  // else{
  //   printf("NAO FOI POSSIVEL ENCONTRAR O SIZE\n");
  //   return 0;
  // }
}
int isDirectory(const char *path) {
  struct stat statbuf;
  if (stat(path, &statbuf) != 0)
    return 0;
  return S_ISDIR(statbuf.st_mode);
}
void copy(copy_struc *spa) {
    int nread, nwrite;
    long int size = 0;
    char *buffer[bufferSize];
    int arq_org = open(spa->argumento1, O_RDONLY);
    int arq_destino = open(spa->argumento2, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    size = findSize(spa->argumento1);
    while ((nread = read(arq_org, buffer, bufferSize)) > 0 && !spa->stop) {
        if (write(arq_destino, buffer, nread) != nread) {
          printf("tentando escrever : %s\n",spa->argumento1);
          printf("em  : %s\n",spa->argumento2);
          printf("\nErro escrevendo %s\n", &spa->argumento1);
        }else{
          spa->porcentagem = findSize(spa->argumento2) * 100 / size;
        }
        
    
    }
  if (spa->stop) {
    remove(spa->argumento2);
  }
}
void copydir(copy_struc *spa) {
    DIR *dp;
    DIR *dp2;
    char diretoryname[255];
    char diretorydestname[255];
    long int porcentagemtotal;
    struct dirent *ep;
    long int totalsize,localsize;
    

    // printf("armento1 : %s\n",spa->argumento1);
    // printf("armento2 : %s\n",spa->argumento2);
    // printf("DIRETORIO :%s\n", spa->argumento1);
    
    sprintf(diretoryname, "%s",spa->argumento1);
    sprintf(diretorydestname, "%s",spa->argumento2);

    mkdir(spa->argumento2,0777);

    dp = opendir(spa->argumento1);
    dp2 = opendir(spa->argumento2);
    printf("LOG DO COPY:\n");
    while (ep = readdir(dp)){
      totalsize+=findSize(ep->d_name);
      printf("nome arquivo : %s\n",ep->d_name);
      printf("localsize    : %ld\n\n\n ///////////////////\n",findSize(ep->d_name));
    }
    (void)closedir(dp);
    dp = opendir(spa->argumento1);
        
    printf("SIZE TOTAL DIR : %ld\n",totalsize);
    if (dp != NULL) {

        // ChangeDirectory(dp2);
        while (ep = readdir(dp)){
            sprintf(spa->argumento1, "%s/%s",diretoryname,ep->d_name);
            sprintf(spa->argumento2, "%s/%s",diretorydestname,ep->d_name);
            copy(spa);
            localsize+=findSize(ep->d_name);
            printf("localsize : %ld\n",localsize);

            spa->porcentagem_total= localsize*100/totalsize;
            printf("PORCENTAGEMMMM dir :   %ld\n", spa->porcentagem_total);
        }
        printf("Fechando os dirs\n");
        (void)closedir(dp);
        (void)closedir(dp2);
    }else {
      perror("Não conseguiu achar o diretorio/ ou abrir o mesmo");
    }
}
void *copy_func(void *_arg) {

    copy_struc *spa = _arg;
    printf("copyfunc\n");
    printf("%d\n",isDirectory(spa->argumento1));
    printf("%d\n",isDirectory(spa->argumento2));

    if(isDirectory(spa->argumento1)){
        printf("é diretorio \n");
        spa->isDir=1;
        copydir(spa);

    }else{
        printf("Nao é  \n");
        spa->isDir=0;
        copy(spa);
    }
  
  
  // pthread_mutex_lock(&copy_mutex);
  // pthread_mutex_unlock(&copy_mutex);
}
int main(int argc, char *argv[]) {
  copy_struc teste;
  teste.porcentagem = 0;
  teste.stop = 0;
  teste.porcentagem_total=0;
  sprintf(teste.argumento1, "%s", argv[1]);
  sprintf(teste.argumento2, "%s", argv[2]);

  pthread_t t1;
  pthread_create(&t1, NULL, copy_func, &teste);
  InitWindow(screenWidth, screenHeight, bufferInterface);

  SetTargetFPS(60);
  // loop principal
  while (!WindowShouldClose()) // fica enquanto a aba nao for fechada
  {
    sprintf(bufferInterface, "Copiando arquivo de %s para %s", teste.argumento1, teste.argumento2);

    if (IsKeyPressed(KEY_SPACE)) {
      teste.stop = 1;
      break;
    }
    //----------------------------------------------------------------------------------

    // Ver se ja acabou
    //----------------------------------------------------------------------------------
    printf("PORCENTAGEMMMM dir :   %ld\n", teste.porcentagem_total);
    if (teste.porcentagem >= 100) {
      BeginDrawing();
      ClearBackground(RAYWHITE);
      DrawText("FINALIZOU:", 255, 150, 20, LIGHTGRAY);
      DrawRectangle(200, 200, 400, 12, LIGHTGRAY);
      DrawRectangle(200, 200, 4 * (int)teste.porcentagem, 12, MAROON);
      DrawRectangle(200, 300, 4*(int)teste.porcentagem_total, 12, GREEN);
      EndDrawing();
      teste.stop = 1;
      sleep(2);
      break;
    }
    //----------------------------------------------------------------------------------

    // Desenha a tela
    //----------------------------------------------------------------------------------
    BeginDrawing();

    ClearBackground(RAYWHITE);

    DrawText(bufferInterface, 100, 150, 20, LIGHTGRAY);

    DrawRectangle(200, 200, 400, 12, LIGHTGRAY);
    DrawRectangle(200, 200, 4 * (int)teste.porcentagem, 12, MAROON);
    DrawRectangle(200, 300, 4*(int)teste.porcentagem_total, 12, GREEN);
    DrawRectangleLines(200, 200, 400, 12, GRAY);

    DrawText("Precione espaco para fechar", 215, 250, 20, LIGHTGRAY);

    EndDrawing();
    //----------------------------------------------------------------------------------
  }
    (void) pthread_join(t1, NULL);
  if (!teste.stop) {
    remove(teste.argumento2);
  }
  printf("PORCENTAGEMMMM      %ld\n", teste.porcentagem);
  

  exit(1);
}