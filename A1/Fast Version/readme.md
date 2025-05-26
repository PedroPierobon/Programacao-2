# LEIAME — Trabalho 2 de Programação 2

**Aluno:** Pedro Pierobon Marynowski  
**GRR:** 20241729

---

## 📁 Arquivos e diretórios do pacote

- `main.c` — Arquivo com a função principal e controle do fluxo de execução.
- `vinac.h` — Definições de structs usadas em todo o projeto.
- `lz.c` — Implementação do algoritmo de compressão LZ (Lempel-Ziv).
- `lz.h` — Cabeçalho da compressão com funções auxiliares.
- `Makefile` — Automação da compilação do projeto.

---

## ⚙️ Algoritmos, estruturas de dados e decisões de implementação

O projeto utiliza um sistema de arquivamento com compressão baseada em uma implementação própria do algoritmo **LZ (Lempel-Ziv)**, adaptado para identificar padrões repetidos e reduzir o tamanho dos arquivos armazenados.

As estruturas principais utilizadas foram:

```c
struct infoMember {
    char name[MAX_NAME_SIZE];
    off_t originalSize;
    long diskSize;
    time_t modTime;
    int pos;
    long offset;
};

struct directory {
    size_t N;
    struct infoMember *members;
};

Dificuldades encontradas

Uma dificuldade significativa foi o gerenciamento de offsets corretos para múltiplos membros, especialmente ao lidar com operações de inserção, leitura e remoção. Problemas com alocação dinâmica de memória e manipulação de arquivos binários foram comuns, sendo contornados com uso cuidadoso de fseek, fread e fwrite, além de testes com arquivos de diferentes tamanhos.

Bugs conhecidos

    Inserir o mesmo membro várias vezes causa repetição desnecessária no índice e sobrescrita dos dados.

    A operação de remoção (remove) pode causar segfault, possivelmente relacionado a ponteiros nulos ou manipulação incorreta de índices após realocação.