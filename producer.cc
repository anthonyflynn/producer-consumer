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
    }
    sem_wait(semid, SPACE); // down(SPACE)
    sem_wait(semid, MUTEX); // down(MUTEX)
    int queue_end = shmem_ptr -> end;
    int job_id = queue_end + 1;
    int job_duration = job_d[i]; // RANDOM NUMBER
    shmem_ptr -> job[queue_end].id = job_id; //job id
    shmem_ptr -> job[queue_end].duration = job_duration; //job duration
    shmem_ptr -> end = job_id % 5;
    printf("Producer(%i) time%3i: Job id %i duration %i\n", producer_id, job_t[i], job_id, job_duration);
    //producer_jobs--;
    sem_signal(semid, MUTEX); // up(MUTEX)
    sem_signal(semid, ITEM); // up(ITEM)
  }

    
  /*
  time_spent = (time(NULL) - producer_start); // job created
  sem_wait(semid, SPACE); // down(SPACE)
  sem_wait(semid, MUTEX); // down(MUTEX)
  int queue_end = shmem_ptr -> end;
  int job_id = queue_end + 1;
  int job_duration = 6; // RANDOM NUMBER
  shmem_ptr -> job[queue_end].id = job_id; //job id
  shmem_ptr -> job[queue_end].duration = job_duration; //job duration
  shmem_ptr -> end = job_id % 5;
  printf("Producer(%i) time%3i: Job id %i duration %i\n", producer_id, time_spent, job_id, job_duration);
  producer_jobs--;
  sem_signal(semid, MUTEX); // up(MUTEX)
  sem_signal(semid, ITEM); // up(ITEM)
  sleep(2);

  time_spent = (time(NULL) - producer_start);
  sem_wait(semid, SPACE); // down(SPACE)
  sem_wait(semid, MUTEX); // down(MUTEX)
  queue_end = shmem_ptr -> end;
  job_id = queue_end + 1;
  job_duration = 6; // RANDOM NUMBER
  shmem_ptr -> job[queue_end].id = job_id; //job id
  shmem_ptr -> job[queue_end].duration = job_duration; //job duration
  shmem_ptr -> end = job_id % 5;
  printf("Producer(%i) time%3i: Job id %i duration %i\n", producer_id, time_spent, job_id, job_duration);
  producer_jobs--;
  sem_signal(semid, MUTEX); // up(MUTEX)
  sem_signal(semid, ITEM); // up(ITEM)
  sleep(4);

  time_spent = (time(NULL) - producer_start);
  sem_wait(semid, SPACE); // down(SPACE)
  sem_wait(semid, MUTEX); // down(MUTEX)
  queue_end = shmem_ptr -> end;
  job_id = queue_end + 1;
  job_duration = 7; // RANDOM NUMBER
  shmem_ptr -> job[queue_end].id = job_id; //job id
  shmem_ptr -> job[queue_end].duration = job_duration; //job duration
  shmem_ptr -> end = job_id % 5;
  printf("Producer(%i) time%3i: Job id %i duration %i\n", producer_id, time_spent, job_id, job_duration);
  producer_jobs--;
  sem_signal(semid, MUTEX); // up(MUTEX)
  sem_signal(semid, ITEM); // up(ITEM)
  sleep(1);
  
  time_spent = (time(NULL) - producer_start);
  sem_wait(semid, SPACE); // down(SPACE)
  sem_wait(semid, MUTEX); // down(MUTEX)
  queue_end = shmem_ptr -> end;
  job_id = queue_end + 1;
  job_duration = 3; // RANDOM NUMBER
  shmem_ptr -> job[queue_end].id = job_id; //job id
  shmem_ptr -> job[queue_end].duration = job_duration; //job duration
  shmem_ptr -> end = job_id % 5;
  printf("Producer(%i) time%3i: Job id %i duration %i\n", producer_id, time_spent, job_id, job_duration);
  producer_jobs--;
  sem_signal(semid, MUTEX); // up(MUTEX)
  sem_signal(semid, ITEM); // up(ITEM)
  sleep(4);

  time_spent = (time(NULL) - producer_start);
  sem_wait(semid, SPACE); // down(SPACE)
  sem_wait(semid, MUTEX); // down(MUTEX)
  queue_end = shmem_ptr -> end;
  job_id = queue_end + 1;
  job_duration = 2; // RANDOM NUMBER
  shmem_ptr -> job[queue_end].id = job_id; //job id
  shmem_ptr -> job[queue_end].duration = job_duration; //job duration
  shmem_ptr -> end = job_id % 5;
  printf("Producer(%i) time%3i: Job id %i duration %i\n", producer_id, time_spent, job_id, job_duration);
  producer_jobs--;
  sem_signal(semid, MUTEX); // up(MUTEX)
  sem_signal(semid, ITEM); // up(ITEM)

  printf("Producer(%i) time%3i: No more jobs to generate\n", producer_id, time_spent);
  */

  printf("Producer(%i) time%3i: No more jobs to generate\n", producer_id, job_t[producer_jobs - 1]);


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
  sleep(60);
  



  return 0;
}
