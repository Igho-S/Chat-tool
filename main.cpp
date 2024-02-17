#include <iostream> // libraries for I/O 
#include <cstring> // library for string functions
#include <sys/socket.h> // socket functions
#include <arpa/inet.h> // for working with internet addresses
#include <unistd.h> // standard constants and types
#include <unordered_map> // for working with an unordered map data structure

#define MAX_MSG_LEN 1000 // maximum length of a message
#define PORT 1234 //default port number

struct ThreadArgs { // structure for passing arguments to threads
    int sockfd;
    sockaddr_in cliaddr; 
};

struct UserInfo { // for storing user information
    std::string nickname;
};

std::unordered_map<std::string, UserInfo> userMap; // unordered map to store user information using socket identifier

void addUser(const std::string& identifier, const std::string& nickname) { // to add a user
    UserInfo userInfo;
    userInfo.nickname = nickname;
    userMap[identifier] = userInfo;
}

std::string getNickname(const std::string& identifier) { // to get and map a nickname
    auto it = userMap.find(identifier);
    if (it != userMap.end()) {
        return it->second.nickname;
    }
    return "";
}

void *receiveMessages(void *args) { //function for processing received messages
    struct ThreadArgs *threadArgs = (struct ThreadArgs *)args;
    char buffer[MAX_MSG_LEN];
    socklen_t len = sizeof(threadArgs->cliaddr);
    
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int n = recvfrom(threadArgs->sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&(threadArgs->cliaddr), &len);
        if (n < 0) { //if there are errors in receiving the message, throwback an error
            std::cerr << "Error in recv_from" << std::endl;
            continue;
        }
        buffer[n] = '\0'; // null-terminate received message
        
        std::string senderIdentifier = std::string(inet_ntoa(threadArgs->cliaddr.sin_addr)) + ":" + std::to_string(ntohs(threadArgs->cliaddr.sin_port)); //sender identifier
        std::string senderNickname = getNickname(senderIdentifier); //sender's nickname
        std::string messageContent(buffer); // buffer to string
        std::cout << "(" << inet_ntoa(threadArgs->cliaddr.sin_addr) << ") " << senderNickname << ": " << messageContent << std::endl; //display message
    }
    return NULL; //exit
}

void *sendMessage(void *args) { //function for processing sent messages
    struct ThreadArgs *threadArgs = (struct ThreadArgs *)args; // to cast the srgument to correct structure type
    char buffer[MAX_MSG_LEN]; // buffer to store message
    struct sockaddr_in servaddr; // server address structure
    
    memset(&servaddr, 0, sizeof(servaddr)); //clear server address
    servaddr.sin_family = AF_INET; // for IPv4
    servaddr.sin_port = htons(PORT); // set port
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // set ip
    
    while (true) {
        std::string message; 
        std::getline(std::cin, message);
        std::string fullMessage = getNickname(std::to_string(threadArgs->sockfd)) + ": " + message; // message with senders nickname
        strcpy(buffer, fullMessage.c_str()); // copy to buffer
        sendto(threadArgs->sockfd, buffer, strlen(buffer), 0, (struct sockaddr *)&servaddr, sizeof(servaddr)); // send message
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 3) { //handling error in input
        std::cerr << "Usage: " << argv[0] << " <IPv4 address> <port>" << std::endl;
        return 1;
    }
    
    std::string nickname;
    std::cout << "Enter your nickname: ";
    std::cin >> nickname;
    
    int sockfd; //socket file descriptor
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        std::cerr << "Error in socket creation" << std::endl; // throw error if socket fails to be created
        return 1;
    }
    
    struct sockaddr_in servaddr; //server address
    memset(&servaddr, 0, sizeof(servaddr)); //clear address
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(atoi(argv[2]));
    servaddr.sin_addr.s_addr = inet_addr(argv[1]);
    
    if (bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) { //bind socket
        std::cerr << "Error in binding" << std::endl; //throw error if socket couldn't be bound
        return 1;
    }
    
    pthread_t recvThread, sendThread; //thread variables
    struct ThreadArgs args; //thread argument structure
    args.sockfd = sockfd; //to set socket file descriptor in thread
    
    addUser(std::to_string(sockfd), nickname); //add user to user map
    
    pthread_create(&sendThread, NULL, sendMessage, (void *)&args); // thread for sending messages
    pthread_create(&recvThread, NULL, receiveMessages, (void *)&args); // thread for receiving messages
    
    pthread_join(recvThread, NULL); //wait for receive thread to finish
    pthread_join(sendThread, NULL);// wait for send thread to finish
    
    close(sockfd); // close socket
    
    return 0;
}


