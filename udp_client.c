//#include "udp_client.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main() {

    int ret;
    int sockfd;
    struct sockaddr_in servaddr, cliaddr;
    char send_buffer[1024];
    char recv_buffer[1024];
    char user_input[1024];
    char msg[201];
    socklen_t len;

    char recent_msg[201];

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        printf("socket() error: %s.\n", strerror(errno));
        return -1;
    }
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(32000);

    memset(&cliaddr, 0, sizeof(cliaddr));
    cliaddr.sin_family = AF_INET;
    cliaddr.sin_addr.s_addr = inet_addr("0.0.0.0");
    cliaddr.sin_port = htons(32001);

    bind(sockfd,
         (struct sockaddr *) &cliaddr,
         sizeof(cliaddr));

    while (1) {
        fgets(user_input, sizeof(user_input), stdin);

        int m = 0;

        if(strncmp(user_input, "login#", 6) == 0)
        {
            if(strncmp(user_input, "login#user1&client1", 15) == 0)
            {
                //printf("login succesful");
                m = strlen(user_input) - 6;

                memcpy(send_buffer + 4, user_input + 5, m);

                send_buffer[0] = 0x55; // These are constants you defined.
                send_buffer[1] = 0x31;
                send_buffer[2] = 0x01;
                send_buffer[3] = m;

                // The sendto() function send the designated number of bytes in the "send_buffer" to the destination address.
                ret = sendto(sockfd, send_buffer, m + 4, 0, (struct sockaddr *) &servaddr, sizeof(servaddr));
                if (ret <= 0) {
                    printf("sendto() error: %s.\n", strerror(errno));
                    return -1;
                }

                recvfrom(sockfd, recv_buffer, sizeof(recv_buffer), 0, (struct sockaddr *) &cliaddr, &len);
                if (recv_buffer[0] != 0x4A || recv_buffer[1] != 0x56)
                {

                    // Bad message!!! Skip this iteration.
                    continue;

                }
                else
                {
                    if (recv_buffer[2] == 0x02)
                    {
                        memset(msg, 0, sizeof(msg));
                        memcpy(msg, recv_buffer + 4, recv_buffer[3]);
                        printf("%s\n", msg);

                        while(strncmp(user_input, "logout#", 7) != 0)
                        {
                            fgets(user_input, sizeof(user_input), stdin);

                            if(strncmp(user_input, "post#", 5) == 0)
                            {
                                m = strlen(user_input);

                                memcpy(send_buffer + 4, user_input + 5, m);

                                send_buffer[0] = 0x55; // These are constants you defined.
                                send_buffer[1] = 0x31;
                                send_buffer[2] = 0x03;
                                send_buffer[3] = m;

                                ret = sendto(sockfd, send_buffer, m + 4, 0, (struct sockaddr *) &servaddr, sizeof(servaddr));
                                if (ret <= 0) {
                                    printf("sendto() error: %s.\n", strerror(errno));
                                    return -1;
                                }

                                recvfrom(sockfd, recv_buffer, sizeof(recv_buffer), 0, (struct sockaddr *) &cliaddr, &len);
                                if (recv_buffer[0] != 0x4A || recv_buffer[1] != 0x56)
                                {
                                    continue;
                                }
                                else
                                {
                                    if (recv_buffer[2] == 0x04)
                                    {
                                        memset(msg, 0, sizeof(msg));
                                        memcpy(msg, recv_buffer + 4, recv_buffer[3]);
                                        strcpy(user_input, msg);
                                        printf("Session timed-out\n");

                                    }
                                    else if (recv_buffer[2] == 0x05){
                                        memset(msg, 0, sizeof(msg));
                                        memcpy(msg, recv_buffer + 4, recv_buffer[3]);
                                        printf("post_ack#successful\n");
                                    }
                                }

                            }//end post# if
                            else if(strncmp(user_input, "subscribe#", 10) == 0)
                            {
                                m = strlen(user_input)-10;

                                memcpy(send_buffer + 4, user_input + 10, m);

                                send_buffer[0] = 0x55; // These are constants you defined.
                                send_buffer[1] = 0x31;
                                send_buffer[2] = 0x07;
                                send_buffer[3] = m;
                                ret = sendto(sockfd, send_buffer, m + 4, 0, (struct sockaddr *) &servaddr, sizeof(servaddr));
                                if (ret <= 0) {
                                    printf("sendto() error: %s.\n", strerror(errno));
                                    return -1;
                                }
                                recvfrom(sockfd, recv_buffer, sizeof(recv_buffer), 0, (struct sockaddr *) &cliaddr, &len);
                                if (recv_buffer[0] != 0x4A || recv_buffer[1] != 0x56)
                                {
                                    continue;

                                }
                                else
                                {
                                    if (recv_buffer[2] == 0x04)
                                    {
                                        memset(msg, 0, sizeof(msg));
                                        memcpy(msg, recv_buffer + 4, recv_buffer[3]);
                                        strcpy(user_input, msg);
                                        printf("Session timed-out\n");

                                    }
                                    else if (recv_buffer[2] == 0x06){
                                        memset(msg, 0, sizeof(msg));
                                        memcpy(msg, recv_buffer + 4, recv_buffer[3]);
                                        printf("%s\n", msg);
                                    }
                                    else if (recv_buffer[2] == 0x08){
                                        memset(msg, 0, sizeof(msg));
                                        memcpy(msg, recv_buffer + 4, recv_buffer[3]);
                                        printf("%s\n", msg);
                                        strcpy(user_input, "logout#");
                                    }
                                }

                            }//end subscribe if
                            else if(strncmp(user_input, "unsubscribe#", 12) == 0)
                            {
                                m = strlen(user_input)-12;

                                memcpy(send_buffer + 4, user_input + 12, m);

                                send_buffer[0] = 0x55; // These are constants you defined.
                                send_buffer[1] = 0x31;
                                send_buffer[2] = 0x09;
                                send_buffer[3] = m;

                                ret = sendto(sockfd, send_buffer, m + 4, 0, (struct sockaddr *) &servaddr, sizeof(servaddr));
                                if (ret <= 0) {
                                    printf("sendto() error: %s.\n", strerror(errno));
                                    return -1;
                                }
                                recvfrom(sockfd, recv_buffer, sizeof(recv_buffer), 0, (struct sockaddr *) &cliaddr, &len);
                                if (recv_buffer[0] != 0x4A || recv_buffer[1] != 0x56)
                                {
                                    continue;
                                }
                                else
                                {
                                    if (recv_buffer[2] == 0x04)
                                    {
                                        memset(msg, 0, sizeof(msg));
                                        memcpy(msg, recv_buffer + 4, recv_buffer[3]);
                                        strcpy(user_input, msg);
                                        printf("Session timed-out\n");

                                    }
                                    else if (recv_buffer[2] == 0x06){
                                        memset(msg, 0, sizeof(msg));
                                        memcpy(msg, recv_buffer + 4, recv_buffer[3]);
                                        printf("%s\n", msg);
                                    }
                                    else if (recv_buffer[2] == 0x08){
                                        memset(msg, 0, sizeof(msg));
                                        memcpy(msg, recv_buffer + 4, recv_buffer[3]);
                                        printf("%s\n", msg);
                                        strcpy(user_input, "logout#");
                                    }


                                }

                            }//end unsubscribe if
                            else
                            {
                                printf("Session reset by client.\n\n");
                                strcpy(user_input, "logout#");
                            }
                        }//end logout while

                        printf("You have been logged out. \n");
                    }
                    else
                    {
                        continue;
                    }
                }
            }
            else if(strncmp(user_input, "login#user2&client2", 16) == 0)
            {
                m = strlen(user_input) - 6;
                memcpy(send_buffer + 4, user_input + 5, m);

                send_buffer[0] = 0x55; // These are constants you defined.
                send_buffer[1] = 0x32;
                send_buffer[2] = 0x01;
                send_buffer[3] = m;

                // The sendto() function send the designated number of bytes in the "send_buffer" to the destination address.
                ret = sendto(sockfd, send_buffer, m + 4, 0, (struct sockaddr *) &servaddr, sizeof(servaddr));
                if (ret <= 0) {
                    printf("sendto() error: %s.\n", strerror(errno));
                    return -1;
                }

                // TODO: You are supposed to call the recvfrom() function here.
                // The client will receive the acknowledgement from the server.
                recvfrom(sockfd, recv_buffer, sizeof(recv_buffer), 0, (struct sockaddr *) &cliaddr, &len);
                if (recv_buffer[0] != 0x4A || recv_buffer[1] != 0x56)
                {
                    continue;
                }
                else
                {
                    if (recv_buffer[2] == 0x02)
                    {
                        memset(msg, 0, sizeof(msg));
                        memcpy(msg, recv_buffer + 4, recv_buffer[3]);
                        printf("%s\n", msg);

                        while(strncmp(user_input, "logout#", 7) != 0)
                        {
                            fgets(user_input, sizeof(user_input), stdin);

                            if(strncmp(user_input, "post#", 5) == 0)
                            {
                                m = strlen(user_input);

                                memcpy(send_buffer + 4, user_input + 5, m);

                                send_buffer[0] = 0x55; // These are constants you defined.
                                send_buffer[1] = 0x32;
                                send_buffer[2] = 0x03;
                                send_buffer[3] = m;

                                ret = sendto(sockfd, send_buffer, m + 4, 0, (struct sockaddr *) &servaddr, sizeof(servaddr));
                                if (ret <= 0) {
                                    printf("sendto() error: %s.\n", strerror(errno));
                                    return -1;
                                }
                                recvfrom(sockfd, recv_buffer, sizeof(recv_buffer), 0, (struct sockaddr *) &cliaddr, &len);
                                if (recv_buffer[0] != 0x4A || recv_buffer[1] != 0x56)
                                {
                                    continue;
                                }
                                else
                                {
                                    if (recv_buffer[2] == 0x04)
                                    {
                                        memset(msg, 0, sizeof(msg));
                                        memcpy(msg, recv_buffer + 4, recv_buffer[3]);
                                        strcpy(user_input, msg);
                                        printf("Session timed-out\n");

                                    }
                                    else if (recv_buffer[2] == 0x05){
                                        memset(msg, 0, sizeof(msg));
                                        memcpy(msg, recv_buffer + 4, recv_buffer[3]);
                                        printf("post_ack#successful\n");
                                    }

                                }

                            }//end post# if
                            else if(strncmp(user_input, "subscribe#", 10) == 0)
                            {
                                m = strlen(user_input)-10;

                                memcpy(send_buffer + 4, user_input + 10, m);

                                send_buffer[0] = 0x55; // These are constants you defined.
                                send_buffer[1] = 0x32;
                                send_buffer[2] = 0x07;
                                send_buffer[3] = m;

                                // The sendto() function send the designated number of bytes in the "send_buffer" to the destination address.
                                ret = sendto(sockfd, send_buffer, m + 4, 0, (struct sockaddr *) &servaddr, sizeof(servaddr));
                                if (ret <= 0) {
                                    printf("sendto() error: %s.\n", strerror(errno));
                                    return -1;
                                }
                                recvfrom(sockfd, recv_buffer, sizeof(recv_buffer), 0, (struct sockaddr *) &cliaddr, &len);
                                if (recv_buffer[0] != 0x4A || recv_buffer[1] != 0x56)
                                {
                                    continue;
                                }
                                else
                                {
                                    if (recv_buffer[2] == 0x04)
                                    {
                                        memset(msg, 0, sizeof(msg));
                                        memcpy(msg, recv_buffer + 4, recv_buffer[3]);
                                        strcpy(user_input, msg);
                                        printf("Session timed-out\n");

                                    }
                                    else if (recv_buffer[2] == 0x06){
                                        memset(msg, 0, sizeof(msg));
                                        memcpy(msg, recv_buffer + 4, recv_buffer[3]);
                                        printf("%s\n", msg);
                                    }
                                    else if (recv_buffer[2] == 0x08){
                                        memset(msg, 0, sizeof(msg));
                                        memcpy(msg, recv_buffer + 4, recv_buffer[3]);
                                        printf("%s\n", msg);
                                        strcpy(user_input, "logout#");
                                    }


                                }

                            }//end subscribe if
                            else if(strncmp(user_input, "unsubscribe#", 12) == 0)
                            {
                                m = strlen(user_input)-12;

                                memcpy(send_buffer + 4, user_input + 12, m);

                                send_buffer[0] = 0x55; // These are constants you defined.
                                send_buffer[1] = 0x32;
                                send_buffer[2] = 0x09;
                                send_buffer[3] = m;

                                // The sendto() function send the designated number of bytes in the "send_buffer" to the destination address.
                                ret = sendto(sockfd, send_buffer, m + 4, 0, (struct sockaddr *) &servaddr, sizeof(servaddr));
                                if (ret <= 0) {
                                    printf("sendto() error: %s.\n", strerror(errno));
                                    return -1;
                                }

                                // TODO: You are supposed to call the recvfrom() function here.
                                // The client will receive the acknowledgement from the server.
                                recvfrom(sockfd, recv_buffer, sizeof(recv_buffer), 0, (struct sockaddr *) &cliaddr, &len);
                                if (recv_buffer[0] != 0x4A || recv_buffer[1] != 0x56)
                                {
                                    continue;
                                }
                                else
                                {
                                    if (recv_buffer[2] == 0x04)
                                    {
                                        memset(msg, 0, sizeof(msg));
                                        memcpy(msg, recv_buffer + 4, recv_buffer[3]);
                                        strcpy(user_input, msg);
                                        printf("Session timed-out\n");

                                    }
                                    else if (recv_buffer[2] == 0x06){
                                        memset(msg, 0, sizeof(msg));
                                        memcpy(msg, recv_buffer + 4, recv_buffer[3]);
                                        //strcpy(user_input, msg);
                                        printf("%s\n", msg);
                                    }
                                    else if (recv_buffer[2] == 0x08){
                                        memset(msg, 0, sizeof(msg));
                                        memcpy(msg, recv_buffer + 4, recv_buffer[3]);
                                        //strcpy(user_input, msg);
                                        printf("%s\n", msg);
                                        strcpy(user_input, "logout#");
                                    }


                                }

                            }//end unsubscribe if
                            else
                            {
                                printf("Session reset by client.\n\n");
                                strcpy(user_input, "logout#");
                            }

                        }//end logout while

                        printf("You have been logged out\n");

                    }
                    else
                    {
                        continue;
                    }
                }
            }
            
            else
            {
                printf("login_ack#unsuccessful\n");
                continue;
            }
        }
        else
        {
            printf("login_ack#unsuccessful\n");
            continue;
        }
    }
}
