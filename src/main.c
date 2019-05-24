#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <pthread.h>


pthread_mutex_t trava;


//variaveis globais
int contador=0;

int num_threads=0,cafe[4],  recebe=0, end=0, ocupada[4]={0, 0, 0, 0};
unsigned int numero;


int isprimo(unsigned int num){
	unsigned int i=2;
	if(num<2)
		return 0;
	if(num==2)
		return 1;
	for (; i<= num/2; i++){
		if(num %i ==0)
			return 0;
	}
	return 1;
}

void *worker(void *arg) {
	int *id=(int*)arg;
	int num, threadid=*id;

	free(id);

	//printf("W %d Sucesso gerando thread \n", threadid);

	while(!end){
		//printf("W %d Esperando trampo! \n", threadid);

		cafe[threadid]=1;

		while(cafe[threadid]);	//espera acabar o cafe e receber trabalho

		num=numero;				//le o numero
		pthread_mutex_lock(&trava);
		recebe=1;
		pthread_mutex_unlock(&trava);

		//printf("W %d Comecei a labuta num=%u contador=%d \n", threadid, num, contador);
		//sleep(2);
		if(isprimo(num)){		//se primo incrementa o contador
			pthread_mutex_lock(&trava); 
			contador++;		
			pthread_mutex_unlock(&trava); 
		}
		//printf("W %d terminei! num=%u contador=%d \n", threadid, num, contador );
	}
	//printf("W %d thread acabou\n", threadid);
	return NULL;
}


int main(int argc, char **argv) {

	pthread_t thread[4];

	int i,j=0;
	int * tid ;
	int resultado;

	//faz novas threads
	for(j=0; j<4; j++){
		tid=malloc(sizeof(int));
		(*tid)=j;
		pthread_create(&thread[j], NULL, worker, tid);
	}
	j=0;
	char c=0, s[100];
	//Le a entrada do programa ate EOL
	while(c != '\n'){


		//le numero inteiro para numero
		i=0;
		scanf("%c", &c);
		while('0'<= c && c <= '9'){
			s[i]=c;		
			i++;
			scanf("%c", &c);
		}
		s[i]='\0';
		numero=atoi(s);

		while(!cafe[j]){ 	//espera abrir uma thread	
			j++;	//numero da thread que esta sendo utilizada nesse loop
			j%=4;
		}

		cafe[j]=0;			//tira o cafe do trabalhador fazendo com que trabalhe

		
		while(!recebe);			//espera a thread receber o numero

		pthread_mutex_lock(&trava);	//pega a lock do recebe
		recebe=0;
		pthread_mutex_unlock(&trava);//destrava a thread main com o numero ja lido 



		//printf("thread recebeu tarefa: num=%u,  i=%d\n",numero, j);
		////printf("gerei thread: num=%u, threads=%d i=%d\n",numero, num_threads, j);




	}
	while(!cafe[0] ||!cafe[1] ||!cafe[2] ||!cafe[3] ); //espera todo mundo na sala de cafe

	pthread_mutex_lock(&trava); 
	resultado=contador;		
	pthread_mutex_unlock(&trava); 

	numero=1;

	end=1;	//da o sinal para as threads de que o expediente terminou nessa execucao

	//printf("Encerrado\n");
	

	for(i=0; i<4; i++){
		cafe[i]=0;
		pthread_join(thread[i], NULL);
	}

	printf("%d\n", resultado);


	return 0;
}
