/*
 * FreeRTOS Kernel V10.0.1
 * Copyright (C) 2017 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

 /******************************************************************************
  * This project provides two demo applications.  A simple blinky style project,
  * and a more comprehensive test and demo application.  The
  * mainCREATE_SIMPLE_BLINKY_DEMO_ONLY setting is used to select between the two.
  * The simply blinky demo is implemented and described in main_blinky.c.  The
  * more comprehensive test and demo application is implemented and described in
  * main_full.c.
  *
  * This file implements the code that is not demo specific, including the
  * hardware setup and FreeRTOS hook functions.
  *
  *******************************************************************************
  * NOTE: Windows will not be running the FreeRTOS demo threads continuously, so
  * do not expect to get real time behaviour from the FreeRTOS Windows port, or
  * this demo application.  Also, the timing information in the FreeRTOS+Trace
  * logs have no meaningful units.  See the documentation page for the Windows
  * port for further information:
  * http://www.freertos.org/FreeRTOS-Windows-Simulator-Emulator-for-Visual-Studio-and-Eclipse-MingW.html
  *

  *
  *******************************************************************************
  */

  /* Standard includes. */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <math.h>


//#include <conio.h>

/* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"



/* This project provides two demo applications.  A simple blinky style demo
application, and a more comprehensive test and demo application.  The
mainCREATE_SIMPLE_BLINKY_DEMO_ONLY setting is used to select between the two.

If mainCREATE_SIMPLE_BLINKY_DEMO_ONLY is 1 then the blinky demo will be built.
The blinky demo is implemented and described in main_blinky.c.

If mainCREATE_SIMPLE_BLINKY_DEMO_ONLY is not 1 then the comprehensive test and
demo application will be built.  The comprehensive test and demo application is
implemented and described in main_full.c. */
#define mainCREATE_SIMPLE_BLINKY_DEMO_ONLY	1

/* This demo uses heap_5.c, and these constants define the sizes of the regions
that make up the total heap.  heap_5 is only used for test and example purposes
as this demo could easily create one large heap region instead of multiple
smaller heap regions - in which case heap_4.c would be the more appropriate
choice.  See http://www.freertos.org/a00111.html for an explanation. */
#define mainREGION_1_SIZE	7201
#define mainREGION_2_SIZE	29905
#define mainREGION_3_SIZE	6407
#define g 9.81  // Valor aproximado da acelera��o devido � gravidade na Terra

/*-----------------------------------------------------------*/

/*
 * main_blinky() is used when mainCREATE_SIMPLE_BLINKY_DEMO_ONLY is set to 1.
 * main_full() is used when mainCREATE_SIMPLE_BLINKY_DEMO_ONLY is set to 0.
 */
 //extern void main_blinky( void );
 //extern void main_full( void );

 /*
  * Only the comprehensive demo uses application hook (callback) functions.  See
  * http://www.freertos.org/a00016.html for more information.
  */
  //void vFullDemoTickHookFunction( void );
  //void vFullDemoIdleFunction( void );

  /*
   * This demo uses heap_5.c, so start by defining some heap regions.  It is not
   * necessary for this demo to use heap_5, as it could define one large heap
   * region.  Heap_5 is only used for test and example purposes.  See
   * http://www.freertos.org/a00111.html for an explanation.
   */
static void  prvInitialiseHeap(void);

/*
 * Prototypes for the standard FreeRTOS application hook (callback) functions
 * implemented within this file.  See http://www.freertos.org/a00016.html .
 */
void vApplicationMallocFailedHook(void);
void vApplicationIdleHook(void);
void vApplicationStackOverflowHook(TaskHandle_t pxTask, char* pcTaskName);
void vApplicationTickHook(void);
void vApplicationGetIdleTaskMemory(StaticTask_t** ppxIdleTaskTCBBuffer, StackType_t** ppxIdleTaskStackBuffer, uint32_t* pulIdleTaskStackSize);
void vApplicationGetTimerTaskMemory(StaticTask_t** ppxTimerTaskTCBBuffer, StackType_t** ppxTimerTaskStackBuffer, uint32_t* pulTimerTaskStackSize);

/*
 * Writes trace data to a disk file when the trace recording is stopped.
 * This function will simply overwrite any trace files that already exist.
 */
static void prvSaveTraceFile(void);

/*-----------------------------------------------------------*/

/* When configSUPPORT_STATIC_ALLOCATION is set to 1 the application writer can
use a callback function to optionally provide the memory required by the idle
and timer tasks.  This is the stack that will be used by the timer task.  It is
declared here, as a global, so it can be checked by a test that is implemented
in a different file. */
StackType_t uxTimerTaskStack[configTIMER_TASK_STACK_DEPTH];

/* Notes if the trace is running or not. */
static BaseType_t xTraceRunning = pdTRUE;

/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/
// FUN��ES CRIADAS POR N�S

#define ZONA_HABITADA 400   //Valor que delimita a zona habitada
#define CHANCE_DE_ACERTO 70 //Representa a porcentagem de chance que a defesa tem de sucesso
#define NUM_MAX_MISSEIS 5   //N�mero m�ximo que a base de ataque pode enviar em cada rodada
/*
	Vari�veis globais

	Essas vari�veis ser�o usadas para apresentar o relat�rio cumulativo dos resultados
	que a base de defesa tem conseguido. Cada indicativo � acumulado de modo que, ap�s
	cada rodada de ataque e defesa, os novos resultados sejam somados aos resultados 
	anteriores e apresentem o rendimento total, e n�o apresentem, apenas,o resultado 
	de cada rodada
*/

int total_misseis = 0, //Essa vari�vel contabiliza quantos m�sseis o atacante disparou, independente da dire��o
	interceptados = 0, //Essa vari�vel contabiliza quantos m�sseis a base de defesa conseguiu defender
	perdidos = 0,	   //Essa vari�vel contabiliza quantos m�sseis n�o foram interceptados e atingiram uma zona habitada
	inofensivo = 0;	   //Essa vari�vel contabiliza quantos m�sseis ca�ram em zona inabitada


/* 
	Estrutura Coordenada

	Vari�vel cujos atributos abrigam dois valores, sendo eles correspondentes
	�s coordenadas do m�ssil disparado em X e em Y
*/
typedef struct {
	float posicao_X;
	float posicao_Y;
} Coordenada;

/*
	Estrutura Missil

	Vari�vel que abriga as informa��es pertinentes ao m�ssil. Cada missil possui
	dois atributos:
	int quantidade - armazena a informa��o de quantos misseis foram disparados
	Coordenada destino - armazena a orienta��o do respectivo missil, o dado de 
						 qual a dire��o que o missil tem 

*/
typedef struct {
	int quantidade;
	Coordenada destino;
} Missil;

QueueHandle_t ataqueQueue;//Fila para organizar a posi��o da task de Ataque na disputa pela CPU
QueueHandle_t defesaQueue;//Fila para organizar a posi��o da task de Defesa na disputa pela CPU


/*
	Essa fun��o apresenta um relat�rio total a partir de todo o hist�rico
	de ataques e defesas que j� tenha acontecido.

	Input: Nada
	Output: Sem retorno de vari�veis

	Toda a informa��o apresentada � extra�da de vari�veis globais. No que 
	cada situa��o acontece (seja intercepta��o, falha de defesa, ou ataque
	em zona inabitada) cada vari�vel correspondente � atualizada contando
	o n�mero de ocorr�ncias de cada caso. Nessa fun��o, � feito um calculo
	percentual de cada situa��o, expressando em porcentagem a quantidade
	de ocorr�ncias de cada situa��o.

	Esses dados s�o apresentados ao fim de cada rodada de Ataque e Defesa. Ao
	final do per�odo de defesa, s�o apresentados os resultados.

	Como exemplo do que � apresentado:

	==============ESTATISTICAS DO CONFRONTO:==============
	Total de misseis enviados: N
	Porcentagem de misseis interceptados: XX.XX%
	Porcentagem de misseis nao defendidos: YY.YY%
	Porcentagem de misseis que cairam em zona inabitada: ZZ.ZZ%
	==========================================

*/
void Relatorio() {

	printf("==============ESTATISTICAS DO CONFRONTO:==============\n");
	printf("Total de misseis enviados: %d\n", total_misseis);
	printf("Porcentagem de misseis interceptados: %.2f%%\n", ((float)interceptados / (float)total_misseis) * 100);
	printf("Porcentagem de misseis nao defendidos: %.2f%%\n", ((float)perdidos / (float)total_misseis) * 100);
	printf("Porcentagem de misseis que cairam em zona inabitada: %.2f%%\n", ((float)inofensivo / (float)total_misseis) * 100);
	printf("==========================================\n");

}

/*
	Essa fun��o calcula com base em fun��es f�sicas uma coordenada
	final para ser o destino de um m�ssil

	Input: 
		missil - Apontador do tipo M�ssil, que armazena o n�mero de m�sseis enviados
		destino - Apontador do tipo Coordenada, que armazena o valor X e Y de para onde o missil vai

	Output: N�o retorna nada, mas faz modifica��es nas vari�veis que recebe atrav�s dos ponteiros

*/
void CalcularTrajetoria(Missil* missil, Coordenada* destino) {
	
	int valor;						//valor rand�mico que muda o destino de cada missil
	float velocidadeInicial = 30.0; //Velocidade inicial, definida arbitrariamente
	float anguloLancamento = 45.0;  // �ngulo de lan�amento em graus, definido arbitrariamente

	/*	
	A partir de defini��es iniciais como o �ngulo de disparo e a velocidade 
	inicial do missil, � calculado a velocidade inicial em cada eixo pela 
	express�o:
		V0x = V0*cos(aL)         [sendo aL o angulo de lan�amento em rad/s]		
	*/

	float velocidadeInicialX = velocidadeInicial * cos(anguloLancamento * (3.1415 / 180.0));
	float velocidadeInicialY = velocidadeInicial * sin(anguloLancamento * (3.1415 / 180.0));

	/*	
	Com isso, � calculado o tempo de voo pela express�o:
		t = (2*V0)/g			[sendo g a constante gravitacional apx 9.81]
	*/
	float tempoVoo = (2.0 * velocidadeInicialY) / 9.81;

	/*
	Dada a quantidade de misseis disparados, para cada um deles define um destino
	usando a express�o:
		X = b*V0x*t					[sendo b o valor rand�mico]
		Y = (g*t^2)/2				[sendo g a gravidade e t o tempoVoo]
	*/
	for (int i = 0; i < missil->quantidade; i++) {

		valor = rand() % 5 + 1;

		destino->posicao_X = valor * velocidadeInicialX * tempoVoo;
		destino->posicao_Y = 0.5 * 9.81 * pow(tempoVoo, 2);

	}

	/*
	Por fim, guarda na vari�vel do tipo Missil, no atributo do tipo Coordenada,
	quais as coordenadas que indicam para onde o missil ir�
	*/
	missil->destino = *destino;
}

/*
	Essa fun��o verifica se o destino horizontal do missil atinge uma zona
	habitada ou uma zona inabitada

	Input: missil - Ponteiro com a informa��o do destino do missil
		   atacarAreaHabitada - Ponteiro de vari�vel booleana que indica SIM
								ou N�O para o crit�rio de ser zona habitada

	Output: N�o retorna nada, mas modifica a vari�vel recebida por meio
			do seu apontador, para sinalizar o tipo de zona
*/
void IdentificarAreasHabitadas(Missil* missil, bool* atacarAreaHabitada) {
	
	/*
	O crit�rio para ser zona habitada � estar numa regi�o dentro do alcance
	de ZONA_HABITADA, macro definida arbitrariamente.

	Se o destino em X do missil exceder o limite da ZONA_HABITADA, significa
	que est� se dirigindo a uma zona deserta e n�o � uma amea�a. Caso contr�rio,
	o missil deve ser interceptado e a vari�vel sinalizadora � definida como
	verdadeira
	*/

	if (missil->destino.posicao_X < ZONA_HABITADA) {
		*atacarAreaHabitada = true;
	}
	else {
		*atacarAreaHabitada = false;
	}
}

/*
	Essa fun��o, a partir de probabilidade, envia um missil para interceptar
	cada missil identificado como amea�a. 

	Input: missil - Ponteiro das informa��es sobre o missil
		   destino - Ponteiro da coordenada de para onde o missil est� indo

	Output: N�o retorna nada, mas apresenta se houve sucesso ou falha na
	intercepta��o do missil de ataque
*/
void DispararMissilInterceptador(Missil* missil, Coordenada* destino) {

	//Apresenta a mensagem contendo o destino do missil que se almeja interceptar
	printf("Missil interceptador disparado na direcao: (%.2f, %.2f)\n", destino->posicao_X, destino->posicao_Y);

	// Simula��o de probabilidade
	float probSucesso = CHANCE_DE_ACERTO;

	/*
	Usando da gera��o aleat�ria, obt�m-se um n�mero entre 1 e 99, caso esse n�mero
	seja menor que a chance de acerto, significa que a base conseguir� interceptar.
	Por outro lado, se esse n�mero seja maior que a chance de acerto (cuja probabilidade
	disso acontecer deve ser menor), a defesa n�o consegue interceptar o missil.
	*/
	if ((rand() % 100 + 1) < probSucesso) {
		vTaskDelay(pdMS_TO_TICKS(100));			//Um delay usado para simular o tempo da trajet�ria
												//do missil disparado para intercept���o

		printf("Sucesso na interceptacao!\n");	//Apresenta a mensagem de que houve intercepta��o
		interceptados++;						//O contador de misseis interceptados � incrementado
												//e a cada vez que isso acontecer, o n�mero de
												//intercept���es aumenta
	}
	else {
		printf("Falha na interceptacao!\n");	//Apresenta a mensagem de que n�o houve intercepta��o
		perdidos++;								//O contador de misseis n�o interceptados � incrementado
	}											//e a cada vez que isso acontecer, o n�mero que representa
}												//falhas de defesa aumenta


/*
	Essa fun��o executa todas as atividades que a Base respons�vel por atacar
	deve fazer:
		* Gera m�sseis
		* Calcula a trajet�ria de cada um
		* Envia o sinal de ataque para BaseDeDefesa
	
	Input: Nada

	Output: N�o retorna nada, mas se comunica com BaseDeDefesa e apresenta
	o desenvolvimento da batalha (na etapa de ataque)
	
*/
void BaseDeAtaque(void* x) {
	while (1) {
		//Espera por intervalos de tempo aleat�rios entre 0 e 1000ms para efetuar um ataque
		vTaskDelay(pdMS_TO_TICKS(rand() % 1000));

		//Define uma quantidade aleat�ria de misseis que ser�o disparados por rodada
		Missil missil ={0};
		missil.quantidade = rand() % NUM_MAX_MISSEIS + 1;  //Esse n�mero � entre 1 e NUM_MAX_MISSEIS

		total_misseis += missil.quantidade;				   //O contador da quantidade total de misseis
														   //de misseis disparados � acrescentado da 
														   //quantidade de novos misseis lan�ados

		/*
		O loop for ir� contar at� o n�mero de misseis que ser� disparado no presente
		ataque, para que, em cada itera��o do loop, um missil por cada vez tenha sua
		trajet�ria definida e seja enviado, junto com suas informa��es para a base de 
		defesa pela fun��o xQueueSend
		*/
		for (int i = 0; i < missil.quantidade; i++) {

			CalcularTrajetoria(&missil, &missil.destino);	//Define valores X e Y para o missil em quest�o

			if (xQueueSend(defesaQueue, &missil, portMAX_DELAY) == pdTRUE) {
															//Envia o missil para a defesa
				printf("Missil de ataque enviado. Destino: (%.2f, %.2f)\n", missil.destino.posicao_X, missil.destino.posicao_Y);
															//Apresenta a mensagem indicando qual o destino
															//do missil com suas coord (X, Y)
			}
			else {
				printf("Falha ao enviar missil para defesa.\n");
															//No caso de acontecer algum erro na comunica��o
															//entre as bases, essa mensagem � apresentada
			}
		}

	}
}


/*
	Essa fun��o tem como objetivo executar todas as medidas de defesa
	para interceptar os misseis enviados pela BaseDeAtaque. A saber:
		* Esperar o ataque iniciar
		* Identificar a quantidade de misseis que est� recebendo
		* Verificar se cada missil se dirige a uma zona habitada
		* Disparar misseis para interceptar o missil advers�rio
		* Ignorar misseis que n�o apresentam perigo
		* Enviar informa��es da defesa para BaseDeAtaque
		
	Input: Nada

	Output: N�o retorna nada, mas se comunica com BaseDeAtaque, e apresenta
	o desenvolvimento da batalha (na etapa de defesa)
*/
void BaseDeDefesa(void* x) {

	int n = 0;											// Contador de misseis tratados pela defesa
														// Essa vari�vel � usada para que a informa��o geral
														// de cada ataque s� seja apresentada no in�cio do
														// processo de defesa

	while (1) {
		Missil missil;

		printf("Aguardando missil de ataque...\n");		//Espera at� que a BaseDeAtaque sinalize envio de misseis

		if (xQueueReceive(defesaQueue, &missil, portMAX_DELAY) == pdTRUE) {
			if (n == 0)									//Contador zero indica que a defesa ainda n�o agiu, portanto,
			{											//a mensagem de in�cio s� � apresentada uma vez ao iniciar a defesa															
				printf("Ataque iniciado. %d misseis detectados.\n", missil.quantidade);
			}
														
			bool atacarAreaHabitada;					//Vari�vel que sinaliza se a zona � habitada ou n�o
			IdentificarAreasHabitadas(&missil, &atacarAreaHabitada);// Determina se o m�ssil est� direcionado a �reas habitadas


			/*
			A partir daqui: Se a zona for identifcada como habitada, inicia-se o
			processo de intercepta��o: Disparando o m�ssil interceptador a partir
			do destino conhecido do missil que vem

			A cada missil disparado, o contador de misseis atirados pela BaseDeDefesa
			� incrementado
			*/
			if (atacarAreaHabitada) {		

				printf("Missil direcionado a area habitada. Iniciando defesa.\n");

				DispararMissilInterceptador(&missil, &missil.destino);
				n ++;									//conta mais um missil tratado pela defesa
														//Nesse caso, houve a tentativa de interceptar

				// Envia informa��es sobre os m�sseis interceptados de volta para a base de ataque
				if (xQueueSend(ataqueQueue, &missil, portMAX_DELAY) == pdTRUE) {
					printf("Informacoes sobre interceptacao enviadas para o ataque.\n");
				}
				else {
					printf("Falha ao enviar informacoes sobre missil interceptado para a base de ataque.\n");
														//Caso haja falha na comunica��o com BaseDeAtaque
														//essa mensagem � exibida
				}
			}
			else {										//Caso o sinalizador de �rea habitada seja falso,
														//isso implica em zona inabitada e o missil que vem
														//pode ser ignorado
				printf("Missil dirigido a area inabitada. Ignorado\n");
				n++;									//conta mais um missil tratado pela defesa
														//Nesse caso, o missil foi ignorado por n�o ser amea�a
				inofensivo++;							//contador de misseis que ca�ram em zona inabitada 
														//� incrementado cada vez que isso acontece
			}
		}
		else {
			printf("Falha ao receber missil da base de ataque.\n");
														//Essa mensagem � exibida caso n�o haja comunica��o
														//eficaz com a BaseDeAtaque e a defesa n�o seja
														//informada do lan�amento de misseis contra ela
		}
		if (n == missil.quantidade) {					//Se o contador for igual, indica que a defesa tratou
														//N misseis para interceptar N misseis que vieram do ataque
			Relatorio();								//Ao fim da rodada de ataque e defesa o relat�rio � apresentado
			n = 0;										//A vari�vel que indica o n�mero de misseis tratados � reiniciada
														//para quando uma nova onda de ataques come�ar
		}
	}
}

// FIM DAS FUN��ES CRIADAS POR N�S
/*-----------------------------------------------------------*/

int main(void)
{
	/* This demo uses heap_5.c, so start by defining some heap regions.  heap_5
	is only used for test and example reasons.  Heap_4 is more appropriate.  See
	http://www.freertos.org/a00111.html for an explanation. */
	prvInitialiseHeap();

	/* Initialise the trace recorder.  Use of the trace recorder is optional.
	See http://www.FreeRTOS.org/trace for more information. */
	vTraceEnable(TRC_START);
	// Inicializa��o do FreeRTOS e cria��o das filas
	ataqueQueue = xQueueCreate(10, sizeof(Missil));
	defesaQueue = xQueueCreate(10, sizeof(Missil));
	if (ataqueQueue != NULL && defesaQueue != NULL) {
		printf("Filas criadas com sucesso.\n");
	}
	else {
		printf("Falha ao criar filas.\n");
	}

	//Cria��o das tarefas
	BaseType_t statusAtaque = xTaskCreate(BaseDeAtaque, "BaseAtaque", 1000, NULL, tskIDLE_PRIORITY + 2, NULL);
	BaseType_t statusDefesa = xTaskCreate(BaseDeDefesa, "BaseDefesa", 1000, NULL, tskIDLE_PRIORITY + 1, NULL);
	//xTaskCreate(MonitorAreas, "Monitor", 1000, NULL, tskIDLE_PRIORITY + 1, NULL);

	if (statusAtaque == pdPASS && statusDefesa == pdPASS) {
		printf("Tarefas criadas com sucesso.\n");
	}
	else {
		printf("Falha ao criar tarefas.\n");
	}
	//Inicializa��o do kernel FreeRTOS
	vTaskStartScheduler();
	for (;;);
	return 0;
}

/*-----------------------------------------------------------*/

void vApplicationMallocFailedHook(void)
{
	/* vApplicationMallocFailedHook() will only be called if
	configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h.  It is a hook
	function that will get called if a call to pvPortMalloc() fails.
	pvPortMalloc() is called internally by the kernel whenever a task, queue,
	timer or semaphore is created.  It is also called by various parts of the
	demo application.  If heap_1.c, heap_2.c or heap_4.c is being used, then the
	size of the	heap available to pvPortMalloc() is defined by
	configTOTAL_HEAP_SIZE in FreeRTOSConfig.h, and the xPortGetFreeHeapSize()
	API function can be used to query the size of free heap space that remains
	(although it does not provide information on how the remaining heap might be
	fragmented).  See http://www.freertos.org/a00111.html for more
	information. */
	vAssertCalled(__LINE__, __FILE__);
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook(void)
{
	/* vApplicationIdleHook() will only be called if configUSE_IDLE_HOOK is set
	to 1 in FreeRTOSConfig.h.  It will be called on each iteration of the idle
	task.  It is essential that code added to this hook function never attempts
	to block in any way (for example, call xQueueReceive() with a block time
	specified, or call vTaskDelay()).  If application tasks make use of the
	vTaskDelete() API function to delete themselves then it is also important
	that vApplicationIdleHook() is permitted to return to its calling function,
	because it is the responsibility of the idle task to clean up memory
	allocated by the kernel to any task that has since deleted itself. */

	/* Uncomment the following code to allow the trace to be stopped with any
	key press.  The code is commented out by default as the kbhit() function
	interferes with the run time behaviour. */
	/*
		if( _kbhit() != pdFALSE )
		{
			if( xTraceRunning == pdTRUE )
			{
				vTraceStop();
				prvSaveTraceFile();
				xTraceRunning = pdFALSE;
			}
		}
	*/

#if ( mainCREATE_SIMPLE_BLINKY_DEMO_ONLY != 1 )
	{
		/* Call the idle task processing used by the full demo.  The simple
		blinky demo does not use the idle task hook. */
		vFullDemoIdleFunction();
	}
#endif
}
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook(TaskHandle_t pxTask, char* pcTaskName)
{
	(void)pcTaskName;
	(void)pxTask;

	/* Run time stack overflow checking is performed if
	configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
	function is called if a stack overflow is detected.  This function is
	provided as an example only as stack overflow checking does not function
	when running the FreeRTOS Windows port. */
	vAssertCalled(__LINE__, __FILE__);
}
/*-----------------------------------------------------------*/

void vApplicationTickHook(void)
{
	/* This function will be called by each tick interrupt if
	configUSE_TICK_HOOK is set to 1 in FreeRTOSConfig.h.  User code can be
	added here, but the tick hook is called from an interrupt context, so
	code must not attempt to block, and only the interrupt safe FreeRTOS API
	functions can be used (those that end in FromISR()). */
#if ( mainCREATE_SIMPLE_BLINKY_DEMO_ONLY != 1 )
	{
		vFullDemoTickHookFunction();
	}
#endif /* mainCREATE_SIMPLE_BLINKY_DEMO_ONLY */
}
/*-----------------------------------------------------------*/

void vApplicationDaemonTaskStartupHook(void)
{
	/* This function will be called once only, when the daemon task starts to
	execute	(sometimes called the timer task).  This is useful if the
	application includes initialisation code that would benefit from executing
	after the scheduler has been started. */
}
/*-----------------------------------------------------------*/

void vAssertCalled(unsigned long ulLine, const char* const pcFileName)
{
	static BaseType_t xPrinted = pdFALSE;
	volatile uint32_t ulSetToNonZeroInDebuggerToContinue = 0;

	/* Called if an assertion passed to configASSERT() fails.  See
	http://www.freertos.org/a00110.html#configASSERT for more information. */

	/* Parameters are not used. */
	(void)ulLine;
	(void)pcFileName;

	printf("ASSERT! Line %ld, file %s, GetLastError() %ld\r\n", ulLine, pcFileName, GetLastError());

	taskENTER_CRITICAL();
	{
		/* Stop the trace recording. */
		if (xPrinted == pdFALSE)
		{
			xPrinted = pdTRUE;
			if (xTraceRunning == pdTRUE)
			{
				vTraceStop();
				prvSaveTraceFile();
			}
		}

		/* You can step out of this function to debug the assertion by using
		the debugger to set ulSetToNonZeroInDebuggerToContinue to a non-zero
		value. */
		while (ulSetToNonZeroInDebuggerToContinue == 0)
		{
			__asm { NOP };
			__asm { NOP };
		}
	}
	taskEXIT_CRITICAL();
}
/*-----------------------------------------------------------*/

static void prvSaveTraceFile(void)
{
	FILE* pxOutputFile;

	fopen_s(&pxOutputFile, "Trace.dump", "wb");

	if (pxOutputFile != NULL)
	{
		fwrite(RecorderDataPtr, sizeof(RecorderDataType), 1, pxOutputFile);
		fclose(pxOutputFile);
		printf("\r\nTrace output saved to Trace.dump\r\n");
	}
	else
	{
		printf("\r\nFailed to create trace dump file\r\n");
	}
}
/*-----------------------------------------------------------*/

static void  prvInitialiseHeap(void)
{
	/* The Windows demo could create one large heap region, in which case it would
	be appropriate to use heap_4.  However, purely for demonstration purposes,
	heap_5 is used instead, so start by defining some heap regions.  No
	initialisation is required when any other heap implementation is used.  See
	http://www.freertos.org/a00111.html for more information.

	The xHeapRegions structure requires the regions to be defined in start address
	order, so this just creates one big array, then populates the structure with
	offsets into the array - with gaps in between and messy alignment just for test
	purposes. */
	static uint8_t ucHeap[configTOTAL_HEAP_SIZE];
	volatile uint32_t ulAdditionalOffset = 19; /* Just to prevent 'condition is always true' warnings in configASSERT(). */
	const HeapRegion_t xHeapRegions[] =
	{
		/* Start address with dummy offsets						Size */
		{ ucHeap + 1,											mainREGION_1_SIZE },
		{ ucHeap + 15 + mainREGION_1_SIZE,						mainREGION_2_SIZE },
		{ ucHeap + 19 + mainREGION_1_SIZE + mainREGION_2_SIZE,	mainREGION_3_SIZE },
		{ NULL, 0 }
	};

	/* Sanity check that the sizes and offsets defined actually fit into the
	array. */
	configASSERT((ulAdditionalOffset + mainREGION_1_SIZE + mainREGION_2_SIZE + mainREGION_3_SIZE) < configTOTAL_HEAP_SIZE);

	/* Prevent compiler warnings when configASSERT() is not defined. */
	(void)ulAdditionalOffset;

	vPortDefineHeapRegions(xHeapRegions);
	printf("teste");
}
/*-----------------------------------------------------------*/

/* configUSE_STATIC_ALLOCATION is set to 1, so the application must provide an
implementation of vApplicationGetIdleTaskMemory() to provide the memory that is
used by the Idle task. */
void vApplicationGetIdleTaskMemory(StaticTask_t** ppxIdleTaskTCBBuffer, StackType_t** ppxIdleTaskStackBuffer, uint32_t* pulIdleTaskStackSize)
{
	/* If the buffers to be provided to the Idle task are declared inside this
	function then they must be declared static - otherwise they will be allocated on
	the stack and so not exists after this function exits. */
	static StaticTask_t xIdleTaskTCB;
	static StackType_t uxIdleTaskStack[configMINIMAL_STACK_SIZE];

	/* Pass out a pointer to the StaticTask_t structure in which the Idle task's
	state will be stored. */
	*ppxIdleTaskTCBBuffer = &xIdleTaskTCB;

	/* Pass out the array that will be used as the Idle task's stack. */
	*ppxIdleTaskStackBuffer = uxIdleTaskStack;

	/* Pass out the size of the array pointed to by *ppxIdleTaskStackBuffer.
	Note that, as the array is necessarily of type StackType_t,
	configMINIMAL_STACK_SIZE is specified in words, not bytes. */
	*pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}
/*-----------------------------------------------------------*/

/* configUSE_STATIC_ALLOCATION and configUSE_TIMERS are both set to 1, so the
application must provide an implementation of vApplicationGetTimerTaskMemory()
to provide the memory that is used by the Timer service task. */
void vApplicationGetTimerTaskMemory(StaticTask_t** ppxTimerTaskTCBBuffer, StackType_t** ppxTimerTaskStackBuffer, uint32_t* pulTimerTaskStackSize)
{
	/* If the buffers to be provided to the Timer task are declared inside this
	function then they must be declared static - otherwise they will be allocated on
	the stack and so not exists after this function exits. */
	static StaticTask_t xTimerTaskTCB;

	/* Pass out a pointer to the StaticTask_t structure in which the Timer
	task's state will be stored. */
	*ppxTimerTaskTCBBuffer = &xTimerTaskTCB;

	/* Pass out the array that will be used as the Timer task's stack. */
	*ppxTimerTaskStackBuffer = uxTimerTaskStack;

	/* Pass out the size of the array pointed to by *ppxTimerTaskStackBuffer.
	Note that, as the array is necessarily of type StackType_t,
	configMINIMAL_STACK_SIZE is specified in words, not bytes. */
	*pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}