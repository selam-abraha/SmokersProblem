#include <stdio.h>
#include <sys/types.h>
#include <semaphore.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

sem_t table, agent, smoker[3]; 
int	smokeCount=0;
int randNum=0;
int cont=1;

void* agentThdFunc(){
    int smoke0=0, smoke1=0, smoke2=0;
    //this while loop will run infinite number of times 
    while(1){
        sem_wait(&table);
        //this while loop will run until all smokers die
        while(1){
            randNum= rand()%3;
            //make sure that the smoker for random number isn't dead already
            if(randNum==0 && smoke0<smokeCount){
                printf("agent produced tobacco and paper\n");
                smoke0++;
                break;
            }
            if(randNum==1 && smoke1<smokeCount){
                printf("agent produced matches and tobacco\n");
                smoke1++;
                break;
            }
            if(randNum==2 && smoke2<smokeCount){
                printf("agent produced matches and paper\n");
                smoke2++;
                break;
            }
            //this means all smokers are dead so terminate 
            if (smoke0>=smokeCount && smoke1>=smokeCount && smoke2>=smokeCount){
                cont=0;
                exit(0);
            }
        }
        sem_post(&table);
        sem_post(&smoker[randNum]);
        sem_wait(&agent);  
    }
    return NULL;   
}

void* smokersThdFunc(void* threadNum){
    int smoke0=0, smoke1=0, smoke2=0;
    int j= (intptr_t) threadNum;
    printf("Smoker %d starts..\n",j);
    while(cont){
        sem_wait(&smoker[j]);
        sem_wait(&table);
        usleep(rand()%1500000);

        //it is smoker 0's turn to smoke
        if(randNum==0){
            printf("\033[0;31mSmoker %d completed smoking\033[0m\n", 0);
            smoke0++;
            if(smoke0>=smokeCount){
                printf("Smoker 0 dies of cancer.\n");
            } 
        }

        //it is smoker 1's turn to smoke 
        if(randNum==1){
            printf("\033[0;32mSmoker %d completed smoking\033[0m\n", 1);
            smoke1++;
            if(smoke1>=smokeCount){
                printf("Smoker 1 dies of cancer.\n");
            }
        }

        //it is smoker 2's turn to smoke
        if(randNum==2){
            printf("\033[0;34mSmoker %d completed smoking\033[0m\n", 2); 
            smoke2++;
            if(smoke2>=smokeCount){
                printf("Smoker 2 dies of cancer.\n");
            }
        }
        //making sure to continue only if we have threads that are alive
        if (cont != 0) {
            sem_post(&table);
            sem_post(&agent);
        }
    }
    return NULL;
}

int main(int argc, char* argv[]){
    if(argc!=3){
        printf("Incorrect number of command line arguments.\nProgram terminating!\n");
        exit(1);
    }

    smokeCount= atoi(argv[2]);
    if(smokeCount<3 || smokeCount>10){
        printf("Incorrect somoke count.\nProgram terminating!\n");
        exit(1);
    }

    //initalizing semaphores
    sem_init (&table, 0, 1);
    sem_init (&agent, 0, 0);
    for(int i=0; i<3; i++){
        sem_init(&smoker[i],0,0);
    }

    //declaring and creating threads
    pthread_t agent1, smokerTobacco, smokerPaper, smokerMatch;
    pthread_create(&agent1, NULL, &agentThdFunc, NULL);
    pthread_create(&smokerTobacco, NULL, &smokersThdFunc, (void*)(intptr_t)0);
    pthread_create(&smokerPaper, NULL, &smokersThdFunc, (void*)(intptr_t)1);
    pthread_create(&smokerMatch, NULL, &smokersThdFunc, (void*)(intptr_t)2);

    //joining threads
    pthread_join (agent1, NULL);
    pthread_join (smokerTobacco, NULL);
    pthread_join (smokerPaper, NULL);
    pthread_join (smokerMatch, NULL);

    //destroying semaphores 
    sem_destroy(&agent);
    sem_destroy(&table);
    for(int i=0; i<3; i++){
        sem_destroy(&smoker[i]);
    }
    exit(0);
}
