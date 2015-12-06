// Producers

# include "helper.h"

short unsigned int ITEM(0), SPACE(1), MUTEX(2);

int main (int argc, char *argv[])
{
  int producer_id, producer_jobs;
  int semid, shmid;
  QUEUE *shmem_ptr;
  time_t producer_start = time(NULL);
  //int time_spent;

  producer_id = check_arg(argv[1]);
  producer_jobs = check_arg(argv[2]);
  
  shmid = shm_create(SHM_KEY, SHM_SIZE);
  shmem_ptr = shm_attach(shmid);
  semid = sem_attach(SEM_KEY);
  //printf("\n");

  int *job_t = new int[producer_jobs];
  int *job_d = new int[producer_jobs];
  job_t[0] = 0;
  job_d[0] = 6;
  job_t[1] = 2;
  job_d[1] = 6;
  job_t[2] = 6;
  job_d[2] = 7;
  job_t[3] = 7;
  job_d[3] = 3;
  job_t[4] = 11;
  job_d[4] = 2;

  for(int i = 0; i < producer_jobs; i++) {
    while((time(NULL) - producer_start) < job_t[i]) {
    } // Loop if elapsed time < job start time
    sem_wait(semid, SPACE); // down(SPACE) - check room in list
    sem_wait(semid, MUTEX); // down(MUTEX) - enter critical section
    int queue_end = shmem_ptr -> end; // current end of the queue
    int job_id = queue_end + 1; //job_id = 1 + queue location
    int job_duration = job_d[i];
    shmem_ptr -> job[queue_end].id = job_id; //assign job id
    shmem_ptr -> job[queue_end].duration = job_duration; //assign job duration
    shmem_ptr -> end = job_id % 5; // increment end of queue
    printf("Producer(%i) time%3i: Job id %i duration %i\n", producer_id, 
	   job_t[i], job_id, job_duration);
    //producer_jobs--;
    sem_signal(semid, MUTEX); // up(MUTEX) - exit critical region
    sem_signal(semid, ITEM); // up(ITEM) - increment item count semphore
  }

  printf("Producer(%i) time%3i: No more jobs to generate\n", producer_id, 
	 job_t[producer_jobs - 1]); // when not more jobs to produce
  
  shm_detach(shmem_ptr); // detach from shared memory

  sleep(60); // sleep to avoid System V semaphore value reset

  return 0;
}
