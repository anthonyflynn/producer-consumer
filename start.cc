/******************************************************************
 * Program for setting up semaphores/shared memory before running
 * the Producer and Consumer 
 ******************************************************************/

# include "helper.h"

int main (int argc, char **argv)
{
  int list_length;
  int shmid, semid;
  QUEUE *shmem_ptr;
  list_length = check_arg(argv[1]);
  //clock_t begin_all = clock();

  //SETTING UP SHARED MEMORY:
  shmid = shm_create(SHM_KEY, SHM_SIZE);
  shmem_ptr = shm_attach(shmid);

  shmem_ptr = new QUEUE;
  shmem_ptr -> size = list_length;
  shmem_ptr -> front = 0;
  shmem_ptr -> end = 0;

  //SETTING UP SEMAPHORES: -- COULD USE SETALL HERE INSTEAD IN SEMUN
  semid = sem_create(SEM_KEY, 3);
  sem_init(semid, 0, 0); // item: ensures list not empty
  sem_init(semid, 1, list_length); // space: ensures list not full
  sem_init(semid, 2, 1); // mutex

  //sleep(5); // SET TO A MORE APPROPRIATE VALUE
  //sem_close(semid);
  //shm_detach(shmem_ptr);
  return 0;
}
