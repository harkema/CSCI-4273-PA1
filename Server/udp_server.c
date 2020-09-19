/*
 * udpserver.c - A simple UDP echo server
 * usage: udpserver <port>
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <dirent.h>

#define BUFSIZE 1024

/*
 * error - wrapper for perror
 */
void error(char *msg) {
  perror(msg);
  exit(1);
}

int main(int argc, char **argv) {
  int sockfd; /* socket */
  int portno; /* port to listen on */
  int clientlen; /* byte size of client's address */
  struct sockaddr_in serveraddr; /* server's addr */
  struct sockaddr_in clientaddr; /* client addr */
  struct hostent *hostp; /* client host info */
  char buf[BUFSIZE]; /* message buf */
  char *hostaddrp; /* dotted decimal host addr string */
  int optval; /* flag value for setsockopt */
  int n; /* message byte size */

  char* fileBase = "foo";
  char* fileNumber1 = "1";
  char* fileNumber2 = "2";
  char* fileNumber3 = "3";
  char fileName[128];

  /*
   * check command line arguments
   */
  if (argc != 2) {
    fprintf(stderr, "usage: %s <port>\n", argv[0]);
    exit(1);
  }
  portno = atoi(argv[1]);

  /*
   * socket: create the parent socket
   */
  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0)
    error("ERROR opening socket");

  /* setsockopt: Handy debugging trick that lets
   * us rerun the server immediately after we kill it;
   * otherwise we have to wait about 20 secs.
   * Eliminates "ERROR on binding: Address already in use" error.
   */
  optval = 1;
  setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR,
	     (const void *)&optval , sizeof(int));

  /*
   * build the server's Internet address
   */
  bzero((char *) &serveraddr, sizeof(serveraddr));
  serveraddr.sin_family = AF_INET;
  serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
  serveraddr.sin_port = htons((unsigned short)portno);

  /*
   * bind: associate the parent socket with a port
   */
  if (bind(sockfd, (struct sockaddr *) &serveraddr,
	   sizeof(serveraddr)) < 0)
    error("ERROR on binding");

  /*
   * main loop: wait for a datagram, then echo it
   */
  clientlen = sizeof(clientaddr);
  while (1)
  {

    /*
     * recvfrom: receive a UDP datagram from a client
     */
    bzero(buf, BUFSIZE);
    n = recvfrom(sockfd, buf, BUFSIZE, 0,
		 (struct sockaddr *) &clientaddr, &clientlen);
    if (n < 0)
      error("ERROR in recvfrom");

    /*
     * gethostbyaddr: determine who sent the datagram
     */
    hostp = gethostbyaddr((const char *)&clientaddr.sin_addr.s_addr,
			  sizeof(clientaddr.sin_addr.s_addr), AF_INET);
    if (hostp == NULL)
      error("ERROR on gethostbyaddr");
    hostaddrp = inet_ntoa(clientaddr.sin_addr);
    if (hostaddrp == NULL)
      error("ERROR on inet_ntoa\n");
    printf("server received datagram from %s (%s)\n",
	   hostp->h_name, hostaddrp);
    printf("server received %d/%d bytes: %s\n", strlen(buf), n, buf);



    //Respond to client commands

    //ls command

    if(strstr(buf, "ls") != NULL)
    {
      bzero(buf, BUFSIZE);
      struct dirent *de;  // Pointer for directory entry

      // opendir() returns a pointer of DIR type.
      DIR *dr = opendir(".");

      if (dr == NULL)  // opendir returns NULL if couldn't open directory
      {
          printf("Could not open current directory" );
          return 0;
      }

      int counter = 0;
      while ((de = readdir(dr)) != NULL)
      {
              printf("%s\n", de->d_name);
              if(counter == 0)
              {
                strcpy(buf, de->d_name);
              }
              else
              {
                strcat(buf, de->d_name);
              }
              counter+=1;

      }

      closedir(dr);
    }

    //put command
    //look for seg fault
    else if (strstr(buf, "put") != NULL)
    {
      bzero(fileName, 128);

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

      printf("Name:%s\n", fileName);
      FILE *fp = fopen(fileName, "wb");
      if(fp == NULL)
      {
         bzero(buf, BUFSIZE);
         strcpy(buf, "File could not create correctly");
      } else
      {
         int successfullyWritten = 0;
         do {
          bzero(buf, BUFSIZE);
          n = recvfrom(sockfd, buf, BUFSIZE, 0,(struct sockaddr *) &clientaddr, &clientlen);
            //Check if file is over with EOF marker
          if(buf[n - 1] == EOF)
          {
             n = n - 1;
          }
          successfullyWritten = fwrite(buf, 1, n, fp);
         }
         while (successfullyWritten == BUFSIZE);
          fclose(fp);
          bzero(buf, BUFSIZE);
          strcpy(buf, "File successfully written");
      }

    }

    //delete command
    else if(strstr(buf, "delete") != NULL)
    {
      bzero(fileName, 128);

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


      FILE *fp = fopen(fileName, "r");
      if(fp == NULL)
      {
         bzero(buf, BUFSIZE);
         strcpy(buf, "File does not exist");
      }
      else
      {
       printf("Deleting file now...\n");
       fclose(fp);
       int fileDeleted = remove(fileName);
       if(fileDeleted == 0)
       {
         strcpy(buf, "File Deleted Successfully");
       }
       else
       {
         strcpy(buf, "Error: File Not Deleted");
       }
      }

    }

    //get command
    else if(strstr(buf, "get"))
    {
      bzero(fileName, 128);

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

       FILE *fp = fopen(fileName, "rb");

       if(fp == NULL)
       {
        printf("File!\n");
        }
       else
       {
          //Send to client
          int successfullyRead = 0;
          do
          {
            bzero(buf, BUFSIZE);
            successfullyRead = fread(buf, 1, BUFSIZE, fp);
            sendto(sockfd, buf, successfullyRead, 0, (struct sockaddr *) &clientaddr, clientlen);
          } while(successfullyRead == BUFSIZE);
          fclose(fp);
          char *str = "File successfully read";
          strcpy(buf, str);
        }

    }

    else
    {
      bzero(buf, BUFSIZE);
      char *str = "Uknown command.\n";
      strcpy(buf, str);
    }
    /*
     * sendto: echo the input back to the client
     */
    n = sendto(sockfd, buf, strlen(buf), 0, &clientaddr, clientlen);
    printf("N:%d\n", n);
    if (n < 0)
      error("ERROR in sendto");
  }
  return 0;
}
