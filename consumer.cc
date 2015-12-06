// Consumer

# include "helper.h"

int main (int argc, char *argv[])
{
  int consumer_id;
  int semid, shmid;
  QUEUE *shmem_ptr;
  time_t consumer_start = time(NULL);
  int time_spent;

  consumer_id = check_arg(argv[1]);
  
  shmid = shm_create(SHM_KEY, SHM_SIZE);
  shmem_ptr = shm_attach(shmid);
  semid = sem_attach(SEM_KEY);


  while(sem_timewait(semid, 0, 10) != -1) { // down(item)
      sem_wait(semid, 2); // down(mutex)
      JOBTYPE job = shmem_ptr -> job[shmem_ptr -> front];
      int id = (job.id);
      int duration = (job.duration);
      time_spent = (time(NULL) - consumer_start);
      (shmem_ptr -> front) = ((shmem_ptr -> front) + 1) % 5;
      printf("Consumer(%i) time %i: Job id %i executing sleep duration %i\n", consumer_id, time_spent, id, duration);
      sem_signal(semid, 2); // up(mutex)
      sem_signal(semid, 1); // up(space)
      sleep(duration);
      time_spent = (time(NULL) - consumer_start);
      printf("Consumer(%i) time %i: Job id %i completed\n", consumer_id, time_spent, id);
    }

  time_spent = (time(NULL) - consumer_start);
  printf("Consumer(%i) time %i: No Jobs left\n", consumer_id, time_spent);

  shm_detach(shmem_ptr);
  shm_close(shmid);
  sem_close(semid);


  return 0;
}
