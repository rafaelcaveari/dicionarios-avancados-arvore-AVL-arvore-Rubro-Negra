#include <iostream>
#include <chrono> //Medição de tempo de execução de um trecho de código
#include <map> //Biblioteca padrão para comparar com a implementação de uma árvore rubro-negra

//Usado para calcular o tempo de execução de um trecho de código
class Timer
{
public:
    Timer() : beg_(clock_::now()) {}
    void reset() { beg_ = clock_::now(); }
    double elapsed() const {
        return std::chrono::duration_cast<second_>
            (clock_::now() - beg_).count(); }

private:
    typedef std::chrono::high_resolution_clock clock_;
    typedef std::chrono::duration<double, std::ratio<1> > second_;
    std::chrono::time_point<clock_> beg_;
};


//Usei como referência as aulas do professor André Backes, disponíveis em http://www.facom.ufu.br/~backes/
//Foi implementado uma variação da Árvore Rubro-Negra para faciliar
//Árvore Rubro-Negra Caída Para a Esquerda - Left-Leaning Red–black (LLRB), proposta por Robert Sedgewick (2008)
//Propriedades da Árvore Rubro-Negra:
//- Todo nó deve ser preto ou vermelho (esquema de coloração para manter o balanceamento da árvore)
//- A raiz é sempre preta
//- Todo nó folha (NULL) é preto (como toda folha possui dois ponteiros NULL, pode-se ignorar isso para representar a árvore fins práticos e didáticos)
//- Se um nó é vemelho, então os seus filhos são pretos (não existem nós vermelhos consecutivos)
//- Todos os caminhos de um nó para os nós folhas descendentes contém o mesmo número de nós pretos
//- Permite rebalanceamento local para manter as propriedades da árvore (rotações e ajuste de cores)
//- No pior caso, os algoritmos das operações serão O(log(n))

// A LLRB atenderá a todas essas propriedades, adicionando-se mais uma:
//- Se um nó é vermelho, então ele será filho esquerdo do seu pai
// Dessa forma caímos na implementação de uma árvore 2-3 (não é uma árvore binária)
// Na árvore 2-3, cada nó pode armazenar um ou dois valores e, dependendo da quantidade de valores armazenados, ter dois (um valor) ou três (dois valores) filhos
// Funcionamento semelhante a árvore binária, agora possuindo também um subarvore do meio, onde estarão os elementos
// maiores do que o primeiro, mas menores que o segundo valor do nó pai
// A implementação corresponde a implementação de uma árvore 2-3 se considerarmos que o nó vemelho será sempre o valor menor
// de um nó contendo dois valores e três subárvores
// Assim, balancear a árvore rubro-negra equivale a manipular uma árvore 2-3, uma tarefa muito mais simples do que manipular uma árvore AVL ou uma rubro-negra convencional
// Com exceção da "inserção" e da "remoção", as demais funções (criar, liberar e buscar) da LLRB são idênticas a de uma Árvore Binária

// As operações de busca, inserção e remoção de elementos possuem complexidade O(log N), no qual N é o número de elementos da árvore), que são aplicados a árvore de busca binária

//Definição das cores
#define RED 1
#define BLACK 0

typedef struct NO* ArvLLRB;

//Definição do nó do dicionário. Tipo de dado armazenado
struct NO{
    int info; //Informação propriamente dita
    int chave; //Chave associada à informação
    struct NO *esq; //Filho esquerdo
    struct NO *dir; //Filho direito
    int cor; //Cor do nó
};

//Cria a árvore alocando o nó raiz e retornando o ponteiro (ponteiro para ponteiro, fica mais fácil mudar quem é a raiz da árvore, se necessário)
//Idêntica à árvore binária
ArvLLRB* cria_ArvLLRB(){
    ArvLLRB* raiz = (ArvLLRB*) malloc(sizeof(ArvLLRB));
    if(raiz != NULL){//Erro na alocação de memória
        *raiz = NULL;
    }
    return raiz;
}

//Liberar um nó
void libera_NO(struct NO* no){
    if(no == NULL) //Se já está vazia, não tem nada a fazer.
        return;
    libera_NO(no->esq); //Caso contrário libera recursivamente todos os nós.
    libera_NO(no->dir);
    free(no); //Como usei malloc, usar o free ao invés de delete.
    no = NULL;
}

//Liberar toda a árvore, utilizando a função libera_NO. Igual da árvore binária
void libera_ArvLLRB(ArvLLRB* raiz){
    if(raiz == NULL)
        return;
    libera_NO(*raiz);//libera cada nó
    free(raiz);//libera a raiz
}

//A consulta é idêntica a uma Àrvore Binária. Por padrão vamos buscar pela chave
int consulta_ArvLLRB(ArvLLRB *raiz, int valor){
    if(raiz == NULL) //Árvopre vazia?
        return 0;//Não encontrou
    struct NO* atual = *raiz;
    while(atual != NULL){
        if(valor == atual->chave){ //Valor procurado está no nó atual
            return 1; //Encontrou
        }
        if(valor > atual->chave) //Se o valor procurado for maior que o nó atual, procurar na subávore direita
            atual = atual->dir;
        else
            atual = atual->esq; //Se o valor procurado for menor que o nó atual, procurar na subávore esquerda
    }
    return 0; //Não encontrou
}

//==================Implementação das rotações==========================
//Diferente da AVL, só temos dois tipos de rotação: à esquerda e à direita
//Dado um conjunto de três nós, a rotação visa deslocar um nó vermelho que esteja à esquerda para à direita e vice-versa

//Rotação à esquerda
//Apenas rotaciona, não trata a violação de dois filhos vermelhos consecutivos, por exemplo
struct NO* rotacionaEsquerda(struct NO* A){
    struct NO* B = A->dir; //Nó B como filho direito de A
    A->dir = B->esq; // Mover B para o lugar de A
    B->esq = A; //A se torna filho esquerdo de B
    B->cor = A->cor; //B recebe a cor de A
    A->cor = RED; //Cor de A fica vermelha
    return B;
}

//Rotação à direita. Não trata outras violações da LLRB
struct NO* rotacionaDireita(struct NO* A){
    struct NO* B = A->esq; //Nó B como filho esquerdo de A
    A->esq = B->dir; // Mover B para o lugar de A
    B->dir = A; //A se torna filho direito de B
    B->cor = A->cor; //B recebe a cor de A
    A->cor = RED; //Cor de A fica vermelha
    return B;
}
//Função auxiliar para retornar a cor do nó
int cor(struct NO* H){
    if(H == NULL)
        return BLACK; //Propriedade da Árvore Rubro-Negra. Todo nó folha (NULL) é preto
    else
        return H->cor;
}

//Função auxiliar para inverter a cor do pai e de seus filhos
void trocaCor(struct NO* H){
    H->cor = !H->cor; //Inverte a cor do pai
    if(H->esq != NULL) //Inverte a cor do filho da esquerda
        H->esq->cor = !H->esq->cor;
    if(H->dir != NULL)//Inverte a cor do filho da direita
        H->dir->cor = !H->dir->cor;
}

// Função auxiliar utilizada na função insere_ArvLLRB
// Trata dos seguintes casos:
//-Se a raiz é NULL: insira o nó
//-Se o valor é menor do que a raiz: vá para a subárvore à esquerda
//-Se o valor é maior que a raiz: vá para a subárvore à direita
//Aplique o método recursivamente
//Ao voltar na recurssão, verificar as propriedades de cada subárvore e aplicar as rotações ou mudança de cor
//necessárias caso alguma propriedade foi violada
struct NO* insereNO(struct NO* H, int chave, int valor, int *resp){
    if(H == NULL){ //Ávore vazia ou estou em um nó folha
        struct NO *novo; //Crio o novo nó
        novo = (struct NO*)malloc(sizeof(struct NO)); //Alocação de memória
        if(novo == NULL){ //Verificação da alocação de memória
            *resp = 0;
            return NULL;
        }
        //Preenche o novo nó com as informações
        novo->info = valor;
        novo->chave = chave;
        novo->cor = RED; //Novo nó é sempre vermelho
        novo->dir = NULL; //Como é um nó folha, esq=NULL e dir=NULL
        novo->esq = NULL;
        *resp = 1; //Deu certo a inserção
        return novo;
    }

    if(chave == H->chave)
        *resp = 0;// Valor duplicado. Não faz nada e informa que não deu certo a inserção
    else{
        if(chave < H->chave)
            H->esq = insereNO(H->esq,chave,valor,resp); //Valor é menor que o pai, vou pra subárvore da esquerda
        else
            H->dir = insereNO(H->dir,chave,valor,resp); //Valor é maior que o pai, vou pra subárvore da direita
    }

    //Voltando da recursão verifico se as propriedades da árvore se mantém

    //nó vermelho é sempre filho à esquerda (LLRB)
    if(cor(H->dir) == RED && cor(H->esq) == BLACK)
        H = rotacionaEsquerda(H); //Rotação à esquerda

    //Filho e Neto são vermelhos
    //Filho vira pai de 2 nós vermelhos
    if(cor(H->esq) == RED && cor(H->esq->esq) == RED)
        H = rotacionaDireita(H); //Rotação à direita

    //2 filhos Vermelhos: troca cor!
    if(cor(H->esq) == RED && cor(H->dir) == RED)
        trocaCor(H); //Troca a cor do pai e dos filhos

    return H;
}

//Função de gerenciamento da inserção
int insere_ArvLLRB(ArvLLRB* raiz, int chave, int valor){
    int resp; //Guarda a resposta da inserção passada como parâmetro e usada na recursão
    *raiz = insereNO(*raiz,chave,valor,&resp); //Função responsável pela busca do local de inserção do nó
    if((*raiz) != NULL) //Árvore está vazia.
        (*raiz)->cor = BLACK; //Raiz é sempre preta

    return resp;
}

//Temos necessidade de outras 3 funções (além das rotações) para restabelecer o balanceamento da árvore
// e garantir que suas propriedades sejam respeitadas:
//-Mover um nó vermelho para a esquerda
//-Mover um nó vermelho para a direita
//-Arrumar o balanceamento

//Função auxiliar usada no rebalanceamento da árvore
//Trata 3 situações:
//- Se o filho direito é vermelho: rotação à esquerda
//- Se o filho esquerdo e o neto da esquerda são vermelhos: rotação à direita
//- Se ambos os filhos são vermelhos: trocar a cor do pai e dos filhos
struct NO* balancear(struct NO* H){
    if(cor(H->dir) == RED)//nó vermelho é sempre filho à esquerda
        H = rotacionaEsquerda(H);

    //Filho da esquerda e neto da esquerda são vermelhos
    if(H->esq != NULL && cor(H->esq) == RED && cor(H->esq->esq) == RED)
        H = rotacionaDireita(H);

    //2 filhos Vermelhos: troca cor!
    if(cor(H->esq) == RED && cor(H->dir) == RED)
        trocaCor(H);
    return H;
}

//Função auxiliar para mover um nó vermelho para a esquerda e restabelecer o balanceamento da árvore
//Essas funções movimentam um nó vermelho para a subárvore esquerda ou direita, dependendo da situação em que se encontra
struct NO* move2EsqRED(struct NO* H){
    trocaCor(H); //Troca a cor do nó e dos filhos
    if(cor(H->dir->esq) == RED){ //Filho a esquerda do filho direito é vermelho
        H->dir = rotacionaDireita(H->dir); //Aplica rotação à direita no filho direito
        H = rotacionaEsquerda(H); //Rotação à esquerda no pai e seus filhos
        trocaCor(H); //Troca a cor do nó e dos filhos
    }
    return H;
}

//Função auxiliar para mover um nó vermelho para a direita e restabelecer o balanceamento da árvore
//Essas funções movimentam um nó vermelho para a subárvore esquerda ou direita, dependendo da situação em que se encontra
struct NO* move2DirRED(struct NO* H){
    trocaCor(H); //Troca a cor do nó e dos filhos
    if(cor(H->esq->esq) == RED){ //Filho a esquerda do filho esquerdo é vermelho
        H = rotacionaDireita(H); //Aplicar rotação à direita no pai
        trocaCor(H);//Troca a cor do nó e dos filhos
    }
    return H;
}

//Remoção
// Existem 3 tipos de remoção:
//- Nó folha (sem filhos)
//- Nó com 1 filho
//- Nó com 2 filhos
//Ao voltar na recursão, verificar as propriedades de cada subárvore e aplicar as rotações e mudanças de cor necessárias

struct NO* removerMenor(struct NO* H){
    if(H->esq == NULL){ //Não tem filho à esquerda
        free(H);
        return NULL;
    }
    if(cor(H->esq) == BLACK && cor(H->esq->esq) == BLACK)
        H = move2EsqRED(H);

    H->esq = removerMenor(H->esq); //Indo pra esquerda
    return balancear(H);
}

// Função auxiliar que procura pelo nó mais a esquerda (que é o menor nó)
struct NO* procuraMenor(struct NO* atual){
    struct NO *no1 = atual;
    struct NO *no2 = atual->esq;
    while(no2 != NULL){
        no1 = no2;
        no2 = no2->esq;
    }
    return no1;
}

//Função responsável pela busca do nó a ser removido
struct NO* remove_NO(struct NO* H, int valor){
    if(valor < H->chave){ //Valor a ser removido é menor que o valor do pai?
        if(cor(H->esq) == BLACK && cor(H->esq->esq) == BLACK)
            H = move2EsqRED(H); //Move para esquerda

        H->esq = remove_NO(H->esq, valor); //Vou para a subárvore da esquerda
    }else{
        if(cor(H->esq) == RED) //No esquerdo é vermelho,
            H = rotacionaDireita(H);

        if(valor == H->chave && (H->dir == NULL)){ //Nó folha. Só remove o nó
            free(H);
            return NULL;
        }

        if(cor(H->dir) == BLACK && cor(H->dir->esq) == BLACK)
            H = move2DirRED(H); //Move para direita

        if(valor == H->chave){
            struct NO* x = procuraMenor(H->dir); //Menor nó da subárvore da direita
            H->chave = x->chave; //Troco de lugar com o nó H
            H->dir = removerMenor(H->dir); //Remove o menor
        }else
            H->dir = remove_NO(H->dir, valor);
    }
    return balancear(H); //Antes de voltar na recursão, devemos balancear a árvore
}

//Função de gerenciamento da remoção
int remove_ArvLLRB(ArvLLRB *raiz, int valor){
    if(consulta_ArvLLRB(raiz,valor)){ //Verifica primeiro se existe o elemento a ser removido na árvore
        struct NO* h = *raiz;
        *raiz = remove_NO(h,valor); //Função responsável pela busca do nó a ser removido
        if(*raiz != NULL)
            (*raiz)->cor = BLACK; //Raiz deve ser preta
        return 1; //Deu certo
    }else
        return 0; //Deu errado
}

//Imprime um percurso em-ordem para testes
void emOrdem_ArvLLRB(ArvLLRB *raiz, int H){
    if(raiz == NULL)
        return;

    if(*raiz != NULL){
        emOrdem_ArvLLRB(&((*raiz)->esq),H+1);

        if((*raiz)->cor == RED)
            printf("R - Chave %d: - Valor: %d\n\n",(*raiz)->chave,(*raiz)->info);
        else
            printf("B - Chave %d: - Valor: %d\n\n",(*raiz)->chave,(*raiz)->info);

        emOrdem_ArvLLRB(&((*raiz)->dir),H+1);
    }
}

//Exemplo de uso
int main(){
    Timer tmr;
    int encontrouMapa=0, encontrouLLRB=0; //Variáveis de controle
    double t = tmr.elapsed(); //Tempo de execução do trecho de código em segundos (s)
    int N = 11000000; //Número de nós da árvore/Mapa. Range 500.000-15.000.000
    int num_rodadas = 500000; //Número de rodadas para pegar a média de tempos e diminuir distorções
    double somaMAPA = 0.0, somaLLRB = 0.0; //Tempos para cada implementação
    double *tempo_LLRB, *tempo_MAPA;
    tempo_LLRB = (double *)malloc(N * sizeof(double)); //Vetores para armazenar os tempos de inserção de cada valor
    tempo_MAPA = (double *)malloc(N * sizeof(double));
    int res; //Resposta se a inserção ocorreu corretamente (1) ou não (0)
    std::cout<<"N = "<<N<<" - Numero de rodadas = "<<num_rodadas<<std::endl;
    std::cout<<std::endl;

    //=============================Mapa============================================
    std::map<int, int> Mapa; //Para comparar com a implementação de árvore LLRB
    std::cout<<"Inserindo elementos no Mapa..."<<std::endl;
    std::cout<<std::endl;
    for(int i=0;i<N;i++){ //Cria um Mapa adicionando os elementos na seguência: 0, 2, 3, 4...N-1
        Mapa.insert(std::pair<int, int>(i, i));
        }


    std::cout<<"Buscando elementos e calculando tempo..."<<std::endl;
    std::cout<<std::endl;
    for(int j=0;j<num_rodadas;j++) //Fazer várias rodadas para tentar evitar muitas distorções
    {
        tmr.reset();
        //======================Zera o contador. Código a ser medido deve estar depois dessa linha
        if(Mapa.find(j)!= Mapa.end())
        {
        //======================Para o contador. Código a ser medido deve estar antes dessa linha
            t = tmr.elapsed();
            encontrouMapa=1; //Seta pra 1 pra verificar no final se encontrou o elemento sem problemas
        }
        tempo_MAPA[j] = t;
    }

    for(int j=0;j<num_rodadas;j++) //Calcula o tempo total para encontrar o elemento em num_rodadas rodadas
    {
        somaMAPA = somaMAPA + tempo_MAPA[j];
    }
    free(tempo_MAPA); //Libera o vetor
    std::cout << "Tempo medio para encontrar elemento no Mapa com " << N << " nos = " << somaMAPA/num_rodadas << " segundos" << std::endl; //Exibe na tela os valores
    std::cout<<std::endl;
    //=============================LLRB============================================
    ArvLLRB *llrb = cria_ArvLLRB();//Cria árvore LLRB
    std::cout<<"Inserindo elementos na LLRB..."<<std::endl;
    std::cout<<std::endl;
    for(int i=0;i<N;i++) //Insere elementos na árvore
    {
        res = insere_ArvLLRB(llrb,i,i);
    }

    std::cout<<"Buscando elementos e calculando tempo..."<<std::endl;
    std::cout<<std::endl;
    for(int j=0;j<num_rodadas;j++)
    {
        tmr.reset();
        //======================Zera o contador. Código a ser medido deve estar depois dessa linha
        if(consulta_ArvLLRB(llrb,j)== 1) //Zero não encontrou, 1 encontrou
        {
        //======================Para o contador. Código a ser medido deve estar antes dessa linha
            t = tmr.elapsed();
            encontrouLLRB=1; //Seta pra 1 pra verificar no final se encontrou o elemento sem problemas
        }
        tempo_LLRB[j] = t;
    }

    libera_ArvLLRB(llrb);

    for(int j=0;j<num_rodadas;j++) //Calcula o tempo total para encontrar o elemento em num_rodadas rodadas
    {
        somaLLRB = somaLLRB + tempo_LLRB[j];
    }
    std::cout << "Tempo medio para encontrar elemento na LLRB com " << N << " nos = " << somaLLRB/num_rodadas << " segundos" << std::endl; //Exibe na tela
    std::cout<<std::endl;
    free(tempo_LLRB); //Libera o vetor

    if((encontrouLLRB==0)||(encontrouMapa==0))
        std::cout << "Elemento nao encontrado. Desconsiderar dados!!!" << std::endl;

    return 0;
}
