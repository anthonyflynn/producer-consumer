// Consumer

# include "helper.h"

short unsigned int ITEM(0), SPACE(1), MUTEX(2);

int main (int argc, char *argv[])
{
  int consumer_id;
  int semid, shmid;
  QUEUE *shmem_ptr;
  time_t consumer_start = time(NULL); // (d) track time at beginning
  int time_spent;

  // (a) set consumer_id to number in command line argument:
  consumer_id = check_arg(argv[1]);
  
  // (b) associate with the shared memory, if available:
  shmid = shm_create(SHM_KEY, SHM_SIZE);
  shmem_ptr = shm_attach(shmid);
  semid = sem_attach(SEM_KEY); // attach to sempahore set

  // (c) take a job from circular queue and sleep for specified duration.
  // if there are no jobs in the queue, block for up to 10 seconds
  while(sem_timewait(semid, ITEM, 10) != -1) { // down(ITEM).  Block for 10sec
      sem_wait(semid, MUTEX); // down(MUTEX) - enter critical section
      int queue_front = shmem_ptr -> front; // get front of queue position
      int job_id = shmem_ptr -> job[queue_front].id; // job ID
      int job_duration = shmem_ptr -> job[queue_front].duration; //job duration
      (shmem_ptr -> front) = (queue_front + 1) % 5; // increment queue front
      time_spent = (time(NULL) - consumer_start); // record time
      printf("Consumer(%i) time%3i: Job id %i executing sleep duration %i\n", 
	     consumer_id, time_spent, job_id, job_duration);
      sem_signal(semid, MUTEX); // up(MUTEX) - exit critical section
      sem_signal(semid, SPACE); // up(SPACE) - increment space semaphore
      sleep(job_duration); // sleep for duration specified in job
      time_spent = (time(NULL) - consumer_start);
      printf("Consumer(%i) time%3i: Job id %i completed\n", consumer_id, 
	     time_spent, job_id); // completion of item consumption
    }

  time_spent = (time(NULL) - consumer_start);
  printf("Consumer(%i) time%3i: No Jobs left\n", consumer_id, time_spent);

  int test = shm_check_attached(shmid);
  if(test == 1) {
    printf("Consumer(%i) closing shared memory and semaphores\n",consumer_id);
    shm_detach(shmem_ptr);
    shm_close(shmid);
    sem_close(semid);
  } else {
    shm_detach(shmem_ptr);
    sleep(20);
  }
  return 0;
}
