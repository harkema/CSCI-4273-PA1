/*
 * udpclient.c - A simple UDP client
 * usage: udpclient <host> <port>
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define BUFSIZE 1024

/*
 * error - wrapper for perror
 */
void error(char *msg) {
    perror(msg);
    exit(0);
}

int main(int argc, char **argv) {
    int sockfd, portno, n;
    int serverlen;
    struct sockaddr_in serveraddr;
    struct hostent *server;
    char *hostname;
    char buf[BUFSIZE];
    char* fileBase = "foo";
    char* fileNumber1 = "1";
    char* fileNumber2 = "2";
    char* fileNumber3 = "3";
    char fileName[128];



    /* check command line arguments */
    if (argc != 3) {
       fprintf(stderr,"usage: %s <hostname> <port>\n", argv[0]);
       exit(0);
    }

    hostname = argv[1];
    printf("Hostname: %s\n", hostname);
    portno = atoi(argv[2]);
    printf("Port Number: %d\n", portno);


    /* socket: create the socket */
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");

    /* gethostbyname: get the server's DNS entry */
    server = gethostbyname(hostname);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host as %s\n", hostname);
        exit(0);
    }

    /* build the server's Internet address */
    bzero((char *) &serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
	  (char *)&serveraddr.sin_addr.s_addr, server->h_length);
    serveraddr.sin_port = htons(portno);

    int valid = 1;

    while(valid == 1)
    {

    /* get a message from the user */
    bzero(buf, BUFSIZE);
    printf("Please enter msg: ");
    fgets(buf, BUFSIZE, stdin);

    // char buf[BUFSIZE];
    // strcpy(buf, buf);

    /* send the message to the server */
    serverlen = sizeof(serveraddr);

    //n == number of bytes sent on success
    //n = sendto(sockfd, buf, strlen(buf), 0, &serveraddr, serverlen);

    if(strstr(buf, "exit") != NULL)
    {
        printf("Goodbye...\n");
        valid = 0;
    }

    //If no message was given
    if(n < 0)
    {
      error("ERROR in sendto");
    }

    //command is put
    else if(strstr(buf, "put") != NULL)
    {
      printf("PUT command in progress...\n");

      //extract file name
      strncpy(fileName, fileBase, sizeof(fileName));


      if(strstr(buf, "1") != NULL)
      {
        strncat(fileName, fileNumber1, (sizeof(fileName) - strlen(fileName)));
      }
      else if (strstr(buf, "2") != NULL)
      {
        strncat(fileName, fileNumber2, (sizeof(fileName) - strlen(fileName)));
      }
      else
      {
        strncat(fileName, fileNumber3, (sizeof(fileName) - strlen(fileName)));
      }

      printf("Filename:%s\n", fileName);

      if(fileName != NULL)
      {
        //change last character to \0 for reading
        //fileName[strlen(fileName)-1] = '\0';
        FILE *filePtr = fopen(fileName, "rb");

        //check if file is valid
        if(filePtr == NULL)
        {
          printf("File invalid. Try again.\n");
          //valid = 0;
        }
        else
        {
          n = sendto(sockfd, buf, strlen(buf), 0, &serveraddr, serverlen);


          if (n < 0)
          {
	          error("ERROR in sendto");
	        }

          int readSuccess = 0;
                //loop through file information
              do
              {
                //starting at ptr to buf, replace w/ 0s to erase
               bzero(buf, BUFSIZE);
               readSuccess = fread(buf, 1, BUFSIZE, filePtr);
               sendto(sockfd, buf, readSuccess, 0, &serveraddr, serverlen);
              }
              while(readSuccess == BUFSIZE);
               fclose(filePtr);
        }
      }

      else
      {
        printf("No File Provided.\n");
	      bzero(buf, BUFSIZE);
	      strcpy(buf, "No File Prodivded");
	      sendto(sockfd, buf, BUFSIZE, 0, &serveraddr, serverlen);
      }

      n = recvfrom(sockfd, buf, BUFSIZE, 0, (struct sockaddr *) &serveraddr, &serverlen);
      printf("Echo from server: %s\n", buf);
    }

    //command is get
    else if(strstr(buf, "get") != NULL)
    {
      printf("GET command in progress...\n");

      n = sendto(sockfd, buf, strlen(buf), 0, &serveraddr, serverlen);

      //extract file name
      strncpy(fileName, fileBase, sizeof(fileName));


      if(strstr(buf, "1") != NULL)
      {
        strncat(fileName, fileNumber1, (sizeof(fileName) - strlen(fileName)));
      }
      else if (strstr(buf, "2") != NULL)
      {
        strncat(fileName, fileNumber2, (sizeof(fileName) - strlen(fileName)));
      }
      else
      {
        strncat(fileName, fileNumber3, (sizeof(fileName) - strlen(fileName)));
      }



      if(fileName != NULL)
      {
        //change last character to \0 for reading
        //fileName[strlen(fileName)-1] = '\0';
        FILE *filePtr = fopen(fileName, "wb");

        //check if file is valid
        if(filePtr == NULL)
        {
          printf("File invalid. Try again.\n");
          valid = 0;
        }
        else
        {

          n = sendto(sockfd, buf, strlen(buf), 0, &serveraddr, serverlen);

          if (n < 0)
          {
	          error("ERROR in sendto");
	        }

          int writeSuccess = 0;
                //loop through file information
              do
              {
                //starting at ptr to buf, replace w/ 0s to erase
               bzero(buf, BUFSIZE);
               n = recvfrom(sockfd, buf, BUFSIZE, 0, (struct sockaddr *) &serveraddr, &serverlen);

               //Indicate when the file has been read fully
               if(buf[n - 1] == EOF)
               {
		          	    n = n - 1;
		           }

               writeSuccess = fwrite(buf, 1, n, filePtr);

              }
              while(writeSuccess == BUFSIZE);
               fclose(filePtr);
               bzero(buf, BUFSIZE);
               printf("Write successful\n");
        }

      }

      else
      {
        printf("No File Provided.\n");
	      bzero(buf, BUFSIZE);
	      strcpy(buf, "No File Prodivded");
	      sendto(sockfd, buf, BUFSIZE, 0, &serveraddr, serverlen);
      }


    }

    //command is ls or delete - just send the message
    else if((strstr(buf, "ls") !=NULL) || strstr(buf, "delete") != NULL)
    {
          printf("Command in progress...\n");
          n = sendto(sockfd, buf, strlen(buf), 0, &serveraddr, serverlen);
          if (n < 0)
          {
            error("ERROR in sendto");
          }
          bzero(buf, BUFSIZE);

          n = recvfrom(sockfd, buf, BUFSIZE, 0, (struct sockaddr *) &serveraddr, &serverlen);
          printf("Echo from server: %s\n", buf);
    }

    // else
    // {
    //   n = sendto(sockfd, buf, strlen(buf), 0, &serveraddr, serverlen);
    // }

    // bzero(buf, BUFSIZE);
    // /* print the server's reply */
    // if(valid == 1)
    // {
    //   n = recvfrom(sockfd, buf, strlen(buf), 0, &serveraddr, &serverlen);
    //
    //   if (n < 0)
    //   {
    //     error("ERROR in recvfrom");
    //   }
    //   printf("Echo from server: %s\n", buf);
    // }
  }
  return 0;
}
