#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "http.h"

#define UNLISTED 1
#define PUBLIC 0
#define MAX_LINE_COUNT 1000
#define DEVELOPER_KEY "9054c14a9a763edcd4657437f696b689"
#define SEND_SCHEMA "https"
#define SEND_HOST "pastebin.com"
#define SEND_PAGE "api/api_post.php"

int read_stream(char* lines[MAX_LINE_COUNT], FILE* stream) {
  int i = 0;
  size_t line_s = 0, readed = 0;
  while (i < MAX_LINE_COUNT && !feof(stream)) {
    readed = getline(&lines[i], &line_s, stream);
    if ((int)(readed) <= 0) {
      break;
    }
    lines[i][readed] = 0;
    i++;
  }
  return i;
}

int read_texteditor(char* lines[MAX_LINE_COUNT]) {
  FILE* tmp_file;
  char temp_filename[] = "/tmp/XXXXXX";
  char editor_cmd[27];
  int line_cnt;
  mkstemp(temp_filename);
  sprintf(editor_cmd, "/usr/bin/editor %s", temp_filename);
  system(editor_cmd);
  tmp_file = fopen(temp_filename, "r");
  line_cnt = read_stream(lines, tmp_file);
  fclose(tmp_file);
  return line_cnt;
}

int main(int argc, char** argv) {
  unsigned char listtype = UNLISTED;
  char* request_format = "api_dev_key=%s&api_option=paste&api_paste_code=%s";
  char* request;
  char* response;
  char* title = "Created  with PBPASTE";
  char* code;
  char* encoded;
  char* lines[MAX_LINE_COUNT];
  int i = 0, line_count;
  int code_length = 0, position = 0, len = 0;
  //Read args
  for (i = 1; i < argc; i++) {
    if (argv[i][0] != '-') {
      printf("Unrecognized option %s\n", argv[i]);
      return 1;
    }
    switch (argv[i][1]) {
      case 't':
        title = argv[i+1];
        i++;
      break;
      case 'p':
        listtype = PUBLIC;
      break;
      case 'u':
        listtype = UNLISTED;
      break;
      default:
      printf("Unknown option %s\n", argv[i]);
      return 1;
    }
  }
  //If pipe is avaliable, read from pipe
  if (!isatty(fileno(stdin))) {
    line_count = read_stream(lines, stdin);
  } else {
    //In other cases - use text editor for open
    line_count = read_texteditor(lines);
  }
  printf("Title: %s, type: %d, lines %d\n", title, listtype, line_count);
  //Count code length
  for (i = 0; i < line_count; i++) {
    code_length += strlen(lines[i]);
  }
  code = (char*)malloc((code_length + 1)*sizeof(char));
  for (i = 0; i < line_count; i++) {
    len = strlen(lines[i]);
    memcpy(code + position, lines[i], len);
    position += len;
  }
  encoded = url_encode(code, code_length);
  request = (char*)malloc(strlen(DEVELOPER_KEY) + strlen(request_format) + strlen(encoded));
  sprintf(request, request_format, DEVELOPER_KEY, encoded);
  response = post(SEND_SCHEMA, SEND_HOST, SEND_PAGE, request);
  //Finalize
  free(code);
  free(encoded);
  free(request);
  fflush(stdout);
  return 0;
}