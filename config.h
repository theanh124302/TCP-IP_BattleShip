#ifndef CONFIG_H

#define CONFIG_H
#define PORT 9000
#define MAX_CLIENTS 10
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
    std::string position;
    //int socket;
    int accountId;
    int opponent;
    int status;
    int sign;
    int incorrect;
    int findStatus;
    int boardId;

    Account (std::string user, std::string pass, std::string pos/*, int client_socket*/, int id, int opp, int sta, int si, int incor, int find, int board)
    {
        username = user;
        password = pass;
        position = pos;
        //socket = client_socket;
        accountId = id;
        opponent = opp;
        status = sta;
        sign = si;
        incorrect = incor;
        findStatus = find;

        boardId = board;
    }

    Account(){};
};
struct Board
{
    int id;
    int p1ID;
    int p2ID;
    std::string p1;
    std::string p2;
    Board (int NewID, int p1_ID, int p2_ID, std::string p1pos, std::string p2pos){
        id = NewID;
        p1ID = p1_ID;
        p2ID = p2_ID;
        p1 = p1pos;
        p2 = p2pos;
    }
    Board(){};
};

std::vector<Account> accountsList;
std::vector<Board> boardList;

#endif // CONFIG_H



