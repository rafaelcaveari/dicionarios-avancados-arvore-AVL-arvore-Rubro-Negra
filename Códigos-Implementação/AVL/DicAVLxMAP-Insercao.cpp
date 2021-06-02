#include <iostream>
#include <chrono> //Medi��o de tempo de execu��o de um trecho de c�digo
#include <map> //Biblioteca padr�o para comparar com a implementa��o de uma �rvore AVL

//Usei como refer�ncia as aulas do professor Andr� Backes, dispon�veis em http://www.facom.ufu.br/~backes/
//Com exce��o da "inser��o" e da "remo��o", as demais fun��es (criar, liberar e busca) da "�rvore AVL" s�o id�nticas a de uma �rvore Bin�ria
//Importante utilizar a flag -O3 para otimizar o c�digo e obter resultados coerentes
//As opera��es de busca, inser��o e remo��o de elementos possuem complexidade O(log N), no qual N � o n�mero de elementos da �rvore), que s�o aplicados a �rvore de busca bin�ria

//Usado para calcular o tempo de execu��o de um trecho de c�digo
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

//Defini��o do n� do dicion�rio
struct NO{
    int info;
    int chave; //Chave associada � informa��o
    int altura; //Armazenar a altura daquela sub�rvore. Usado para c�lculo do Fator de Balanceamento sempre que adicionar ou remover algum elemento.
    struct NO *esq; //Filho esquerdo
    struct NO *dir; //Filho direito
};

//Cria a �rvore alocando o n� raiz e retornando o ponteiro

ArvAVL* cria_ArvAVL(){
    ArvAVL* raiz = (ArvAVL*) malloc(sizeof(ArvAVL));
    if(raiz != NULL) //Erro na aloca��o de mem�ria
        *raiz = NULL;
    return raiz;
}

//Liberar um n�
void libera_NO(struct NO* no){
    if(no == NULL) //Se j� est� vazia, n�o tem nada a fazer.
        return;
    libera_NO(no->esq); //Caso contr�rio libera recursivamente todos os n�s.
    libera_NO(no->dir);
    free(no); //Como usei malloc, usar o free ao inv�s de delete.
    no = NULL;
}

//Liberar toda a �rvore, utilizando a fun��o libera_NO
void libera_ArvAVL(ArvAVL* raiz){
    if(raiz == NULL)
        return;
    libera_NO(*raiz);//libera cada n�
    free(raiz);//libera a raiz
}

//Fun��o auxiliar que retorna a altura de um determinado n�
int altura_NO(struct NO* no){
    if(no == NULL)
        return -1; //Se o n� n�o existe, definimos que a altura � -1
    else
    return no->altura;
}

//Fun��o auxiliar para calcular o FB do n�
int fatorBalanceamento_NO(struct NO* no){
    return labs(altura_NO(no->esq) - altura_NO(no->dir)); //Defini��o: FB = h(esqu) - h(dir). labs retorna o m�dulo.
}

//Fun��o auxiliar para calcular a maior altura entre duas sub�rvores
int maior(int x, int y){
    if(x > y)
        return x;
    else
        return y;
}

//Imprime um percurso pr�-ordem para testes
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

//Imprime um percurso p�s-ordem para testes
void posOrdem_ArvAVL(ArvAVL *raiz){
    if(raiz == NULL)
        return;
    if(*raiz != NULL){
        posOrdem_ArvAVL(&((*raiz)->esq));
        posOrdem_ArvAVL(&((*raiz)->dir));
        printf("Chave %d:  - Valor: %d\n",(*raiz)->chave,(*raiz)->info);
    }
}

//A consulta � id�ntica a uma �rvore Bin�ria
int consulta_ArvAVL(ArvAVL *raiz, int valor){
    if(raiz == NULL) //�rvore vazia?
        return 0; //N�o encontrou
    struct NO* atual = *raiz;
    while(atual != NULL){
        if(valor == atual->chave){ //Valor procurado est� no n� atual
            return 1; //Encontrou
        }
        if(valor > atual->chave) //Se o valor procurado for maior que o n� atual, procurar na sub�vore direita
            atual = atual->dir;
        else
            atual = atual->esq; //Se o valor procurado for menor que o n� atual, procurar na sub�vore esquerda
    }
    return 0; //N�o encontrou
}


//==================Implementa��o das rota��es==========================

//Rota��o � direita (LL)
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

//Rota��o � esquerda (RR)
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

//Rota��o dupla � direita (LR). S�o duas rota��es simples.
void RotacaoLR(ArvAVL *A){
    RotacaoRR(&(*A)->esq); //Rota��o � esquerda na sub-�rvore da esquerda
    RotacaoLL(A); //Rota��o � direita na �rvore original
}

//Rota��o dupla � esquerda (RL). S�o duas rota��es simples.
void RotacaoRL(ArvAVL *A){
    RotacaoLL(&(*A)->dir); //Rota��o � direita na sub�rvore da direita
    RotacaoRR(A); //Rota��o � esquerda na �rvore original
}
//==================================================

//Inserir um valor na AVL
//Se a raiz for vazia ou folha, inserir o n�
//Se o valor a ser inserido for menor que a raiz, v� para a sub�rvore esquerda
//Se o valor a ser inserido for maior que a raiz, v� para a sub�rvore direita
//Aplicar o m�todo recursivamente
//Ao voltar na recurs�o, recalcular o valor da altura de cada sub�rvore
//Aplicar as rota��es necess�rias se FB = +2 ou -2
int insere_ArvAVL(ArvAVL *raiz, int chave, int valor){
    int res;
    if(*raiz == NULL){//Se a raiz for vazia ou for uma folha, inserir o n�
        struct NO *novo;
        novo = (struct NO*)malloc(sizeof(struct NO));
        if(novo == NULL)
            return 0; //Aloca��o de mem�ria falhou
        novo->info = valor;
        novo->chave = chave;
        novo->altura = 0; //Folha ou raiz
        novo->esq = NULL;
        novo->dir = NULL;
        *raiz = novo;
        return 1; //Inser��o ocorreu corretamente
    }

    struct NO *atual = *raiz;
    if(chave < atual->chave){ //Se o valor a ser inserido for menor que a raiz, v� para a sub�rvore esquerda
        if((res = insere_ArvAVL(&(atual->esq), chave, valor)) == 1){ //Aplicar o m�todo recursivamente inserindo os valores
            if(fatorBalanceamento_NO(atual) >= 2){ //Balanceamento. Aplicar as rota��es necess�rias se FB = +2 ou -2
                if(chave < (*raiz)->esq->chave ){
                    RotacaoLL(raiz); //Valor a ser inserido est� na parte "externa" da �rvore. Rota��o � direita.
                }else{
                    RotacaoLR(raiz); //Valor a ser inserido est� na parte "interna" da �rvore. Rota��o dupla � direita
                }
            }
        }
    }else{
        if(chave > atual->chave){ //Se o valor a ser inserido for maior que a raiz, v� para a sub�rvore direita
            if((res = insere_ArvAVL(&(atual->dir), chave, valor)) == 1){ //Aplicar o m�todo recursivamente inserindo os valores
                if(fatorBalanceamento_NO(atual) >= 2){ //Balanceamento. Aplicar as rota��es necess�rias se FB = +2 ou -2
                    if((*raiz)->dir->chave < chave){
                        RotacaoRR(raiz); //Valor a ser inserido est� na parte "externa" da �rvore. Rota��o � esquerda.
                    }else{
                        RotacaoRL(raiz); //Valor a ser inserido est� na parte "interna" da �rvore. Rota��o dupla � esquerda
                    }
                }
            }
        }else{
            return 0; //Valor j� encontra-se na �rvore e n�o ser� inserido
        }
    }

    atual->altura = maior(altura_NO(atual->esq),altura_NO(atual->dir)) + 1; //Ao voltar na recurs�o, recalcular o valor da altura de cada sub�rvore

    return res; //Retorna 1 se inser��o ocorreu corretamente ou 0 (aloca��o de mem�ria falhou ou chave j� se encontra na �rvore)
}

//Fun��o auxiliar utilizada na remo��o de um n�. Trata da remo��o de um n� com 2 filhos
//Procura pelo n� mais a esquerda
struct NO* procuraMenor(struct NO* atual){
    struct NO *no1 = atual;
    struct NO *no2 = atual->esq;
    while(no2 != NULL){
        no1 = no2;
        no2 = no2->esq;
    }
    return no1;
}

//Remove n� da �rvore. Podemos ter 3 situa��es:
// - n� folha (sem filhos)
// - n� com 1 filho
// - n� com 2 filhos
//Devemos tamb�m tratar o balanceamento da mesma forma que na inser��o
//Remover um n� da sub�rvore da direita equivale a inserir um n� na sub�rvore da esquerda
int remove_ArvAVL(ArvAVL *raiz, int valor){
	if(*raiz == NULL){// N�o se pode remover de uma �rvore vazia
	    return 0; //Remo��o falhou!
	}

    int res; //Resposta da remo��o, j� que � uma fun��o recursiva
	if(valor < (*raiz)->chave){ //Se o valor a remover for menor que a raiz, vou para a sub�rvore da esquerda recursivamente
	    if((res = remove_ArvAVL(&(*raiz)->esq,valor)) == 1){ //Se conseguir remover, ent�o calculo o FB para ver se preciso balancear
            if(fatorBalanceamento_NO(*raiz) >= 2){
                if(altura_NO((*raiz)->dir->esq) <= altura_NO((*raiz)->dir->dir)) //Diferen�a de altura dos filhos. Removi da esquerda, rebalancear na direita
                    RotacaoRR(raiz); //N� est� na parte "externa" da �rvore. Rota��o � esquerda.
                else
                    RotacaoRL(raiz); //N� est� na parte "interna" da �rvore. Rota��o dupla � esquerda
            }
	    }
	}

	if((*raiz)->chave < valor){ //Se o valor a remover for maior que a raiz, vou para a sub�rvore da direita recursivamente
	    if((res = remove_ArvAVL(&(*raiz)->dir, valor)) == 1){ //Se conseguir remover, ent�o calculo o FB para ver se preciso balancear
            if(fatorBalanceamento_NO(*raiz) >= 2){
                if(altura_NO((*raiz)->esq->dir) <= altura_NO((*raiz)->esq->esq)) //Diferen�a de altura dos filhos. Remove da direita, rebalancear na esquerda
                    RotacaoLL(raiz); //N� est� na parte "externa" da �rvore. Rota��o � direita
                else
                    RotacaoLR(raiz); //N� est� na parte "interna" da �rvore. Rota��o dupla � direita
            }
	    }
	}

	if((*raiz)->chave == valor){
	    if(((*raiz)->esq == NULL || (*raiz)->dir == NULL)){// N� tem 1 filho ou nenhum
			struct NO *oldNode = (*raiz);
			if((*raiz)->esq != NULL) //Qual filho � folha?
                *raiz = (*raiz)->esq;
            else
                *raiz = (*raiz)->dir;
			free(oldNode);
		}else { // N� tem 2 filhos
			struct NO* temp = procuraMenor((*raiz)->dir); //Procurar pelo menor valor da sub�rvore da direita
			(*raiz)->chave = temp->chave; //Substituir pelo n� mais a esquerda da sub�rvore da direita
			remove_ArvAVL(&(*raiz)->dir, (*raiz)->chave); //Remove recursivamente para tratar os problemas que podemos ter
            if(fatorBalanceamento_NO(*raiz) >= 2){ //Tratar o balanceamento ap�s a remo��o. Removi da su�rvore da direita, tenho que balancear a sub�rvore da esquerda
				if(altura_NO((*raiz)->esq->dir) <= altura_NO((*raiz)->esq->esq))
					RotacaoLL(raiz); //N� est� na parte "externa" da �rvore. Rota��o � direita
				else
					RotacaoLR(raiz); //N� est� na parte "interna" da �rvore. Rota��o dupla � direita
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
    double t = tmr.elapsed(); //Tempo de execu��o do trecho de c�digo em segundos (s)
    int N = 8500000; //N�mero de n�s da �rvore/Mapa. Range 500.000-15.000.000
    int num_rodadas = 1; //N�mero de rodadas para pegar a m�dia de tempos e evitar distor��es
    double somaMAPA = 0.0, somaAVL = 0.0; //Tempos para cada implementa��o
    double *tempo_AVL, *tempo_MAPA;
    tempo_AVL = (double *)malloc(N * sizeof(double)); //Vetores para armazenar os tempos de inser��o de cada valor
    tempo_MAPA = (double *)malloc(N * sizeof(double));
    std::cout<<"N = "<<N<<std::endl;
    std::cout<<std::endl;
    ArvAVL* avl; //Ponteiro para ponteiro. �rvore AVL implementada
    int res; //Resposta se a inser��o ocorreu corretamente (1) ou n�o (0)

    //=============================Mapa============================================
    //Cria um Mapa adicionando os elementos na segu�ncia: 0, 1, 2, 3...N-1
    std::cout<<"Inserindo elementos no Mapa e calculando o tempo..."<<std::endl;
    std::cout<<std::endl;
    for(int j=0;j<num_rodadas;j++)
    {
        std::map<int, int> Mapa; //Para comparar com a implementa��o de �rvore AVL. Redefino a cada rodada
        for(int i=0;i<N;i++){
            tmr.reset();
            //======================Zera o contador. C�digo a ser medido deve estar depois dessa linha
            Mapa.insert(std::pair<int, int>(i, i));
            //======================Para o contador. C�digo a ser medido deve estar antes dessa linha
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
        avl = cria_ArvAVL(); //Cria �rvore AVL
        for(int i=0;i<N;i++)
        {
            tmr.reset();
            //======================Zera o contador. C�digo a ser medido deve estar depois dessa linha
            res = insere_ArvAVL(avl,i,i);
            //======================Para o contador. C�digo a ser medido deve estar antes dessa linha
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
