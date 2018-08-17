#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <libgrimoire/system/thread.h>
#include <libgrimoire/grid/peer.h>
#include <libgrimoire/grid/acceptor.h>

void * chomama(void * param)
{
	printf("%s(%d)\n", __func__, __LINE__);
	usleep(100000);
	return NULL;
}

void * chomamama(void * param)
{
	printf("%s(%d)\n", __func__, __LINE__);
	usleep(100000);
	return NULL;
}

int main(int argc, char * argv[])
{
	printf("Hello! Happy world!\n");
	thread_t * test_thread;
	peer_t * peer;
	acceptor_t * acceptor;

	list_t * list = create_list(NULL, NULL, NULL);
	task_t * task1 = create_task("chomama", chomama, NULL, NULL);
	task_t * task2 = create_task("chomamama", chomamama, NULL, NULL);
	list->enqueue_data(list, task1);
	list->enqueue_data(list, task2);
	test_thread = create_thread(list);

	peer = create_peer(AF_UNIX);
	acceptor = create_acceptor(AF_UNIX);

	peer->destroy(peer);
	acceptor->destroy(acceptor);

	test_thread->execute_once(test_thread);
//	test_thread->run(test_thread);
	sleep(1);

	test_thread->destroy(test_thread);

	sleep(10);

	return 0;
}
