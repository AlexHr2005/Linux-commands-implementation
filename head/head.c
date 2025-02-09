#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

int converToInt(char* str){
	int result = 0;
	char curr;
	
	for(int i = 0; i < strlen(str); i++){
		curr = str[i];
		if(curr < 48 || curr > 57){
			printf("ERROR: Third command line argument must be a number\n");
			exit(4);
		}
		
		result += curr - 48;
		result *= 10;
	}
	result /= 10;
	
	return result;
}

int main(int argc, char* argv[]){
	if(!(argc > 1)) {
		printf("ERROR: unsufficient number of command line arguments!\n");
		exit(1);
	}
	
	int fd = open(argv[1], O_RDONLY);
	
	if(-1 == fd){ //in that way we are checking if there is valid file name argument
		printf("ERROR: Could not open the file\n");
		close(fd);
		exit(2);
	}
	
	int new_lines = 0;
	int lines_to_read;
	char buffer[10];
	int read_status;
	
	if(2 == argc){
		lines_to_read = 10;
	}
	else lines_to_read = converToInt(argv[2]);
	
	while(0 != (read_status = read(fd, buffer, 10))){
		if(-1 == read_status){
			printf("ERROR: Could not read from file\n");
			close(fd);
			exit(3);
		}
			
		for(int i = 0; i < read_status; i++){
			if('\n' == buffer[i]) {
				new_lines++;
				if(new_lines == lines_to_read) {
					//flushing the stdout buffer
					printf("%c", buffer[i]);
				}
			}
			
			if(new_lines < lines_to_read){
				printf("%c", buffer[i]);
			}
			
		}
	}
	
	
	close(fd);
	return 0;
}