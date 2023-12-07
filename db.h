#pragma once

#define MAX_EMAIL_LENGTH 128
#define MAX_PASSWORD_LENGTH 128
#define MAX_LETTER_LENGTH 512
#define MAX_COUNT_LETTER 512
#define MAX_COUNT_USERS 128
#define DATABASE_FILE "db"

typedef struct letter {
  char content[MAX_LETTER_LENGTH];
} letter;

typedef struct user {
  char email[MAX_EMAIL_LENGTH];
  char password[MAX_PASSWORD_LENGTH];
  letter letters[MAX_COUNT_LETTER];
  size_t count_letter;
} user;

typedef struct database {
  user users[MAX_COUNT_USERS];
  size_t count_users;
} database;

static database db;

void write_db(FILE *file);
void read_db(FILE *file);
int* get_stat(user _user);
int** get_list(user _user);
letter get_letter(user _user, int num);
void delete_letter(user *_user, int num);
user* get_user_by_email(char email[MAX_EMAIL_LENGTH]);