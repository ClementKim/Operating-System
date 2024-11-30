#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

// define TRUE and False
#define TRUE 1
#define FALSE 0

// define MACRO variables
#define MAXIMUM_CODE 1000
#define MAXIMUM_PROGRAMMER 5
#define MAXIMUM_GIT 10

// global variables
int code[MAXIMUM_CODE] = {0};
int programmer_todo = 0;
int git_tracking = 0;

void *writer(void *arg){
    int line_number;

    srand((unsigned int)time(NULL));
    while (TRUE){
        while (programmer_todo > 0){
            printf("programmers are on a work\n");
        }

        line_number = rand()%MAXIMUM_CODE;

        programmer_todo++;

        if (git_tracking > 0){
            programmer_todo--;
            continue;
        }

        code[line_number] = rand()%1000;

        printf("programmer is writing %d on line number %d\n", code[line_number], line_number);

        programmer_todo--;
        printf("present status of programmer: %d\n", programmer_todo);
    }
}

void *reader(void *arg){
    int line_number;
    int check;

    srand((unsigned int)time(NULL));

    while (TRUE){
        while (git_tracking > 0){
            printf("git: need 'git add *' command\n");
        }

        line_number = rand()%MAXIMUM_CODE;

        git_tracking++;

        check = code[line_number];

        if (!check)
            printf("failed to push since nothing changed on line number %d\n", line_number);
        else
            printf("pushed since changed checked on line number %d\n", line_number);

        git_tracking--;

        printf("git is tracking %d changes\n", git_tracking);
    }
}

int main(void){
    pthread_t programmer[MAXIMUM_PROGRAMMER];
    pthread_t git_branches[MAXIMUM_GIT];
    pthread_attr_t attribute;

    int number_of_programmers;
    int number_of_git_branches;

    int flag = FALSE;

    printf("type the number of programmers(maximum 5): ");
    scanf("%d", &number_of_programmers);

    while (!((number_of_programmers >= 1) && (number_of_programmers <= 5))){
        printf("the typed range must be 0 to 5\n");
        printf("type the number of programmers(maximum 5): ");
        scanf("%d", &number_of_programmers);
    }

    printf("type the number of git_branches(maximum 10): ");
    scanf("%d", &number_of_git_branches);

    while (!((number_of_git_branches >= 1) && (number_of_git_branches <= 10))){
        printf("the typed range must be 1 to 10\n");
        printf("type the number of git_branches(maximum 10): ");
        scanf("%d", &number_of_git_branches);
    }

    // error 1 : initialize error
    if (pthread_attr_init(&attribute) < 0){
        printf("error on pthread_attr_init\n");
        printf("tring 10 times to initialize\n");

        // handling error: trying 10 times to initialize
        for (int i = 0; i < 10; i++){
            if (!(pthread_attr_init(&attribute) < 0)){
                flag = TRUE;
                break;
            }
        }

        // failed even though tried 10 times, exit
        if (!flag){
            printf("tried pthread_attr_init 10 times, but failed\n");
            exit(1);
        }

        flag = FALSE;
    }

    for (int i = 0; i < number_of_programmers; i++){
        // error 2 : create error (in this case, for writer)
        if (pthread_create(&programmer[i], &attribute, writer, NULL) != 0){
            printf("error on pthread_create for writer\n");
            printf("trying 10 times to create writer\n");

            // handling error : trying 10 times to create writer
            for (int j = 0; j < 10; j++){
                if (!(pthread_create(&programmer[i], &attribute, writer, NULL) != 0)){
                    flag = TRUE;
                    break;
                }
            }

            // failed even though tried 10 times, exit
            if (!flag){
                printf("tried pthread_creat 10 times for writer, but failed\n");
                exit(1);
            }

            flag = FALSE;
        }
    }

    for (int i = 0; i < number_of_git_branches; i++){
        // error 2 : create error (in this case, for reader)
        if (pthread_create(&git_branches[i], &attribute, reader, NULL) != 0){
            printf("error on pthread_create for reader\n");
            printf("trying 10 times to create reader\n");

            // handling error : trying 10 times to create reader
            for (int j = 0; j < 10; j++){
                if (!(pthread_create(&git_branches[i], &attribute, reader, NULL) != 0)){
                    flag = TRUE;
                    break;
                }
            }

            // failed even though tried 10 times, exit
            if (!flag){
                printf("tried pthread_creat 10 times for reader, but failed\n");
                exit(1);
            }

            flag = FALSE;
        }
    }

    for (int i = 0; i < number_of_programmers; i++){
        // error 3 : join error (in this case, for writer)
        if (pthread_join(programmer[i], NULL) < 0){
            printf("error on pthread_join for writer\n");
            printf("trying 10 times to join writer\n");

            // handling error : trying 10 times to join writer
            for (int i = 0; i < 10; i++){
                if (!(pthread_join(programmer[i], NULL) < 0)){
                    flag = TRUE;
                    break;
                }
            }

            // failed even though tried 10 times, exit
            if (!flag){
                printf("tried pthread_join 10 times for writer, but failed\n");
                exit(1);
            }

            flag = FALSE;
        }
    }

    for (int i = 0; i < number_of_git_branches; i++){
        // error 3 : join error (in this case, for reader)
        if (pthread_join(git_branches[i], NULL) < 0){
            printf("error on pthread_join for reader\n");
            printf("trying 10 times to join reader\n");

            // handling error : trying 10 times to join reader
            for (int i = 0; i < 10; i++){
                if (!(pthread_join(git_branches[i], NULL) < 0)){
                    flag = TRUE;
                    break;
                }
            }

            // failed even though tried 10 times, exit
            if (!flag){
                printf("tried pthread_join 10 times for reader, but failed\n");
                exit(1);
            }

            flag = FALSE;
        }
    }

    // terminate threads
    pthread_exit(NULL);

    return 0;
}
