# producer-consumer
Producer-Consumer model using semaphores and shared memory

Models mutual exclusion / blocking for access to a shared circular list of jobs.

Can be executed from the command line by running the run1.sh and run2.sh executables.

New runX.sh executable files can be created to adjust:
  1) length of circular list (argument provided for the ./start file)
  2) number of consumers (one ./consumer file for each consumer, argument = consumer id)
  3) number of producers (one ./producer file for each producer, arguments = producer id, number of jobs to produce)
