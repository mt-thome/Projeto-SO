# Simulador de Sistema Operacional
## Visão Geral
Este projeto implementa um simulador de sistema operacional multi-processos com funcionalidades básicas de escalonamento, gerenciamento de memória e controle de processos. O simulador é capaz de executar programas sintéticos descritos em arquivos de texto, simular a execução de instruções, gerenciar memória virtual com paginação e controlar recursos do sistema através de semáforos.

## Funcionalidades
- Gerenciamento de Processos: Criação, execução e término de processos
- Escalonamento: Baseado em algoritmo de prioridade considerando operações de I/O
- Gerenciamento de Memória: Implementação de memória virtual com paginação 
    - Páginas de 1KB
    - Algoritmo de substituição baseado no relógio (Clock)
    - Máximo de 16 páginas por processo
- Controle de Recursos: Semáforos para sincronização entre processos
- Simulação de I/O: Operações de disco, impressora e sistema de arquivos
- Interface de Linha de Comando: Monitoramento de processos e estado da memória

## Estrutura do Projeto
```
C/Projeto-SO/
├── bin/                  # Binários compilados
├── include/              # Arquivos de cabeçalho
│   ├── bcp.h            # Estrutura do Bloco de Controle de Processo
│   ├── cpu.h            # Simulador de CPU
│   ├── events.h         # Manipulação de eventos do sistema
│   ├── interface.h      # Interface com o usuário
│   ├── memory.h         # Gerenciador de memória
│   ├── parser.h         # Parser para programas sintéticos
│   └── scheduler.h      # Escalonador de processos
├── obj/                  # Arquivos objeto da compilação
├── src/                  # Código-fonte
│   ├── bcp.c            # Implementação do gerenciador de processos
│   ├── cpu.c            # Implementação do simulador de CPU
│   ├── events.c         # Implementação do manipulador de eventos
│   ├── interface.c      # Implementação da interface com o usuário
│   ├── main.c           # Ponto de entrada do programa
│   ├── memory.c         # Implementação do gerenciador de memória
│   ├── parser.c         # Implementação do parser
│   └── scheduler.c      # Implementação do escalonador
├── test/                 # Arquivos de teste
│   └── program.txt      # Exemplo de programa sintético
├── makefile              # Build script
└── README.md             # Documentação
```

## Formato de Programa Sintético
Os programas são descritos em arquivos texto com o seguinte formato:
```
[Nome do processo]
[ID do segmento]
[Prioridade]
[Tamanho do segmento em KB]
[Lista de semáforos separados por espaço]
[Instruções, uma por linha]
```

### Instruções Suportadas
- `exec [tempo]` - Simula tempo de execução na CPU
- `read [blocos]` - Simula operação de leitura do disco
- `write [blocos]` - Simula operação de escrita no disco
- `P $[semáforo]` - Operação P (down) em semáforo
- `V $[semáforo]` - Operação V (up) em semáforo

## Como Executar

```
# Executar o simulador
./bin/os_simulator

# Criar um programa de teste
make test_program

# Verificar vazamentos de memória
make memcheck
```

## Monitoramento do Sistema
O sistema oferece comandos para monitorar o estado da simulação:

1. Listar Processos: Exibe todos os processos no sistema com seus estados
2. Exibir Memória: Mostra o mapa de memória e estatísticas de uso
3. Gerenciar Processos: Criar, iniciar e finalizar processos

## Créditos
Desenvolvido como projeto educacional para a disciplina de Sistemas Operacionais por:
- Breno Bernal
- Felipe Matheus Possari
- Matheus Thomé da Silva
- Santiago Martins

## Notas de Implementação
- O simulador utiliza usleep() para simular o tempo de execução das instruções
- O mecanismo de paginação suporta até 16 páginas por processo
- O algoritmo do relógio (Clock) é usado para substituição de páginas
-       A detecção de deadlocks não está implementada na versão atual