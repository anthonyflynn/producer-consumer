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
      sem_wait(semid, MUTEX); // Enter critical section
      int queue_front = shmem_ptr -> front;
      int job_id = shmem_ptr -> job[queue_front].id; // job ID
      int job_duration = shmem_ptr -> job[queue_front].duration; //job duration
      (shmem_ptr -> front) = (queue_front + 1) % 5;
      time_spent = (time(NULL) - consumer_start);
      printf("Consumer(%i) time%3i: Job id %i executing sleep duration %i\n", consumer_id, time_spent, job_id, job_duration);
      sem_signal(semid, MUTEX); // up(MUTEX)
      sem_signal(semid, SPACE); // up(SPACE)
      sleep(job_duration); // sleep for duration specified in job
      time_spent = (time(NULL) - consumer_start);
      printf("Consumer(%i) time%3i: Job id %i completed\n", consumer_id, time_spent, job_id);
    }

  time_spent = (time(NULL) - consumer_start);
  printf("Consumer(%i) time%3i: No Jobs left\n", consumer_id, time_spent);
  sleep(11);

  shm_detach(shmem_ptr);
  shm_close(shmid);
  sem_close(semid);


  return 0;
}
