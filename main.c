
#include <stdio.h>	
#include <stdlib.h>			
#include <pthread.h>	
#include <semaphore.h>
#include <unistd.h>	
#include <time.h>							
#define HOLDTRUE 10

pthread_t *Students;	 //"begin by creating n students. Each will run as a separate thread"	
pthread_t TA; //"The TA will run as a separate thread as well."	
int ChairCount, NumberOfStudents, i,  IndexC = 0;
sem_t Sleeping, StudentSem, ChairsSem[3];
pthread_mutex_t ChairAccess;
void *Student(void *num), *TeacherAssistant(), printChairInfo();


int main(int argc, char* argv[])
{
  srand(time(NULL));

	sem_init(&Sleeping, 0, 0); //binary sem of if TA is sleeping
	sem_init(&StudentSem, 0, 0);
  sem_init(&ChairsSem[0], 0, 0); //init chair 1
  sem_init(&ChairsSem[1], 0, 0); //init chair 2
  sem_init(&ChairsSem[2], 0, 0); //init chair 3

	pthread_mutex_init(&ChairAccess, NULL);
	
	printf("How many students: ");
  scanf("%d", &NumberOfStudents);
	//allocates mem for students
	Students = (pthread_t*) malloc(sizeof(pthread_t)*NumberOfStudents); //"begin by creating n students. Each will run as a separate thread"	

	pthread_create(&TA, NULL, TeacherAssistant, NULL);	 //creates TA
	for(i = 0; i < NumberOfStudents; i++) // "begin by creating n students. Each will run as a separate thread"	
		pthread_create(&Students[i], NULL, Student,(void*) (long)i);

	//joins student threads
	pthread_join(TA, NULL);
	for(i = 0; i < NumberOfStudents; i++)
		pthread_join(Students[i], NULL);

	//clean up mem
	free(Students); 
	return 0;
}


void *Student(void *num) 
{
	int randtime = rand() % 10 + 1; //get random int 

	while(HOLDTRUE == 10)
	{
		printf("[%ld] is doing programming assignment.\n", (long)num+1);
		sleep(randtime);//"Perhaps the best option for simulating students programming—as well as the TA providing help to a student—is to have the appropriate threads sleep for a random period of time."
		printf("[%ld] seeks help from TA\n", (long)num+1); //after random time seek help
		pthread_mutex_lock(&ChairAccess); //locks chair to prevent read write errors
		int count = ChairCount; //assigns self count to amount of chairs filled
		pthread_mutex_unlock(&ChairAccess); //makes chair access available again

		if(count < 3)
		{
			if(count == 0)		
				sem_post(&Sleeping); //"If a student arrives and notices that the TA is sleeping, the student must notify the TA using a semaphore"
			else
				printf("[%ld] found an available chair.\n", (long)num+1);
			  pthread_mutex_lock(&ChairAccess); //locks current chair access
			  int index = (IndexC + ChairCount) % 3;
			  ChairCount++;
			  printChairInfo(ChairCount);
			  pthread_mutex_unlock(&ChairAccess); //releases access to current chair
			  sem_wait(&ChairsSem[index]);// leave chair
			  printf("[%ld] is in TA Office. \n", (long)num+1); //"If the TA is available, they will obtain help"
			  sem_wait(&StudentSem);
			  printf("[%ld] left TA Office.\n",(long)num+1);
		}
		else // no chair is available
			printf("[%ld] could not find an available chair. \n", (long)num+1); //"or, if no chairs are available, will resume programming and will seek help at a later time."
	}}

void *TeacherAssistant()
{
	while(HOLDTRUE == 10)
	{
		  sem_wait(&Sleeping);//TA starts in sleeping state as there are no students
		  printf("[TA update] TA has been woken\n");

		while(&Sleeping > 0)
		{
			pthread_mutex_lock(&ChairAccess); //enter critical
      if(ChairCount == 0)  //if all chairs are empty
			{
				pthread_mutex_unlock(&ChairAccess);  
				break; //break out of this loop, return to the napping state
			}
      printf("---Student entered TA office\n");
			sem_post(&ChairsSem[IndexC]); 
			ChairCount--;
			printChairInfo(ChairCount);
			IndexC = (IndexC + 1) % 3;
			pthread_mutex_unlock(&ChairAccess); //opens chair access, exits critical
			printf("[TA update] TA is helping a student.\n");
			sleep(5); //"Perhaps the best option for simulating students programming—as well as the TA providing help to a student—is to have the appropriate threads sleep for a random period of time."
			sem_post(&StudentSem); //tell student they can leave office
      printf("[TA update] TA released student.\n");
		}}}

void printChairInfo(int amount){
  if(amount == 0){printf("[Chair Info] = [E] [E] [E]\n---%d Chairs available\n", 3-amount);}
  if(amount == 1){printf("[Chair Info] = [F] [E] [E]\n---%d Chairs available\n", 3-amount);}
  if(amount == 2){printf("[Chair Info] = [F] [F] [E]\n---%d Chairs available\n", 3-amount);}
  if(amount == 3){printf("[Chair Info] = [F] [F] [F]\n---%d Chairs available\n", 3-amount);}
}