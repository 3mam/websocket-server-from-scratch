#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>

#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include <string.h>
#include <unistd.h>
#include "base64.h"
#include "macro.h"
#include "sha1.h"
#include "str.h"

void error(const char* msg) {
  perror(msg);
  exit(1);
}

str* tokken(char* buf, ui32 buf_size) {
  str* key = str_init(24);
  str* key_word = str_new("Sec-WebSocket-Key: ");
  str* id = str_new("258EAFA5-E914-47DA-95CA-C5AB0DC85B11");

  for (i32 i = 0; i < buf_size; i++) {
    for (i32 j = 0; j < key_word->size; j++) {
      if (key_word->ptr[j] == buf[i + j]) {
        if (' ' == buf[i + j]) {
          for (i32 k = 0; k < key->size; k++) {
            key->ptr[k] = buf[k + i + j + 1];
          }
        }
      } else {
        break;
      }
    }
  }

  str* keyid = str_add(2, key, id);
  str* b64 = str_init(28);
  str* result = str_init(20);

  SHA1(result->ptr, keyid->ptr, keyid->size);
  b64_encode(result->ptr, result->size, b64->ptr);

  str_del(key);
  str_del(key_word);
  str_del(id);
  str_del(keyid);
  str_del(result);
  return b64;
}

int main(int argc, char* argv[]) {
  int sockfd, newsockfd, portno;
  socklen_t clilen;
  char* buffer = MAKE(char, 1024);
  struct sockaddr_in cli_addr;
  struct sockaddr_in serv_addr;
  int n;

  sockfd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
  int option = 1;
  // resolve problem witch busy addres
  setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
  if (sockfd < 0)
    error("ERROR opening socket");

  portno = 8080;  // atoi(argv[1]);
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(portno);
  if (bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
    error("ERROR on binding");
  /* code */

  listen(sockfd, 5);

  int* ws_list = MAKE(int, 32 * 1024);
  int ws_index = 0;
  ui8 var[] = {0b10000010, 4, 1, 2, 3, 4};
  while (1) {
    clilen = sizeof(cli_addr);

    newsockfd =
        accept4(sockfd, (struct sockaddr*)&cli_addr, &clilen, SOCK_NONBLOCK);
    if (newsockfd < 0) {
      // error("ERROR on accept");
      sleep(1);
      if (ws_index != 0) {
        for (int i = ws_index; i--;) {
          write(ws_list[i], var, 6);
        }
      }
      continue;
    }
    ws_list[ws_index] = newsockfd;
    ws_index++;

    n = read(newsockfd, buffer, 1024);

    if (n < 0)
      error("ERROR reading from socket");
    printf("%s\n", buffer);

    str* key = tokken(buffer, 1024);

    n = write(newsockfd, "HTTP/1.1 101 Switching Protocols\r\n", 34);
    n = write(newsockfd, "Upgrade: websocket\r\n", 20);
    n = write(newsockfd, "Connection: Upgrade\r\n", 21);
    n = write(newsockfd, "Sec-WebSocket-Accept: ", 22);
    n = write(newsockfd, key->ptr, key->size);
    n = write(newsockfd, "\r\n", 2);
    n = write(newsockfd, "\r\n", 2);
    n = write(newsockfd, var, 6);
    str_del(key);

    if (n < 0)
      error("ERROR writing to socket");
    // close(newsockfd);
  }
  //
  // close(sockfd);
  return 0;
}