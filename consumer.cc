// Consumer

# include "helper.h"

short unsigned int ITEM(0), SPACE(1), MUTEX(2);

int main (int argc, char *argv[])
{
  int consumer_id;
  int semid, shmid; //IDs for the semaphores and shared memory
  QUEUE *shmem_ptr; //Pointer to the shared memory
  time_t consumer_start = time(NULL); // track time at beginning
  int elapsed_time(0);//Tracks elapsed time from beginning of consumer running

  if(argc != 2) {
    fprintf(stderr,"Error: consumer(s) require(s) 1 command line arguments\n");
    return 1;
  }

  // Set consumer_id to number in command line argument:
  consumer_id = check_arg(argv[1]);
  
  if(consumer_id == -1) {
    fprintf(stderr, "Error: argument provided for consumer(s) non-integer\n");
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
    fprintf(stderr, "Error attaching semaphore set ('semget')\n");
    return 1;
  }

  // Get the size of the circular queue in shared memory:
  int queue_size = shmem_ptr->size;

  // Take a job from circular queue and sleeps for specified duration.
  // if there are no jobs in the queue, block for up to 10 seconds
  while(sem_timewait(semid, ITEM, 10) != -1) { // down(ITEM).Block for max 10s
      sem_wait(semid, MUTEX); // down(MUTEX) - block if necessary

      //ENTER CRITICAL SECTION:
      //Get current job details:
      int queue_front = shmem_ptr -> front; // get current circular queue front
      int job_id = shmem_ptr -> job[queue_front].id;//get job ID from job array
      int job_duration = shmem_ptr -> job[queue_front].duration; //job duration
 
      //Increment front of the queue:
      (shmem_ptr->front)=(queue_front+1) % queue_size;
      elapsed_time = (time(NULL) - consumer_start); // record time
      printf("Consumer(%i) time%3i: Job id %i executing sleep duration %i\n", 
	     consumer_id, elapsed_time, job_id, job_duration); // print status

      //EXIT CRITICAL SECTION:
      sem_signal(semid, MUTEX); // up(MUTEX) - exit critical section
      sem_signal(semid, SPACE); // up(SPACE) - increment space semaphore

      //Process job - sleep for duration specified:
      sleep(job_duration); 

      elapsed_time = (time(NULL) - consumer_start); // record time
      printf("Consumer(%i) time%3i: Job id %i completed\n", consumer_id, 
	     elapsed_time, job_id); // output status: completion of item
    }

  elapsed_time = (time(NULL) - consumer_start);
  printf("Consumer(%i) time%3i: No Jobs left\n", consumer_id, elapsed_time);
  // when no more jobs to consume, output status

  // Code to check if there are any processess attached to the shared memory
  // before deleting it.
  int error(0);
  int attached_processes = shm_check_attached(shmid); // # processes attached
  if(attached_processes == 1) {  //When this consumer is only process attached
    printf("Consumer(%i) time%3i: Closing shared memory and semaphores\n",
	   consumer_id, elapsed_time);

    //Detach from shared memory
    error = shm_detach(shmem_ptr);
    if(error == -1) {
      fprintf(stderr, "Error detaching from shared memory ('shmdt')\n");
      return 1;
    }
    
    error = shm_close(shmid);
    if(error == -1) {
      fprintf(stderr, "Error closing shared memory ('shmctl')\n");
      return 1;
    }

    error = sem_close(semid);
    if(error == -1) {
      fprintf(stderr, "Error closing semaphores ('semctl')\n");
      return 1;
    }

  } else {
    error = shm_detach(shmem_ptr); // detach from shared memory
    if(error == -1) {
      fprintf(stderr, "Error detaching from shared memory ('shmdt')\n");
      return 1;
    }

    sleep(20); // wait for other consumers to complete jobs
  }

  return 0;
}
