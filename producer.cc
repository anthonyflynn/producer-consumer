// Producer

# include "helper.h"

int main (int argc, char *argv[])
{
  int producer_id, producer_jobs;
  int semid, shmid;
  QUEUE *shmem_ptr;
  time_t producer_start = time(NULL);
  int time_spent;

  producer_id = check_arg(argv[1]);
  producer_jobs = check_arg(argv[2]);
  
  shmid = shm_create(SHM_KEY, SHM_SIZE);
  shmem_ptr = shm_attach(shmid);
  semid = sem_attach(SEM_KEY);
  printf("\n");


  sem_wait(semid, 1); // down(space)
  sem_wait(semid, 2); // down(mutex)
  int job_id = (shmem_ptr -> end) + 1;
  int job_duration = 6; // RANDOM NUMBER
  shmem_ptr -> job[shmem_ptr -> end].id = job_id; 
  shmem_ptr -> job[shmem_ptr -> end].duration = job_duration;
  (shmem_ptr -> end) = job_id % 5;
  time_spent = (time(NULL) - producer_start);
  printf("Producer(%i) time %i: Job id %i duration %i\n", producer_id, time_spent, job_id, job_duration);
  producer_jobs--;
  sem_signal(semid, 2); // up(mutex)
  sem_signal(semid, 0); // up(item)
  sleep(2);

  sem_wait(semid, 1); // down(space)
  sem_wait(semid, 2); // down(mutex)
  job_id = (shmem_ptr -> end) + 1;
  job_duration = 6; // RANDOM NUMBER
  shmem_ptr -> job[shmem_ptr -> end].id = job_id; 
  shmem_ptr -> job[shmem_ptr -> end].duration = job_duration;
  (shmem_ptr -> end) = job_id % 5;
  time_spent = (time(NULL) - producer_start);
  printf("Producer(%i) time %i: Job id %i duration %i\n", producer_id, time_spent, job_id, job_duration);
  producer_jobs--;
  sem_signal(semid, 2); // up(mutex)
  sem_signal(semid, 0); // up(item)
  sleep(4);

  sem_wait(semid, 1); // down(space)
  sem_wait(semid, 2); // down(mutex)
  job_id = (shmem_ptr -> end) + 1;
  job_duration = 7; // RANDOM NUMBER
  shmem_ptr -> job[shmem_ptr -> end].id = job_id; 
  shmem_ptr -> job[shmem_ptr -> end].duration = job_duration;
  (shmem_ptr -> end) = job_id % 5;
  time_spent = (time(NULL) - producer_start);
  printf("Producer(%i) time %i: Job id %i duration %i\n", producer_id, time_spent, job_id, job_duration);
  producer_jobs--;
  sem_signal(semid, 2); // up(mutex)
  sem_signal(semid, 0); // up(item)
  sleep(2);
  
  printf("Producer(%i) time %i: No more jobs to generate\n", producer_id, time_spent);




  /*
  while(job_count < producer_jobs) {
    JOBTYPE* job = new JOBTYPE;
    (job -> id) = producer_jobs; //ERROR - NEEDS TO BE FIXED
    (job -> duration) = 4;
    job_count++;
  }
  */

  //QUIT WHEN THERE ARE NO MORE JOBS LEFT TO PRODUCE
  shm_detach(shmem_ptr);
  sleep(30);
  



  return 0;
}
