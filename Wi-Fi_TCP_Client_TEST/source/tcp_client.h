#ifndef TCP_CLIENT_H_
#define TCP_CLIENT_H_

#define MAX_ARRAY_LENGTH (50u)

struct ThreadArgs{
	char array[MAX_ARRAY_LENGTH];
	int size;
};

void tcp_client_task(void *arg);

#endif /* TCP_CLIENT_H_ */
