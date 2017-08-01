#include <string.h>
#include <stdlib.h>
#include <curl/curl.h>

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

char* post(char* url, char* content, void* callback) {
    CURL *curl;
    CURLcode res;

    /* In windows, this will init the winsock stuff */
    curl_global_init(CURL_GLOBAL_ALL);

    /* get a curl handle */
    curl = curl_easy_init();
    if(curl) {
      /* First set the URL that is about to receive our POST. This URL can
         just as well be a https:// URL if that is what should receive the
         data. */
      curl_easy_setopt(curl, CURLOPT_URL, url);
      /* Now specify the POST data */
      curl_easy_setopt(curl, CURLOPT_POSTFIELDS, content);
      /* Set callback */
      curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback);
      curl_easy_setopt(curl, CURLOPT_WRITEDATA, curl);

      /* Perform the request, res will get the return code */
      res = curl_easy_perform(curl);
      /* Check for errors */
      if(res != CURLE_OK)
        fprintf(stderr, "Error on pastebin posting: %s\n",
                curl_easy_strerror(res));

      /* always cleanup */
      curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
    return 0;
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
