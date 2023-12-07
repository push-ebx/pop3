#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "db.h"


int main(int argc, char const *argv[]) {
  FILE *file;
  
  if ((file = fopen(DATABASE_FILE, "rb+")) == NULL) {
    printf("error");
    exit(1);
  }

  read_db(file);
  user *_user = get_user_by_email("qs@q");
  
  if (_user) printf("%s", _user->password);

  // letter _letter_1 = {"letter #1"};
  // letter _letter_2 = {"hello"};
  // letter _letter_3 = {"letter #3"};

  // user _user = {"u@u", "pass"};
  // _user.letters[0] = _letter_1;
  // _user.letters[1] = _letter_2;
  // _user.letters[2] = _letter_3;
  // _user.count_letter = 3;

  // user _user2 = {"q@q", "123"};
  // _user2.letters[0] = _letter_1;
  // _user2.letters[1] = _letter_2;
  // _user2.letters[2] = _letter_3;
  // _user2.count_letter = 3;

  // db.count_users = 2;
  // db.users[0] = _user;
  // db.users[1] = _user2;
  // write_db(file);

  // read_db(file);

  // int *stat = get_stat(db.users[0]);
  // printf("%d %d\n", stat[0], stat[1]);
  // free(stat);

  // int **list = get_list(db.users[0]);
  
  // for (size_t i = 0; i < db.users[0].count_letter; i++) {
  //   printf("%d %d\n", list[i][0], list[i][1]);
  // }

  // delete_letter(&db.users[0], 1);
  // list = get_list(db.users[0]);
  
  // for (size_t i = 0; i < db.users[0].count_letter; i++) {
  //   printf("%d %d\n", list[i][0], list[i][1]);
  // }
  
  // free(list);

  // printf("%s", db.users[0].letters[1].content);
  // printf("%s", db.users[1].email);
  return 0;
}