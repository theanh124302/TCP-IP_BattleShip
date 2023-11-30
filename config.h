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
#include <fstream>

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

struct Account
{
    std::string username;
    std::string password;
    int status;
    int sign;
    int incorrect;

    Account (std::string user, std::string pass, int sta, int si, int incor)
    {
        username = user;
        password = pass;
        status = sta;
        sign = si;
        incorrect = incor;
    }

    Account(){};
};

// Khai báo các biến toàn cục ở đây
std::vector<Account> accountsList;


#endif // CONFIG_H



