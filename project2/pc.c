#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// define TRUE and FALSE
#define TRUE 1
#define FALSE 0

// define MACRO variables
#define MAXIMUM_COOKIES 10
#define MAXIMUM_FACTORIES 5
#define MAXIMUM_KIDS 10

// global variables
int cookies = 0;

// pointer function about producer which takes void pointer arg as argument
void *producer(void *arg){
    while (TRUE){
        while (cookies > MAXIMUM_COOKIES){
            printf("enough cookies\n");
        }

        cookies++;
        printf("number of cookies: %d\n", cookies);
    }
}

void *consumer(void *arg){
    while (TRUE){
        while (cookies <= 0){
            printf("need producing cookies\n");
        }
        
        cookies--;
        printf("number of cookies: %d\n", cookies);
    }
}

int main(void){
    pthread_t cookie_factory[MAXIMUM_FACTORIES];
    pthread_t kids[MAXIMUM_KIDS];
    pthread_attr_t attribute;

    int number_of_factories;
    int number_of_kids;

    int flag = FALSE;

    printf("type the number of factories(maximum 5): ");
    scanf("%d", &number_of_factories);

    while (!((number_of_factories >= 1) && (number_of_factories <= 5))){
        printf("the typed range must be 1 to 5\n");
        printf("type the number of factories(maximum 5): ");
        scanf("%d", &number_of_factories);
    }

    printf("type the number of kids(maximum 10): ");
    scanf("%d", &number_of_kids);

    while (!((number_of_kids >= 1) && (number_of_kids <= 10))){
        printf("the typed range must be 1 to 10\n");
        printf("type the number of kids(maximum 10): ");
        scanf("%d", &number_of_kids);
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

    for (int i = 0; i < number_of_factories; i++){
        // error 2 : create error (in this case, for producer)
        if (pthread_create(&cookie_factory[i], &attribute, producer, NULL) != 0){
            printf("error on pthread_create for producer\n");
            printf("trying 10 times to create producer\n");

            // handling error : trying 10 times to create producer
            for (int j = 0; j < 10; j++){
                if (!(pthread_create(&cookie_factory[i], &attribute, producer, NULL) != 0)){
                    flag = TRUE;
                    break;
                }
            }

            // failed even though tried 10 times, exit
            if (!flag){
                printf("tried pthread_creat 10 times for producer, but failed\n");
                exit(1);
            }

            flag = FALSE;
        }
    }

    for (int i = 0; i < number_of_kids; i++){
        // error 2 : create error (in this case, for consumer)
        if (pthread_create(&kids[i], &attribute, consumer, NULL) != 0){
            printf("error on pthread_create for consumer\n");
            printf("trying 10 times to create consumer\n");

            // handling error : trying 10 times to create consumer
            for (int j = 0; j < 10; j++){
                if (!(pthread_create(&kids[i], &attribute, consumer, NULL) != 0)){
                    flag = TRUE;
                    break;
                }
            }

            // failed even though tried 10 times, exit
            if (!flag){
                printf("tried pthread_creat 10 times for consumer, but failed\n");
                exit(1);
            }

            flag = FALSE;
        }
    }

    for (int i = 0; i < number_of_factories; i++){
        // error 3 : join error (in this case, for producer)
        if (pthread_join(cookie_factory[i], NULL) < 0){
            printf("error on pthread_join for producer\n");
            printf("trying 10 times to join producer\n");

            // handling error : trying 10 times to join producer
            for (int i = 0; i < 10; i++){
                if (!(pthread_join(cookie_factory[i], NULL) < 0)){
                    flag = TRUE;
                    break;
                }
            }

            // failed even though tried 10 times, exit
            if (!flag){
                printf("tried pthread_join 10 times for producer, but failed\n");
                exit(1);
            }

            flag = FALSE;
        }
    }

    for (int i = 0; i < number_of_kids; i++){
        // error 3 : join error (in this case, for consumer)
        if (pthread_join(kids[i], NULL) < 0){
            printf("error on pthread_join for consumer\n");
            printf("trying 10 times to join consumer\n");

            // handling error : trying 10 times to join consumer
            for (int i = 0; i < 10; i++){
                if (!(pthread_join(kids[i], NULL) < 0)){
                    flag = TRUE;
                    break;
                }
            }

            // failed even though tried 10 times, exit
            if (!flag){
                printf("tried pthread_join 10 times for consumer, but failed\n");
                exit(1);
            }

            flag = FALSE;
        }
    }

    // terminate threads
    pthread_exit(NULL);

    return 0;
}
