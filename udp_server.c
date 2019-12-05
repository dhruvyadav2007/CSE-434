#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <ctype.h>

char* currentTime(){
    time_t rawTime;
    time ( &rawTime );
    char* temp = ctime (&rawTime);
    return temp;
}

int main() {
    int ret;
    int sockfd;
    struct sockaddr_in servaddr, cliaddr, cliaddr2, cliaddr3;
    char recv_buffer[1024];
    char send_buffer[6000];
    char output[6000];
    int recv_len;
    socklen_t len;

    //session variables
    time_t user1start,user1endwait,user2start,user2endwait,user3start,user3endwait;
    time_t seconds = 30;

    //user subscribtions
    char user1Subscription[100] = "";
    char user2Subscription[100] = "";
    char user3Subscription[100] = "";

    //all the user arrays
    char user1Array[20][300];
    char user2Array[20][300];
    char user3Array[20][300];
    char user12Array[20][300];
    char user13Array[20][300];
    char user23Array[20][300];

    //counter variables for all the words in arrays
    int user1Counter = 0;
    int user2Counter = 0;
    int user3Counter = 0;
    int user12Counter = 0;
    int user13Counter = 0;
    int user23Counter = 0;

    char recent_msg[201];

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        printf("socket() error: %s.\n", strerror(errno));
        return -1;
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(32000);

    memset(&cliaddr, 0, sizeof(cliaddr));
    cliaddr.sin_family = AF_INET;
    cliaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    cliaddr.sin_port = htons(32001);


    bind(sockfd,
         (struct sockaddr *) &servaddr,
         sizeof(servaddr));

    while (1) {
        len = sizeof(cliaddr);
        recv_len = recvfrom(sockfd, recv_buffer, sizeof(recv_buffer), 0, (struct sockaddr *) &cliaddr, &len);

        if (recv_len <= 0) {
            printf("recvfrom() error: %s.\n", strerror(errno));
            return -1;
        }


        if (recv_buffer[0] == 0x55 && recv_buffer[1] == 0x31)
        {
            if (recv_buffer[2] == 0x01)
            {
                memset(recent_msg, 0, sizeof(recent_msg));
                memcpy(recent_msg, recv_buffer + 4, recv_buffer[3]);
                char user_input[1024] = "login_ack#successful\n";

                if(strncmp(user1Subscription, "none",4) == 0)
                {
                    strcat(user_input, "empty\n");

                }
                else if(strncmp(user1Subscription, "user2",5) == 0)
                {
                    for(int i = user2Counter; i >= 0; i--)
                    {
                        strcat(user2Array[i], "");
                        strcat(user_input, user2Array[i]);
                    }
                }
                else if(strncmp(user1Subscription, "user3",5) == 0)
                {
                    for(int i = user3Counter; i >= 0; i--)
                    {
                        strcat(user3Array[i], "");
                        strcat(user_input, user3Array[i]);
                    }
                }
                else if(strncmp(user1Subscription, "user2 and 3",6) == 0)
                {
                    for(int i = user23Counter; i >= 0; i--)
                    {
                        strcat(user23Array[i], "");
                        strcat(user_input, user23Array[i]);
                    }
                }
                //bool user1Online = true;
                int m = strlen(user_input);
                memcpy(send_buffer + 4, user_input, m);

                send_buffer[0] = 0x4A; // These are constants you defined.
                send_buffer[1] = 0x56;
                send_buffer[2] = 0x02;//acknowledgement of posting
                send_buffer[3] = m;

                //start the user timer
                user1start = time(NULL);

                //set the end time
                user1endwait = user1start + seconds;

            }
            else if (recv_buffer[2] == 0x03)//post
            {
                //check the times
                user1start = time(NULL);
                if(user1start >= user1endwait)
                {
                    memset(recent_msg, 0, sizeof(recent_msg));
                    memcpy(recent_msg, recv_buffer + 4, recv_buffer[3]);
                    char user_input[1024] = "logout#";
                    //bool user1Online = false;
                    int m = strlen(user_input);
                    memcpy(send_buffer + 4, user_input, m);

                    send_buffer[0] = 0x4A; // These are constants you defined.
                    send_buffer[1] = 0x56;
                    send_buffer[2] = 0x04;//acknowledgement of posting
                    send_buffer[3] = m;
                }
                else
                {
                    //update the times
                    //start the user timer
                    user1start = time(NULL);

                    //set the end time
                    user1endwait = user1start + seconds;

                    memset(recent_msg, 0, sizeof(recent_msg));
                    memcpy(recent_msg, recv_buffer + 4, recv_buffer[3]);
                    //add user 1 post to all user 1 arrays
                    strcpy(user1Array[user1Counter],recent_msg);
                    strcpy(user12Array[user12Counter],recent_msg);
                    strcpy(user13Array[user13Counter],recent_msg);

                    //update the counters
                    user1Counter++;
                    user12Counter++;
                    user13Counter++;

                    int m = strlen(recent_msg);
                    memcpy(send_buffer + 4, recent_msg, m);

                    send_buffer[0] = 0x4A; // These are constants you defined.
                    send_buffer[1] = 0x56;
                    send_buffer[2] = 0x05;//acknowledgement of posting
                    send_buffer[3] = m;

                }

            }// end recv_buffer == 3
            else if (recv_buffer[2] == 0x07)
            {
                //check the times
                user1start = time(NULL);
                if(user1start >= user1endwait)
                {
                    memset(recent_msg, 0, sizeof(recent_msg));
                    memcpy(recent_msg, recv_buffer + 4, recv_buffer[3]);
                    char user_input[1024] = "logout#";
                    //bool user1Online = false;
                    int m = strlen(user_input);
                    memcpy(send_buffer + 4, user_input, m);

                    send_buffer[0] = 0x4A; // These are constants you defined.
                    send_buffer[1] = 0x56;
                    send_buffer[2] = 0x04;//acknowledgement of posting
                    send_buffer[3] = m;
                }
                else
                {
                    //update the times
                    //start the user timer
                    user1start = time(NULL);

                    //set the end time
                    user1endwait = user1start + seconds;
                    memset(recent_msg, 0, sizeof(recent_msg));
                    memcpy(recent_msg, recv_buffer + 4, recv_buffer[3]);

                    if(strncmp(recent_msg, "user2",5) == 0)
                    {
                        if(strncmp(user1Subscription, "user3", 5)== 0)
                        {
                            strcpy(user1Subscription, "user23");
                            char user_input[1024] = "subscribe_ack#successful\n";
                            int m = strlen(user_input);
                            memcpy(send_buffer + 5, user_input, m);

                            send_buffer[0] = 0x4A; // These are constants you defined.
                            send_buffer[1] = 0x56;
                            send_buffer[2] = 0x06;//acknowledgement of posting
                            send_buffer[3] = m;
                        }
                        else if(strncmp(user1Subscription, "none", 4) == 0)
                        {
                            strcpy(user1Subscription, "user2");
                            char user_input[1024] = "subscribe_ack#successful\n";
                            int m = strlen(user_input);
                            memcpy(send_buffer + 5, user_input, m);

                            send_buffer[0] = 0x4A; // These are constants you defined.
                            send_buffer[1] = 0x56;
                            send_buffer[2] = 0x06;//acknowledgement of posting
                            send_buffer[3] = m;
                        }
                        else if(strncmp(user1Subscription, "user2", 5) == 0)
                        {
                            strcpy(user1Subscription, "user2");
                            char user_input[1024] = "subscribe_ack#failed\n";
                            int m = strlen(user_input);
                            memcpy(send_buffer + 5, user_input, m);

                            send_buffer[0] = 0x4A; // These are constants you defined.
                            send_buffer[1] = 0x56;
                            send_buffer[2] = 0x06;//acknowledgement of posting
                            send_buffer[3] = m;
                        }

                    } //end of if == user2
                    else if(strncmp(recent_msg, "user3",5) == 0)
                    {
                        if(strncmp(user1Subscription, "user2", 5)== 0)
                        {
                            strcpy(user1Subscription, "user23");
                            char user_input[1024] = "subscribe_ack#successful\n";
                            int m = strlen(user_input);
                            memcpy(send_buffer + 5, user_input, m);

                            send_buffer[0] = 0x4A; // These are constants you defined.
                            send_buffer[1] = 0x56;
                            send_buffer[2] = 0x06;//acknowledgement of posting
                            send_buffer[3] = m;
                        }
                        else if(strncmp(user1Subscription, "none", 4) == 0)
                        {
                            strcpy(user1Subscription, "user3");
                            char user_input[1024] = "subscribe_ack#successful\n";
                            int m = strlen(user_input);
                            memcpy(send_buffer + 5, user_input, m);

                            send_buffer[0] = 0x4A; // These are constants you defined.
                            send_buffer[1] = 0x56;
                            send_buffer[2] = 0x06;//acknowledgement of posting
                            send_buffer[3] = m;
                        }
                        else if(strncmp(user1Subscription, "user3", 5) == 0)
                        {
                            strcpy(user1Subscription, "user3");
                            char user_input[1024] = "subscribe_ack#failed\n";
                            int m = strlen(user_input);
                            memcpy(send_buffer + 5, user_input, m);

                            send_buffer[0] = 0x4A; // These are constants you defined.
                            send_buffer[1] = 0x56;
                            send_buffer[2] = 0x06;//acknowledgement of posting
                            send_buffer[3] = m;
                        }

                    }//end if == user3
                    else
                    {
                        memset(recent_msg, 0, sizeof(recent_msg));
                        memcpy(recent_msg, recv_buffer + 4, recv_buffer[3]);
                        char user_input[1024] = "session reset by server\n";
                        //bool user1Online = false;
                        int m = strlen(user_input);
                        memcpy(send_buffer + 4, user_input, m);

                        send_buffer[0] = 0x4A; // These are constants you defined.
                        send_buffer[1] = 0x56;
                        send_buffer[2] = 0x08;//acknowledgement of posting
                        send_buffer[3] = m;

                    }
                } //in time end else



            }//end else if == 7
            else if (recv_buffer[2] == 0x09)
            {
                //check the times
                user1start = time(NULL);
                if(user1start >= user1endwait)
                {
                    memset(recent_msg, 0, sizeof(recent_msg));
                    memcpy(recent_msg, recv_buffer + 4, recv_buffer[3]);
                    char user_input[1024] = "logout#";
                    //bool user1Online = false;
                    int m = strlen(user_input);
                    memcpy(send_buffer + 4, user_input, m);

                    send_buffer[0] = 0x4A; // These are constants you defined.
                    send_buffer[1] = 0x56;
                    send_buffer[2] = 0x04;//acknowledgement of posting
                    send_buffer[3] = m;
                }
                else
                {
                    //update the times
                    //start the user timer
                    user1start = time(NULL);

                    //set the end time
                    user1endwait = user1start + seconds;
                    memset(recent_msg, 0, sizeof(recent_msg));
                    memcpy(recent_msg, recv_buffer + 4, recv_buffer[3]);

                    if(strncmp(recent_msg, "user2",5) == 0)
                    {
                        if(strncmp(user1Subscription, "user23", 6)== 0)
                        {
                            strcpy(user1Subscription, "user3");
                            char user_input[1024] = "unsubscribe_ack#successful\n";
                            int m = strlen(user_input);
                            memcpy(send_buffer + 5, user_input, m);

                            send_buffer[0] = 0x4A; // These are constants you defined.
                            send_buffer[1] = 0x56;
                            send_buffer[2] = 0x06;//acknowledgement of posting
                            send_buffer[3] = m;
                        }
                        else if(strncmp(user1Subscription, "user2", 5) == 0)
                        {
                            strcpy(user1Subscription, "none");
                            char user_input[1024] = "unsubscribe_ack#successful\n";
                            int m = strlen(user_input);
                            memcpy(send_buffer + 5, user_input, m);

                            send_buffer[0] = 0x4A; // These are constants you defined.
                            send_buffer[1] = 0x56;
                            send_buffer[2] = 0x06;//acknowledgement of posting
                            send_buffer[3] = m;
                        }
                        else if(strncmp(user1Subscription, "user3", 5) == 0)
                        {
                            strcpy(user1Subscription, "user3");
                            char user_input[1024] = "subscribe_ack#successful\n";
                            int m = strlen(user_input);
                            memcpy(send_buffer + 5, user_input, m);

                            send_buffer[0] = 0x4A; // These are constants you defined.
                            send_buffer[1] = 0x56;
                            send_buffer[2] = 0x06;//acknowledgement of posting
                            send_buffer[3] = m;
                        }
                        else if(strncmp(user1Subscription, "none", 4) == 0)
                        {
                            strcpy(user1Subscription, "none");
                            char user_input[1024] = "unsubscribe_ack#successful\n";
                            int m = strlen(user_input);
                            memcpy(send_buffer + 5, user_input, m);

                            send_buffer[0] = 0x4A; // These are constants you defined.
                            send_buffer[1] = 0x56;
                            send_buffer[2] = 0x06;//acknowledgement of posting
                            send_buffer[3] = m;
                        }

                    } //end of if == user2
                    else if(strncmp(recent_msg, "user3",5) == 0)
                    {
                        if(strncmp(user1Subscription, "user23", 6)== 0)
                        {
                            strcpy(user1Subscription, "user2");
                            //char user_input[1024] = "Notification: unsubscribed user 3, still subscribed 2. \n";
                            char user_input[1024] = "unsubscribe_ack#successful\n";
                            int m = strlen(user_input);
                            memcpy(send_buffer + 5, user_input, m);

                            send_buffer[0] = 0x4A; // These are constants you defined.
                            send_buffer[1] = 0x56;
                            send_buffer[2] = 0x06;//acknowledgement of posting
                            send_buffer[3] = m;
                        }
                        else if(strncmp(user1Subscription, "user2", 5) == 0)
                        {
                            strcpy(user1Subscription, "user2");
                            char user_input[1024] = "unsubscribe_ack#successful\n";
                            int m = strlen(user_input);
                            memcpy(send_buffer + 5, user_input, m);

                            send_buffer[0] = 0x4A; // These are constants you defined.
                            send_buffer[1] = 0x56;
                            send_buffer[2] = 0x06;//acknowledgement of posting
                            send_buffer[3] = m;
                        }
                        else if(strncmp(user1Subscription, "user3", 5) == 0)
                        {
                            strcpy(user1Subscription, "none");
                            char user_input[1024] = "unsubscribe_ack#successful\n";
                            int m = strlen(user_input);
                            memcpy(send_buffer + 5, user_input, m);

                            send_buffer[0] = 0x4A; // These are constants you defined.
                            send_buffer[1] = 0x56;
                            send_buffer[2] = 0x06;//acknowledgement of posting
                            send_buffer[3] = m;
                        }
                        else if(strncmp(user1Subscription, "none", 4) == 0)
                        {
                            strcpy(user1Subscription, "none");
                            char user_input[1024] = "unsubscribe_ack#successful\n";
                            int m = strlen(user_input);
                            memcpy(send_buffer + 5, user_input, m);

                            send_buffer[0] = 0x4A; // These are constants you defined.
                            send_buffer[1] = 0x56;
                            send_buffer[2] = 0x06;//acknowledgement of posting
                            send_buffer[3] = m;
                        }
                    }//end if == user3
                    else
                    {
                        memset(recent_msg, 0, sizeof(recent_msg));
                        memcpy(recent_msg, recv_buffer + 4, recv_buffer[3]);
                        char user_input[1024] = "session reset by server\n";
                        //bool user1Online = false;
                        int m = strlen(user_input);
                        memcpy(send_buffer + 4, user_input, m);

                        send_buffer[0] = 0x4A; // These are constants you defined.
                        send_buffer[1] = 0x56;
                        send_buffer[2] = 0x08;//acknowledgement of posting
                        send_buffer[3] = m;

                    }
                } //in time end else



            }//end else if == 9
            else
            {

                // Wrong message format. Skip this iteration.
                continue;
            }

        }//end user1
        else if (recv_buffer[0] == 0x55 && recv_buffer[1] == 0x32)
        {

            //do time checkig for session state here

            if (recv_buffer[2] == 0x01)
            {
                memset(recent_msg, 0, sizeof(recent_msg));
                memcpy(recent_msg, recv_buffer + 4, recv_buffer[3]);
                char user_input[1024] = "login_ack#successful\n";

                //check subscriptions and add in the words for feed.
                if(strncmp(user2Subscription, "none",4) == 0)
                {
                    strcat(user_input, "empty\n");

                }
                else if(strncmp(user2Subscription, "user1",5) == 0)
                {
                    for(int i = user1Counter; i >= 0; i--)
                    {
                        strcat(user1Array[i], "");
                        strcat(user_input, user1Array[i]);
                    }
                }
                else if(strncmp(user2Subscription, "user3",5) == 0)
                {
                    for(int i = user3Counter; i >= 0; i--)
                    {
                        strcat(user3Array[i], "");
                        strcat(user_input, user3Array[i]);
                    }
                }
                else if(strncmp(user1Subscription, "user13",6) == 0)
                {
                    for(int i = user13Counter; i >= 0; i--)
                    {
                        strcat(user13Array[i], "");
                        strcat(user_input, user13Array[i]);
                    }
                }
                //bool user1Online = true;
                int m = strlen(user_input);
                memcpy(send_buffer + 4, user_input, m);

                send_buffer[0] = 0x4A; // These are constants you defined.
                send_buffer[1] = 0x56;
                send_buffer[2] = 0x02;//acknowledgement of posting
                send_buffer[3] = m;

                //start the user timer
                user2start = time(NULL);

                //set the end time
                user2endwait = user2start + seconds;



            }
            else if (recv_buffer[2] == 0x03)//post
            {
                //check the times
                user2start = time(NULL);
                if(user2start >= user2endwait)
                {
                    memset(recent_msg, 0, sizeof(recent_msg));
                    memcpy(recent_msg, recv_buffer + 4, recv_buffer[3]);
                    char user_input[1024] = "logout#";
                    //bool user1Online = false;
                    int m = strlen(user_input);
                    memcpy(send_buffer + 4, user_input, m);

                    send_buffer[0] = 0x4A; // These are constants you defined.
                    send_buffer[1] = 0x56;
                    send_buffer[2] = 0x04;//acknowledgement of posting
                    send_buffer[3] = m;
                }
                else
                {
                    //update the times
                    //start the user timer
                    user2start = time(NULL);

                    //set the end time
                    user2endwait = user2start + seconds;

                    memset(recent_msg, 0, sizeof(recent_msg));
                    memcpy(recent_msg, recv_buffer + 4, recv_buffer[3]);
                    //add user 1 post to all user 1 arrays
                    strcpy(user2Array[user2Counter],recent_msg);
                    strcpy(user12Array[user12Counter],recent_msg);
                    strcpy(user23Array[user23Counter],recent_msg);

                    //update the counters
                    user2Counter++;
                    user12Counter++;
                    user23Counter++;

                    int m = strlen(recent_msg);
                    memcpy(send_buffer + 4, recent_msg, m);

                    send_buffer[0] = 0x4A; // These are constants you defined.
                    send_buffer[1] = 0x56;
                    send_buffer[2] = 0x05;//acknowledgement of posting
                    send_buffer[3] = m;

                }


            }// end recv_buffer == 3
            else if (recv_buffer[2] == 0x07)
            {
                //check the times
                user2start = time(NULL);
                if(user2start >= user2endwait)
                {
                    memset(recent_msg, 0, sizeof(recent_msg));
                    memcpy(recent_msg, recv_buffer + 4, recv_buffer[3]);
                    char user_input[1024] = "logout#";
                    //bool user1Online = false;
                    int m = strlen(user_input);
                    memcpy(send_buffer + 4, user_input, m);

                    send_buffer[0] = 0x4A; // These are constants you defined.
                    send_buffer[1] = 0x56;
                    send_buffer[2] = 0x04;//acknowledgement of posting
                    send_buffer[3] = m;
                }
                else
                {
                    //update the times
                    //start the user timer
                    user1start = time(NULL);

                    //set the end time
                    user2endwait = user2start + seconds;
                    memset(recent_msg, 0, sizeof(recent_msg));
                    memcpy(recent_msg, recv_buffer + 4, recv_buffer[3]);

                    if(strncmp(recent_msg, "user1",5) == 0)
                    {
                        if(strncmp(user2Subscription, "user3", 5)== 0)
                        {
                            strcpy(user2Subscription, "user13");
                            //char user_input[1024] = "Notification: subscribed users 1 and 3\n";
                            char user_input[1024] = "subscribe_ack#successful\n";
                            int m = strlen(user_input);
                            memcpy(send_buffer + 5, user_input, m);

                            send_buffer[0] = 0x4A; // These are constants you defined.
                            send_buffer[1] = 0x56;
                            send_buffer[2] = 0x06;//acknowledgement of posting
                            send_buffer[3] = m;
                        }
                        else if(strncmp(user2Subscription, "none", 4) == 0)
                        {
                            strcpy(user2Subscription, "user1");
                            //char user_input[1024] = "Notification: subscribed to user 1\n";
                            char user_input[1024] = "subscribe_ack#successful\n";
                            int m = strlen(user_input);
                            memcpy(send_buffer + 5, user_input, m);

                            send_buffer[0] = 0x4A; // These are constants you defined.
                            send_buffer[1] = 0x56;
                            send_buffer[2] = 0x06;//acknowledgement of posting
                            send_buffer[3] = m;
                        }
                        else if(strncmp(user2Subscription, "user1", 5) == 0)
                        {
                            strcpy(user2Subscription, "user1");
                            //char user_input[1024] = "Notification: already subscribed to user 1\n";
                            char user_input[1024] = "subscribe_ack#failed\n";
                            int m = strlen(user_input);
                            memcpy(send_buffer + 5, user_input, m);

                            send_buffer[0] = 0x4A; // These are constants you defined.
                            send_buffer[1] = 0x56;
                            send_buffer[2] = 0x06;//acknowledgement of posting
                            send_buffer[3] = m;
                        }

                    } //end of if == user2
                    else if(strncmp(recent_msg, "user3",5) == 0)
                    {
                        if(strncmp(user2Subscription, "user1", 5)== 0)
                        {
                            strcpy(user2Subscription, "user13");
                            //char user_input[1024] = "Notification: subscribed users 1 and 3\n";
                            char user_input[1024] = "subscribe_ack#successful\n";
                            int m = strlen(user_input);
                            memcpy(send_buffer + 5, user_input, m);

                            send_buffer[0] = 0x4A; // These are constants you defined.
                            send_buffer[1] = 0x56;
                            send_buffer[2] = 0x06;//acknowledgement of posting
                            send_buffer[3] = m;
                        }
                        else if(strncmp(user2Subscription, "none", 4) == 0)
                        {
                            strcpy(user2Subscription, "user3");
                            //char user_input[1024] = "Notification: subscribed to user 3\n";
                            char user_input[1024] = "subscribe_ack#successful\n";
                            int m = strlen(user_input);
                            memcpy(send_buffer + 5, user_input, m);

                            send_buffer[0] = 0x4A; // These are constants you defined.
                            send_buffer[1] = 0x56;
                            send_buffer[2] = 0x06;//acknowledgement of posting
                            send_buffer[3] = m;
                        }
                        else if(strncmp(user2Subscription, "user3", 5) == 0)
                        {
                            strcpy(user2Subscription, "user3");
                            //char user_input[1024] = "Notification: alredy subscribed to user 3\n";
                            char user_input[1024] = "subscribe_ack#failed\n";
                            int m = strlen(user_input);
                            memcpy(send_buffer + 5, user_input, m);

                            send_buffer[0] = 0x4A; // These are constants you defined.
                            send_buffer[1] = 0x56;
                            send_buffer[2] = 0x06;//acknowledgement of posting
                            send_buffer[3] = m;
                        }

                    }//end if == user3
                        //}
                    else
                    {
                        memset(recent_msg, 0, sizeof(recent_msg));
                        memcpy(recent_msg, recv_buffer + 4, recv_buffer[3]);
                        char user_input[1024] = "session reset by server\n";
                        //bool user1Online = false;
                        int m = strlen(user_input);
                        memcpy(send_buffer + 4, user_input, m);

                        send_buffer[0] = 0x4A; // These are constants you defined.
                        send_buffer[1] = 0x56;
                        send_buffer[2] = 0x08;//acknowledgement of posting
                        send_buffer[3] = m;

                    }
                } //in time end else



            }//end else if == 7
            else if (recv_buffer[2] == 0x09)
            {
                //check the times
                user2start = time(NULL);
                if(user2start >= user2endwait)
                {
                    memset(recent_msg, 0, sizeof(recent_msg));
                    memcpy(recent_msg, recv_buffer + 4, recv_buffer[3]);
                    char user_input[1024] = "logout#";
                    //bool user1Online = false;
                    int m = strlen(user_input);
                    memcpy(send_buffer + 4, user_input, m);

                    send_buffer[0] = 0x4A; // These are constants you defined.
                    send_buffer[1] = 0x56;
                    send_buffer[2] = 0x04;//acknowledgement of posting
                    send_buffer[3] = m;
                }
                else
                {
                    //update the times
                    //start the user timer
                    user2start = time(NULL);

                    //set the end time
                    user2endwait = user2start + seconds;
                    memset(recent_msg, 0, sizeof(recent_msg));
                    memcpy(recent_msg, recv_buffer + 4, recv_buffer[3]);

                    if(strncmp(recent_msg, "user1",5) == 0)
                    {
                        if(strncmp(user2Subscription, "user13", 6)== 0)
                        {
                            strcpy(user2Subscription, "user3");
//                                                                                                              char user_input[1024] = "Notification: unsubscribed user 1, subscribed to 3\n";
                            char user_input[1024] = "unsubscribe_ack#successful\n";
                            int m = strlen(user_input);
                            memcpy(send_buffer + 5, user_input, m);

                            send_buffer[0] = 0x4A; // These are constants you defined.
                            send_buffer[1] = 0x56;
                            send_buffer[2] = 0x06;//acknowledgement of posting
                            send_buffer[3] = m;
                        }
                        else if(strncmp(user2Subscription, "user1", 5) == 0)
                        {
                            strcpy(user2Subscription, "none");
                            //char user_input[1024] = "Notification: unsubscribed to user 1\n";
                            char user_input[1024] = "unsubscribe_ack#successful\n";
                            int m = strlen(user_input);
                            memcpy(send_buffer + 5, user_input, m);

                            send_buffer[0] = 0x4A; // These are constants you defined.
                            send_buffer[1] = 0x56;
                            send_buffer[2] = 0x06;//acknowledgement of posting
                            send_buffer[3] = m;
                        }
                        else if(strncmp(user2Subscription, "user3", 5) == 0)
                        {
                            strcpy(user2Subscription, "user3");
                            //char user_input[1024] = "Notification: not subscribed user 1 only 3. \n";
                            char user_input[1024] = "unsubscribe_ack#successful\n";
                            int m = strlen(user_input);
                            memcpy(send_buffer + 5, user_input, m);

                            send_buffer[0] = 0x4A; // These are constants you defined.
                            send_buffer[1] = 0x56;
                            send_buffer[2] = 0x06;//acknowledgement of posting
                            send_buffer[3] = m;
                        }
                        else if(strncmp(user2Subscription, "none", 4) == 0)
                        {
                            strcpy(user2Subscription, "none");
//                                                                                                              char user_input[1024] = "Notification: not subscribed to anyone. \n";
                            char user_input[1024] = "unsubscribe_ack#successful\n";
                            int m = strlen(user_input);
                            memcpy(send_buffer + 5, user_input, m);

                            send_buffer[0] = 0x4A; // These are constants you defined.
                            send_buffer[1] = 0x56;
                            send_buffer[2] = 0x06;//acknowledgement of posting
                            send_buffer[3] = m;
                        }

                    } //end of if == user1
                    else if(strncmp(recent_msg, "user3",5) == 0)
                    {
                        if(strncmp(user2Subscription, "user13", 6)== 0)
                        {
                            strcpy(user2Subscription, "user1");
//                                                                                                                char user_input[1024] = "Notification: unsubscribed user 3, still subscribed 1. \n";
                            char user_input[1024] = "unsubscribe_ack#successful\n";
                            int m = strlen(user_input);
                            memcpy(send_buffer + 5, user_input, m);

                            send_buffer[0] = 0x4A; // These are constants you defined.
                            send_buffer[1] = 0x56;
                            send_buffer[2] = 0x06;//acknowledgement of posting
                            send_buffer[3] = m;
                        }
                        else if(strncmp(user2Subscription, "user1", 5) == 0)
                        {
                            strcpy(user2Subscription, "user1");
//                                                                                                                char user_input[1024] = "Notification: not subscribed user 3 only 1.\n";
                            char user_input[1024] = "unsubscribe_ack#successful\n";
                            int m = strlen(user_input);
                            memcpy(send_buffer + 5, user_input, m);

                            send_buffer[0] = 0x4A; // These are constants you defined.
                            send_buffer[1] = 0x56;
                            send_buffer[2] = 0x06;//acknowledgement of posting
                            send_buffer[3] = m;
                        }
                        else if(strncmp(user2Subscription, "user3", 5) == 0)
                        {
                            strcpy(user2Subscription, "none");
//                                                                                                                char user_input[1024] = "Notification: not subscribed user 3. \n";
                            char user_input[1024] = "unsubscribe_ack#successful\n";
                            int m = strlen(user_input);
                            memcpy(send_buffer + 5, user_input, m);

                            send_buffer[0] = 0x4A; // These are constants you defined.
                            send_buffer[1] = 0x56;
                            send_buffer[2] = 0x06;//acknowledgement of posting
                            send_buffer[3] = m;
                        }
                        else if(strncmp(user2Subscription, "none", 4) == 0)
                        {
                            strcpy(user1Subscription, "none");
//                                                                                                                char user_input[1024] = "Notification: not subscribed to anyone. \n";
                            char user_input[1024] = "unsubscribe_ack#successful\n";
                            int m = strlen(user_input);
                            memcpy(send_buffer + 5, user_input, m);

                            send_buffer[0] = 0x4A; // These are constants you defined.
                            send_buffer[1] = 0x56;
                            send_buffer[2] = 0x06;//acknowledgement of posting
                            send_buffer[3] = m;
                        }
                    }//end if == user3
                    else
                    {
                        memset(recent_msg, 0, sizeof(recent_msg));
                        memcpy(recent_msg, recv_buffer + 4, recv_buffer[3]);
                        char user_input[1024] = "session reset by server\n";
                        //bool user1Online = false;
                        int m = strlen(user_input);
                        memcpy(send_buffer + 4, user_input, m);

                        send_buffer[0] = 0x4A; // These are constants you defined.
                        send_buffer[1] = 0x56;
                        send_buffer[2] = 0x08;//acknowledgement of posting
                        send_buffer[3] = m;

                    }
                } //in time end else



            }//end else if == 9
            else
            {

                // Wrong message format. Skip this iteration.
                continue;
            }

        }//end user 2
        else if (recv_buffer[0] == 0x55 && recv_buffer[1] == 0x33)
        {

            //do time checkig for session state here

            if (recv_buffer[2] == 0x01)
            {
                memset(recent_msg, 0, sizeof(recent_msg));
                memcpy(recent_msg, recv_buffer + 4, recv_buffer[3]);
                char user_input[1024] = "login_ack#successful\n";

                //check subscriptions and add in the words for feed.
                if(strncmp(user3Subscription, "none",4) == 0)
                {
                    strcat(user_input, "empty\n");

                }
                else if(strncmp(user3Subscription, "user2",5) == 0)
                {
                    for(int i = user2Counter; i >= 0; i--)
                    {
                        strcat(user2Array[i], "");
                        strcat(user_input, user2Array[i]);
                    }
                }
                else if(strncmp(user3Subscription, "user1",5) == 0)
                {
                    for(int i = user1Counter; i >= 0; i--)
                    {
                        strcat(user1Array[i], "");
                        strcat(user_input, user1Array[i]);
                    }
                }
                else if(strncmp(user3Subscription, "user12",6) == 0)
                {
                    for(int i = user12Counter; i >= 0; i--)
                    {
                        strcat(user12Array[i], "");
                        strcat(user_input, user12Array[i]);
                    }
                }
                //bool user1Online = true;
                int m = strlen(user_input);
                memcpy(send_buffer + 4, user_input, m);

                send_buffer[0] = 0x4A; // These are constants you defined.
                send_buffer[1] = 0x56;
                send_buffer[2] = 0x02;//acknowledgement of posting
                send_buffer[3] = m;

                //start the user timer
                user3start = time(NULL);

                //set the end time
                user3endwait = user3start + seconds;



            }
            else if (recv_buffer[2] == 0x03)//post
            {
                //check the times
                user3start = time(NULL);
                if(user3start >= user3endwait)
                {
                    memset(recent_msg, 0, sizeof(recent_msg));
                    memcpy(recent_msg, recv_buffer + 4, recv_buffer[3]);
                    char user_input[1024] = "logout#";
                    //bool user1Online = false;
                    int m = strlen(user_input);
                    memcpy(send_buffer + 4, user_input, m);

                    send_buffer[0] = 0x4A; // These are constants you defined.
                    send_buffer[1] = 0x56;
                    send_buffer[2] = 0x04;//acknowledgement of posting
                    send_buffer[3] = m;
                }
                else
                {
                   

                }


            }// end recv_buffer == 3
            else if (recv_buffer[2] == 0x07)
            {
                //check the times
                user3start = time(NULL);
                if(user3start >= user3endwait)
                {
                    memset(recent_msg, 0, sizeof(recent_msg));
                    memcpy(recent_msg, recv_buffer + 4, recv_buffer[3]);
                    char user_input[1024] = "logout#";
                    //bool user1Online = false;
                    int m = strlen(user_input);
                    memcpy(send_buffer + 4, user_input, m);

                    send_buffer[0] = 0x4A; // These are constants you defined.
                    send_buffer[1] = 0x56;
                    send_buffer[2] = 0x04;//acknowledgement of posting
                    send_buffer[3] = m;
                }
                else
                {
                    //update the times
                    //start the user timer
                    user3start = time(NULL);

                    //set the end time
                    user3endwait = user3start + seconds;
                    memset(recent_msg, 0, sizeof(recent_msg));
                    memcpy(recent_msg, recv_buffer + 4, recv_buffer[3]);

                    if(strncmp(recent_msg, "user2",5) == 0)
                    {
                        if(strncmp(user3Subscription, "user1", 5)== 0)
                        {
                            strcpy(user3Subscription, "user12");
                            char user_input[1024] = "subscribe_ack#successful\n";
                            int m = strlen(user_input);
                            memcpy(send_buffer + 5, user_input, m);

                            send_buffer[0] = 0x4A; // These are constants you defined.
                            send_buffer[1] = 0x56;
                            send_buffer[2] = 0x06;//acknowledgement of posting
                            send_buffer[3] = m;
                        }
                        else if(strncmp(user3Subscription, "none", 4) == 0)
                        {
                            strcpy(user3Subscription, "user2");
                            char user_input[1024] = "subscribe_ack#successful\n";
                            int m = strlen(user_input);
                            memcpy(send_buffer + 5, user_input, m);

                            send_buffer[0] = 0x4A; // These are constants you defined.
                            send_buffer[1] = 0x56;
                            send_buffer[2] = 0x06;//acknowledgement of posting
                            send_buffer[3] = m;
                        }
                        else if(strncmp(user3Subscription, "user2", 5) == 0)
                        {
                            strcpy(user3Subscription, "user2");
                            char user_input[1024] = "subscribe_ack#failed\n";
                            int m = strlen(user_input);
                            memcpy(send_buffer + 5, user_input, m);

                            send_buffer[0] = 0x4A; // These are constants you defined.
                            send_buffer[1] = 0x56;
                            send_buffer[2] = 0x06;//acknowledgement of posting
                            send_buffer[3] = m;
                        }

                    } //end of if == user2
                    else if(strncmp(recent_msg, "user1",5) == 0)
                    {
                        
                    }//end if == user3
                        //}
                    else
                    {
                        memset(recent_msg, 0, sizeof(recent_msg));
                        memcpy(recent_msg, recv_buffer + 4, recv_buffer[3]);
                        char user_input[1024] = "session reset by server\n";
                        //bool user1Online = false;
                        int m = strlen(user_input);
                        memcpy(send_buffer + 4, user_input, m);

                        send_buffer[0] = 0x4A; // These are constants you defined.
                        send_buffer[1] = 0x56;
                        send_buffer[2] = 0x08;//acknowledgement of posting
                        send_buffer[3] = m;

                    }
                } //in time end else

            }//end else if == 7
            else if (recv_buffer[2] == 0x09)
            {
                //check the times
                user3start = time(NULL);
                if(user3start >= user3endwait)
                {
                    memset(recent_msg, 0, sizeof(recent_msg));
                    memcpy(recent_msg, recv_buffer + 4, recv_buffer[3]);
                    char user_input[1024] = "logout#";
                    //bool user1Online = false;
                    int m = strlen(user_input);
                    memcpy(send_buffer + 4, user_input, m);

                    send_buffer[0] = 0x4A; // These are constants you defined.
                    send_buffer[1] = 0x56;
                    send_buffer[2] = 0x04;//acknowledgement of posting
                    send_buffer[3] = m;
                }
                
            else
            {
                continue;
            }

        }
        else
        {
            continue;
        }
        // the echoed message, using "cliaddr" as the destination address.
        ret = sendto(sockfd, send_buffer, strlen(send_buffer), 0, (struct sockaddr *) &cliaddr, sizeof(cliaddr));

        if (ret <= 0) {
            printf("sendto() error: %s.\n", strerror(errno));
            return -1;
        }
    }

    close(sockfd);
    return 0;
}

