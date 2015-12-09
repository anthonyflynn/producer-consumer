/******************************************************************
 * Program for setting up semaphores/shared memory before running
 * the Producer and Consumer 
 ******************************************************************/

# include "helper.h"

short unsigned int ITEM(0), SPACE(1), MUTEX(2);

int main (int argc, char **argv)
{
  int list_length; // size of circular queue
  int shmid, semid; // IDs for the semaphores and shared memory
  QUEUE *shmem_ptr; // Pointer to the shared memory

  if(argc != 2) {
    fprintf(stderr, "Error: start requires 1 command line argument\n");
    return 1;
  }

  list_length = check_arg(argv[1]); // get size of queue from command line

  if(list_length == -1) {
    fprintf(stderr, "Error: arguments provided for start not an integer\n");
    return 1;
  }
  else if(list_length > 500) {
    fprintf(stderr, "Error: list size too large (must be <= 500 jobs)\n");
    return 1;
  }
  else if(list_length <=0) {
    fprintf(stderr, "Error: list size must be >= 1\n");
    return 1;
  }

  // Creates shared memory segment with SHM_key or, if it already exists, 
  // attaches to the existing shared memory and returns a pointer to it:
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

  // Set parameters of the QUEUE structure in shared memory
  //shmem_ptr = new QUEUE;
  shmem_ptr -> size = list_length;
  shmem_ptr -> front = 0;
  shmem_ptr -> end = 0;

  //Create semaphore and initialise values
  semid = sem_create(SEM_KEY, 3);
  if(semid == -1) {
    fprintf(stderr, "Error creating semaphore set ('semget')\n");
    return 1;
  }

  int error(0);
  error = sem_init(semid, ITEM, 0); // ensures list not empty
  error = sem_init(semid, SPACE, list_length); // ensures list not full
  error = sem_init(semid, MUTEX, 1); // ensures mutual exclusion of access

  if(error == -1) {
    fprintf(stderr, "Error initialising semaphore set ('semctl')\n");
    return 1;
  }

  return 0;
}
