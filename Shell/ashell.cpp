//Huy Nguyen and Howard
#include <unistd.h>     // getcwd()
#include <sys/types.h>  //  pid_t
#include <sys/wait.h>   // wait()
#include <vector>
#include <iostream>
#include <string.h>
#include <limits.h>      // to use PATH_MAX
#include <termios.h>
#include <ctype.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdio.h>
#include <fcntl.h>
#define MAX_LEN 1024
#define READ_END 0
#define WRITE_END 1
using namespace std;
string get_path(char *tmp)
{
	if (getcwd(tmp, PATH_MAX) != 0) {
		return tmp;
	}
}//get_path()          function that will return the current directory
void ResetCanonicalMode(int fd, struct termios *savedattributes)
{
	tcsetattr(fd, TCSANOW, savedattributes);
}//ResetCanonicalMode()
void SetNonCanonicalMode(int fd, struct termios *savedattributes)
{
	struct termios TermAttributes;
	char *name;
	// Make sure stdin is a terminal.
	if (!isatty(fd)) {
		exit(0);
	}
	// Save the terminal attributes so we can restore them later.
	tcgetattr(fd, savedattributes);
	// Set the funny terminal modes.
	tcgetattr (fd, &TermAttributes);
	TermAttributes.c_lflag &= ~(ICANON | ECHO); // Clear ICANON and ECHO.
	TermAttributes.c_cc[VMIN] = 1;
	TermAttributes.c_cc[VTIME] = 0;
	tcsetattr(fd, TCSAFLUSH, &TermAttributes);
}//SetNonCanonicalMode()
int main()
{
	int count, ct = 0, go = 0, go2 = 0, check = 0;
	char add[MAX_LEN];
	char renew[MAX_LEN];
	struct termios SavedTermAttributes;
	char tmp[PATH_MAX];
	char *p;
	int k = 0;
	int b = 0;
	int on = 0;
	char rep[] = ".../";
	string  h[10];
	string str;
	int up = 0;
	string hist_command[MAX_LEN];
	char ss[1024];
	char command[MAX_LEN];
	const char *bound = " ";    // boundary to use when splitting the input
	SetNonCanonicalMode(STDIN_FILENO, &SavedTermAttributes); //Set to Non-CanonicalMode
	while (1) {
		check = 0;
		get_path(tmp);   // getting current directory path
		if (strlen(tmp) > 16) { //if the path is longer than 16 characters
			for (p = rep; *p; p++) { //print /... if current path is more than 16 characters
				write(1, p, 1); //write out /...
			}//for()*/
			char *pth;
			vector<char*>path;
			char box[PATH_MAX];
			for (int i = 0;i < sizeof(tmp);i++)
				box[i] = tmp[i];   //store the path to box array
			pth = strtok(box, "/"); //split the element of the path by '/'
			while (pth != NULL) {
				path.push_back(pth); //push the splited elements in vector path
				pth = strtok(NULL, "/"); //token will point to whatever after '/'
			}
			char** store = new char*[path.size()+1]; //allocating memory for char arraystore
			int d = 0;
			for (int a = 0;a < path.size();a++) {
				store[a] = path[a]; //storing elements inside the vector to the store array
				d++; //updating count
			}
			store[path.size()] = NULL; //take out garbage
			write(1, store[d-1], strlen(store[d-1]));//print out the last element in the current path
			write(1, ">", 1);
		} else {
			// if current path is less than 16. Print out the whole path/
			for (p = tmp; *p; p++) { //print out the current directory path
				write(1, p, 1);
			}//for()
			write(1, ">", 1);
		}//else()
		int counter = 0;
		count = read(STDIN_FILENO, command, 1024);  //read in user input
		char ch[1024];
		char ch2;
		char ch3;
		if (*command == 0x04)
			exit(0);
		else if (*command ==  '\x1b') { //esc
			ch2 = command[1];
			if (ch2 == '\x5b') { //']'
				ch3 = command[2];
				if (ch3 == '\x41') { //"A" UP ARROW KEY
					if (go == 0)
						write(1, "\r\a", 2);
					else {
						for (int i = go - 1;i < go;i++) {
							strcpy(ss, h[i].c_str());
						}
						strcpy(command, ss);
						write(1, ss, strlen(ss));
						write(1, "       ", 7);
						write(1, "\r", 1);
						go--;
					}
					continue;
				}//if()
				else if (ch3 == '\x42') { //"B" DOWN ARROW KEY
					if (go == ct)
						write(1, "\r\a", 2);
					else {
						for (int i = go + 1;i > go;i--) {
							strcpy(ss, h[i].c_str());
						}
						write(1, ss, strlen(ss));
						go++;
						write(1, "\r", 1);
					}
					continue;
				}//else if()
				write(1, command, strlen(command));
			}//if()
			write(1, "       ", 7);
			write(1, "\r\a", 2);
		}//else if()
		else if (*command ==  0x7F) { //when backspace is hit
			write(1, "\r\a", 2); //ring the bell if no userinput
			continue;
		}//else if()
		else if (*command ==  0x0A) {
			on = 1;
			h[ct++] = string(ss);
			if (ct == 10) {
				ct = 9; //reset ct when history reach 11th element
				for (int i = 0; i < 9;i++) {
					h[i] = h[i+1]; //updating the history array
				}//for()
			}//if()
			strcpy(command, ss);
			go = go2;
			write(1, "\n", 1);
		}//else if()
		else if (*command == 0x04) {
			exit(0);
		} else {
			write(1, command, 1);
			char *add = new char[1024]; //allocating memory for add array
			strncpy(add, command, 1); //copy command to add array
			ResetCanonicalMode(STDIN_FILENO, &SavedTermAttributes); //Reset back to CanonicalMode
			count = read(STDIN_FILENO, command, 1024); //continue grabbing user input
			strcat(add, command); //concaternate the command together
			strcpy(command, add);
			str = string(ch);
			count++;
			for (int i = 0;i < str.size();i++) {
				hist_command[i] = str[i];
				check++;
			}
			go++;
			go2++;
			if (go == 10)
				go = 9;
			if (go2 == 10)
				go2 = 9;
			if (count == 1)
				return(main());   //when only enter is hit do nothing
			else if (count > 0) {
				command[count-1] = '\0'; //create the end of the line for the user input
			}//else if()
			SetNonCanonicalMode(STDIN_FILENO, &SavedTermAttributes); //Set back to Non-CanonicalMode
		}//else()
		vector<char*> args;         //creating a vector char array to store the operation
		char* operation = strtok(command, bound); //splitting the command i
		char* temp = operation; // storing the operation to the temp pointer
		while ( temp != NULL ) {
			args.push_back( temp ); // adding the operation to the vector
			temp = strtok( NULL, bound ); //The token will point to what after " "
		}//while()
		char** argv = new char*[args.size()+1];  //allocating memory
		for ( int k = 0; k < args.size(); k++ ) {
			argv[k] = args[k];               //convert to array
			check++;
		}
		check = check - 5;   //set up check for different case
		argv[args.size()] = NULL;      //take out garbage
		struct stat s;
		if (check > 2) {
			for (int i = 0;i < sizeof(argv);i++) {
				if (strcmp(argv[i], ">") == 0) { //redirection case
					int fds[2];
					int num;
					int fd;
					char c;
					pipe(fds);
					pid_t pid;
					if (fork() == 0) { //child process #1
						fd = open(argv[i+1], O_RDWR | O_CREAT, 0666);  //open or create the file that user input
						if (fd < 0) {
							char error[] = "Cannot open file.\n"; //error open the file
							write(1, error, strlen(error));
							continue;
						}//if()
						dup2(fds[READ_END], STDIN_FILENO); //duplicate stdin
						close(fds[WRITE_END]);  //close the stdout of the pipe
						while ((num = read(0, &c, 1)) > 0)
							write(fd, &c, 1);  //write data to the file
					}//if()
					else if ((pid = fork() == 0)) { //child process #2
						dup2(fds[WRITE_END], WRITE_END); //duplicate stdout
						close(fds[READ_END]); //close the stdin of the pipe
						execlp(argv[0], argv[0], argv[i-1], NULL); //execute the first command
					}//else if()
					else {
						wait(NULL);   //wait for process to die
						close(fds[READ_END]);  //close both read/write
						close(fds[WRITE_END]);
					}//else()
					for (int i = 0;i < sizeof(ss);i++) //reset ss
						ss[i] = '\0';
				}//if()
				else if (strcmp(argv[i], "<") == 0) { //redirection case
					int fds2[2];
					int num;
					int fd2;
					char f;
					pipe(fds2);
					pid_t pid2;
					if (fork() == 0) { //child process #1
						fd2 = open(argv[i-1], O_RDWR | O_CREAT, 0666);  //open or create the file that user input
						if (fd2 < 0) {
							char error[] = "Cannot open file.\n"; //error open the file
							write(1, error, strlen(error));
							continue;
						}//if()
						dup2(fds2[READ_END], STDIN_FILENO); //duplicate stdin
						close(fds2[WRITE_END]);  //close the stdout of the pipe
						while ((num = read(0, &f, 1)) > 0)
							write(fd2, &f, 1);  //write data to the file
					}//if()
					else if ((pid2 = fork() == 0)) { //child process #2
						dup2(fds2[WRITE_END], WRITE_END); //duplicate stdout
						close(fds2[READ_END]); //close the stdin of the pipe
						execlp(argv[0], argv[0], argv[i+1], NULL); //execute the first command
					}//else if()
					else {
						wait(NULL);   //wait for process to die
						close(fds2[READ_END]);  //close both read/write
						close(fds2[WRITE_END]);
					}//else()
					for (int i = 0;i < sizeof(ss);i++) //reset ss
						ss[i] = '\0';
				}//else if()
				else if (strcmp(argv[i], "|") == 0) { //piping case
					int pipefd[2];
					pipe(pipefd);
					pid_t piping, piping2;
					if ((piping = fork()) == 0) { //child process #1
						dup2(pipefd[WRITE_END], STDOUT_FILENO); //duplicate stdout
						close(pipefd[READ_END]);  //close stdin of the pipe
						close(pipefd[WRITE_END]); //close stdout of the pipe
						execlp(argv[0], argv[0], argv[i-1], NULL); //execute the first command
					}//if()
					else if ((piping2 = fork()) == 0) { //child process #2
						dup2(pipefd[READ_END], STDIN_FILENO); //duplicate stdin
						close(pipefd[WRITE_END]);  //close stdout of the pipe
						execlp(argv[i+1], argv[i+1], argv[i+2], NULL); //execute the second command
					}//else if()
					else {
						close(pipefd[WRITE_END]);
						waitpid(piping, NULL, 0);
						waitpid(piping2, NULL, 0);
					}//else()
					for (int i = 0;i < sizeof(ss);i++)  //reset ss
						ss[i] = '\0';
				}//else if()
			}//for()
		}//if()
		else {
			pid_t cmd;
			cmd = fork();
			if (cmd < 0) {
				char msg[] = "Fork failed.\n";
				write(1, msg, strlen(msg));
				exit(0);
			} else if (cmd == 0) {
				if (strcmp(operation, "exit" ) == 0) { //exit  if input is exit
					exit(0);
				}//if()
				else if ( strcmp(operation, "history") == 0 || strcmp(ss, "history") == 0) { //print out history if input is history
					if (on != 1) {
						h[ct++] = "history";  //keep track of the command
					}
					on = 0;
					int gogo = 0;
					string hh[10];
					for (int i = 0;i < ct;i++) {
						if (h[i] == "\n") {
							h[i] = "remove";
						}
					}
					for (int i = 0;i < ct;i++) {
						if (h[i] != "remove") {
							hh[gogo] = h[i];
							gogo++;
						}
					}
					ct = gogo;
					for (int i = 0;i < gogo;i++) {
						h[i] = hh[i];
					}
					for (int i = 0;i < ct;i++) { // print out the list of previous commands
						char num = (char)(((int)'0') + i); //converting i to char in order to usewrite() system call
						char *lst = new char[1024];
						write(1, &num, sizeof(num));
						write(1, ". " , 2);
						strcpy(lst, h[i].c_str());
						write(1, lst, strlen(lst));
						write(1, "\n", 1);
					}//for()
					for (int i = 0;i < sizeof(ss);i++)
						ss[i] = '\0';
				}//else if()
				else if (strcmp(operation, "pwd") == 0 || strcmp(ss, "pwd") == 0) { //print out the current directory path if type in pwd
					long size;
					char *buf;
					char *ptr;
					if (on != 1) {
						h[ct++] = "pwd";
						if (ct == 10) {
							ct = 9;
							for (int i = 0; i < 9;i++) {
								h[i] = h[i+1];
							}//for()
						}//if()
					}
					on = 0;
					size = pathconf("/", _PC_PATH_MAX); //get the size of the path
					if ((buf = (char*)malloc((size_t)size)) != NULL) //allocating memory for a pointer to store the path
						ptr = getcwd(buf, (size_t)size); //get the current path
					for (p = ptr;*p;p++) {
						write(1, p, 1); //print out the path
					}//for()
					write(1, "\n", 1);
					for (int i = 0;i < sizeof(ss);i++)
						ss[i] = '\0';
				}//else if()
				else if (strcmp(operation, "ls") == 0 || strcmp(ss, "ls") == 0) { //list out what inside a directory or the current directory
					DIR *dp;
					struct stat filestat;
					struct dirent *filename;
					if (on != 1) {
						h[ct++] = "ls";
						if (ct == 10) {
							ct = 9;
							for (int i = 0; i < 9;i++) {
								h[i] = h[i+1];
							}//for()
						}//if()
					}
					on = 0;
					if (argv[1] != NULL) {
						if (stat(argv[1], &filestat) < 0) {
							char msg[] = "Failed to open directory ";
							write(1, msg, sizeof(msg));
							write(1, "\"", 1);
							write(1, argv[1], strlen(argv[1]));
							write(1, "/\"", 2);
							write(1, "\n", 1);
							continue;
						}//if()
						dp = opendir(argv[1]);
						while (filename = readdir(dp)) {
							stat(filename->d_name, &filestat); //getting information of files
							(S_ISDIR(filestat.st_mode)) ? write(1, "d", 1) : write(1, "-", 1);
							(filestat.st_mode & S_IRUSR) ? write(1, "r", 1) : write(1, "-", 1);
							(filestat.st_mode & S_IWUSR) ? write(1, "w", 1) : write(1, "-", 1);
							(filestat.st_mode & S_IXUSR) ? write(1, "x", 1) : write(1, "-", 1);
							(filestat.st_mode & S_IRGRP) ? write(1, "r", 1) : write(1, "-", 1);
							(filestat.st_mode & S_IWGRP) ? write(1, "w", 1) : write(1, "-", 1);
							(filestat.st_mode & S_IXGRP) ? write(1, "x", 1) : write(1, "-", 1);
							(filestat.st_mode & S_IROTH) ? write(1, "r", 1) : write(1, "-", 1);
							(filestat.st_mode & S_IWOTH) ? write(1, "w", 1) : write(1, "-", 1);
							(filestat.st_mode & S_IXOTH) ? write(1, "x", 1) : write(1, "-", 1);
							char *name = filename->d_name;
							write(1, " ", 1);
							write(1, name, strlen(name));
							write(1, "\n", 1);
						}//while()
					}//if()
					else {
						// Taken care of ls if there is no user input
						if (stat(tmp, &filestat) < 0)
							return 1;
						dp = opendir(tmp);
						while (filename = readdir(dp)) {
							stat(filename->d_name, &filestat); //getting the information of files
							(S_ISDIR(filestat.st_mode)) ? write(1, "d", 1) : write(1, "-", 1);
							(filestat.st_mode & S_IRUSR) ? write(1, "r", 1) : write(1, "-", 1);
							(filestat.st_mode & S_IWUSR) ? write(1, "w", 1) : write(1, "-", 1);
							(filestat.st_mode & S_IXUSR) ? write(1, "x", 1) : write(1, "-", 1);
							(filestat.st_mode & S_IRGRP) ? write(1, "r", 1) : write(1, "-", 1);
							(filestat.st_mode & S_IWGRP) ? write(1, "w", 1) : write(1, "-", 1);
							(filestat.st_mode & S_IXGRP) ? write(1, "x", 1) : write(1, "-", 1);
							(filestat.st_mode & S_IROTH) ? write(1, "r", 1) : write(1, "-", 1);
							(filestat.st_mode & S_IWOTH) ? write(1, "w", 1) : write(1, "-", 1);
							(filestat.st_mode & S_IXOTH) ? write(1, "x", 1) : write(1, "-", 1);
							char *name = filename->d_name;
							write(1, " ", 1);
							write(1, name, strlen(name));
							write(1, "\n", 1);
						}//while()
					}//else()
					for (int i = 0; i < sizeof(ss);i++)
						ss[i] = '\0';
					closedir(dp);
				}//else if()
				else if (strcmp (operation, "cd") == 0 || strcmp(ss, "cd") == 0) { //do the ls  operation if input is ls
					if (on != 1) {
						h[ct++] = "cd";
						if (ct == 10) {
							ct = 9;
							for (int i = 0; i < 9;i++) {
								h[i] = h[i+1];
							}//for()
						}//if()
					}
					on = 0;
					if (argv[1] == NULL) {
						chdir(getenv("HOME"));   //changing to the root
					}//if()
					else if (!opendir(argv[1])) { //check if argument is a valid directory.
						char msg[] = "Error changing directory.\n";
						write(1, msg, sizeof(msg));
						continue;
					}//else if()
					else {
						chdir(argv[1]); //changing to input directory
						continue;
					}//else()
					for (int i = 0;i < sizeof(ss);i++)
						ss[i] = '\0';
				}//else if()
				else { //taking care of other commands
					if (on != 1) {
						h[ct++] = operation;
						if (ct == 10) {
							ct = 9;
							for (int i = 0; i < 9;i++) {
								h[i] = h[i+1];
							}//for()
						}//if()
					}//if()
					on = 0;
					pid_t cpid;
					cpid = fork();
					if (cpid < 0) {
						char msg[] = "Fork failed.\n";
						write(1, msg, strlen(msg));
					} else if (cpid == 0) {
						execvp(operation, argv); //execute the first command
						if ((execvp(operation, argv)) == -1 && isprint(command[0])) { //checking for error while execution
							char error[] = "Failed to execute \"";
							write(1, error, sizeof(error));
							write(1, operation, strlen(operation));
							write(1, "/\"", 2);
							write(1, "\n", 1);
							continue;
						}//if()
						for (int i = 0;i < sizeof(ss);i++)
							ss[i] = '\0';
					} else
						wait(NULL);
				}//else()
			}//else if()
			else
				wait(NULL); //wait for the child process to die.
		}//else()
	}//while()
	ResetCanonicalMode(STDIN_FILENO, &SavedTermAttributes);  //reset CanonicalMode
	return 0;
}//main()

