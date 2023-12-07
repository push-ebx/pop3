#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <signal.h>
#include "db.h"

#define PORT 110
#define BUFFER_SIZE 1024

#define QUIT "+OK Goodbye\n"
#define USER_ACCEPT "+OK User accepted\n"
#define PASS_ACCEPT "+OK Pass accepted\n"
#define GREENING "+OK POP3 server ready\n"
#define ERR_CMD_NOT_SUP "-ERR Command not supported\n"
#define ERR_NULL_USERNAME "-ERR Input username\n"
#define ERR_NULL_PASS "-ERR Input password\n"
#define ERR_HAS_BEEN_AUTH "-ERR User has been authtorized\n"
#define ERR_USER_NOT_FOUND "-ERR User not found\n"
#define ERR_PASS "-ERR Uncorrect password\n"
#define MSG_DEL "+OK message deleted\n"

FILE *file;

typedef struct client {
  char username[128];
  char password[128];
  char is_authtorized;
} client;

char **split(char *str[]) {
  char *ptr = strtok(str, " ");
  char** arr = malloc(sizeof(char *) * 512);
  size_t i = 0;
  
  while(ptr) {
    arr[i++] = ptr;
    ptr = strtok(NULL, " ");
  }

  return arr;
}

void *handle_client(void *arg) {
  int client_socket = *((int *)arg);
  char buffer[BUFFER_SIZE];
  ssize_t bytes_received;
  client _client;
  user *_user;

  while ((bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0)) > 0) {
    buffer[bytes_received - 2] = '\0';
    
    char **command = split(buffer);

    if (!strcmp(command[0], "QUIT")) {
      send(client_socket, QUIT, strlen(QUIT), 0);
      break;
    }
    else if (!strcmp(command[0], "USER")) {
      if (_client.is_authtorized) send(client_socket, ERR_HAS_BEEN_AUTH, strlen(ERR_HAS_BEEN_AUTH), 0);
      else {
        if (command[1]) {
          _user = get_user_by_email(command[1]);
          
          if (_user) {
            send(client_socket, USER_ACCEPT, strlen(USER_ACCEPT), 0);
            strcpy(_client.username, command[1]);
          }
          else {
            send(client_socket, ERR_USER_NOT_FOUND, strlen(ERR_USER_NOT_FOUND), 0);
          }
        }
        else {
          send(client_socket, ERR_NULL_USERNAME, strlen(ERR_NULL_USERNAME), 0);
        }
      }
    }
    else if (!strcmp(command[0], "PASS")) {
      if (_client.is_authtorized) send(client_socket, ERR_HAS_BEEN_AUTH, strlen(ERR_HAS_BEEN_AUTH), 0);
      else {
        if (strlen(_client.username)) {
          if (command[1]) {
            if (!strcmp(command[1], _user->password)) {
              strcpy(_client.password, command[1]);
              _client.is_authtorized = 1;
              
              send(client_socket, PASS_ACCEPT, strlen(PASS_ACCEPT), 0);
            }
            else {
              send(client_socket, ERR_PASS, strlen(ERR_PASS), 0);
            }
          }
          else {
            send(client_socket, ERR_NULL_PASS, strlen(ERR_NULL_PASS), 0);
          } 
        }
        else {
          send(client_socket, ERR_NULL_USERNAME, strlen(ERR_NULL_USERNAME), 0);
        }
      }
    }
    else if (!strcmp(command[0], "STAT")) {
      if (_client.is_authtorized) {
        int *stat = get_stat(*_user);
        char stat_str[BUFFER_SIZE] = {'\0'};

        for (size_t i = 0, offset = 0; i < 2; i++) {
          offset += sprintf(stat_str + offset, "%d ", stat[i]);
        }
        stat_str[strlen(stat_str)] = '\n';

        send(client_socket, stat_str, strlen(stat_str), 0);
      }
      else {
        send(client_socket, ERR_CMD_NOT_SUP, strlen(ERR_CMD_NOT_SUP), 0);
      }
    }
    else if (!strcmp(command[0], "LIST")) {
      if (_client.is_authtorized) {
        int **list = get_list(*_user);
        char list_str[BUFFER_SIZE] = {'\0'};
        
        for (size_t i = 0, offset = 0; i < _user->count_letter; i++) {
          offset += sprintf(list_str + offset, "%d %d\n", list[i][0], list[i][1]);
        }

        send(client_socket, list_str, strlen(list_str), 0);
      }
      else {
        send(client_socket, ERR_CMD_NOT_SUP, strlen(ERR_CMD_NOT_SUP), 0);
      }
    }
    else if (!strcmp(command[0], "RETR")) {
      if (_client.is_authtorized) {
        char letter_str[BUFFER_SIZE] = {'\0'};
        letter _letter = get_letter(*_user, atoi(command[1]));
        
        strcpy(letter_str, _letter.content);
        letter_str[strlen(letter_str)] = '\n';

        send(client_socket, letter_str, strlen(letter_str), 0);
      }
      else {
        send(client_socket, ERR_CMD_NOT_SUP, strlen(ERR_CMD_NOT_SUP), 0);
      }
    }
    else if (!strcmp(command[0], "DELE")) {
      if (_client.is_authtorized) {
        delete_letter(_user, atoi(command[1]));

        send(client_socket, MSG_DEL, strlen(MSG_DEL), 0);
      }
      else {
        send(client_socket, ERR_CMD_NOT_SUP, strlen(ERR_CMD_NOT_SUP), 0);
      }
    }
    else {
      send(client_socket, ERR_CMD_NOT_SUP, strlen(ERR_CMD_NOT_SUP), 0);
    }
  }
  
  close(client_socket);
  free(arg);
  pthread_exit(NULL);
}

void handleCtrlC(int signal) {
  write_db(file);
  exit(0);
}

int main() {
  if ((file = fopen(DATABASE_FILE, "rb+")) == NULL) {
    printf("error");
    exit(1);
  }

  read_db(file);

  if (signal(SIGINT, handleCtrlC) == SIG_ERR) {
    perror("Unable to set up signal handler");
    return 1;
  }

  int server_socket, client_socket;
  struct sockaddr_in server_address, client_address;
  socklen_t client_address_len = sizeof(client_address);

  pthread_t thread_id;

  server_socket = socket(AF_INET, SOCK_STREAM, 0);
  setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int));

  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = INADDR_ANY;
  server_address.sin_port = htons(PORT);
  bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address));

  listen(server_socket, 1);
  printf("server listening on port %d...\n", PORT);

  while (1) {
    client_socket = accept(server_socket, (struct sockaddr*)&client_address, &client_address_len);
    printf("client: %s:%d\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));

    int *arg = (int *)malloc(sizeof(int));
    *arg = client_socket;

    if (pthread_create(&thread_id, NULL, handle_client, (void *)arg) != 0) {
      perror("create thread error");
      return 1;
    }

    send(client_socket, GREENING, strlen(GREENING), 0);
    pthread_detach(thread_id);
  }

  close(server_socket);

  return 0;
}