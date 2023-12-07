#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "db.h"

void write_db(FILE *file) {
  fseek(file, 0, SEEK_SET);
  
  if (!fwrite(&db, sizeof(db), 1, file)) {
    printf("\nWrite sb error\n");
    exit(1);
  }
}

void read_db(FILE *file) {
  fseek(file, 0, SEEK_END);

  if (ftell(file) > 0) {
    fseek(file, 0, SEEK_SET);
    fread(&db, sizeof(db), 1, file);
  }
}

int* get_stat(user _user) {
  int* stat = calloc(2, sizeof(int)), len;
  
  for (size_t i = 0; i < MAX_COUNT_LETTER; i++) {
    if (len = strlen(_user.letters[i].content)) stat[0]++, stat[1] += len;
  }

  return stat;
}

int** get_list(user _user) {
  int **list = malloc(_user.count_letter * sizeof(int*)), len, j = 0;

  for (size_t i = 0; i < _user.count_letter; i++) list[i] = calloc(2, sizeof(int));

  for (size_t i = 0; i < MAX_COUNT_LETTER; i++) {
    if ((len = strlen(_user.letters[i].content))) list[j][0] = i, list[j++][1] = len;
  }

  return list;
}

letter get_letter(user _user, int num) {
  return _user.letters[num];
}

void delete_letter(user *_user, int num) {
  _user->count_letter--;
  strcpy(_user->letters[num].content, "");
}

user* get_user_by_email(char email[MAX_EMAIL_LENGTH]) {
  for (size_t i = 0; i < db.count_users; i++) {
    if (!strcmp(db.users[i].email, email)) return &db.users[i];
  }
  
  return 0;
}