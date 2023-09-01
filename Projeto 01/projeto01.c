#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

#define NUM_PRODUCERS 3
#define NUM_CONSUMERS 3
#define BUFFER_SIZE 9

typedef struct {
	int queue[BUFFER_SIZE];
	int counter; // Contagem num itens no buffer
	int init; // Representa o primeiro item do buffer
	int end; // Representa o último item do buffer
	sem_t mutex;
    	sem_t full;
        sem_t empty;
		
} Buffer;

Buffer buffer;

void initBuffer (Buffer* buffer){

	(*buffer).init = 0; //indice de entrada
	(*buffer).end = 0; //indice final

}

void *produce(void *arg){

	int item;
	int id = *(int*)arg;
	while(1){
		item = rand()%100; // Produz um novo item
		
		sem_wait(&buffer.empty); // Espera espaço disponível no buffer (sinalização)
		sem_wait(&buffer.mutex); // Entra seção crítica
		
		//Código pra adicionar no buffer
		buffer.queue[buffer.end] = item;
		buffer.end = (buffer.end + 1) % BUFFER_SIZE;
		
		printf("Produtor %d produziu: %d\n", id, item);
		
		sem_post(&buffer.mutex); // Sai seção crítica
		sem_post(&buffer.full); // Sinaliza item no buffer
		
		//sleep(1); // Espera antes de produzir novamente
			
	}
}

void *consume(void *arg){
	
	int item;
	int id = *(int*)arg;
	while(1){
		
		sem_wait(&buffer.full); // Espera a sinalização (um item no buffer para consumir)
		sem_wait(&buffer.mutex); // Entra seção crítica
		
		//Código pra retirar do buffer
		item = buffer.queue[buffer.init]; //Retira o primeiro item da fila
		buffer.init = (buffer.init + 1) % BUFFER_SIZE;
		
		printf("Consumidor %d consumiu: %d\n", id, item);
		
		sem_post(&buffer.mutex); // Sai seção crítica
		sem_post(&buffer.empty); // Sinaliza espaço vazio no buffer
		
		//sleep(1); // Espera antes de consumir novamente
			
	}
}

int main(){
	
	initBuffer(&buffer);
	
	// Inicializar semaforos
	sem_init(&buffer.mutex,0,1);
	sem_init(&buffer.full,0,0);
	sem_init(&buffer.empty,0,BUFFER_SIZE);
	
	pthread_t producer[NUM_PRODUCERS], consumer[NUM_CONSUMERS];
	
	for(int i = 0; i < 3; i++){
	
		int id_prod = i;
		pthread_create(&producer[i], NULL, produce, &id_prod); // cria thread do produtor	
		
	}
	for(int i = 0; i < 3; i++){
	
		int id_cons = i;
		pthread_create(&consumer[i], NULL, consume, &id_cons); // cria thread do consumidor
	
		
	}
	
	for(int i = 0; i < 3 ; i++){
	
		pthread_join(producer[i], NULL); // espera a thread acabar a execução
		pthread_join(consumer[i], NULL);
	}
	
	sem_destroy(&buffer.mutex);
	sem_destroy(&buffer.full);
	sem_destroy(&buffer.empty);
	
	return 0;


}
