#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

typedef struct {
	
	int period;
	int deadline;
	int exec_time;
	char name[5];
	int carga;
		
} Task;

typedef struct{
	int num_carga;
	int num_tarefas;
	Task tarefas[100];
}Carga;

int max_div(int a, int b) {
    if (b == 0)
        return a;
    return max_div(b, a % b);
}

int min_mul(int a, int b){
    return (a * b) / max_div(a, b);

}

bool is_divisor(int num, int div){
	
	if(num%div==0)return true;
	else return false;
}

void cyclic_executive_scheduler(Carga carga){

	float load = 0;
	int mmc;
	int mdc;
		
	for(int i = 0; i<carga.num_tarefas;i++){
		
		mmc = carga.tarefas[i].period;
		mdc = carga.tarefas[i].period;
		for(int j = 0; j<carga.num_tarefas;j++){
			mmc = min_mul(mmc, carga.tarefas[j].period);
			mdc = max_div(mdc, carga.tarefas[j].period);
		}
		
		
		load += (float)carga.tarefas[i].exec_time/(float)carga.tarefas[i].period;

	}
	
	//Determinar o ciclo menor
	int great_exec_time = carga.tarefas[0].exec_time, 
	short_period = carga.tarefas[0].period;
	for(int i = 0; i < carga.num_tarefas; i++){
		if(carga.tarefas[i].exec_time>great_exec_time){
			great_exec_time = carga.tarefas[i].exec_time;
		}
		if(carga.tarefas[i+1].period<short_period){
			short_period = carga.tarefas[i].period;
		}	

		
	}
	
	int divisors[50] = {};
	int pos = 0;
	
	for(int i = great_exec_time; i<short_period;i++){
		if(is_divisor(mmc,i)){
			divisors[pos] = i;
			pos ++;
		}
	}
	
	int possible_lower_cycle[50]={};
	
	for(int i=0; i < pos; i++){
		if(((2*divisors[i])-max_div(divisors[i],carga.tarefas[i].period))<=carga.tarefas[i].deadline)
			possible_lower_cycle[i] = divisors[i];
	} 
	
	int lower_cycle = short_period;
	
	for (int i = 0; i < 50; i++) {
        	if (possible_lower_cycle[i] != 0 && possible_lower_cycle[i] < lower_cycle) {
            		lower_cycle = possible_lower_cycle[i];
        	}
    	}
	
	printf("\tExecutivo: ");
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

	int prioridades[carga.num_tarefas];
    	int tempo_execucao[carga.num_tarefas];
	float load = 0;

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
		int taskIndex = prioridades[i];
		int period = carga.tarefas[taskIndex].period;
		int exec_time = carga.tarefas[taskIndex].exec_time;

		load += (float)exec_time / period; 

   	}
	 if (load <= (carga.num_tarefas * (pow(2, (1.0 / carga.num_tarefas)) - 1))) {
		printf("\tRM: SIM\n");
	    } else if (load <= 1.0) {
		    printf("\tRM: INCONCLUSIVO\n");
	}else {
		printf("\tRM: NÃO\n");
    	}


}

void earlier_deadline_first_scheduler(Carga carga){

	float load;
	
	for(int i = 0; i<carga.num_tarefas; i++){
		if(carga.tarefas[i].period<=carga.tarefas[i].deadline){
			load += (float)carga.tarefas[i].exec_time/(float)carga.tarefas[i].deadline;
		}
		else{
			load += (float)carga.tarefas[i].exec_time/fmin((float)carga.tarefas[i].deadline,(float)carga.tarefas[i].period);
		}
	
	}
	
	printf("\tEDF: ");
	if(load<=1){
		printf("SIM\n");
	}else{
		printf("NÃO\n");
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

    	
    return 0;
}

