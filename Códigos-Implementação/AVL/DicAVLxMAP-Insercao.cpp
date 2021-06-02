#include <iostream>
#include <chrono> //Medição de tempo de execução de um trecho de código
#include <map> //Biblioteca padrão para comparar com a implementação de uma árvore AVL

//Usei como referência as aulas do professor André Backes, disponíveis em http://www.facom.ufu.br/~backes/
//Com exceção da "inserção" e da "remoção", as demais funções (criar, liberar e busca) da "Árvore AVL" são idênticas a de uma Árvore Binária
//Importante utilizar a flag -O3 para otimizar o código e obter resultados coerentes
//As operações de busca, inserção e remoção de elementos possuem complexidade O(log N), no qual N é o número de elementos da árvore), que são aplicados a árvore de busca binária

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


typedef struct NO* ArvAVL;

//Definição do nó do dicionário
struct NO{
    int info;
    int chave; //Chave associada à informação
    int altura; //Armazenar a altura daquela subárvore. Usado para cálculo do Fator de Balanceamento sempre que adicionar ou remover algum elemento.
    struct NO *esq; //Filho esquerdo
    struct NO *dir; //Filho direito
};

//Cria a árvore alocando o nó raiz e retornando o ponteiro

ArvAVL* cria_ArvAVL(){
    ArvAVL* raiz = (ArvAVL*) malloc(sizeof(ArvAVL));
    if(raiz != NULL) //Erro na alocação de memória
        *raiz = NULL;
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

//Liberar toda a árvore, utilizando a função libera_NO
void libera_ArvAVL(ArvAVL* raiz){
    if(raiz == NULL)
        return;
    libera_NO(*raiz);//libera cada nó
    free(raiz);//libera a raiz
}

//Função auxiliar que retorna a altura de um determinado nó
int altura_NO(struct NO* no){
    if(no == NULL)
        return -1; //Se o nó não existe, definimos que a altura é -1
    else
    return no->altura;
}

//Função auxiliar para calcular o FB do nó
int fatorBalanceamento_NO(struct NO* no){
    return labs(altura_NO(no->esq) - altura_NO(no->dir)); //Definição: FB = h(esqu) - h(dir). labs retorna o módulo.
}

//Função auxiliar para calcular a maior altura entre duas subárvores
int maior(int x, int y){
    if(x > y)
        return x;
    else
        return y;
}

//Imprime um percurso pré-ordem para testes
void preOrdem_ArvAVL(ArvAVL *raiz){
    if(raiz == NULL)
        return;
    if(*raiz != NULL){
        printf("Chave %d: - Valor: %d\n",(*raiz)->chave,(*raiz)->info);
        preOrdem_ArvAVL(&((*raiz)->esq));
        preOrdem_ArvAVL(&((*raiz)->dir));
    }
}

//Imprime um percurso em-ordem para testes
void emOrdem_ArvAVL(ArvAVL *raiz){
    if(raiz == NULL)
        return;
    if(*raiz != NULL){
        emOrdem_ArvAVL(&((*raiz)->esq));
        printf("Chave %d: - Valor: %d\n",(*raiz)->chave,(*raiz)->info);
        emOrdem_ArvAVL(&((*raiz)->dir));
    }
}

//Imprime um percurso pós-ordem para testes
void posOrdem_ArvAVL(ArvAVL *raiz){
    if(raiz == NULL)
        return;
    if(*raiz != NULL){
        posOrdem_ArvAVL(&((*raiz)->esq));
        posOrdem_ArvAVL(&((*raiz)->dir));
        printf("Chave %d:  - Valor: %d\n",(*raiz)->chave,(*raiz)->info);
    }
}

//A consulta é idêntica a uma Àrvore Binária
int consulta_ArvAVL(ArvAVL *raiz, int valor){
    if(raiz == NULL) //Árvore vazia?
        return 0; //Não encontrou
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

//Rotação à direita (LL)
void RotacaoLL(ArvAVL *A){
    //printf("RotacaoLL\n");
    struct NO *B; //Nova raiz
    B = (*A)->esq; //Filho da esquerda vira a nova raiz
    (*A)->esq = B->dir; //O filho da direita do filho da esquerda vira filho da esquerda do filho da direita
    B->dir = *A; //Raiz original vira filho da direita da nova raiz
    (*A)->altura = maior(altura_NO((*A)->esq),altura_NO((*A)->dir)) + 1; //Atualiza a altura da raiz original
    B->altura = maior(altura_NO(B->esq),(*A)->altura) + 1; //Atualiza a altura da nova raiz
    *A = B;
}

//Rotação à esquerda (RR)
void RotacaoRR(ArvAVL *A){
    //printf("RotacaoRR\n");
    struct NO *B; //Nova raiz
    B = (*A)->dir; //Filho da direita vira a nova raiz
    (*A)->dir = B->esq; //O filho da esquerda do filho da direita vira filho da direita do filho da esquerda
    B->esq = (*A); //Raiz original vira filho da esquerda da nova raiz
    (*A)->altura = maior(altura_NO((*A)->esq),altura_NO((*A)->dir)) + 1; //Atualiza a altura da raiz original
    B->altura = maior(altura_NO(B->dir),(*A)->altura) + 1; //Atualiza a altura da nova raiz
    (*A) = B;
}

//Rotação dupla à direita (LR). São duas rotações simples.
void RotacaoLR(ArvAVL *A){
    RotacaoRR(&(*A)->esq); //Rotação à esquerda na sub-árvore da esquerda
    RotacaoLL(A); //Rotação à direita na árvore original
}

//Rotação dupla à esquerda (RL). São duas rotações simples.
void RotacaoRL(ArvAVL *A){
    RotacaoLL(&(*A)->dir); //Rotação à direita na subárvore da direita
    RotacaoRR(A); //Rotação à esquerda na árvore original
}
//==================================================

//Inserir um valor na AVL
//Se a raiz for vazia ou folha, inserir o nó
//Se o valor a ser inserido for menor que a raiz, vá para a subárvore esquerda
//Se o valor a ser inserido for maior que a raiz, vá para a subárvore direita
//Aplicar o método recursivamente
//Ao voltar na recursão, recalcular o valor da altura de cada subárvore
//Aplicar as rotações necessárias se FB = +2 ou -2
int insere_ArvAVL(ArvAVL *raiz, int chave, int valor){
    int res;
    if(*raiz == NULL){//Se a raiz for vazia ou for uma folha, inserir o nó
        struct NO *novo;
        novo = (struct NO*)malloc(sizeof(struct NO));
        if(novo == NULL)
            return 0; //Alocação de memória falhou
        novo->info = valor;
        novo->chave = chave;
        novo->altura = 0; //Folha ou raiz
        novo->esq = NULL;
        novo->dir = NULL;
        *raiz = novo;
        return 1; //Inserção ocorreu corretamente
    }

    struct NO *atual = *raiz;
    if(chave < atual->chave){ //Se o valor a ser inserido for menor que a raiz, vá para a subárvore esquerda
        if((res = insere_ArvAVL(&(atual->esq), chave, valor)) == 1){ //Aplicar o método recursivamente inserindo os valores
            if(fatorBalanceamento_NO(atual) >= 2){ //Balanceamento. Aplicar as rotações necessárias se FB = +2 ou -2
                if(chave < (*raiz)->esq->chave ){
                    RotacaoLL(raiz); //Valor a ser inserido está na parte "externa" da árvore. Rotação à direita.
                }else{
                    RotacaoLR(raiz); //Valor a ser inserido está na parte "interna" da árvore. Rotação dupla à direita
                }
            }
        }
    }else{
        if(chave > atual->chave){ //Se o valor a ser inserido for maior que a raiz, vá para a subárvore direita
            if((res = insere_ArvAVL(&(atual->dir), chave, valor)) == 1){ //Aplicar o método recursivamente inserindo os valores
                if(fatorBalanceamento_NO(atual) >= 2){ //Balanceamento. Aplicar as rotações necessárias se FB = +2 ou -2
                    if((*raiz)->dir->chave < chave){
                        RotacaoRR(raiz); //Valor a ser inserido está na parte "externa" da árvore. Rotação à esquerda.
                    }else{
                        RotacaoRL(raiz); //Valor a ser inserido está na parte "interna" da árvore. Rotação dupla à esquerda
                    }
                }
            }
        }else{
            return 0; //Valor já encontra-se na árvore e não será inserido
        }
    }

    atual->altura = maior(altura_NO(atual->esq),altura_NO(atual->dir)) + 1; //Ao voltar na recursão, recalcular o valor da altura de cada subárvore

    return res; //Retorna 1 se inserção ocorreu corretamente ou 0 (alocação de memória falhou ou chave já se encontra na árvore)
}

//Função auxiliar utilizada na remoção de um nó. Trata da remoção de um nó com 2 filhos
//Procura pelo nó mais a esquerda
struct NO* procuraMenor(struct NO* atual){
    struct NO *no1 = atual;
    struct NO *no2 = atual->esq;
    while(no2 != NULL){
        no1 = no2;
        no2 = no2->esq;
    }
    return no1;
}

//Remove nó da árvore. Podemos ter 3 situações:
// - nó folha (sem filhos)
// - nó com 1 filho
// - nó com 2 filhos
//Devemos também tratar o balanceamento da mesma forma que na inserção
//Remover um nó da subárvore da direita equivale a inserir um nó na subárvore da esquerda
int remove_ArvAVL(ArvAVL *raiz, int valor){
	if(*raiz == NULL){// Não se pode remover de uma árvore vazia
	    return 0; //Remoção falhou!
	}

    int res; //Resposta da remoção, já que é uma função recursiva
	if(valor < (*raiz)->chave){ //Se o valor a remover for menor que a raiz, vou para a subárvore da esquerda recursivamente
	    if((res = remove_ArvAVL(&(*raiz)->esq,valor)) == 1){ //Se conseguir remover, então calculo o FB para ver se preciso balancear
            if(fatorBalanceamento_NO(*raiz) >= 2){
                if(altura_NO((*raiz)->dir->esq) <= altura_NO((*raiz)->dir->dir)) //Diferença de altura dos filhos. Removi da esquerda, rebalancear na direita
                    RotacaoRR(raiz); //Nó está na parte "externa" da árvore. Rotação à esquerda.
                else
                    RotacaoRL(raiz); //Nó está na parte "interna" da árvore. Rotação dupla à esquerda
            }
	    }
	}

	if((*raiz)->chave < valor){ //Se o valor a remover for maior que a raiz, vou para a subárvore da direita recursivamente
	    if((res = remove_ArvAVL(&(*raiz)->dir, valor)) == 1){ //Se conseguir remover, então calculo o FB para ver se preciso balancear
            if(fatorBalanceamento_NO(*raiz) >= 2){
                if(altura_NO((*raiz)->esq->dir) <= altura_NO((*raiz)->esq->esq)) //Diferença de altura dos filhos. Remove da direita, rebalancear na esquerda
                    RotacaoLL(raiz); //Nó está na parte "externa" da árvore. Rotação à direita
                else
                    RotacaoLR(raiz); //Nó está na parte "interna" da árvore. Rotação dupla à direita
            }
	    }
	}

	if((*raiz)->chave == valor){
	    if(((*raiz)->esq == NULL || (*raiz)->dir == NULL)){// Nó tem 1 filho ou nenhum
			struct NO *oldNode = (*raiz);
			if((*raiz)->esq != NULL) //Qual filho é folha?
                *raiz = (*raiz)->esq;
            else
                *raiz = (*raiz)->dir;
			free(oldNode);
		}else { // Nó tem 2 filhos
			struct NO* temp = procuraMenor((*raiz)->dir); //Procurar pelo menor valor da subárvore da direita
			(*raiz)->chave = temp->chave; //Substituir pelo nó mais a esquerda da subárvore da direita
			remove_ArvAVL(&(*raiz)->dir, (*raiz)->chave); //Remove recursivamente para tratar os problemas que podemos ter
            if(fatorBalanceamento_NO(*raiz) >= 2){ //Tratar o balanceamento após a remoção. Removi da suárvore da direita, tenho que balancear a subárvore da esquerda
				if(altura_NO((*raiz)->esq->dir) <= altura_NO((*raiz)->esq->esq))
					RotacaoLL(raiz); //Nó está na parte "externa" da árvore. Rotação à direita
				else
					RotacaoLR(raiz); //Nó está na parte "interna" da árvore. Rotação dupla à direita
			}
		}
		if (*raiz != NULL)
            (*raiz)->altura = maior(altura_NO((*raiz)->esq),altura_NO((*raiz)->dir)) + 1; //Atualiza as alturas
		return 1;
	}

	(*raiz)->altura = maior(altura_NO((*raiz)->esq),altura_NO((*raiz)->dir)) + 1;

	return res;
}

//Exemplo de uso
int main(){
    Timer tmr;
    double t = tmr.elapsed(); //Tempo de execução do trecho de código em segundos (s)
    int N = 8500000; //Número de nós da árvore/Mapa. Range 500.000-15.000.000
    int num_rodadas = 1; //Número de rodadas para pegar a média de tempos e evitar distorções
    double somaMAPA = 0.0, somaAVL = 0.0; //Tempos para cada implementação
    double *tempo_AVL, *tempo_MAPA;
    tempo_AVL = (double *)malloc(N * sizeof(double)); //Vetores para armazenar os tempos de inserção de cada valor
    tempo_MAPA = (double *)malloc(N * sizeof(double));
    std::cout<<"N = "<<N<<std::endl;
    std::cout<<std::endl;
    ArvAVL* avl; //Ponteiro para ponteiro. Árvore AVL implementada
    int res; //Resposta se a inserção ocorreu corretamente (1) ou não (0)

    //=============================Mapa============================================
    //Cria um Mapa adicionando os elementos na seguência: 0, 1, 2, 3...N-1
    std::cout<<"Inserindo elementos no Mapa e calculando o tempo..."<<std::endl;
    std::cout<<std::endl;
    for(int j=0;j<num_rodadas;j++)
    {
        std::map<int, int> Mapa; //Para comparar com a implementação de árvore AVL. Redefino a cada rodada
        for(int i=0;i<N;i++){
            tmr.reset();
            //======================Zera o contador. Código a ser medido deve estar depois dessa linha
            Mapa.insert(std::pair<int, int>(i, i));
            //======================Para o contador. Código a ser medido deve estar antes dessa linha
            t = tmr.elapsed();
            tempo_MAPA[i] = t;
        }
    }
    std::cout<<"Calculando o tempo medio de insercao..."<<std::endl;
    std::cout<<std::endl;
    for(int j=0;j<N;j++) //Calcula o tempo total para inserir N valores no Mapa
    {
        somaMAPA = somaMAPA + tempo_MAPA[j];
    }
    std::cout << "Tempo medio para preencher Mapa com " << N << " nos = " << somaMAPA << " segundos" << std::endl; //Exibe na tela os valores
    std::cout<<std::endl;
    free(tempo_MAPA);
    //=============================AVL============================================
    std::cout<<"Inserindo elementos na AVL e calculando o tempo..."<<std::endl;
    std::cout<<std::endl;
    for(int j=0;j<num_rodadas;j++)
    {
        avl = cria_ArvAVL(); //Cria árvore AVL
        for(int i=0;i<N;i++)
        {
            tmr.reset();
            //======================Zera o contador. Código a ser medido deve estar depois dessa linha
            res = insere_ArvAVL(avl,i,i);
            //======================Para o contador. Código a ser medido deve estar antes dessa linha
            t = tmr.elapsed();
            tempo_AVL[i] = t;
        }
        libera_ArvAVL(avl);
    }



    std::cout<<"Calculando o tempo medio de insercao..."<<std::endl;
    std::cout<<std::endl;
    for(int j=0;j<N;j++) //Calcula o tempo total para inserir N valores na AVL
    {
        somaAVL = somaAVL + tempo_AVL[j];
    }
    std::cout << "Tempo medio para preencher AVL com " << N << " nos = " << somaAVL << " segundos" << std::endl; //Exibe na tela
    std::cout<<std::endl;
    free(tempo_AVL); //Libera o vetor


    return 0;
}
