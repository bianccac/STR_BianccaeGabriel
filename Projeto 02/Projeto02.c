#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>//Usada para função de potenciação
#include <unistd.h>//Usada para possibilitar a escolhe de algoritmos e cargas no terminal
#include <string.h>
#include <stdbool.h>

typedef struct {
	
	int period;
	int deadline;
	int exec_time;
	char name[5];
	int carga;
		
} Task;
//Estrutura de tarefa que contém as informações importantes acerca de uma, as quais serão obtidas do .txt

typedef struct{
	int num_carga;
	int num_tarefas;
	Task tarefas[100];
}Carga;
//Estrutura de carga que possui como atributo o seu número identificador, quantas tarefas possui e um array que armazena cada tarefa bem como suas informações importantes

int max_div(int a, int b) {
    if (b == 0)
        return a;
    return max_div(b, a % b);
}
//Função que calcula o MDC

int min_mul(int a, int b){
    return (a * b) / max_div(a, b);

}
//Função que calcula o MMC

bool is_divisor(int num, int div){
	
	if(num%div==0)return true;
	else return false;
}
//Função que checa se um dado número "div" é divisor de um dado número "num"

void cyclic_executive_scheduler(Carga carga){

	float load = 0;//Variável que armazenará o total de carga demandada
	int mmc;
	int mdc;
		
	for(int i = 0; i<carga.num_tarefas;i++){
		
		mmc = carga.tarefas[i].period;
		mdc = carga.tarefas[i].period;
		for(int j = 0; j<carga.num_tarefas;j++){
			mmc = min_mul(mmc, carga.tarefas[j].period);
			mdc = max_div(mdc, carga.tarefas[j].period);
		}
		//Calcula o MMC e MDC com os períodos dados de cada tarefa
		
		
		load += (float)carga.tarefas[i].exec_time/(float)carga.tarefas[i].period;
		//Para cada iteração acrescenta a carga referente à uma tarefa da carga
	}
	
	//Determinar o ciclo menor
	int great_exec_time = carga.tarefas[0].exec_time, //Delimitadores de qual será o ciclo menor
	short_period = carga.tarefas[0].period;		  //Deve ser maior que o maior tempo de exec e menor que o menor período
	
	for(int i = 0; i < carga.num_tarefas; i++){
		if(carga.tarefas[i].exec_time>great_exec_time){
			great_exec_time = carga.tarefas[i].exec_time;
		}
		//Verifica se os próximos valores de tempo de execução são maiores do que o primeiro do array
		if(carga.tarefas[i+1].period<short_period){
			short_period = carga.tarefas[i].period;
		}	
		//Verifica se os próximos valores de período são menores do que o primeiro do array
		
	}
	
	int divisors[50] = {};//Lista que armazena os divisores de um número
	int pos = 0;//Contador para indicar em que posição do array "divisors" está
	
	for(int i = great_exec_time; i<short_period;i++){//i assume números dentro do intervalo definido no último for( )
		if(is_divisor(mmc,i)){//A função is_divisor() retorna "true" caso o número seja um divisor
			divisors[pos] = i;//O número é acrescentado na lista de divisores
			pos ++;//O indicador de posição do array "divisors" aponta para a próxima posição
		}
	}
	
	int possible_lower_cycle[50]={};//Array para armazenar candidatos a ciclo menor
	
	for(int i=0; i < pos; i++){
		if(((2*divisors[i])-max_div(divisors[i],carga.tarefas[i].period))<=carga.tarefas[i].deadline)
			possible_lower_cycle[i] = divisors[i];
		//Condição para ser um candidato a menor ciclo
	} 
	
	int lower_cycle = short_period;//Define o ciclo menor já como o menor período para encontrar valores que satisfazem a condição
	
	for (int i = 0; i < 50; i++) {//percorre a lista de possíveis ciclos menores
        	if (possible_lower_cycle[i] != 0 && possible_lower_cycle[i] < lower_cycle) {//previne de ter algum zero no array
            		lower_cycle = possible_lower_cycle[i];//lower_cycle sempre assumirá o menor valor encontrado
        	}
    	}
	
	printf("\tExecutivo: ");//Apresenta as informações definidas anteriormente
	if(load<1){
		printf("SIM\n");
		printf("\t\tCiclo maior: %d\n", mmc);
        	printf("\t\tCiclo menor: %d\n", lower_cycle);
	}
	else{
		printf("NÃO\n");
		printf("\t\tCiclo maior: 0\n");
        	printf("\t\tCiclo menor: 0\n");
	}
	
}

void rate_monotonic_scheduler(Carga carga){

	int prioridades[carga.num_tarefas];//array que define as prioridades
    	int tempo_execucao[carga.num_tarefas];//array que armazena os tempos de execução de cada tarefa de uma carga
	float load = 0;//Variável que armazenará o total de carga demandada

	// Inicializa as prioridades
	    for (int i = 0; i < carga.num_tarefas; i++) {
		prioridades[i] = i; // As prioridades são indexadas pelos índices das tarefas
		tempo_execucao[i] = 0;
	    }
	
	// Ordena com base nos períodos (menor para o maior)
	for (int i = 0; i < carga.num_tarefas-1; i++) {
		for (int j = i+1; j < carga.num_tarefas; j++) {
		    if (carga.tarefas[i].period > carga.tarefas[j].period) {
		        int temp = prioridades[i];
		        prioridades[i] = prioridades[j];
		        prioridades[j] = temp;
		    }
       		}
   	}
   	 
   	 
	
	for (int i = 0; i < carga.num_tarefas; i++) {
		int taskIndex = prioridades[i];//Seleciona o índice da tarefa pela prioridade
		int period = carga.tarefas[taskIndex].period;
		int exec_time = carga.tarefas[taskIndex].exec_time;

		load += (float)exec_time / period;//Calcula demanda de processamento em cada tarefa

   	}
   	//Apresenta as informações definidas anteriormente
	 if (load <= (carga.num_tarefas * (pow(2, (1.0 / carga.num_tarefas)) - 1))) {
		printf("\tRM: SIM\n");
	    } else if (load <= 1.0) {
		    printf("\tRM: INCONCLUSIVO\n");
	}else {
		printf("\tRM: NÃO\n");
    	}


}

void earlier_deadline_first_scheduler(Carga carga){

	float load=0;//Variável que armazenará o total de carga demandada
	bool flag_a = false; //caso em que deve ser tomado o menor valor entre período e deadline, D é arbitrário
	bool flag_m = false; //caso em que D < P e o teste é apenas necessário
	
	for(int i = 0; i<carga.num_tarefas; i++){
		if(carga.tarefas[i].deadline < carga.tarefas[i].period){
			flag_m = true;
			//Se D < P, o teste é apenas necessário
		}
		else if(carga.tarefas[i].period == carga.tarefas[i].deadline){
			continue;	
			//Se D = P, usar-se-á o Deadline e o teste é suficiente	
		}
		else{
			flag_a = true;
			/*Para um D arbitrário, entra-se no caso especial em que será usado o menor entre D e P
			Uma única ocorrência já define a flag como true o que já redireciona o 
			código para a condição de D arbitrário*/
		}
	}

	printf("\tEDF: ");//Apresenta os resultados	

	if(flag_a){
		for(int j = 0; j < carga.num_tarefas; j++){
			load += (float)carga.tarefas[j].exec_time/fmin(carga.tarefas[j].deadline,carga.tarefas[j].period);
			//Para D arbitrário, calcula a carga considerando o menor entre D e P na razão com o tempo de execução	
		}
		if(load<=1){
			printf("INCONCLUSIVO\n");
		}else{
			printf("NÃO\n");
		}
	}
	else if(flag_m){
		for(int j = 0; j <carga.num_tarefas; j++){
			load += (float)carga.tarefas[j].exec_time/(float)carga.tarefas[j].deadline;
			//Tanto se D < P ou se D = P, usa-se o D
		}
		if(load<=1){
			printf("INCONCLUSIVO\n");
		}else{
			printf("NÃO\n");
		}
	}else{
		for(int j = 0; j <carga.num_tarefas; j++){
			load += (float)carga.tarefas[j].exec_time/(float)carga.tarefas[j].deadline;
			//Se D = P, usa-se o D
		}
		if(load<=1){
			printf("SIM\n");
		}else{
			printf("NÃO\n");
		}
	}
	
}

int main(int argc, char *argv[]) {
    int opt;
    int load = -1;
    char *file_name;
    char *algoritmo = NULL;

    // Processar opções da linha de comando
    while ((opt = getopt(argc, argv, "n:a:")) != -1) {
        switch (opt) {
            case 'n':
                load = atoi(optarg); // Converte o argumento para inteiro
                break;
            case 'a':
                algoritmo = optarg; // Armazena o nome do algoritmo
                break;
            default:
                fprintf(stderr, "Uso: %s -n <numero> -a <algoritmo>\n", argv[0]);
                return 1;
        }
    }


    // Verifica se o nome do arquivo foi passado como argumento
    if (optind < argc) {
        file_name = argv[optind];
        printf("Nome do arquivo: %s\n", file_name);
    } else {
        fprintf(stderr, "Uso: %s -n <numero> -a <algoritmo> <nome_do_arquivo>\n", argv[0]);
        return 1;
    }    
    
    char *nome_arquivo = argv[optind];
    FILE *arquivo = fopen(nome_arquivo, "r");


    // Verifica se o arquivo foi aberto corretamente
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo.\n");
        return 1;
    }
    Carga cargas[100];
    int num_cargas=-1;
	
    // Lê e processa o conteúdo do arquivo linha por linha
    char linha[100]; // Defina um tamanho apropriado para a linha
    while (fgets(linha, sizeof(linha), arquivo)) {
        if (isdigit(linha[0])) {
            // Esta linha indica o número da carga
            num_cargas++;
	    cargas[num_cargas].num_carga = num_cargas+1;
            cargas[num_cargas].num_tarefas = 0;
        } else {
		Task tarefa;
            // Esta linha contém informações de uma tarefa
            //aloca cada informação do .txt no respectivo atributo designado para a referida informação
            sscanf(linha, "T%s %d %d %d", 
		tarefa.name,
                &(tarefa.exec_time),
                &(tarefa.period), 
                &(tarefa.deadline));
	    cargas[num_cargas].tarefas[cargas[num_cargas].num_tarefas] = tarefa;
            cargas[num_cargas].num_tarefas++;
        }
    }


    // Fecha o arquivo
    fclose(arquivo);

    // Acessando as informações das tarefas
    /*for (int i = 0; i <= num_cargas; i++) {
        printf("Carga %d:\n", cargas[i].num_carga);
        for (int j = 0; j < cargas[i].num_tarefas; j++) {
            printf("Tarefa %d: Execução=%d, Período=%d, Deadline=%d\n",
                j + 1, //apresenta a correta distribuição das informações
                cargas[i].tarefas[j].exec_time, 
                cargas[i].tarefas[j].period, 
                cargas[i].tarefas[j].deadline);
        }
        printf("\n");
    }*/
    //Esse for apresenta as informações da carga escolhida e da tarefa
    
     for(int i = 0; i <= num_cargas; i++){
        if (load != -1 && load != cargas[i].num_carga) {
            continue; // Pula esta carga se não for a desejada
        }

        printf("Carga %d\n", i+1);
        
        //Verifica qual algoritmo foi escolhido no terminal (apresenta todos por default)
        if (algoritmo == NULL || strcmp(algoritmo, "EC") == 0) {
            cyclic_executive_scheduler(cargas[i]);
        }

        if (algoritmo == NULL || strcmp(algoritmo, "RM") == 0) {
            rate_monotonic_scheduler(cargas[i]);
        }

        if (algoritmo == NULL || strcmp(algoritmo, "EDF") == 0) {
            earlier_deadline_first_scheduler(cargas[i]);
        }
    }
