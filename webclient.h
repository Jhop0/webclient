/*
 * webclient.h
 *
 *  Created on: Jan 24, 2015
 *      Author: adminuser
 */

#ifndef WEBCLIENT_H_
#define WEBCLIENT_H_

#define SOCKET_ERROR        -1
#define QUEUE_SIZE          5
#define HOST_NAME_SIZE      255

#define RECV_BUFFER_SIZE	1024
#define SEND_BUFFER_SIZE	1024
#define GETFILE_TEST		"1kb-sample-file-0.png"
#define GETFILE_STATUS_ERR	"GetFile FILE_NOT_FOUND 0 0"
#define GETFILE_STATUS_OK	"GetFile OK "
#define GETFILE_FILE_NOT_FOUND	"FILE_NOT_FOUND"

/* webclient */
int recv_file(int ,char*, unsigned long);
void error (const char*);

/* clientSocket */
int socketInit ();
int serverConnect(char*, int, int);

/* stringOps */
void removeSubstring(char*, const char*);
char* trimwhitespace(char*);
void getFileResParse(char*, char**, char**, char*);

#endif /* WEBCLIENT_H_ */
