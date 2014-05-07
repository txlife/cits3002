#include "trustcloud.h"

int main()
{
    //printf("%d\n", SEND_FILE);
    struct sockaddr_in server;
    struct sockaddr_in dest;
    int status,socket_fd, client_fd,num;
    socklen_t size;

    //char buffer[1024];
    char *buff;
//  memset(buffer,0,sizeof(buffer));
    int yes =1;



    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0))== -1) {
        fprintf(stderr, "Socket failure!!\n");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    memset(&server, 0, sizeof(server));
    memset(&dest,0,sizeof(dest));
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = INADDR_ANY; 
    if ((bind(socket_fd, (struct sockaddr *)&server, sizeof(struct sockaddr )))== -1)    { //sizeof(struct sockaddr) 
        fprintf(stderr, "Binding Failure\n");
        exit(EXIT_FAILURE);
    }

    if ((listen(socket_fd, BACKLOG))== -1){
        fprintf(stderr, "Listening Failure\n");
        exit(EXIT_FAILURE);
    }

    while(1) {

        size = sizeof(struct sockaddr_in);

        if ((client_fd = accept(socket_fd, (struct sockaddr *)&dest, &size))==-1 ) {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        printf("Server got connection from client %s\n", inet_ntoa(dest.sin_addr));

        char header[1024];
        // read header - then do action based on header parsing
        if ((num = recv(client_fd, header, 1024,0))== -1) {
                perror("recv");
                exit(EXIT_FAILURE);
        }
        else if (num == 0) {
                printf("Connection closed\n");
                //So I can now wait for another client
                continue;
        } 
        
        // printf("%s\n", buffer);
        while(TRUE){
        // if client requests to uplaod file
        if (strcmp(header, "send") == 0) {
            char *file_name = "written.txt";
            // TODO get file_name from header
            receive_file(client_fd, file_name);
        }
        
        else if (strcmp(header, "list") == 0) {
        	char **files;
        	size_t count;
        	unsigned int i;
        	count = file_list("./", &files);
        	printf("There are %zu files in the directory,transmitting file list.\n", count);
            for (i = 0; i < count; i++) {
            	send_message(client_fd,files[i]);
            	sleep(1);
            }
            printf("File list transmitting completed.\n");
            close(client_fd);
            printf("Client connection closed.\n");
		}
        break;
        }
    } //Outer While

    close(socket_fd);
    return 0;
} //End of main
