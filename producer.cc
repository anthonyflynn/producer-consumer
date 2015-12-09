// Producers

# include "helper.h"

short unsigned int ITEM(0), SPACE(1), MUTEX(2);

int main (int argc, char *argv[])
{
  int producer_id(0), producer_jobs(0);
  int semid(0), shmid(0); // IDs for the semaphores and shared memory
  QUEUE *shmem_ptr; // Pointer to the shared memory
  int elapsed_time(0); // Tracks time from the beginning (starts at time 0)
  int sleep_time(0); //Tracks 2-4s delay between jobs being added to queue
  int job_id(0); //Represents ID for a particular job
  int job_duration(0); //Represents duration for a particular job (2-7s)

  if(argc != 3) {
    fprintf(stderr,"Error: producer(s) require(s) 2 command line arguments\n");
    return 1;
  }

  producer_id = check_arg(argv[1]); // gets producer ID from command line
  producer_jobs = check_arg(argv[2]); // gets number of jobs from command line

  if((producer_id == -1) || (producer_jobs == -1)) {
    fprintf(stderr, "Error: arguments provided for producer(s) non-integer\n");
    return 1;
  }
  
  //Attaches to the existing shared memory and returns a pointer to it:
  shmid = shm_create(SHM_KEY, SHM_SIZE);
  if(shmid == -1) {
    fprintf(stderr, "Error creating shared memory ('shmget')\n");
    return 1;
  }

  shmem_ptr = shm_attach(shmid);
  if(shmem_ptr == (QUEUE*) -1) {
    fprintf(stderr, "Error attaching to shared memory ('shmat')\n");
    return 1;
  }
 
  // Attaches to the semaphore set with key SEM_KEY
  semid = sem_attach(SEM_KEY);
  if(semid == -1) {
    fprintf(stderr, "Error attaching to semaphore set ('semget')\n");
    return 1;
  }
  
  // Get the size of the circular queue in shared memory:
  int queue_size = shmem_ptr -> size;

  /*
    job_time[0] = 0;
    job_duration[0] = 6;
    job_time[1] = 2;
    job_duration[1] = 6;
    job_time[2] = 6;
    job_duration[2] = 7;
    job_time[3] = 7;
    job_duration[3] = 3;
    job_time[4] = 11;
    job_duration[4] = 2;
  */
  
  for(int i = 0; i < producer_jobs; i++) {
    if(i > 0) { // first job created at time 0; thereafter 2-4s delay per job
      sleep_time = ((rand() % 3) + 2); // random number from 2-4
      sleep(sleep_time);
    }

    elapsed_time += sleep_time; // excludes time spent blocking (per spec)
    job_duration = (rand() % 6) + 2; // random number from 2-7 

    sem_wait(semid, SPACE); //down(SPACE) - check room in list, otherwise block
    sem_wait(semid, MUTEX); //down(MUTEX) - block if necessary
    
    //ENTER CRITICAL SECTION:
    int queue_end = shmem_ptr -> end;//get current circular queue end location
    job_id = queue_end + 1; //job_id = 1 + circular queue end location

    //Update shared queue with produced job detalis
    shmem_ptr -> job[queue_end].id = job_id; //assign job id to job array
    shmem_ptr -> job[queue_end].duration = job_duration; //assign job duration
    shmem_ptr -> end = job_id % queue_size; // increment value of end of queue

    //Print the status of the job:
    printf("Producer(%i) time%3i: Job id %i duration %i\n", producer_id,
	   elapsed_time, job_id, job_duration);

    //EXIT CRITICAL SECTION
    sem_signal(semid, MUTEX); // up(MUTEX) - exit critical section
    sem_signal(semid, ITEM); // up(ITEM) - increment item count semphore
  }
 
  printf("Producer(%i) time%3i: No more jobs to generate\n", producer_id, 
	 elapsed_time); // when no more jobs to produce, output status

  int error(0);
  error = shm_detach(shmem_ptr); // detach from shared memory
  if(error == -1) {
    fprintf(stderr, "Error detaching from shared memory ('shmdt')\n");
    return 1;
  }
  
  sleep(100); // sleep to avoid System V semaphore value reset

  return 0;
}
