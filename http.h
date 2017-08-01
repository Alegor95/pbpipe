#ifndef HTTP_H_
#define HTTP_H_

char* post(char* url, char* content, void* callback);
char* url_encode(char* content, int length);

#endif
