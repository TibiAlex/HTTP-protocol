#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <stdio.h>
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.hpp"
#include "requests.hpp"
#include <iostream>

#define MSG_LEN 8192

char *compute_get_request(char *host, char *url, char *query_params,
                            char **cookies, int cookies_count, bool has_header)
{
    char *message = (char*)calloc(BUFLEN, sizeof(char));
    char *line = (char*)calloc(LINELEN, sizeof(char));

    // Step 1: write the method name, URL, request params (if any) and protocol type
    if (query_params != NULL) {
        sprintf(line, "GET %s?%s HTTP/1.1", url, query_params);
    } else {
        sprintf(line, "GET %s HTTP/1.1", url);
    }

    compute_message(message, line);

    // Step 2: add the host
    memset(line, 0, LINELEN);
    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    // Step 3 (optional): add headers and/or cookies, according to the protocol format
    if (cookies != NULL) {
        if(has_header == true) {
            char auth[MSG_LEN];
            strcpy(auth, "Authorization: ");
            strcat(auth, cookies[0]);
            compute_message(message, auth);
        } else {
            memset(line, 0, LINELEN);
            strcat(line, "Cookie: ");
            for (int i = 0; i < cookies_count - 1; i++) {
                strcat(line, cookies[i]); 
                strcat(line, ";"); 
            }

            strcat(line, cookies[cookies_count - 1]); 
            compute_message(message, line);
        }
    }
    
    // Step 4: add final new line
    compute_message(message, "");
    return message;
}

char *compute_post_request(char *host, char *url, char* content_type, char **body_data,
                            int body_data_fields_count, char **cookies, int cookies_count, bool has_header)
{
    char *message = (char*)calloc(BUFLEN, sizeof(char));
    char *line = (char*)calloc(LINELEN, sizeof(char));
    char *body_data_buffer = (char*)calloc(LINELEN, sizeof(char));

    // Step 1: write the method name, URL and protocol type
    sprintf(line, "POST %s HTTP/1.1", url);
    compute_message(message, line);
    
    // Step 2: add the host
    memset(line, 0, LINELEN);
    sprintf(line, "Host: %s", host);
    compute_message(message, line);
    /* Step 3: add necessary headers (Content-Type and Content-Length are mandatory)
            in order to write Content-Length you must first compute the message size
    */

    int body_data_size = 0;
    for (int i = 0; i < body_data_fields_count - 1; i++) {
        strcat(body_data_buffer, body_data[i]); 
        strcat(body_data_buffer, "&"); 

        body_data_size += strlen(body_data[i]) + 1;
    }
    strcat(body_data_buffer, body_data[body_data_fields_count - 1]); 
    body_data_size += strlen(body_data[body_data_fields_count - 1]);

    memset(line, 0, LINELEN);
    sprintf(line, "Content-Type: %s", content_type);
    compute_message(message, line);

    memset(line, 0, LINELEN);
    sprintf(line, "Content-Length: %d", body_data_size);
    compute_message(message, line);


    // Step 4 (optional): add cookies
    if (cookies != NULL) {
        if(has_header == true) {
            char auth[MSG_LEN];
            strcpy(auth, "Authorization: ");
            strcat(auth, cookies[0]);
            compute_message(message, auth);
        } else {
            memset(line, 0, LINELEN);
            strcat(line, "Cookie: ");
            for (int i = 0; i < cookies_count - 1; i++) {
                strcat(line, cookies[i]); 
                strcat(line, ";"); 
            }

            strcat(line, cookies[cookies_count - 1]); 
            compute_message(message, line);
        }
    }
    // Step 5: add new line at end of header
    compute_message(message, "");

    // Step 6: add the actual payload data
    strcat(message, body_data_buffer);

    free(line);
    return message;
}

char *compute_delete_request(char *host, char *url, char* content_type, char **body_data,
                            int body_data_fields_count, char **cookies, int cookies_count, bool has_header)
{
    char *message = (char*)calloc(BUFLEN, sizeof(char));
    char *line = (char*)calloc(LINELEN, sizeof(char));
    char *body_data_buffer = (char*)calloc(LINELEN, sizeof(char));

    // Step 1: write the method name, URL and protocol type
    sprintf(line, "DELETE %s HTTP/1.1", url);
    compute_message(message, line);

    // Step 2: add the host
    memset(line, 0, LINELEN);
    sprintf(line, "Host: %s", host);
    compute_message(message, line);
    /* Step 3: add necessary headers (Content-Type and Content-Length are mandatory)
            in order to write Content-Length you must first compute the message size
    */

    int body_data_size = 0;
    for (int i = 0; i < body_data_fields_count - 1; i++) {
        strcat(body_data_buffer, body_data[i]); 
        strcat(body_data_buffer, "&"); 
        body_data_size += strlen(body_data[i]) + 1;
    }
    // strcat(body_data_buffer, body_data[body_data_fields_count - 1]); 
    // body_data_size += strlen(body_data[body_data_fields_count - 1]);

    memset(line, 0, LINELEN);
    sprintf(line, "Content-Type: %s", content_type);
    compute_message(message, line);
    memset(line, 0, LINELEN);
    sprintf(line, "Content-Length: %d", body_data_size);
    compute_message(message, line);

    // Step 4 (optional): add cookies
    if (cookies != NULL) {
        if(has_header == true) {
            char auth[MSG_LEN];
            strcpy(auth, "Authorization: ");
            strcat(auth, cookies[0]);
            compute_message(message, auth);
        } else {
            memset(line, 0, LINELEN);
            strcat(line, "Cookie: ");
            for (int i = 0; i < cookies_count - 1; i++) {
                strcat(line, cookies[i]); 
                strcat(line, ";"); 
            }

            strcat(line, cookies[cookies_count - 1]); 
            compute_message(message, line);
        }
    }
    // Step 5: add new line at end of header
    compute_message(message, "");

    // Step 6: add the actual payload data
    strcat(message, body_data_buffer);

    free(line);
    return message;
}