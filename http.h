#ifndef HTTP_H_
#define HTTP_H_

char* post(char* schema, char* host, char* page, char* content);
char* url_encode(char* content, int length);

#endif
