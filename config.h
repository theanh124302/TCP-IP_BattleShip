#ifndef CONFIG_H

#define CONFIG_H
#define PORT 9000
#define MAX_CLIENTS 5
#define BUFF_SIZE 1024

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <cstring>

enum TypeMassage
{
    LOGIN = 1,
    LOGOUT,
    SIGNUP,
    CHANGE_PASS,
    FIND_MATCH,
    EXIT_FIND_MATCH,
    READY,
    ATTACK,
    GIVEUP,
    REPLAY
};

struct Node
{
    char username[BUFF_SIZE];
    char password[BUFF_SIZE];
    int status;
    int sign;
    int incorrect;
    struct Node *next;
};

#endif // CONFIG_H



