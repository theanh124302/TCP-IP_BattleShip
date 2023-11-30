#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 8080
#define MAX_CLIENTS 5
#define BUFF_SIZE 1024
int request = 0;

struct Node {
    char username[BUFF_SIZE];
    char password[BUFF_SIZE];
    int status;
    int sign;
    int incorrect;
    struct Node* next;
};
void insertNode(struct Node** head, char* user, char* pass, int stat, int sign, int incorrect) {
    struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));
    strcpy(newNode->username, user);
    strcpy(newNode->password, pass);
    newNode->status = stat;
    newNode->sign = sign;
    newNode->incorrect = incorrect;
    newNode->next = *head;
    *head = newNode;
}
int SignIn(char* user, char* pass) {
    struct Node* head = NULL;
    char username[BUFF_SIZE];
    char password[BUFF_SIZE];
    char fileName[] = "account.txt";
    FILE* file = fopen(fileName, "r");
    int stat, sign, incorrect;
    while (fscanf(file, "%s %s %d %d %d", username, password, &stat, &sign, &incorrect) != EOF) {
        insertNode(&head, username, password, stat, sign, incorrect);
    }
    fclose(file);
    struct Node* current = head;
    while (current != NULL) {
        if (strcmp(current->username, user) == 0) {
            if(current->status==0){
                //Tai khoan bi block 
                return 4;
            }
            while(1){
                if(strcmp(current->password, pass) != 0){
                    current->incorrect++;
                    if(current->incorrect >=3){
                        current->status = 0;
                            FILE* file = fopen(fileName, "w");
                            current = head;
                            while (current != NULL) {
                                fprintf(file, "%s %s %d %d %d\n", current->username, current->password, current->status, current->sign, current->incorrect);
                                current = current->next;
                            }
                            fclose(file);
                        //Tai khoan da bi khoa do nhap sai qua 3 lan
                        return 2;
                    }
                    else
                    {
                        FILE* file = fopen(fileName, "w");
                        current = head;
                        while (current != NULL) {
                            fprintf(file, "%s %s %d %d %d\n", current->username, current->password, current->status, current->sign, current->incorrect);
                            current = current->next;
                        }
                        fclose(file);
                        //Sai pass
                        return 3;
                    }
                }
                else if (current->sign==0)
                {
                    current->incorrect=0;
                    current->sign=1;
                    FILE* file = fopen(fileName, "w");
                    current = head;
                    while (current != NULL) {
                        fprintf(file, "%s %s %d %d %d\n", current->username, current->password, current->status, current->sign, current->incorrect);
                        current = current->next;
                    }
                    fclose(file);
                    //Login thanh cong
                    return 1;
                }else{
                    //Tai khoan dang duoc dang nhap o client khac
                    return 5;
                }
            }
        }
        current = current->next;
    }
    //Khong tim duoc user
    return 0;
}
int SignOut(char* user){
    struct Node* head = NULL;
    char username[BUFF_SIZE];
    char password[BUFF_SIZE];
    char fileName[] = "account.txt";
    FILE* file = fopen(fileName, "r");
    int stat, sign, incorrect;
    while (fscanf(file, "%s %s %d %d %d", username, password, &stat, &sign, &incorrect) != EOF) {
        insertNode(&head, username, password, stat, sign, incorrect);
    }
    struct Node* current = head;
    fclose(file);
    file = fopen(fileName, "w");
    while (current != NULL) {
        if (strcmp(current->username, user) == 0) {
            current->sign=0;
        }
        current = current->next;
    }
    current = head;
    while (current != NULL) {
        fprintf(file, "%s %s %d %d %d\n", current->username, current->password, current->status, current->sign, current->incorrect);
        current = current->next;
    }
    fclose(file);
    return 0;
}

int SignUp(char* user, char* pass){
    struct Node* head = NULL;
    char username[BUFF_SIZE];
    char password[BUFF_SIZE];
    char fileName[] = "account.txt";
    FILE* file = fopen(fileName, "r");
    int stat, sign, incorrect;
    while (fscanf(file, "%s %s %d %d %d", username, password, &stat, &sign, &incorrect) != EOF) {
        insertNode(&head, username, password, stat, sign, incorrect);
    }
    fclose(file);
    struct Node* current = head;
    while (current != NULL) {
        if (strcmp(current->username, user) == 0) {
            //User da ton tai
            return 1;
        }
        current = current->next;
    }
    insertNode(&head, user, pass, 1, 0, 0);
    file = fopen(fileName, "w");
    current = head;
    while (current != NULL) {
        fprintf(file, "%s %s %d %d %d\n", current->username, current->password, current->status, current->sign, current->incorrect);
        current = current->next;
    }
    fclose(file);
    //Dang ky thanh cong
    return 0;
}

int ChangePass(char* user, char* newpass){
    struct Node* head = NULL;
    char username[BUFF_SIZE];
    char password[BUFF_SIZE];
    char fileName[] = "account.txt";
    FILE* file = fopen(fileName, "r");
    int stat, sign, incorrect;
    while (fscanf(file, "%s %s %d %d %d", username, password, &stat, &sign, &incorrect) != EOF) {
        insertNode(&head, username, password, stat, sign, incorrect);
    }
    struct Node* current = head;
    fclose(file);
    file = fopen(fileName, "w");
    while (current != NULL) {
        if (strcmp(current->username, user) == 0) {
            strcpy(current->password,newpass);
        }
        current = current->next;
    }
    current = head;
    while (current != NULL) {
        fprintf(file, "%s %s %d %d %d\n", current->username, current->password, current->status, current->sign, current->incorrect);
        current = current->next;
    }
    fclose(file);
    return 0;
}


void *handle_client(void *socket_desc) {
    int client_socket = *(int *)socket_desc;
    char buffer[BUFF_SIZE];
    char user[BUFF_SIZE];
    char pass[BUFF_SIZE];
    char cfpass[BUFF_SIZE];
    char newpass[BUFF_SIZE];
    char resultString[BUFF_SIZE];
    int i,j,opt,result,sign=0;
    int read_size;
    while ((read_size = recv(client_socket, buffer, BUFF_SIZE, 0)) > 0) {
        write(1, buffer, read_size);
        buffer[read_size-1]='\0';
        if(buffer[0]<48||buffer[0]>57){
            send(client_socket, "Type error!", BUFF_SIZE, 0);
        }
        else{
            opt=buffer[0]-48;
            if(opt==1){ //1user+pass
                i=1;
                j=0;
                while (buffer[i]!=43)
                {
                    user[j]=buffer[i];
                    i++;
                    j++;
                }
                user[j]='\0';
                printf("%s",user);
                j=0;
                i++;

                while (buffer[i]!=0)
                {
                    pass[j]=buffer[i];
                    i++;
                    j++;
                }
                pass[j]='\0';
                j=0;
                result = SignIn(user,pass);
                if(result==1){
                    sign = 1;
                }
                sprintf(resultString, "1%d", result);
                send(client_socket, resultString, BUFF_SIZE, 0);
            }
            
            
            else if (opt==2){ //2user
                i=1;
                j=0;
                while (buffer[i]!=0)
                {
                    user[j]=buffer[i];
                    i++;
                    j++;
                }
                user[j]='\0';
                SignOut(user);
                send(client_socket, "20", BUFF_SIZE, 0);
            }
            
            else if (opt == 3){ //3user+pass+cfpass
                i=1;
                j=0;
                while (buffer[i]!=43)
                {
                    user[j]=buffer[i];
                    i++;
                    j++;
                }
                user[j]='\0';
                printf("%s",user);
                j=0;
                i++;

                while (buffer[i]!=43)
                {
                    pass[j]=buffer[i];
                    i++;
                    j++;
                }
                pass[j]='\0';
                j=0;
                i++;
                while (buffer[i]!=0)
                {
                    cfpass[j]=buffer[i];
                    i++;
                    j++;
                }
                cfpass[j]='\0';
                j=0;
                i++;
                if(strcmp(pass,cfpass)!=0){ //User da ton tai
                    send(client_socket, "32", BUFF_SIZE, 0);
                }else{
                result = SignUp(user,pass);
                sprintf(resultString, "3%d", result);
                send(client_socket, resultString, BUFF_SIZE, 0);
                }
            }
            

            else if (opt == 4){ //4user+pass=newpass
                i=1;
                j=0;
                while (buffer[i]!=43)
                {
                    user[j]=buffer[i];
                    i++;
                    j++;
                }
                user[j]='\0';
                printf("%s",user);
                j=0;
                i++;

                while (buffer[i]!=43)
                {
                    pass[j]=buffer[i];
                    i++;
                    j++;
                }
                pass[j]='\0';
                j=0;
                i++;
                while (buffer[i]!=0)
                {
                    newpass[j]=buffer[i];
                    i++;
                    j++;
                }
                newpass[j]='\0';
                j=0;
                i++;
                if(strcmp(pass,newpass)==0){ //pass moi trung pass cu
                    send(client_socket, "41", BUFF_SIZE, 0);
                }else{
                result = ChangePass(user,newpass);
                sprintf(resultString, "4%d", result);
                send(client_socket, resultString, BUFF_SIZE, 0);
                }
            }
            
            else{
                send(client_socket, "NonOpt", BUFF_SIZE, 0);
            }
        }
    }
    if (read_size == 0) {
        printf("Client disconnected\n");
        if(sign == 1){
            SignOut(user);
            sign = 0;
        }
    } else if (read_size == -1) {
        perror("recv failed");
    }

    close(client_socket);
    //free(client_socket);
    return NULL;
}

int main() {
    int server_fd, new_socket, c;
    struct sockaddr_in server, client;

    // Tạo socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Cấu hình địa chỉ và cổng của server
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

    // Gán địa chỉ và cổng cho socket
    if (bind(server_fd, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Lắng nghe kết nối đến server
    if (listen(server_fd, MAX_CLIENTS) < 0) {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    puts("Waiting for incoming connections...");

    c = sizeof(struct sockaddr_in);

    while ((new_socket = accept(server_fd, (struct sockaddr *)&client, (socklen_t *)&c))) {
        puts("Connection accepted");

        pthread_t sniffer_thread;
        int new_sock[BUFF_SIZE];
        *new_sock = new_socket;

        // Tạo thread để xử lý client mới
        if (pthread_create(&sniffer_thread, NULL, handle_client, (void *)new_sock) < 0) {
            perror("could not create thread");
            return 1;
        }

        // Gắn kết thúc của thread
        pthread_detach(sniffer_thread);
        puts("Handler assigned");
    }

    if (new_socket < 0) {
        perror("accept failed");
        return 1;
    }

    return 0;
}