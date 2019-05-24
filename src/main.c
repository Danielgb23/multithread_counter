#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <pthread.h>


pthread_mutex_t cafe[4];
pthread_mutex_t trava;
pthread_mutex_t livre[4];

pthread_cond_t espera[4];

//variaveis globais
int contador=0;

int num_threads=0,  recebe=0, end=0, ocupada[4]={0, 0, 0, 0};
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

	printf("Sucesso gerando thread %d\n", threadid);

	while(!end){
		printf("Esperando trampo! id=%d\n", threadid);

		ocupada[threadid]=0;
		//a thread chega aqui sem a lock cafe mas a do recebe esta livre
		pthread_mutex_lock(&cafe[threadid]);	//fica esperando a principal cortar o cafezinho(soltar a lock cafe)
		pthread_mutex_unlock(&cafe[threadid]);	
		ocupada[threadid]=1;
		num=numero;				//le o numero
		pthread_mutex_lock(&trava);
		recebe=1;
		pthread_mutex_unlock(&trava);

		printf("Comecei a labuta num=%u contador=%d id=%d\n", num, contador, threadid);
		//sleep(2);
		if(isprimo(num)){		//se primo incrementa o contador
			pthread_mutex_lock(&trava); 
			contador++;		
			pthread_mutex_unlock(&trava); 
		}
		printf("terminei! num=%u contador=%d id=%d\n", num, contador, threadid);
	}
	printf("thread %d acabou\n", threadid);
	return NULL;
}


int main(int argc, char **argv) {

	pthread_t thread[4];

	int i,j=0;
	int * tid ;
	int resultado;

	//faz novas threads
	for(j=0; j<4; j++){
		pthread_mutex_lock(&cafe[j]);	//deixa a maquina de cafe j ligada(ie pega trava cafe j)
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

		while(ocupada[j]){ 	//espera abrir uma thread	
			j++;	//numero da thread que esta sendo utilizada nesse loop
			j%=4;
		}

		//o programa chega aqui com a thread j travada em cafe[j].
		pthread_mutex_unlock(&cafe[j]);		//libera a thread j

		while(!recebe);			//espera a thread receber o numero

		pthread_mutex_lock(&trava);	//pega a lock do recebe
		recebe=0;
		pthread_mutex_unlock(&trava);//destrava a thread main com o numero ja lido 

		pthread_mutex_lock(&cafe[j]);		//liga a maquina de(pega a trava) cafe para onde o trabalhador volta(para) apos terminar a tarefa


		printf("thread recebeu tarefa: num=%u,  i=%d\n",numero, j);
		//printf("gerei thread: num=%u, threads=%d i=%d\n",numero, num_threads, j);




	}
	numero=1;	//encerra as threads com um numero que nao adiciona ao contador
	for(i=0; i<4; i++)
		pthread_mutex_unlock(&cafe[i]);		//libera a thread j

	end=1;	//da o sinal para as threads de que o expediente terminou nessa execucao
	printf("Encerrado\n");
	

	for(i=0; i<4; i++){
		pthread_join(thread[i], NULL);
	}

	printf("%d\n", contador);


	return 0;
}
