#include "config.h"

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
        file >> acc.password >> acc.position >> acc.accountId >> acc.opponent >> acc.status >> acc.sign >> acc.incorrect >> acc.findStatus >> acc.boardId;
        accountsList.push_back(acc);
    }
    file.close();
}

// Rewrite to file
void WriteFile()
{
    FILE *file = fopen("account.txt", "w");
    for (auto it : accountsList)
    {
        fprintf(file, "%s %s %s %d %d %d %d %d %d %d\n", it.username.c_str(), it.password.c_str(), it.position.c_str(),it.accountId, it.opponent, it.status, it.sign, it.incorrect, it.findStatus, it.boardId);
    }
    fclose(file);
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
        file >> board.p1ID >> board.p2ID >> board.p1 >> board.p2;
        boardList.push_back(board);
    }
    file.close();
}

void WriteBoardFile()
{
    FILE *file = fopen("board.txt", "w");
    for (auto it : boardList)
    {
        fprintf(file, "%d %d %d %s %s\n", it.id, it.p1ID, it.p2ID, it.p1.c_str(), it.p2.c_str());
    }
    fclose(file);
}


int SignIn(std::string user, std::string pass)
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
                    account.opponent = 0;
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

int SignOut(std::string user)
{
    for (auto &account : accountsList)
    {
        if (account.username == user)
        {
            account.sign = 0;
            account.opponent = 0;
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
    Account account = Account(user, pass, NULL, id, 0, 1, 0, 0, 0, 0);
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


int FindMatch(std::string acc_Id){
    int accId = std::stoi(acc_Id);
    int oppId;
    int check = 0;
    for (auto &account : accountsList)
    {
        if (account.accountId != accId && account.findStatus == 1)
        {
            account.opponent = accId;
            account.findStatus = 2;
            oppId = account.accountId;
            check = 1;
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
        return oppId;
    }else{
        for (auto &account : accountsList)
        {
            if (account.accountId == accId)
            {
                account.opponent = 0;
                account.findStatus = 1; 
                break;
            }
        }
        WriteFile();
        return 0;
    }
}




int ExitMatch(std::string acc_Id){
    int accId = std::stoi(acc_Id);
    int oppId;
    for (auto &account : accountsList)
    {
        if (account.accountId == accId){
            oppId = account.opponent;
            account.opponent = 0;
            account.findStatus = 0;
            break;
        }
    }
    if(oppId!=0){
        for (auto &account : accountsList)
        {
            if (account.accountId == oppId)
            {
                account.opponent = 0;
                account.findStatus = 0;
                break;
            }
        }
    }
    WriteFile();
    return 0;
}


int CreateBoard(int p1ID, int p2ID, std::string p1, std::string p2)
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
    Board board = Board(newID, p1ID, p2ID, p1, p2);
    boardList.push_back(board);
    WriteBoardFile();
    // Dang ky thanh cong
    return newID;
}


std::string Ready(std::string acc_Id, std::string position){
    int accId = std::stoi(acc_Id);
    std::string oppPos;
    int oppId;

    int new_board_ID = 0;
    int check = 0;
    for (auto &account : accountsList)
    {
        if (account.accountId == accId){
            oppId = account.opponent;
            account.findStatus = 3;
            account.position = position;
            break;
        }
    }
    WriteFile();
    for (auto &account : accountsList)
    {
        if (account.accountId == oppId){
            if(account.findStatus == 3){
                new_board_ID = CreateBoard(accId, account.accountId, position, account.position);
                account.boardId = new_board_ID;
                account.findStatus = 4;
                oppPos = account.position;
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
        return oppPos;
    }
    return "0";
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
            
            opt = std::stoi(tokens.at(0));
            switch (opt)
            {
            case TypeMassage::LOGIN:
                /* code */
                result = SignIn(tokens.at(1), tokens.at(2));
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
                result = FindMatch(tokens.at(1));
                sprintf(resultString, "5%d", result);
                send(client_socket, resultString, BUFF_SIZE, 0);
                break;
            case TypeMassage::EXIT_FIND_MATCH:
                result = ExitMatch(tokens.at(1));
                sprintf(resultString, "6%d", result);
                send(client_socket, resultString, BUFF_SIZE, 0);
                break;
            case TypeMassage::READY:
                resultS = Ready(tokens.at(1), tokens.at(2));
                sprintf(resultString, "7%s", resultS.c_str());
                send(client_socket, resultString, BUFF_SIZE, 0);
                break;
            case TypeMassage::ATTACK:
                /* code */
                break;
            case TypeMassage::GIVEUP:
                /* code */
                break;
            case TypeMassage::REPLAY:
                /* code */
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