#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

#define HTTP_PORT 80
#define BUFFER_SIZE 200

#define MIN_ULETTER_CODE 65
#define MAX_ULETTER_CODE 90
#define MIN_LLETTER_CODE 97
#define MAX_LLETTER_CODE 122
#define MIN_NUMBER_CODE 48
#define MAX_NUMBER_CODE 57
#define DOT_CODE 16
#define LOWCASE_CODE 95
#define TILDA_CODE 126

char* post(char* schema, char* host, char* page, char* content) {
  int socketfd;
  struct hostent *hptr;
	struct sockaddr_in servaddr;
  char **pptr;
  char addr[50];
  //
  char* request_format = "POST /%s HTTP/1.0\r\nContent-Length: %d\r\n\r\n%s";
  char* request;
  int content_len, url_len, request_len;
  //
  char response_line[BUFFER_SIZE];
  int response_line_len = 0;
  //Count request params
  content_len = strlen(content);
  url_len = strlen(host) + strlen(page);
  request_len = content_len + url_len + strlen(request_format);
  request = (char*)malloc(request_len);
  //Format request
  sprintf(request, request_format, page, content_len, content);
  printf("request %s\n", request);
  //Open socket
  socketfd = socket(AF_INET, SOCK_STREAM, 0);
  //Get host
	if ((hptr = gethostbyname(host)) == NULL) {
		fprintf(stderr, " gethostbyname error for host: %s: %s",
			host, hstrerror(h_errno));
		exit(1);
	}
	if (hptr->h_addrtype == AF_INET
	    && (pptr = hptr->h_addr_list) != NULL) {
    inet_ntop(hptr->h_addrtype, *pptr, addr, sizeof(addr));
	} else {
		fprintf(stderr, "Error call inet_ntop \n");
	}
  bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(HTTP_PORT);
	inet_pton(AF_INET, addr, &servaddr.sin_addr);
  //Finnaly, connect
	connect(socketfd, (struct sockaddr *) & servaddr, sizeof(servaddr));
  //Write request
  write(socketfd, request, strlen(request));
  while(response_line_len = read(socketfd, response_line, BUFFER_SIZE)) {
    response_line[response_line_len] = 0;
    printf("%s\n", response_line);
  }
  close(socketfd);
  return content;
}

short url_allowed(char c) {
  if (MIN_ULETTER_CODE <= c && c <= MAX_ULETTER_CODE ) return 1; //[A-Z]
  if (MIN_LLETTER_CODE <= c && c <= MAX_LLETTER_CODE) return 1; //[a-z]
  if (MIN_NUMBER_CODE <= c && c <= MAX_NUMBER_CODE) return 1; //[0-9]
  if (c == DOT_CODE) return 1; // [.]
  if (c == LOWCASE_CODE) return 1; // [_]
  if (c == TILDA_CODE) return 1; // [~]
  return 0;
}

char* url_encode(char* string, int length){
  char* encoded;
  char char_encoded[3] = "%00";
  int encoded_length = 0, i, pos = 0, char_len;
  //Count encode length - all not allowed characters will take three symbols
  for (i = 0; i < length; i++) {
    encoded_length += url_allowed(string[i])?1:3;
  }
  //Set encoded size
  encoded = (char*)malloc(encoded_length+1);
  //Encode
  for (i = 0; i < length; i++) {
    char_len = url_allowed(string[i])?1:3;
    if (char_len == 1) {
      memcpy(encoded + pos, &string[i], 1);
    } else {
      sprintf(char_encoded, "%%%02X", (unsigned char)string[i]);
      memcpy(encoded + pos, char_encoded, char_len);
    }
    pos += char_len;
  }
  encoded[encoded_length] = 0;
  return encoded;
}
