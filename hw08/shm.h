#include <pthread.h>
#include <semaphore.h>

#define	MY_ID		60
#define	SHM_KEY		(0x9000 + MY_ID)
#define	SHM_SIZE	1024
#define	SHM_MODE	(SHM_R | SHM_W | IPC_CREAT)

#define EMPTY_SEM_KEY	(0x5000 + MY_ID)
#define FULL_SEM_KEY	(0x6000 + MY_ID)
#define MUTEX_SEM_KEY	(0x7000 + MY_ID)

typedef struct {
	char data[256];
	int ready;
	pthread_mutex_t mutex;
	pthread_cond_t cond;
	sem_t EmptySem;
	sem_t FullSem;
} SharedResource;

extern SharedResource shared;