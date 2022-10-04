

/**
 * @ubitname_assignment1
 * @author  Fullname <ubitname@buffalo.edu>
 * @version 1.0
 *
 * @section LICENSE
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details at
 * http://www.gnu.org/copyleft/gpl.html
 *
 * @section DESCRIPTION
 *
 * This contains the main function. Add further description here....
 */
#include <iostream>

#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <string>
#include <sys/time.h>
#include <stdio.h>
#include <errno.h>
#include <vector>
#include "../include/global.h"
#include "../include/logger.h"
#include <cstdlib>
#include <algorithm>
#include <stdint.h>
#define STDIN 0


using namespace std;

/**
 * main function
 *
 * @param  argc Number of arguments
 * @param  argv The argument list
 * @return 0 EXIT_SUCCESS
 */

struct client_info{
    int port;
    string ip_addr;
    string hostname;

};

string convertInt(int number)
{
    if (number == 0)
        return "0";
    string temp="";
    string returnvalue="";
    while (number>0)
    {
        temp+=number%10+48;
        number/=10;
    }
    for (int i=0;i<temp.length();i++)
        returnvalue+=temp[temp.length()-i-1];
    return returnvalue;
}
bool input_begin_LOGIN(string str1, string str2,int str_len){
    int i=0;
    while(i < str_len){
        if(str1[i] != str2[i]) return false;
        i++;
    }
    return true;
}

bool contains(char* str1, string str2,int str_len){
    int i=0;
    while(i < str_len){
        if(*str1 == '\0') return false;
        if(*str1 != str2[i]) return false;
        str1++;
        i++;
    }
    return true;
}

bool Compare(const client_info a, const client_info b){
    return a.port < b.port;
}
void print_list(vector<client_info>& c) {
    for (int i = 0; i < c.size(); ++i) {
        cse4589_print_and_log("%-5d\t%-35s\t%-20s\t%-8d\n",i+1,c[i].hostname.c_str(),c[i].ip_addr.c_str(),c[i].port);
    }
}



int main(int argc, char **argv)
{

	/*Init. Logger*/
	cse4589_init_log(argv[2]);

	/* Clear LOGFILE*/
    fclose(fopen(LOGFILE, "w"));

    /*Start Here*/

    bool is_login = false;
    string SERVER_IP_ADDR = " ";
    int SERVER_PORT_ID = 0;

    int PORT = atoi(argv[2]);

    if(PORT < 1){
        printf("Invalid Port number\n");
        return -1;
    }

    char* role = argv[1];
    vector<client_info> all_client;


    char* _IP = (char*) malloc(256);
    memcpy(_IP,"IP",256);
    char* _PORT = (char*) malloc(8);
    memcpy(_PORT,"PORT",256);
    char* _LIST = (char*) malloc(8);
    memcpy(_LIST,"LIST",256);
    char* _REFRESH = (char*) malloc(8);
    memcpy(_REFRESH,"REFRESH",256);
    char* _EXIT = (char*) malloc(8);
    memcpy(_EXIT,"EXIT",256);
    char* _LOGIN = (char*) malloc(8);
    memcpy(_LOGIN,"LOGIN",256);
    char* _AUTHOR = (char*) malloc(8);
    memcpy(_AUTHOR,"AUTHOR",256);


    char myIP[16];
    struct sockaddr_in google_addr, my_addr;
    int google_socket;

    // Connect to server
    if ((google_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Can't open stream socket.");
        exit(-1);
    }

    // Set server_addr
    bzero(&google_addr, sizeof(google_addr));
    google_addr.sin_family = AF_INET;
    google_addr.sin_addr.s_addr = inet_addr("8.8.8.8");
    google_addr.sin_port = htons(PORT);

    // Connect to google server
    if (connect(google_socket, (struct sockaddr *) &google_addr, sizeof(google_addr)) < 0) {
        perror("Connect server error");
        close(google_socket);
        exit(-1);
    }

    // Get my ip address and port
    bzero(&my_addr, sizeof(my_addr));
    socklen_t len = sizeof(my_addr);
    getsockname(google_socket, (struct sockaddr *) &my_addr, &len);
    inet_ntop(AF_INET, &my_addr.sin_addr, myIP, sizeof(myIP));
    my_addr.sin_port = htons(PORT);

//    printf("Local ip address: %s\n", myIP);
//    printf("Local port : %u\n", ntohs(my_addr.sin_port));

    if (role[0] == 's'){

        int clientSize = 5;
        int opt = 1;
        int clientSocketList[5];
        for(int i=0;i<clientSize;i++){
            clientSocketList[i] = 0;
        }

        if(setsockopt(google_socket, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof opt) <0){
            cout<<"Socket setting failed!"<<endl;
            return -1;
        }

        int serverSocket;
        serverSocket = socket(AF_INET,SOCK_STREAM,0);
        if(serverSocket<0) cout<<"server socket failed!"<<endl;

        if(bind(serverSocket,(sockaddr*)&my_addr,sizeof(my_addr)) == -1){
            cout<<"Socket binding failed!"<<endl;
            perror("bind failed. Error");
            return -1;
        }

        if(listen(serverSocket,5) != 0){
            cout<<"server is not ready"<<endl;
        }

        int addrLen = sizeof(my_addr);

        struct timeval tv = {
        };
        tv.tv_sec = 2;
        tv.tv_usec = 50000;

        for (int i=0;i < clientSize;i++) {
            clientSocketList[i] = 0;
        }
        //


        int max_sd = serverSocket;
        printf("\n[PA1-Server@CSE489/589]$ ");
        fflush(stdout);

        fd_set master_fd;
        fd_set read_fd_set;
        FD_ZERO(&master_fd);
        FD_ZERO(&read_fd_set);
        FD_SET(STDIN,&master_fd);
        FD_SET(serverSocket,&master_fd);

        while(true) {



            read_fd_set = master_fd;
            if (::select(max_sd + 1, &read_fd_set, NULL, NULL, NULL) < 0) {
                perror("error");
                exit('4');
            }

            for (int i = 0; i <= max_sd; i++) {

                if (FD_ISSET(i, &read_fd_set)) {
                    if (i == serverSocket) {
                        int new_fd = accept(serverSocket, (struct sockaddr *) &my_addr, (socklen_t *) &addrLen);
                        if (new_fd == -1) {
                            perror("accept");
                        } else {
                            FD_SET(new_fd, &master_fd);
                            if (new_fd > max_sd) {
                                max_sd = new_fd;
                            }
                            cout << "new connection is joining in" << endl;
                        }
                    } else if(i == STDIN) {

                        string input;
                        std::getline(std::cin,input);
                        if(input == _PORT){
                            cse4589_print_and_log("[PORT:SUCCESS]\n");
                            cse4589_print_and_log("PORT:%d\n", ntohs(my_addr.sin_port));
                            cse4589_print_and_log("[PORT:END]\n");

                        }else if(input == _IP){
                            cse4589_print_and_log("[IP:SUCCESS]\n");
                            cse4589_print_and_log("IP:%s\n", inet_ntoa(my_addr.sin_addr));
                            cse4589_print_and_log("[IP:END]\n");
                        }else if(input == _LIST){
                            cse4589_print_and_log("[LIST:SUCCESS]\n");
                            print_list(all_client);
                            cse4589_print_and_log("[LIST:END]\n");

                        }else if(input == _AUTHOR){
                            cse4589_print_and_log("[AUTHOR:SUCCESS]\n");
                            cse4589_print_and_log("I, %s, have read and understood the course academic integrity policy.\n","tingjiez");
                            cse4589_print_and_log("[AUTHOR:END]\n");
                        }else if(input == _REFRESH){
                            // server does not refresh
                        }else{
                            cse4589_print_and_log("Invalid request.\n");
                        }


                    } else {
                        char *receive_buf = (char *) malloc(1024);
                        memset(receive_buf, 0, 1024);
                        int recv_byte = recv(i, receive_buf, 1024, 0);
                        if (recv_byte == 0) {
                            printf("selectserver: socket %d hung up\n", i);
                            close(i);
                            FD_CLR(i, &master_fd);
                        } else if (recv_byte < 0) {
                            perror("recv");
                            close(i);
                            FD_CLR(i, &master_fd);
                        } else {
                            char flag = receive_buf[0];
                            struct client_info new_client;
                            // handle new client connection
                            if (flag == '0') {
                                // collect information for new client
                                int k, m;
                                string buffer_port = "";
                                char *buffer_ip = (char *) malloc(64);
                                char *buffer_host = (char *) malloc(64);
                                memset(buffer_host,0,64);
                                memset(buffer_ip,0,64);
                                for (k = 2, m = 0; receive_buf[k] != ' '; ++k, ++m) {
                                    buffer_ip[m] = receive_buf[k];
                                }
                                for (k = k + 1, m = 0; receive_buf[k] != ' '; ++k, ++m) {
                                    buffer_port += receive_buf[k];
                                }
                                for (k = k + 1, m = 0; k < recv_byte; ++k, ++m) {
                                    buffer_host[m] = receive_buf[k];
                                }
                                new_client.port = atoi(buffer_port.c_str());
                                new_client.ip_addr = buffer_ip;
                                new_client.hostname = buffer_host;
                                // find the right position in sorting list, then insert into it
                                all_client.push_back(new_client);
                                sort(all_client.begin(), all_client.end(), Compare);

                                // SEND BUFFER to client
                                string buffer = "0 ";
                                for (int i = 0; i < all_client.size(); ++i) {

                                    string b = all_client[i].hostname + " " + all_client[i].ip_addr + " " +
                                               convertInt(all_client[i].port) + " ";
                                    buffer += b;
                                }
                                buffer[buffer.size()-1] = '\0';
                                char *send_buf = (char *) malloc(1024);
                                memset(send_buf, 0, 1024);
                                int j;
                                for (j = 0; j < buffer.size(); ++j) {
                                    send_buf[j] = buffer[j];
                                }
                                int sendRes = send(i, (void *) send_buf, j, 0);

                                if(sendRes == -1) cout<<"send client failed."<<endl;

                                cout << "send client_list to client123: " << send_buf << endl;
//                                free(send_buf);
//                                free(buffer_ip);
//                                free(buffer_host);

                            } else if (flag == '1') {
                                cout << "server recv refresh from server!" << endl;

                            }

                            //inform user of socket number - used in send and receive commands
                        }
                    }
                }
            }


            printf("\n[PA1-Server@CSE489/589]$ ");
            fflush(stdout);



        }
    }
    else if (role[0] == 'c') {

        int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (clientSocket == -1) {
            cout << "client socket set up failed" << endl;
            return -1;
        }

        printf("\n[PA1-Client@CSE489/589]$ ");
        fflush(stdout);

        int max_sd = clientSocket;


        fd_set master_fd;
        fd_set read_fd_set;
        FD_ZERO(&master_fd);
        FD_ZERO(&read_fd_set);
        FD_SET(STDIN,&master_fd);
        FD_SET(clientSocket,&master_fd);

        while(true) {

            read_fd_set = master_fd;
            if (::select(max_sd + 1, &read_fd_set, NULL, NULL, NULL) < 0) {
                perror("error");
                exit('4');
            }

            for (int i = 0; i <= max_sd; i++) {

                if (FD_ISSET(i, &read_fd_set)) {
                    if (i == clientSocket) {
                        char *server_packet = (char *) malloc(1024);
                        memset(server_packet,'\0',1024);
                        int recv_byte = recv(i, server_packet, 1024, 0);
                        if (recv_byte == 0) {
                            printf("selectserver: socket %d hung up\n", i);
                            close(i);
                            FD_CLR(i, &master_fd);
                        } else if (recv_byte < 0) {
                            perror("recv");
                            close(i);
                            FD_CLR(i, &master_fd);
                        } else {

                            if(server_packet[0] == '0'){

                                server_packet += 2;
                                vector<client_info> client_from_server;
                                cout<<"recv: "<<server_packet<<endl;
                                while (*server_packet){

                                    client_info c;
                                    string str = "";
                                    while (*server_packet !=' '){
                                        str += *server_packet;
                                        server_packet++;
                                    }

                                    c.hostname = str;
                                    str = "";
                                    server_packet++;

                                    while (*server_packet !=' '){
                                        str += *server_packet;
                                        server_packet++;
                                    }

                                    c.ip_addr = str;
                                    str = "";
                                    server_packet++;
                                    while (*server_packet and *server_packet !=' '){
                                        str += *server_packet;
                                        server_packet++;
                                        if(*server_packet == '\0') break;
                                    }
                                    if(*server_packet == ' ') server_packet++;
                                    c.port = atoi(&str[0]);
                                    client_from_server.push_back(c);
                                }
                                all_client = client_from_server;
                            }
                            else if (server_packet[0] == '1') {
                                cout << "server recv refresh from server!" << endl;
                            }

                            //inform user of socket number - used in send and receive commands
                        }
                    }
                    else if(i == STDIN) {

                        string input;
                        std::getline(std::cin,input);

                        if(input == _PORT){
                            cse4589_print_and_log("[PORT:SUCCESS]\n");
                            cse4589_print_and_log("PORT:%d\n", ntohs(my_addr.sin_port));
                            cse4589_print_and_log("[PORT:END]\n");

                        }else if(input == _IP){
                            cse4589_print_and_log("[IP:SUCCESS]\n");
                            cse4589_print_and_log("IP:%s\n", inet_ntoa(my_addr.sin_addr));
                            cse4589_print_and_log("[IP:END]\n");
                        }else if(input == _LIST){
                            cse4589_print_and_log("[LIST:SUCCESS]\n");
                            print_list(all_client);
                            cse4589_print_and_log("[LIST:END]\n");

                        }else if(input == _AUTHOR){
                            cse4589_print_and_log("[AUTHOR:SUCCESS]\n");
                            cse4589_print_and_log("I, %s, have read and understood the course academic integrity policy.\n","tingjiez");
                            cse4589_print_and_log("[AUTHOR:END]\n");
                        }else if(input == _REFRESH){

                        }else if(input.size()>10 && input_begin_LOGIN(input,_LOGIN,5)){

                            // login to server
                            string input_ip_addr, input_portID;
                            int i;
                            for (i = 6; input[i] != ' ' and input[i] != '\0'; i++) {
                                input_ip_addr += input[i];
                            }

                            for (i += 1; input[i] != '\n' and input[i] != '\0' ; i++) {
                                input_portID += input[i];
                            }

                            char *_ipaddr = &input_ip_addr[0];
                            struct in_addr server_ip_addr;
                            inet_aton(_ipaddr, &(server_ip_addr));

                            int server_pID = atoi(&input_portID[0]);

                            sockaddr_in new_addr;
                            new_addr.sin_family = AF_INET;
                            new_addr.sin_port = htons(server_pID);
                            new_addr.sin_addr = server_ip_addr;

                            if (connect(clientSocket, (sockaddr *) &new_addr, sizeof(new_addr)) == -1) {
                                cse4589_print_and_log("[LOGIN:ERROR]\n");
                                break;
                            }else {

                                my_addr = new_addr;
                                my_addr.sin_port = htons(PORT);

                                // send msg to server
                                string packet_flag = "0";
                                string packet_addr = inet_ntoa(my_addr.sin_addr);
                                string packet_port = convertInt(ntohs(my_addr.sin_port));

                                char *name = (char *) malloc(64);
                                gethostname(name, 64);

                                string packet_hostname = name;
                                string space = " ";
                                string packet = packet_flag + space + packet_addr + space + packet_port + space +
                                                packet_hostname;
                                char *buf = (char *) malloc(1024);
                                int j;
                                for (j = 0; j < packet.size(); ++j) {
                                    buf[j] = packet[j];
                                }
                                int sendRes = send(clientSocket, (void *) buf, j, 0);
                                if (sendRes == -1) {
                                    cout << "Send LOGIN msg to server Failed!" << endl;
                                } else {
                                    //cout << "Send msg success" << buf << endl;
                                }
                                cse4589_print_and_log("[LOGIN:SUCCESS]\n");
                                cse4589_print_and_log("[LOGIN:END]\n");
                            }
                        }else{
                            cse4589_print_and_log("Client receive an invalid request.\n");
                        }


                    } else {
                        cout<<"wrong entry"<<endl;
                    }
                }
            }

            printf("\n[PA1-Client@CSE489/589]$ ");
            fflush(stdout);

            //free(server_packet);
        }

    } else {
        printf("invalid parameter!\n");
        return -1;
    }

    return 0;
}
