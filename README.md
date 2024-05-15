# TP Sistemas Operacionais XV6

## Authors:

Marcos Paulo Ferreira de Souza: https://github.com/MP022

João Vítor Santana Depollo: https://github.com/@JoaoVitorSD

### 1. Introdução

O objetivo deste trabalho é implementar alterações na política de escalonamento do sistema operacional xv6. A maior parte das alterações foram feitas pelas chamadas do scheduler e na inicialização do proc, na função `allocproc()`.

### Scheduller

A prempção do scheduler ocorre com a alteração de um loop que na sua nova versão incrementa uma variável `prempt`, quando atinge o valor da variável `INTERV`, a variável `prempt` tem o seu valor zerado e a função `premptProcess()` é chamada. Através dessa função diferentes políticas de escalonamento são aplicadas para escolher o próximo processo a ser executado de acordo com a sua prioridade.

## Prioridades
Foram definidas 4 níveis de prioridades, sendo eles:
- Low
- Medium
- High
- Realtime

A implementação foi feita via enum, no arquivo `proc.h`:
```c
enum procpriority
{
  LOW,
  MEDIUM,
  HIGH,
  REALTIME
};	
```

## Escalonamento

Cada prioridade possui uma política de escalonamento diferente, e para evitar inanição também foi implementada uma função que aumenta a prioridade dos processos que ficarem X titcks sem serem executados. Neste contexto, foram implementadas as seguintes políticas de escalonamento:

### Realtime
Utiliza o Round Robin, a ideia é que o processo que está a mais tempo sem ser escolhido para ser executado, seja o primeiro a ser executado. Os processos serão executados com um tempo fixo, e depois serão colocados no final da fila novamente. Para esta implementação, é utilizado o valor `last_cycle` do processo, que é incrementado a cada ciclo em que o processo é escolhido para ser executado. Dessa forma, o round_robin vai buscar o processo com o menor `last_cycle` para ser executado.

### High
Baseado no CFS (Completely Fair Scheduler), a ideia é que o processo que tem a menor quantidade de tempo de CPU, seja o próximo a ser executado. Para isso, é utilizado o valor `rutime` do processo, que é incrementado a cada tick que o processo for escolhido para ser executado. Dessa forma, o CFS vai buscar o processo com o menor `rutime` para ser executado. Essa política não gera inanição e dará maior prioridade para os processos que tem menor tempo de CPU, (io-bound).

### Medium
Similar a política do Round Robin, porém essa considera a quantidade de vezes que o processo foi escolhido para ser executado. A ideia é que o processo que foi executado menos vezes, seja o próximo a ser executado. Dessa forma, se um processo p2 chegar após o processo p1, mas o processo p1 já foi executado 3 vezes, e o processo p2 ainda não foi executado, o processo p2 será executado antes do processo p1 por 3 vezes. Dessa forma, é possivel que aconteça uma inação com os processos mais antigos justamente por já terem sido escolhidos diversas vezes para serem executados. 

### Low
Utiliza o FCFS (First Come, First Served), utilizando o valor `ctime` do processo para identificar qual o processo mais antigo. A ideia é que o processo que chegou primeiro, seja o primeiro a ser executado. Com essa política de escalonamento a inação é um problema. No caso do processo mais vvelho não terminar de executar nunca, já que nessa política o processo só para de executar quando ele termina, os outros processos não serão executados. Mas essa solução funciona bem para processos que ficam mais tempo na cpu (cpu-bound).