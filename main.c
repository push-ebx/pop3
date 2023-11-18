#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_BUFFER_SIZE 1024
#define MAX_USERNAME_LEN 50
#define MAX_PASSWORD_LEN 50
#define MAX_MAILBOX_SIZE 100

// Пример структуры для представления почтового ящика
typedef struct {
  char username[MAX_USERNAME_LEN];
  char password[MAX_PASSWORD_LEN];
  char* messages[MAX_MAILBOX_SIZE];
  int messageCount;
} Mailbox;

Mailbox mailboxes[MAX_MAILBOX_SIZE];  // Пример хранения почтовых ящиков

int main(int argc, char const *argv[]) {
  
  return 0;
}
