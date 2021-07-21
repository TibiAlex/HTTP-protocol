#include <iostream>
#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
/*
* Buzera Tiberiu 323CA
*/

#include <arpa/inet.h>
#include "helpers.hpp"
#include "requests.hpp"
#include <string.h>
#include <string>
#include <vector>

using namespace std;

// structura pentru retinerea datelor despre carti
typedef struct {
    char title[30];
    char author[30];
    char genre[20];
    char publisher[20];
    int page_count;
} book_info;

#define MSG_LEN 8192

// functie ce se ocupa cu inregistrarea unui nou client
void process_register(int sockfd) {
    char username[50], password[50];
    scanf("%s %s", username, password);
    // Cream un payload ce contine numele de utilizator si parolasub forma de JSON
    char **payload = (char**)malloc(sizeof(char*));
    payload[0] = (char*)malloc(MSG_LEN * sizeof(char));
    strcat(payload[0], "{");
    strcat(payload[0], "\"username\":");
    strcat(payload[0], "\"");
    strcat(payload[0], username);
    strcat(payload[0], "\"");
    strcat(payload[0], ",");
    strcat(payload[0], "\"password\":");
    strcat(payload[0], "\"");
    strcat(payload[0], password);
    strcat(payload[0], "\"");
    strcat(payload[0], "}");
    // Trimitem un request la server
    char *request = compute_post_request((char*)"34.118.48.238",
        (char*)"/api/v1/tema/auth/register", (char*)"application/json",
        payload, 1, NULL, 0, false);
    send_to_server(sockfd, request);
    // Primim inapoi un mesaj de la server
    char *receive = receive_from_server(sockfd);
    // Afisam daca a avut loc operatia cu succes sau daca a fost eroare
    char tmp[MSG_LEN];
    strcpy(tmp, receive);
    char *p = strtok(tmp, " ");
    p = strtok(NULL, " ");
    if(strcmp(p, "201") == 0) {
        printf("The account with username %s has been created!\n", username);
    } else {
        char *a = basic_extract_json_response(receive);
        p = strtok(a, "\"");
        p = strtok(NULL, "\"");
        p = strtok(NULL, "\"");
        p = strtok(NULL, "\"");
        printf("%s\n", p);
    }
}

// functie ce se ocupa cu login-ul unui utilizator
void process_login(int sockfd, vector<string> &cookies) {
    char username[50], password[50];
    scanf("%s %s", username, password);
    // Cream un payload ce contine numele de utilizator si parolasub forma de JSON
    char **payload = (char**)malloc(sizeof(char*));
    payload[0] = (char*)malloc(MSG_LEN * sizeof(char));
    strcat(payload[0], "{");
    strcat(payload[0], "\"username\":");
    strcat(payload[0], "\"");
    strcat(payload[0], username);
    strcat(payload[0], "\"");
    strcat(payload[0], ",");
    strcat(payload[0], "\"password\":");
    strcat(payload[0], "\"");
    strcat(payload[0], password);
    strcat(payload[0], "\"");
    strcat(payload[0], "}");
    // Trimitem un request la server
    char *request = compute_post_request((char*)"34.118.48.238",
     (char*)"/api/v1/tema/auth/login", 
     (char*)"application/json", payload, 1, NULL, 0, false);
    send_to_server(sockfd, request);
    // Primim inapoi un mesaj de la server
    char *receive = receive_from_server(sockfd);
    // Afisam daca a avut loc operatia cu succes sau daca a fost eroare
    char tmp[MSG_LEN];
    strcpy(tmp, receive);
    char *p = strtok(tmp, " ");
    p = strtok(NULL, " ");
    if(strcmp(p, "200") == 0) {
        char *cookie = strtok(receive, " ");
        while(cookie != NULL) {
            cookie = strtok(NULL, " ");
            if(strstr(cookie, "Set-Cookie:") != NULL) {
                    cookie = strtok(NULL, " ");
                    break;
            }
        }
        cookies.push_back(cookie);
        cout << "S-a conectat userul " << username << " la server!" << endl; 
    } else {
        char *a = basic_extract_json_response(receive);
        p = strtok(a, "\"");
        p = strtok(NULL, "\"");
        p = strtok(NULL, "\"");
        p = strtok(NULL, "\"");
        printf("%s\n", p);
    }
}

// functie de ce ocupa cu accesarea librariei de catre un utilizator
void process_enter_library(int sockfd, vector<string> &cookies, vector<string> &tokens) {
    char *request;
    // verificam daca exista un utilizator logat
    if(cookies.size() == 0) {
        // Trimitem un request la server
        request = compute_get_request((char*)"34.118.48.238",
         (char*)"/api/v1/tema/library/access", NULL, NULL, 0, false);
    } else {
        // Trimitem un request la server
        char **tmp = (char**)malloc(sizeof(char*));
        tmp[0] = (char*)malloc(MSG_LEN * sizeof(char));
        strcpy(tmp[0], (char*)cookies[cookies.size() - 1].c_str());
        request = compute_get_request((char*)"34.118.48.238",
         (char*)"/api/v1/tema/library/access", NULL, tmp, 1, false);
    }
    send_to_server(sockfd, request);
    // Primim inapoi un mesaj de la server
    char *receive = receive_from_server(sockfd);
    // Afisam daca a avut loc operatia cu succes sau daca a fost eroare
    char tmp[MSG_LEN];
    strcpy(tmp, receive);
    char *p = strtok(tmp, " ");
    p = strtok(NULL, " ");
    if(strcmp(p, "200") == 0) {
        char *a = basic_extract_json_response(receive);
        p = strtok(a, "\"");
        p = strtok(NULL, "\"");
        p = strtok(NULL, "\"");
        p = strtok(NULL, "\"");
        tokens.push_back(p);
        cout << "Tokenul a fost salvat!" << endl;
    } else {
        char *a = basic_extract_json_response(receive);
        p = strtok(a, "\"");
        p = strtok(NULL, "\"");
        p = strtok(NULL, "\"");
        p = strtok(NULL, "\"");
        printf("%s\n", p);
    }
}

// functie ce afisaza ce carti se afisaza in biblioteca unui utilizator
void process_get_books(int sockfd, vector<string> &tokens) {
    char *request;
    // vetificam daca exista vreun utilizator conectat
    if(tokens.size() == 0) {
        // Trimitem un request la server
        request = compute_get_request((char*)"34.118.48.238",
         (char*)"/api/v1/tema/library/books", NULL, NULL, 0, false);
    } else {
        // Trimitem un request la server
        char **tmp = (char**)malloc(sizeof(char*));
        tmp[0] = (char*)malloc(MSG_LEN * sizeof(char));
        strcpy(tmp[0], (char*)tokens[tokens.size() - 1].c_str());
        char bearer[MSG_LEN];
        strcpy(bearer, "Bearer ");
        strcat(bearer, tmp[0]);
        strcpy(tmp[0], bearer);
        request = compute_get_request((char*)"34.118.48.238",
         (char*)"/api/v1/tema/library/books", NULL, tmp, 1, true);
    }
    send_to_server(sockfd, request);
    // Primim inapoi un mesaj de la server
    char *receive = receive_from_server(sockfd);
    // Afisam daca a avut loc operatia cu succes sau daca a fost eroare
    char tmp[MSG_LEN];
    strcpy(tmp, receive);
    char *p = strtok(tmp, " ");
    p = strtok(NULL, " ");
    if(strcmp(p, "200") == 0) {
        char *a = basic_extract_json_response(receive);
        if(a == NULL) {
            printf("Nu exista carti de afisat!\n");
        } else {
            printf("[%s\n", a); 
        }
    } else {
        char *a = basic_extract_json_response(receive);
        p = strtok(a, "\"");
        p = strtok(NULL, "\"");
        p = strtok(NULL, "\"");
        p = strtok(NULL, "\"");
        printf("%s\n", p);
    }
}

// fucntie ce afisaza informatii despre o carte cu un anumit id
void process_get_book(int sockfd, vector<string> &tokens) {
    int id;
    cin >> id;
    char *request;
    char url[50];
    string buffer = to_string(id);
    strcpy(url, "/api/v1/tema/library/books/");
    strcat(url, buffer.c_str());
    // verificam daca este vreun utilizator conectat
    if(tokens.size() == 0) {
        // Trimitem un request la server
        request = compute_get_request((char*)"34.118.48.238", url, NULL, NULL, 0, false);
    } else {
        // Trimitem un request la server
        char **tmp = (char**)malloc(sizeof(char*));
        tmp[0] = (char*)malloc(MSG_LEN * sizeof(char));
        strcpy(tmp[0], (char*)tokens[tokens.size() - 1].c_str());
        char bearer[MSG_LEN];
        strcpy(bearer, "Bearer ");
        strcat(bearer, tmp[0]);
        strcpy(tmp[0], bearer);
        request = compute_get_request((char*)"34.118.48.238", url, NULL, tmp, 1, true);
    }
    send_to_server(sockfd, request);
    // Primim inapoi un mesaj de la server
    char *receive = receive_from_server(sockfd);
    // Afisam daca a avut loc operatia cu succes sau daca a fost eroare
    char tmp[MSG_LEN];
    strcpy(tmp, receive);
    char *p = strtok(tmp, " ");
    p = strtok(NULL, " ");
    if(strcmp(p, "200") == 0) {
        char *a = basic_extract_json_response(receive);
        a[strlen(a)-1] = '\0';
        printf("%s\n", a);
    } else {
        char *a = basic_extract_json_response(receive);
        p = strtok(a, "\"");
        p = strtok(NULL, "\"");
        p = strtok(NULL, "\"");
        p = strtok(NULL, "\"");
        printf("%s\n", p);
    }
}

// functie ce se ocupa cu adaugarea unei noi carti in biblioteca unui client
void process_add_book(int sockfd, vector<string> &tokens) {
    book_info book;
    // citim informatii despre carte in structura declarata
    cin >> book.title >> book.author >> book.genre >> book.publisher >> book.page_count;
    // cream payload-ul cu informatii depre carte sub forma de JSON
    char **payload = (char**)malloc(sizeof(char*));
    payload[0] = (char*)malloc(MSG_LEN * sizeof(char));
    strcat(payload[0], "{");
    strcat(payload[0], "\"title\":");
    strcat(payload[0], "\"");
    strcat(payload[0], book.title);
    strcat(payload[0], "\"");
    strcat(payload[0], ",");
    strcat(payload[0], "\"author\":");
    strcat(payload[0], "\"");
    strcat(payload[0], book.author);
    strcat(payload[0], "\"");
    strcat(payload[0], ",");
    strcat(payload[0], "\"genre\":");
    strcat(payload[0], "\"");
    strcat(payload[0], book.genre);
    strcat(payload[0], "\"");
    strcat(payload[0], ",");
    strcat(payload[0], "\"page_count\":");
    strcat(payload[0], "\"");
    strcat(payload[0], to_string(book.page_count).c_str());
    strcat(payload[0], "\"");
    strcat(payload[0], ",");
    strcat(payload[0], "\"publisher\":");
    strcat(payload[0], "\"");
    strcat(payload[0], book.publisher);
    strcat(payload[0], "\"");
    strcat(payload[0], "}");
    char *request;
    // verificam daca este vreun utilizator conectat
    if(tokens.size() == 0) {
        // Trimitem un request la server
        request = compute_post_request((char*)"34.118.48.238",
         (char*)"/api/v1/tema/library/books", (char*)"application/json",
          payload, 1, NULL, 0, false);
    } else {
        // Trimitem un request la server
        char **tmp = (char**)malloc(sizeof(char*));
        tmp[0] = (char*)malloc(MSG_LEN * sizeof(char));
        strcpy(tmp[0], (char*)tokens[tokens.size() - 1].c_str());
        char bearer[MSG_LEN];
        strcpy(bearer, "Bearer ");
        strcat(bearer, tmp[0]);
        strcpy(tmp[0], bearer);
        request = compute_post_request((char*)"34.118.48.238",
         (char*)"/api/v1/tema/library/books", (char*)"application/json",
          payload, 1, tmp, 1, true);
    }
    send_to_server(sockfd, request);
    // Primim inapoi un mesaj de la server
    char *receive = receive_from_server(sockfd);
    // Afisam daca a avut loc operatia cu succes sau daca a fost eroare
    char tmp[MSG_LEN];
    strcpy(tmp, receive);
    char *p = strtok(tmp, " ");
    p = strtok(NULL, " ");
    if(strcmp(p, "200") == 0) {
        printf("The book has been added successfully!\n");
    } else {
        char *a = basic_extract_json_response(receive);
        p = strtok(a, "\"");
        p = strtok(NULL, "\"");
        p = strtok(NULL, "\"");
        p = strtok(NULL, "\"");
        printf("%s\n", p);
    }
}

// functie ce se ocupa cu stergerea unei carti din biblioteca unui utilizator
void process_delete_book(int sockfd, vector<string> &tokens) {
    int id;
    cin >> id;
    char url[50];
    string buffer = to_string(id);
    strcpy(url, "/api/v1/tema/library/books/");
    strcat(url, buffer.c_str());
    char *request;
    cout << url << endl;
    // verificam daca este vreun utilizator conectat
    if(tokens.size() == 0) {
        // Trimitem un request la server
        request = compute_delete_request((char*)"34.118.48.238",
         url, (char*)"application/json", NULL, 0, NULL, 0, false);
    } else {
        // Trimitem un request la server
        char **tmp = (char**)malloc(sizeof(char*));
        tmp[0] = (char*)malloc(MSG_LEN * sizeof(char));
        strcpy(tmp[0], (char*)tokens[tokens.size() - 1].c_str());
        char bearer[MSG_LEN];
        strcpy(bearer, "Bearer ");
        strcat(bearer, tmp[0]);
        strcpy(tmp[0], bearer);
        request = compute_delete_request((char*)"34.118.48.238",
         url, (char*)"application/json", NULL, 0, tmp, 1, true);
    }
    send_to_server(sockfd, request);
    // Primim inapoi un mesaj de la server
    char *receive = receive_from_server(sockfd);
    // Afisam daca a avut loc operatia cu succes sau daca a fost eroare
    char tmp[MSG_LEN];
    strcpy(tmp, receive);
    char *p = strtok(tmp, " ");
    p = strtok(NULL, " ");
    if(strcmp(p, "200") == 0) {
        printf("The book has been deleted successfully!\n");
    } else {
        char *a = basic_extract_json_response(receive);
        p = strtok(a, "\"");
        p = strtok(NULL, "\"");
        p = strtok(NULL, "\"");
        p = strtok(NULL, "\"");
        printf("%s\n", p);
    }
}


// functie care se ocupa de procesul de logout al unui utilizator
void process_logout(int sockfd, vector<string> &cookies, vector<string> &tokens) {
    char *request;
    // verificam daca este vreun utilizator conectat
    if(cookies.size() == 0) {
        // Trimitem un request la server
        request = compute_get_request((char*)"34.118.48.238",
         (char*)"/api/v1/tema/auth/logout", NULL, NULL, 0, false);
    } else {
        // Trimitem un request la server
        char **tmp = (char**)malloc(sizeof(char*));
        tmp[0] = (char*)malloc(MSG_LEN * sizeof(char));
        strcpy(tmp[0], (char*)cookies[cookies.size() - 1].c_str());
        request = compute_get_request((char*)"34.118.48.238",
         (char*)"/api/v1/tema/auth/logout", NULL, tmp, 1, false);
    }
    send_to_server(sockfd, request);
    // Primim inapoi un mesaj de la server
    char *receive = receive_from_server(sockfd);
    if(cookies.size() != 0) cookies.pop_back(); // golim vectirii de cookies si tokens
    if(tokens.size() != 0) tokens.pop_back();
    // Afisam daca a avut loc operatia cu succes sau daca a fost eroare
    char tmp[MSG_LEN];
    strcpy(tmp, receive);
    char *p = strtok(tmp, " ");
    p = strtok(NULL, " ");
    if(strcmp(p, "200") == 0) {
        printf("You have been logged out!\n");
    } else {
        char *a = basic_extract_json_response(receive);
        p = strtok(a, "\"");
        p = strtok(NULL, "\"");
        p = strtok(NULL, "\"");
        p = strtok(NULL, "\"");
        printf("%s\n", p);
    }
}

int main() {
    int sockfd;
    char command[50];
    vector<string> cookies;
    vector<string> tokens;
    // citim comenzi pana la intalnirea comenzii "exit"
    while(1) {
        sockfd = open_connection((char*)"34.118.48.238", 8080, AF_INET, SOCK_STREAM, 0);
        scanf("%s", command);
        if(strcmp(command, "register") == 0) {
            process_register(sockfd);
        } else if(strcmp(command, "login") == 0) {
            process_login(sockfd, cookies);
        } else if(strcmp(command, "enter_library") == 0) {
            process_enter_library(sockfd, cookies, tokens);
        } else if(strcmp(command, "get_books") == 0) {
            process_get_books(sockfd, tokens);
        } else if(strcmp(command, "get_book") == 0) {
            process_get_book(sockfd, tokens);
        } else if(strcmp(command, "add_book") == 0) {
            process_add_book(sockfd, tokens);
        } else if(strcmp(command, "delete_book") == 0) {
            process_delete_book(sockfd, tokens);
        } else if(strcmp(command, "logout") == 0) {
            process_logout(sockfd, cookies, tokens);
        } else if(strcmp(command, "exit") == 0) {
                break;
        }
    }
}