#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include<fcntl.h>
#define MAX_WORDS 11
#define MAX_CHARS 1024
#define Max_WORD_COMMAND 5



void splitTextIntoWords(char* massage, char** words, int* wordCount,int flag);
void executeCommand( char** words,int wordCount, char * command);
int split_at_substring(char *input, char *before, char *after);
int has_ampersand_at_end(const char *str);
void signal_handler(int sigNum);


typedef struct Command {
    int number;
    char command[Max_WORD_COMMAND];
    pid_t pid;
    struct Command* next;
} Command;
typedef struct CommandList {
    Command* head;
    int nextNumber; // Add nextNumber to keep track of the next number to assign
} CommandList;

void addCommand(CommandList* list,  char* command, pid_t pid) ;
void deleteCommand(CommandList* list, pid_t pid);
void printCommands(const CommandList* list);
Command* searchCommand(const CommandList* list, pid_t pid);
int status;
CommandList myList;
int commandNum=0;
int file1Flag;
int file2Flag;
int fp1;
int fp2;
int FirstSecondCmd=-1;



int main(void) {

    int cmdCount,aliasCount,scriptCount;
    cmdCount=0;
    aliasCount=0;
    scriptCount=0;






    char before[MAX_CHARS]; //used to note the string before || or &&
    char after[MAX_CHARS];  //used to note the string after || or &&
    int wordCount=0;

    //allocating an array for the user massage
    char * massage=(char *) malloc(MAX_CHARS* sizeof(char ));
    if(massage==NULL){
        if(file1Flag==1&&FirstSecondCmd==0){
            dup2(fp1,STDERR_FILENO);
            perror("Unable to allocate memory");
            return EXIT_FAILURE;

        } else if(file2Flag==1&&FirstSecondCmd==1){
            dup2(fp2,STDERR_FILENO);
            perror("Unable to allocate memory");
            return EXIT_FAILURE;
        } else if(file1Flag==1){
            dup2(fp1,STDERR_FILENO);
            perror("Unable to allocate memory");
            return EXIT_FAILURE;
        }
        perror("Unable to allocate memory");
        return EXIT_FAILURE;
    }


    //allocating an array for the user words
    char ** words=(char **) malloc(MAX_WORDS* sizeof(char *));
    if(words==NULL){if(file1Flag==1&&FirstSecondCmd==0){
            dup2(fp1,STDERR_FILENO);
            perror("Unable to allocate memory");
            return EXIT_FAILURE;

        } else if(file2Flag==1&&FirstSecondCmd==1){
            dup2(fp2,STDERR_FILENO);
            perror("Unable to allocate memory");
            return EXIT_FAILURE;
        } else if(file1Flag==1){
            dup2(fp1,STDERR_FILENO);
            perror("Unable to allocate memory");
            return EXIT_FAILURE;
        }
        perror("Unable to allocate memory");
        return EXIT_FAILURE;
    }

    //used to split the before into separate words
    char ** theWordsBefore=(char **) malloc(Max_WORD_COMMAND* sizeof(char *));
    if(theWordsBefore==NULL){
        if(file1Flag==1&&FirstSecondCmd==0){
            dup2(fp1,STDERR_FILENO);
            perror("Unable to allocate memory");
            return EXIT_FAILURE;

        } else if(file2Flag==1&&FirstSecondCmd==1){
            dup2(fp2,STDERR_FILENO);
            perror("Unable to allocate memory");
            return EXIT_FAILURE;
        } else if(file1Flag==1){
            dup2(fp1,STDERR_FILENO);
            perror("Unable to allocate memory");
            return EXIT_FAILURE;
        }
        perror("Unable to allocate memory");
        return EXIT_FAILURE;
    }
    //used to split the after into separate words
    char ** theWordsAfter=(char **) malloc(Max_WORD_COMMAND* sizeof(char *));
    if(theWordsAfter==NULL){
        if(file1Flag==1&&FirstSecondCmd==0){
            dup2(fp1,STDERR_FILENO);
            perror("Unable to allocate memory");
            return EXIT_FAILURE;

        } else if(file2Flag==1&&FirstSecondCmd==1){
            dup2(fp2,STDERR_FILENO);
            perror("Unable to allocate memory");
            return EXIT_FAILURE;
        } else if(file1Flag==1){
            dup2(fp1,STDERR_FILENO);
            perror("Unable to allocate memory");
            return EXIT_FAILURE;
        }
        perror("Unable to allocate memory");
        return EXIT_FAILURE;
    }


    while (1) {
        file1Flag=0;
        file2Flag=0;


        for (int i = 0; i < MAX_WORDS; ++i) {
            words[i] = NULL;

        }
        for (int i = 0; i < Max_WORD_COMMAND; ++i) {

            theWordsAfter[i] = NULL;
            theWordsBefore[i] = NULL;
        }

        size_t size = MAX_CHARS;
        printf("#cmd:%d|#alias:%d|#script lines:%d>",cmdCount,aliasCount,scriptCount);

        getline(&massage, &size, stdin);

        // Check if input text exceeds the maximum allowed characters
        if (strlen(massage) > MAX_CHARS) {
            printf("error your command is too long");
             continue;
        }
        // Remove trailing newline character from get line
        massage[strcspn(massage, "\n")] = 0;
        //to fill the words array with separate words

        if(strcmp(massage,"exit_shell")==0){
            break;
        }

        char input[MAX_CHARS];
        strcpy(input, massage);  // to not affect the original string
        splitTextIntoWords(input, words, &wordCount,1); // the 1 indicates to the 11 words
        int result = split_at_substring(massage, before, after);

        int beforeCount = 0;
        int afterCount = 0;
        char command1[MAX_CHARS];
        char command2[MAX_CHARS];
        strcpy(command1,before);        // to use it to make a list command in the execute command
        strcpy(command2,after);



        splitTextIntoWords(before, theWordsBefore, &beforeCount,0);// the 0 indicates to the 5 words
        splitTextIntoWords(after, theWordsAfter, &afterCount,0);// the 0 indicates to the 5 words

        if(beforeCount>=2) {
            if (strcmp(theWordsBefore[beforeCount - 2], "2>") == 0 && theWordsBefore[beforeCount - 1] != NULL) {
                file1Flag=1;
                printf(" to file1");
                fp1= open(theWordsBefore[beforeCount-1],O_WRONLY|O_CREAT|O_TRUNC,0644);

            }
        }

        if(afterCount>=2) {
            if(strcmp(theWordsAfter[afterCount - 2], "2>") == 0 && theWordsAfter[afterCount - 1] != NULL) {
                file2Flag=1;
                printf(" to file2");
                fp2= open(theWordsAfter[afterCount-1],O_WRONLY|O_CREAT|O_TRUNC,0644);

            }
        }

        if(strcmp(massage,"jops")==0){
            printCommands(&myList);
            cmdCount++;
            continue;
        }


        if (result == 1) { //we found the || command
            FirstSecondCmd=0;
            executeCommand(theWordsBefore,beforeCount,command1);
            if (status !=0 ) {
                FirstSecondCmd=1;
                executeCommand(theWordsAfter,afterCount,command2);
                if (status == 0) {
                    cmdCount++;
                }
            }
        } else if (result == 2) { // we found the && command
            FirstSecondCmd=0;
            executeCommand(theWordsBefore,beforeCount,command1);
            if (status == 0) {
                FirstSecondCmd=1;
                executeCommand(theWordsAfter,afterCount,command2);
                if (status == 0) {
                    cmdCount++;
                }
            }
        } else { // there is no || or && so we continue
            executeCommand(words,wordCount,massage);
            if (status == 0) {
                cmdCount++;
            }
        }

        if(file1Flag==1){
            close(fp1);
        }

        if (file2Flag==1){
            close(fp2);
        }




    }


    //freeing the allocated memories
    free(massage);
    for (int i = 0; i < MAX_WORDS; ++i) {
        free(words[i]);
    }
    free(words);
    for (int i = 0; i < Max_WORD_COMMAND; ++i) {
        free(theWordsBefore[i]);
    }
    free(theWordsBefore);
  for (int i = 0; i < Max_WORD_COMMAND; ++i) {
      free(theWordsAfter[i]);
  }
  free(theWordsAfter);
    return 0;
}



// Function to split text into words
void splitTextIntoWords(char* massage, char** words, int* wordCount,int flag) {
    char* word = NULL;
    int count = 0;

    // Use strTok to split the text by space and newline characters
    word = strtok(massage, " \n");
    while (word != NULL) {
        words[count] = strdup(word);
        if (words[count] == NULL) {
            if(file1Flag==1&&FirstSecondCmd==0){
                dup2(fp1,STDERR_FILENO);
                perror("Unable to allocate memory");
                exit(EXIT_FAILURE);

            } else if(file2Flag==1&&FirstSecondCmd==1){
                dup2(fp2,STDERR_FILENO);
                perror("Unable to allocate memory");
                exit(EXIT_FAILURE);
            } else if(file1Flag==1){
                dup2(fp1,STDERR_FILENO);
                perror("Unable to allocate memory");
                exit(EXIT_FAILURE);
            }
            perror("Unable to allocate memory");
            exit(EXIT_FAILURE);
        }
        count++;
        if(flag==1) {
            if (count > MAX_WORDS) {    //11
                if(file1Flag==1&&FirstSecondCmd==0){
                    dup2(fp1,STDOUT_FILENO);
                    printf("Error: Number of words exceeds %d.\n", MAX_WORDS);

                } else if(file2Flag==1&&FirstSecondCmd==1){
                    dup2(fp1,STDOUT_FILENO);
                    printf("Error: Number of words exceeds %d.\n", MAX_WORDS);
                } else if(file1Flag==1){
                    dup2(fp1,STDOUT_FILENO);
                    printf("Error: Number of words exceeds %d.\n", MAX_WORDS);
                }
                printf("Error: Number of words exceeds %d.\n", MAX_WORDS);
                // Free allocated memory before returning
                for (int i = 0; i < count; i++) {
                    free(words[i]);
                }
                *wordCount = 0;
                return;
            }
        } else{if (count > Max_WORD_COMMAND) {  //5
                printf("Error: Number of words exceeds %d.\n", MAX_WORDS);
                // Free allocated memory before returning
                for (int i = 0; i < count; i++) {
                    free(words[i]);
                }
                *wordCount = 0;
                return;
            }}
        word = strtok(NULL, " \n");
    }

    *wordCount = count;
}


// Function to execute a command using fork and execvp
void executeCommand( char** words,int wordCount, char * command) {
    pid_t x = fork();
    if (x == 0) {
        if(has_ampersand_at_end(words[wordCount-1])==1) {
            words[wordCount - 1] = NULL;
        }
        if(execvp(words[0], words)==-1) {
            if(file1Flag==1&&FirstSecondCmd==0){
                dup2(fp1,STDERR_FILENO);
                perror("execvp failed");
                exit(EXIT_FAILURE);

            } else if(file2Flag==1&&FirstSecondCmd==1){
                dup2(fp2,STDERR_FILENO);
                perror("execvp failed");
                exit(EXIT_FAILURE);
            } else if(file1Flag==1){
                printf("we are here \n");
                dup2(fp1,STDERR_FILENO);
                perror("execvp failed");
                exit(EXIT_FAILURE);
            }
            perror("execvp failed");
            exit(EXIT_FAILURE);
        }


    } else {
        sleep(1);

        if(has_ampersand_at_end(words[wordCount-1])==1) {
            addCommand(&myList,command,x);
            Command *command1= searchCommand(&myList,x);
            printf("[%d] %d\n",command1->number,x);
            signal(SIGCHLD, signal_handler);

        } else{
            wait(&status);
        }
    }
}




// Function to split a string at the first occurrence of a substring
int split_at_substring(char *input, char *before, char *after) {
    const char *pos = strstr(input, "||");
    if(pos!=NULL){
        // Copy everything before the delimiter to 'before'
        strncpy(before, input, pos - input-1);
        before[pos - input] = '\0'; // Null-terminate 'before'

        // Copy everything after the delimiter to 'after'
        strcpy(after, pos + strlen("||")+1);

        return 1; // Delimiter found
    }  pos = strstr(input, "&&");
    if(pos!=NULL){
        // Copy everything before the delimiter to 'before'
        strncpy(before, input, pos - input-1);
        before[pos - input] = '\0'; // Null-terminate 'before'

        // Copy everything after the delimiter to 'after'
        strcpy(after, pos + strlen("&&")+1);

        return 2; // Delimiter found

    } else{
        // If the delimiter is not found, copy the entire input to before and set after to empty
        strcpy(before, input);
        after[0] = '\0';
        return 0; // Delimiter not found
    }

}

int has_ampersand_at_end(const char *str) {
    // Check if the string is not NULL and its length is greater than 0
    if (str != NULL && strlen(str) > 0) {
        // Get the length of the string
        size_t len = strlen(str);

        // Check if the last character is '&'
        if (str[len - 1] == '&'||str[len - 2] == '&') {
            return 1;
        }
    }
    return 0;
}
void signal_handler(int sigNum){
    //int status;
    pid_t x=waitpid(-1,&status,WNOHANG);
    if (x>0) {
        deleteCommand(&myList,x);
    }
}

void addCommand(CommandList* list,  char* command, pid_t pid) {
    Command* newCommand = (Command*)malloc(sizeof(Command));
    if (!newCommand) {if(file1Flag==1&&FirstSecondCmd==0){
            dup2(fp1,STDERR_FILENO);
            perror("Unable to allocate memory");
            exit(EXIT_FAILURE);

        } else if(file2Flag==1&&FirstSecondCmd==1){
            dup2(fp2,STDERR_FILENO);
            perror("Unable to allocate memory");
            exit(EXIT_FAILURE);
        } else if(file1Flag==1){
            dup2(fp1,STDERR_FILENO);
            perror("Unable to allocate memory");
            exit(EXIT_FAILURE);
        }
        perror("Unable to allocate memory");
        return;
    }


    if(myList.head==NULL){
        commandNum=0;
    }
    newCommand->number = commandNum;
    commandNum++;
    strncpy(newCommand->command, command, strlen(command)-1);
    newCommand->pid = pid;
    newCommand->next = NULL;

    if (list->head == NULL) {
        list->head = newCommand;
    } else {
        Command* current = list->head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = newCommand;
    }
}

void deleteCommand(CommandList* list, pid_t pid) {
    Command* current = list->head;
    Command* previous = NULL;

    while (current != NULL && current->pid != pid) {
        previous = current;
        current = current->next;
    }

    if (current == NULL) {if(file1Flag==1&&FirstSecondCmd==0){
            dup2(fp1,STDOUT_FILENO);
            printf("Command with PID %d not found.\n", pid);
            return;

        } else if(file2Flag==1&&FirstSecondCmd==1){
            dup2(fp1,STDOUT_FILENO);
            printf("Command with PID %d not found.\n", pid);
            return;
        } else if(file1Flag==1){
            dup2(fp1,STDOUT_FILENO);
            printf("Command with PID %d not found.\n", pid);
            return;
        }
        printf("Command with PID %d not found.\n", pid);
        return;
    }

    if (previous == NULL) {
        list->head = current->next;
    } else {
        previous->next = current->next;
    }

    free(current);
}

void printCommands(const CommandList* list) {
    Command* current = list->head;

    while (current != NULL) {

        printf("[%d]   Running               %s\n", current->number, current->command);
        current = current->next;
    }
}
Command* searchCommand(const CommandList* list, pid_t pid) {
    Command* current = list->head;
    while (current != NULL) {
        if (current->pid == pid) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}