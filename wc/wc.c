#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

int main(int argc, char* argv[]){
	if(!(argc > 1)) {
		printf("ERROR: unsufficient number of command line arguments!\n");
		exit(1);
	}
	
    
	int fd = open(argv[argc - 1], O_RDONLY);
	
	if(-1 == fd){
		printf("ERROR: Could not open the file\n");
        close(fd);
		exit(2);
	}
	
	int bytes = 0;
	int new_lines = 0;
	char buffer[10];
	int read_status;
	
	while(0 != (read_status = read(fd, buffer, 10))){
		if(-1 == read_status){
			printf("ERROR: Could not read from file\n");
            close(fd);
			exit(3);
		}
		bytes += read_status;
		for(int i = 0; i < read_status; i++){
			if('\n' == buffer[i]){
				new_lines++;
			}
		}
	}
    new_lines++;

	if(2 == argc){
		printf("Lines: %d\n", new_lines);
		printf("Bytes: %d\n", bytes);
	}
	
	if(3 == argc){
		if(!strcmp(argv[1], "-l")){
			printf("Lines: %d\n", new_lines);
		}
		else if(!strcmp(argv[1], "-c")){
			printf("Bytes: %d\n", bytes);
		}
		else {
			printf("ERROR: Unvalid argument is put: %s!\n", argv[1]);
            close(fd);
			exit(4);
		}
	}
	
	if(4 == argc){
		for(int i = 1; i < argc - 1; i++){
			if(!strcmp(argv[i], "-l")){
				printf("Lines: %d\n", new_lines + 1);
			}
			else if(!strcmp(argv[i], "-c")){
				printf("Bytes: %d\n", bytes);
			}
			else {
				printf("ERROR: Unvalid argument is put!: %s\n", argv[i]);
                close(fd);
				exit(4);
			}
		}
	}
	
	close(fd);
	
	return 0;
}