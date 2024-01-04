#include "config.h"

pthread_mutex_t mutex;
int request = 0;
int client_sockets[MAX_CLIENTS];

// Init file to account list
void ReadFile()
{
    std::string username;
    // Load file
    std::ifstream file("account.txt");
    if (!file.is_open())
    {
        printf("Error open file RM.txt\n");
        return;
    }
    // Read the file using fscanf
    while (file >> username)
    {
        Account acc = Account();
        acc.username = username;
        file >> acc.password >> acc.position >> acc.history >> acc.socket >> acc.accountId >> acc.elo >> acc.gold >> acc.skin >> acc.opponent >> acc.status >> acc.sign >> acc.incorrect >> acc.findStatus >> acc.boardId;
        accountsList.push_back(acc);
    }
    file.close();
}


// Rewrite to file
void WriteFile()
{
    pthread_mutex_lock(&mutex);
    FILE *file = fopen("account.txt", "w");
    for (auto it : accountsList)
    {
        fprintf(file, "%s %s %s %s %d %d %d %d %d %d %d %d %d %d %d\n", it.username.c_str(), it.password.c_str(), it.position.c_str(), it.history.c_str(), it.socket, it.accountId, it.elo, it.gold, it.skin, it.opponent, it.status, it.sign, it.incorrect, it.findStatus, it.boardId);
    }
    fclose(file);
    pthread_mutex_unlock(&mutex);
}



void ReadBoardFile()
{
    int boardID;
    // Load file
    std::ifstream file("board.txt");
    if (!file.is_open())
    {
        printf("Error open file RM.txt\n");
        return;
    }
    // Read the file using fscanf
    while (file >> boardID)
    {
        Board board = Board();
        board.id = boardID;
        file >> board.type >> board.p1ID >> board.p2ID >> board.p1 >> board.p2 >> board.socket1 >> board.socket2;
        boardList.push_back(board);
    }
    file.close();
}

void WriteBoardFile()
{
    FILE *file = fopen("board.txt", "w");
    for (auto it : boardList)
    {
        fprintf(file, "%d %d %d %d %s %s %d %d\n", it.id, it.type, it.p1ID, it.p2ID, it.p1.c_str(), it.p2.c_str(), it.socket1, it.socket2);
    }
    fclose(file);
}




int SignIn(std::string user, std::string pass, int client_socket)
{
    // Check acc
    for (auto &account : accountsList)
    {
        if (account.username == user)
        {
            if (account.status == 0)
            {
                // Tai khoan bi block
                return 4;
            }
            while (1)
            {
                if (account.password != pass)
                {
                    account.incorrect++;
                    if (account.incorrect >= 3)
                    {
                        account.status = 0;
                        WriteFile();
                        // Tai khoan da bi khoa do nhap sai qua 3 lan
                        return 2;
                    }
                    else
                    {
                        WriteFile();
                        // Sai pass
                        return 3;
                    }
                }
                else if (account.sign == 0)
                {
                    account.incorrect = 0;
                    account.sign = 1;
                    account.opponent = -1;
                    account.findStatus = 0;
                    account.socket = client_socket;
                    WriteFile();
                    // Login thanh cong
                    return 1;
                }
                else
                {
                    // Tai khoan dang duoc dang nhap o client khac
                    return 5;
                }
            }
        }
    }

    // Khong tim duoc user
    return 0;
}

void updateScore(int userID, int score){
    char history[BUFF_SIZE];
    for (auto &account : accountsList){
        if(account.accountId == userID){
            account.findStatus = 5;
            if((account.elo + score) <= 0){
                account.elo = 0;
            }
            else{
                account.elo += score;
            }
            strncpy(history,account.history.c_str(),BUFF_SIZE);
            if(score<0){
                strncat(history,"l",BUFF_SIZE-1);
                account.history = history;
            }else{
                strncat(history,"w",BUFF_SIZE-1);
                account.history = history;
            }
            break;
        }
    }
    WriteFile();
    return ;
}

int ExitMatch(std::string user){
    int accId,oppId,oppSoc,findStatus=0;
    for (auto &account : accountsList)
    {
        if (account.username == user){
            accId = account.accountId;
            oppId = account.opponent;
            account.opponent = -1;
            account.findStatus = 0;
            break;
        }
    }
    if(oppId!=-1){
        for (auto &account : accountsList)
        {
            if (account.accountId == oppId)
            {
                oppSoc = account.socket;
                account.opponent = -1;
                findStatus = account.findStatus;
                account.findStatus = 0;
                break;
            }
        }
        if(findStatus!=5){
            updateScore(oppId,50);
            updateScore(accId,-30);
            send(oppSoc, "85", BUFF_SIZE, 0);
        }else{
            send(oppSoc, "84", BUFF_SIZE, 0);
        }

    }
    WriteFile();
    return 0;
}

int SignOut(std::string user)
{
    ExitMatch(user);
    for (auto &account : accountsList)
    {
        if (account.username == user)
        {
            account.sign = 0;
            account.socket = 0;
            account.opponent = -1;
            account.findStatus = 0;
            WriteFile();
            break;
        }
    }
    return 0;
}

int SignUp(std::string user, std::string pass)
{
    for (auto &account : accountsList)
    {
        if (account.username == user)
        {
            // User da ton tai
            return 1;
        }
    }
    int id = accountsList.size();
    Account account = Account(user, pass, "null", "H", 0, id, 0, 0, 0, 0, 1, 0, 0, 0, 0);
    accountsList.push_back(account);
    WriteFile();
    // Dang ky thanh cong
    return 0;
}

int ChangePass(std::string user, std::string oldpass, std::string newpass)
{
    for (auto &account : accountsList)
    {
        if (account.username == user)
        {
            if (account.password != oldpass)
            {
                return 2;
            }

            account.password = newpass;
            std::cout << account.password;
            break;
        }
    }
    WriteFile();
    return 0;
}


std::string FindMatch(std::string user){
    int accId;
    int score;
    int oppId;
    int oppSoc;
    std::string oppU;
    int check = 0;
    char reS[BUFF_SIZE];
    for (auto &account : accountsList)
    {
        if (account.username == user)
        {
            accId = account.accountId;
            score = account.elo;
            break;
        }
    }

    for (auto &account : accountsList)
    {
        if (account.accountId != accId && account.findStatus == 1 && abs(account.elo - score)<= 200)
        {
            account.opponent = accId;
            account.findStatus = 2;
            oppSoc = account.socket;
            oppId = account.accountId;
            oppU = account.username;
            check = 1;
            sprintf(reS, "5%s", user.c_str());
            send(oppSoc, reS, BUFF_SIZE, 0);
            break;
        }
    }
    if(check == 1){
        for (auto &account : accountsList)
        {
            if (account.accountId == accId)
            {
                account.opponent = oppId;
                account.findStatus = 2; 
                break;
            }
        }
        WriteFile();
        return oppU;
    }else{
        for (auto &account : accountsList)
        {
            if (account.accountId == accId)
            {
                account.opponent = -1;
                account.findStatus = 1; 
                break;
            }
        }
        WriteFile();
        return "0";
    }
}







int CreateBoard(int p1ID, int p2ID, std::string p1, std::string p2, int socket1, int socket2)
{
    int newID;
    int BoardSize = boardList.size();
    for(int i = 0; i<= BoardSize; i++){
        int check = 0;
        for (auto &board : boardList)
        {
            if (board.id == i)
            {
                check = 1;
            }
        }
        if(check==0){
            newID = i;
            break;
        }

    }
    Board board = Board(newID,2, p1ID, p2ID, p1, p2, socket1, socket2);
    boardList.push_back(board);
    WriteBoardFile();
    return newID;
}





int UpdateBoard(std::string atkPos, int board_ID, int p_ID)
{
    int check = 0;
    int winCheck = 1;
    int oopSoc;
    char reS[BUFF_SIZE];
    int oppID;
    for (auto &board : boardList)
    {
        if (board.id == board_ID)
        {
            if (p_ID==board.p1ID){
                oppID = board.p2ID;
                oopSoc = board.socket2;
                    for (size_t i = 0; i < board.p2.length(); ++i) {
                        if(i%2==0 && board.p2[i] == atkPos[0] && board.p2[i+1] == atkPos[1]){
                            board.p2[i] = 'x';
                            board.p2[i+1] = 'x';
                            check = 1;
                            break;
                        }
                    }
                    if(check == 1){
                        for (size_t i = 0; i < board.p2.length(); ++i) {
                            if(board.p2[i] != 'x'){
                                winCheck = 0;
                                break;
                            }
                        }
                        break;
                    }
            }else if (p_ID == board.p2ID)
            {
                oppID = board.p1ID;
                oopSoc = board.socket1;
                    for (size_t i = 0; i < board.p1.length(); ++i) {
                        if(i%2==0 && board.p1[i] == atkPos[0] && board.p1[i+1] == atkPos[1]){
                            board.p1[i] = 'x';
                            board.p1[i+1] = 'x';
                            check = 1;
                            break;
                        }
                    }
                    if(check == 1){
                        for (size_t i = 0; i < board.p1.length(); ++i) {
                            if(board.p1[i] != 'x'){
                                winCheck = 0;
                                break;
                            }
                        }
                        break;
                    }
            }
            
        }
    }
    if(check == 1){
        WriteBoardFile();
    }
    if(check == 1 && winCheck == 1){
        updateScore(p_ID,50);
        updateScore(oppID,-30);
        send(oopSoc, "82", BUFF_SIZE, 0);
        return 3;
    }else if (check == 1 && winCheck == 0)
    {
        sprintf(reS, "8%s", atkPos.c_str());
        send(oopSoc, reS, BUFF_SIZE, 0);
        return 1;
    }else if (check == 0)
    {
        sprintf(reS, "8%s", atkPos.c_str());
        send(oopSoc, reS, BUFF_SIZE, 0);
        return 0;
    }
    return 0;
}





std::string Ready(std::string user, std::string position, std::string skinID){
    char resPos[BUFF_SIZE],oppSkin[BUFF_SIZE];
    int accId, opp_skinID;
    std::string oppPos;
    int oppId;
    int accSocket;
    int oopSoc;
    int new_board_ID = 0;
    int check = 0;
    for (auto &account : accountsList)
    {
        if (account.username == user){
            oppId = account.opponent;
            accId = account.accountId;
            account.findStatus = 3;
            accSocket = account.socket;
            account.position = position;
            account.skin = atoi(skinID.c_str());
            break;
        }
    }
    WriteFile();
    for (auto &account : accountsList)
    {
        if (account.accountId == oppId){
            if(account.findStatus == 3){
                oopSoc = account.socket;
                new_board_ID = CreateBoard(accId, account.accountId, position, account.position, accSocket, account.socket);
                account.boardId = new_board_ID;
                account.findStatus = 4;
                oppPos = account.position;
                opp_skinID = account.skin;
                snprintf(oppSkin,BUFF_SIZE,"%d",opp_skinID);
                sprintf(resPos, "7%s", skinID.c_str());
                send(oopSoc, resPos, BUFF_SIZE, 0);
                check = 1;
                break;
            };
        }
    }
    if(check==1){
        for (auto &account : accountsList)
        {
            if (account.accountId == accId){
                account.boardId = new_board_ID;
                account.findStatus = 4;
                break;
            }
        }
        WriteFile();
        return oppSkin;
    }
    return "0";
}




int attack(std::string user, std::string atkpos){
    std::string oppPos;
    int result;
    //std::string
    for (auto &account : accountsList)
    {
        if (account.username == user){
            result = UpdateBoard(atkpos,account.boardId,account.accountId);
            return result;
        }
    }
    return 0;
}



int CreatePublicBoard(std::string user){
    int newID,p1ID,socket1;
    for (auto &account : accountsList)
    {
        if (account.username == user){
            p1ID=account.accountId;
            socket1=account.socket;
            
        }
    }
    int BoardSize = boardList.size();
    for(int i = 0; i<= BoardSize; i++){
        int check = 0;
        for (auto &board : boardList)
        {
            if (board.id == i)
            {
                check = 1;
            }
        }
        if(check==0){
            newID = i;
            break;
        }

    }
    Board board = Board(newID,1, p1ID, -1, "x", "x", socket1, -1);
    boardList.push_back(board);
    WriteBoardFile();
    return newID;
}

void ViewBoardList(std::string user){
    char ListID[BUFF_SIZE]="",BoardID[BUFF_SIZE]="",ListRes[BUFF_SIZE]="b";
    int client_socket;
    for (auto it : boardList){
        if(it.type==1){
            snprintf(BoardID,BUFF_SIZE,"%d",it.id);
            strncat(ListID,"+",BUFF_SIZE-1);
            strncat(ListID,BoardID,BUFF_SIZE);
        }
    }
    for (auto &account : accountsList)
    {
        if (account.username == user){
            client_socket = account.socket;
            break;
        }
    }
    strncat(ListRes,ListID,BUFF_SIZE);
    send(client_socket, ListRes, BUFF_SIZE, 0);
}

void ViewOnlineUser(std::string user){
    char ListID[BUFF_SIZE]="",UserOnl[BUFF_SIZE]="",ListRes[BUFF_SIZE]="c";
    int client_socket;
    for (auto it : accountsList){
        if(it.sign==1&&it.username!=user){
            snprintf(UserOnl,BUFF_SIZE,"%s",it.username.c_str());
            strncat(ListID,"+",BUFF_SIZE-1);
            strncat(ListID,UserOnl,BUFF_SIZE);
        }
    }
    for (auto &account : accountsList)
    {
        if (account.username == user){
            client_socket = account.socket;
            break;
        }
    }
    strncat(ListRes,ListID,BUFF_SIZE);
    send(client_socket, ListRes, BUFF_SIZE, 0);
}


int Join(std::string user, std::string Board_ID){
    int BoardID = std::stoi(Board_ID);
    int client_socket,accID,check=0,oPPSocket,oppID;
    std::string oppName;
    char send_string[BUFF_SIZE];
    for (auto &account : accountsList)
    {
        if (account.username == user){
            accID = account.accountId;
            client_socket = account.socket;
            break;
        }
    }
    for (auto &it : boardList){
        if(it.id==BoardID){
            if(it.type==3){
                send(client_socket, "d+0", BUFF_SIZE, 0);
                return 0;
            }
            if(it.type==1){
                oPPSocket = it.socket1;
                oppID = it.p1ID;
                it.type=3;
                it.p2ID=accID;
                it.socket2=client_socket;
                check = 1;
                break;
            }
        }
    }
    for (auto &account : accountsList)
    {
        if (account.accountId == oppID){
            oppName = account.username;
            break;
        }
    }
    if(check==1){
        WriteBoardFile();
        snprintf(send_string,BUFF_SIZE,"d%s",user.c_str());
        send(oPPSocket, send_string, BUFF_SIZE, 0);
        snprintf(send_string,BUFF_SIZE,"d%s",oppName.c_str());
        send(client_socket, send_string, BUFF_SIZE, 0);
    }else{
        send(client_socket, "d+0", BUFF_SIZE, 0);
    }
    return 0;
}


void Start(std::string user, std::string Board_ID){
    int BoardID = std::stoi(Board_ID);
    std::string p1Name,p2Name;
    char send_string[BUFF_SIZE];
    int p1ID,p2ID,p1Soc,p2Soc;
    for (auto &it : boardList){
        if(it.id==BoardID){
            
            p1ID = it.p1ID;
            p2ID = it.p2ID;
            p1Soc = it.socket1;
            p2Soc = it.socket2;
        }
    }
    for (auto &account : accountsList)
    {
        if (account.accountId == p1ID){
            p1Name = account.username;
            account.opponent = p2ID;
            account.findStatus = 2;
        }
        if (account.accountId == p2ID){
            p2Name = account.username;
            account.opponent = p1ID;
            account.findStatus = 2;
        }
    }
    snprintf(send_string,BUFF_SIZE,"5%s",p1Name.c_str());
    send(p2Soc, send_string, BUFF_SIZE, 0);
    //send_string = " "; 
    snprintf(send_string,BUFF_SIZE,"5%s",p2Name.c_str());
    send(p1Soc, send_string, BUFF_SIZE, 0);
}



void Invite(std::string p1Name, std::string p2Name, std::string Board_ID){
    int BoardID = std::stoi(Board_ID);
    char send_string[BUFF_SIZE];
    snprintf(send_string,BUFF_SIZE,"i+%s+%d",p1Name.c_str(),BoardID);
    for (auto &account : accountsList)
    {
        if (account.username == p2Name){
            if(account.findStatus == 0){
                send(account.socket, send_string, BUFF_SIZE, 0);
                break;
            }
        }
    }
    WriteBoardFile();
}

void Accept(std::string username, std::string Board_ID){
    int BoardID = std::stoi(Board_ID);
    int id,soc,check = 0;
    char send_string[BUFF_SIZE];
    snprintf(send_string,BUFF_SIZE,"j+%s",username.c_str());
    for (auto &account : accountsList)
    {
        if (account.username == username){
            id = account.accountId;
            soc = account.socket;
        }
    }
    for (auto &it : boardList){
        if(it.id==BoardID){
            if(it.type == 1){
                it.type = 3;
                it.p2ID = id;
                it.socket2 = soc;
                send(it.socket1, send_string, BUFF_SIZE, 0);
                send(soc, "j+1", BUFF_SIZE, 0);
                check = 1;
                break;
            }
        }
    }
    if(check == 0){
        send(soc, "j+0", BUFF_SIZE, 0);
    }
    WriteBoardFile();
}



void Kick(std::string p1Name, std::string p2Name, std::string Board_ID){
    int BoardID = std::stoi(Board_ID);
    int p1Soc,p2Soc;
    for (auto &it : boardList){
        if(it.id==BoardID){
            p1Soc = it.socket1;
            p2Soc = it.socket2;
            it.p2ID = -1;
            it.socket2 = -1;
            it.type = 1;
        }
    }
    send(p2Soc, "f0", BUFF_SIZE, 0);
    send(p1Soc, "f1", BUFF_SIZE, 0);
    WriteBoardFile();
}

void Leave(std::string userName, std::string Board_ID){
    int BoardID = std::stoi(Board_ID);
    int id;
    for (auto &account : accountsList)
    {
        if (account.username == userName){
            id = account.accountId;
            account.opponent = -1;
            account.findStatus = 0;
        }
    }
    for (auto &it : boardList){
        if(it.id==BoardID){
            it.type = 1;
            if(id == it.p2ID){
                it.p2ID = -1;
                it.socket2 = -1;
                send(it.socket1, "f1", BUFF_SIZE, 0);
            }
            if(id == it.p1ID){
                it.p1ID = it.p2ID;
                it.p2ID = -1;
                it.socket1 = it.socket2;
                it.socket2 = -1;
                if(it.socket1!=-1){
                    send(it.socket1, "f2", BUFF_SIZE, 0);
                }
            }
        }
    }
}


void Replay(std::string username){
    int soc,win,lose,score;
    char send_string[BUFF_SIZE]="0+",replay[BUFF_SIZE]="",text[BUFF_SIZE]="";
    for (auto &account : accountsList)
    {
        if (account.username == username){
            soc = account.socket;
            strncat(replay,account.history.c_str(),BUFF_SIZE-1);
            score = account.elo;
        }
    }
    for (size_t i = 0;i<strlen(replay);i++){
        if(replay[i]=='w'){
            win++;
        }else if (replay[i]=='l')
        {
            lose++;
        }
    }
    snprintf(text,BUFF_SIZE,"%d",win);
    strncat(send_string,text,BUFF_SIZE-1);
    strncat(send_string,"+",BUFF_SIZE-1);
    snprintf(text,BUFF_SIZE,"%d",lose);
    strncat(send_string,text,BUFF_SIZE-1);
    strncat(send_string,"+",BUFF_SIZE-1);
    snprintf(text,BUFF_SIZE,"%d",score);
    strncat(send_string,text,BUFF_SIZE-1);
    send(soc, send_string, BUFF_SIZE, 0);
}

void ViewRank(std::string username){
    int soc,count,max=99999,old,same=0;
    char send_string[BUFF_SIZE]="r", rank[3], score[10];
    for (auto &account : accountsList)
    {
        if (account.username == username){
            soc = account.socket;
        }
    }
    if(accountsList.size()<5){
        count = accountsList.size();
    }else{
        count = 5;
    }
    for (int i = 0;i<count;i++){
        old = max;
        max = 0;
        for (auto &account : accountsList){
            if (account.elo>max&&account.elo<old){
                max = account.elo;
            }
        }
        same = 0;
        for (auto &account : accountsList){
            if (account.elo==max){
                strncat(send_string,"+",BUFF_SIZE-1);
                snprintf(rank,BUFF_SIZE,"%d",i+1);
                strncat(send_string,rank,BUFF_SIZE-1);
                strncat(send_string,"+",BUFF_SIZE-1);
                strncat(send_string,account.username.c_str(),BUFF_SIZE-1);
                strncat(send_string,"+",BUFF_SIZE-1);
                snprintf(score,BUFF_SIZE,"%d",account.elo);
                strncat(send_string,score,BUFF_SIZE-1);
                same ++;
            }
        }
        i+=same-1;
    }
    send(soc, send_string, BUFF_SIZE, 0);
}

void Chat(std::string p1Name, std::string p2Name,std::string mess){
    int check = 0;
    char send_string[BUFF_SIZE]="h+";
    for (auto &account : accountsList)
    {
        if (account.username == p2Name&&account.sign ==1){
            check = 1;
            strncat(send_string,p1Name.c_str(),BUFF_SIZE-1);
            strncat(send_string,"+",BUFF_SIZE-1);
            strncat(send_string,mess.c_str(),BUFF_SIZE-1);
            send(account.socket, send_string, BUFF_SIZE, 0);
        }
    }
    if(check == 0){
        for (auto &account : accountsList)
        {
            if (account.username == p1Name){
                send(account.socket, "h+0", BUFF_SIZE, 0);
            }
        }
    }
}


void *handle_client(void *socket_desc)
{
    int client_socket = *(int *)socket_desc;
    char buffer[BUFF_SIZE];
    char resultString[BUFF_SIZE];
    int opt, result, sign = 0;
    int read_size;
    std::string resultS;
    std::string user;
    while ((read_size = recv(client_socket, buffer, BUFF_SIZE, 0)) > 0)
    {
        write(1, buffer, read_size);
        buffer[read_size - 1] = '\0';
        if (buffer[0] < 48 || buffer[0] > 57)
        {
            send(client_socket, "Type error!", BUFF_SIZE, 0);
        }
        else
        {
            // tách chuỗi và chứa trong vector tokens
            std::vector<std::string> tokens;
            std::istringstream iss(buffer);
            std::string token;
            while (std::getline(iss, token, '+'))
            {
                tokens.push_back(token);
            }
            //ReadFile();
            opt = std::stoi(tokens.at(0));
            switch (opt)
            {
            case TypeMassage::LOGIN:
                /* code */
                result = SignIn(tokens.at(1), tokens.at(2),client_socket);
                if (result == 1)
                {
                    user = tokens[1];
                    sign = 1;
                }
                sprintf(resultString, "1%d", result);
                send(client_socket, resultString, BUFF_SIZE, 0);
                break;
            case TypeMassage::LOGOUT:
                /* code */
                SignOut(tokens.at(1));
                send(client_socket, "20", BUFF_SIZE, 0);
                break;
            case TypeMassage::SIGNUP:
                /* code */
                if (tokens.at(2) != tokens.at(3))
                { // User da ton tai
                    send(client_socket, "32", BUFF_SIZE, 0);
                }
                else
                {
                    result = SignUp(tokens.at(1), tokens.at(2));
                    sprintf(resultString, "3%d", result);
                    send(client_socket, resultString, BUFF_SIZE, 0);
                }
                break;
            case TypeMassage::CHANGE_PASS:
                /* code */
                if (tokens.at(2) == tokens.at(3))
                { // pass moi trung pass cu
                    send(client_socket, "41", BUFF_SIZE, 0);
                }
                else
                {        
                    result = ChangePass(tokens.at(1), tokens.at(2), tokens.at(3));
                    sprintf(resultString, "4%d", result);
                    send(client_socket, resultString, BUFF_SIZE, 0);
                }
                break;
            case TypeMassage::FIND_MATCH:
                /* code */
                resultS = FindMatch(tokens.at(1));
                sprintf(resultString, "5%s", resultS.c_str());
                send(client_socket, resultString, BUFF_SIZE, 0);
                break;
            case TypeMassage::EXIT_FIND_MATCH:
                result = ExitMatch(tokens.at(1));
                sprintf(resultString, "6%d", result);
                send(client_socket, resultString, BUFF_SIZE, 0);
                break;
            case TypeMassage::READY:
                resultS = Ready(tokens.at(1), tokens.at(2), tokens.at(3));
                sprintf(resultString, "7%s", resultS.c_str());
                send(client_socket, resultString, BUFF_SIZE, 0);
                break;
            case TypeMassage::ATTACK:
                result = attack(tokens.at(1), tokens.at(2));
                sprintf(resultString, "8%d", result);
                send(client_socket, resultString, BUFF_SIZE, 0);
                break;
            case TypeMassage::GIVEUP:
                /* code */
                break;
            case TypeMassage::REPLAY:
                Replay(tokens.at(1));
                break;
            case TypeMassage::CREATEBOARD:
                result = CreatePublicBoard(tokens.at(1));
                sprintf(resultString, "a%d", result);
                send(client_socket, resultString, BUFF_SIZE, 0);
                break;
            case TypeMassage::VIEWBOARDLIST:
                ViewBoardList(tokens.at(1));
                break;
            case TypeMassage::VIEWONLINELIST:
                ViewOnlineUser(tokens.at(1));
                break;
            case TypeMassage::JOIN:
                Join(tokens.at(1), tokens.at(2));
                break;
            case TypeMassage::INVITE:
                Invite(tokens.at(1), tokens.at(2), tokens.at(3));
                break;
            case TypeMassage::START:
                Start(tokens.at(1), tokens.at(2));
                break;
            case TypeMassage::KICK:
                Kick(tokens.at(1), tokens.at(2), tokens.at(3));
                break;
            case TypeMassage::ACCEPT:
                Accept(tokens.at(1), tokens.at(2));
                break;
            case TypeMassage::VIEWTOP:
                ViewRank(tokens.at(1));
                break;
            case TypeMassage::CHAT:
                Chat(tokens.at(1), tokens.at(2), tokens.at(3));
                break;
            case TypeMassage::LEAVE:
                Leave(tokens.at(1), tokens.at(2));
                break;
            default:
                send(client_socket, "NonOpt", BUFF_SIZE, 0);
                break;
            }
        }
    }
    if (read_size == 0)
    {
        printf("Client disconnected\n");
        if (sign == 1)
        {
            SignOut(user.c_str());
            sign = 0;
        }
    }
    else if (read_size == -1)
    {
        perror("recv failed");
    }

    close(client_socket);
    // free(client_socket);
    return NULL;
}

// Set defaul sigin of account is 0
void SetDefaulSignIn()
{
    for (auto &account : accountsList)
    {
        account.sign = 0;
    }
    WriteFile();
}

int main()
{
    ReadFile();
    ReadBoardFile();
    int server_fd, new_socket, c;
    struct sockaddr_in server, client;

    // Tạo socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Cấu hình địa chỉ và cổng của server
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

    // Gán địa chỉ và cổng cho socket
    if (bind(server_fd, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Lắng nghe kết nối đến server
    if (listen(server_fd, MAX_CLIENTS) < 0)
    {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        client_sockets[i] = -1;
    }
    int client_index = 0;
    puts("Waiting for incoming connections...");


    c = sizeof(struct sockaddr_in);

    while ((new_socket = accept(server_fd, (struct sockaddr *)&client, (socklen_t *)&c)))
    {
        puts("Connection accepted");
        while (client_sockets[client_index] != -1) {
            client_index = (client_index + 1) % MAX_CLIENTS;
        }
        client_sockets[client_index] = new_socket;
        pthread_t sniffer_thread;
        int new_sock[BUFF_SIZE];
        *new_sock = new_socket;

        // Tạo thread để xử lý client mới
        if (pthread_create(&sniffer_thread, NULL, handle_client, (void *)new_sock) < 0)
        {
            perror("could not create thread!");
            SetDefaulSignIn();
            return 1;
        }

        // Gắn kết thúc của thread
        pthread_detach(sniffer_thread);
        puts("Handler assigned");
    }

    SetDefaulSignIn();

    if (new_socket < 0)
    {
        perror("accept failed");
        return 1;
    }
    return 0;
}
