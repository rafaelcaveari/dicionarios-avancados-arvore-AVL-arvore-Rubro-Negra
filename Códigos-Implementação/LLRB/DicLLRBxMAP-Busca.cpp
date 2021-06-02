#include <iostream>
#include <chrono> //Medi��o de tempo de execu��o de um trecho de c�digo
#include <map> //Biblioteca padr�o para comparar com a implementa��o de uma �rvore rubro-negra

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


//Usei como refer�ncia as aulas do professor Andr� Backes, dispon�veis em http://www.facom.ufu.br/~backes/
//Foi implementado uma varia��o da �rvore Rubro-Negra para faciliar
//�rvore Rubro-Negra Ca�da Para a Esquerda - Left-Leaning Red�black (LLRB), proposta por Robert Sedgewick (2008)
//Propriedades da �rvore Rubro-Negra:
//- Todo n� deve ser preto ou vermelho (esquema de colora��o para manter o balanceamento da �rvore)
//- A raiz � sempre preta
//- Todo n� folha (NULL) � preto (como toda folha possui dois ponteiros NULL, pode-se ignorar isso para representar a �rvore fins pr�ticos e did�ticos)
//- Se um n� � vemelho, ent�o os seus filhos s�o pretos (n�o existem n�s vermelhos consecutivos)
//- Todos os caminhos de um n� para os n�s folhas descendentes cont�m o mesmo n�mero de n�s pretos
//- Permite rebalanceamento local para manter as propriedades da �rvore (rota��es e ajuste de cores)
//- No pior caso, os algoritmos das opera��es ser�o O(log(n))

// A LLRB atender� a todas essas propriedades, adicionando-se mais uma:
//- Se um n� � vermelho, ent�o ele ser� filho esquerdo do seu pai
// Dessa forma ca�mos na implementa��o de uma �rvore 2-3 (n�o � uma �rvore bin�ria)
// Na �rvore 2-3, cada n� pode armazenar um ou dois valores e, dependendo da quantidade de valores armazenados, ter dois (um valor) ou tr�s (dois valores) filhos
// Funcionamento semelhante a �rvore bin�ria, agora possuindo tamb�m um subarvore do meio, onde estar�o os elementos
// maiores do que o primeiro, mas menores que o segundo valor do n� pai
// A implementa��o corresponde a implementa��o de uma �rvore 2-3 se considerarmos que o n� vemelho ser� sempre o valor menor
// de um n� contendo dois valores e tr�s sub�rvores
// Assim, balancear a �rvore rubro-negra equivale a manipular uma �rvore 2-3, uma tarefa muito mais simples do que manipular uma �rvore AVL ou uma rubro-negra convencional
// Com exce��o da "inser��o" e da "remo��o", as demais fun��es (criar, liberar e buscar) da LLRB s�o id�nticas a de uma �rvore Bin�ria

// As opera��es de busca, inser��o e remo��o de elementos possuem complexidade O(log N), no qual N � o n�mero de elementos da �rvore), que s�o aplicados a �rvore de busca bin�ria

//Defini��o das cores
#define RED 1
#define BLACK 0

typedef struct NO* ArvLLRB;

//Defini��o do n� do dicion�rio. Tipo de dado armazenado
struct NO{
    int info; //Informa��o propriamente dita
    int chave; //Chave associada � informa��o
    struct NO *esq; //Filho esquerdo
    struct NO *dir; //Filho direito
    int cor; //Cor do n�
};

//Cria a �rvore alocando o n� raiz e retornando o ponteiro (ponteiro para ponteiro, fica mais f�cil mudar quem � a raiz da �rvore, se necess�rio)
//Id�ntica � �rvore bin�ria
ArvLLRB* cria_ArvLLRB(){
    ArvLLRB* raiz = (ArvLLRB*) malloc(sizeof(ArvLLRB));
    if(raiz != NULL){//Erro na aloca��o de mem�ria
        *raiz = NULL;
    }
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

//Liberar toda a �rvore, utilizando a fun��o libera_NO. Igual da �rvore bin�ria
void libera_ArvLLRB(ArvLLRB* raiz){
    if(raiz == NULL)
        return;
    libera_NO(*raiz);//libera cada n�
    free(raiz);//libera a raiz
}

//A consulta � id�ntica a uma �rvore Bin�ria. Por padr�o vamos buscar pela chave
int consulta_ArvLLRB(ArvLLRB *raiz, int valor){
    if(raiz == NULL) //�rvopre vazia?
        return 0;//N�o encontrou
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
//Diferente da AVL, s� temos dois tipos de rota��o: � esquerda e � direita
//Dado um conjunto de tr�s n�s, a rota��o visa deslocar um n� vermelho que esteja � esquerda para � direita e vice-versa

//Rota��o � esquerda
//Apenas rotaciona, n�o trata a viola��o de dois filhos vermelhos consecutivos, por exemplo
struct NO* rotacionaEsquerda(struct NO* A){
    struct NO* B = A->dir; //N� B como filho direito de A
    A->dir = B->esq; // Mover B para o lugar de A
    B->esq = A; //A se torna filho esquerdo de B
    B->cor = A->cor; //B recebe a cor de A
    A->cor = RED; //Cor de A fica vermelha
    return B;
}

//Rota��o � direita. N�o trata outras viola��es da LLRB
struct NO* rotacionaDireita(struct NO* A){
    struct NO* B = A->esq; //N� B como filho esquerdo de A
    A->esq = B->dir; // Mover B para o lugar de A
    B->dir = A; //A se torna filho direito de B
    B->cor = A->cor; //B recebe a cor de A
    A->cor = RED; //Cor de A fica vermelha
    return B;
}
//Fun��o auxiliar para retornar a cor do n�
int cor(struct NO* H){
    if(H == NULL)
        return BLACK; //Propriedade da �rvore Rubro-Negra. Todo n� folha (NULL) � preto
    else
        return H->cor;
}

//Fun��o auxiliar para inverter a cor do pai e de seus filhos
void trocaCor(struct NO* H){
    H->cor = !H->cor; //Inverte a cor do pai
    if(H->esq != NULL) //Inverte a cor do filho da esquerda
        H->esq->cor = !H->esq->cor;
    if(H->dir != NULL)//Inverte a cor do filho da direita
        H->dir->cor = !H->dir->cor;
}

// Fun��o auxiliar utilizada na fun��o insere_ArvLLRB
// Trata dos seguintes casos:
//-Se a raiz � NULL: insira o n�
//-Se o valor � menor do que a raiz: v� para a sub�rvore � esquerda
//-Se o valor � maior que a raiz: v� para a sub�rvore � direita
//Aplique o m�todo recursivamente
//Ao voltar na recurss�o, verificar as propriedades de cada sub�rvore e aplicar as rota��es ou mudan�a de cor
//necess�rias caso alguma propriedade foi violada
struct NO* insereNO(struct NO* H, int chave, int valor, int *resp){
    if(H == NULL){ //�vore vazia ou estou em um n� folha
        struct NO *novo; //Crio o novo n�
        novo = (struct NO*)malloc(sizeof(struct NO)); //Aloca��o de mem�ria
        if(novo == NULL){ //Verifica��o da aloca��o de mem�ria
            *resp = 0;
            return NULL;
        }
        //Preenche o novo n� com as informa��es
        novo->info = valor;
        novo->chave = chave;
        novo->cor = RED; //Novo n� � sempre vermelho
        novo->dir = NULL; //Como � um n� folha, esq=NULL e dir=NULL
        novo->esq = NULL;
        *resp = 1; //Deu certo a inser��o
        return novo;
    }

    if(chave == H->chave)
        *resp = 0;// Valor duplicado. N�o faz nada e informa que n�o deu certo a inser��o
    else{
        if(chave < H->chave)
            H->esq = insereNO(H->esq,chave,valor,resp); //Valor � menor que o pai, vou pra sub�rvore da esquerda
        else
            H->dir = insereNO(H->dir,chave,valor,resp); //Valor � maior que o pai, vou pra sub�rvore da direita
    }

    //Voltando da recurs�o verifico se as propriedades da �rvore se mant�m

    //n� vermelho � sempre filho � esquerda (LLRB)
    if(cor(H->dir) == RED && cor(H->esq) == BLACK)
        H = rotacionaEsquerda(H); //Rota��o � esquerda

    //Filho e Neto s�o vermelhos
    //Filho vira pai de 2 n�s vermelhos
    if(cor(H->esq) == RED && cor(H->esq->esq) == RED)
        H = rotacionaDireita(H); //Rota��o � direita

    //2 filhos Vermelhos: troca cor!
    if(cor(H->esq) == RED && cor(H->dir) == RED)
        trocaCor(H); //Troca a cor do pai e dos filhos

    return H;
}

//Fun��o de gerenciamento da inser��o
int insere_ArvLLRB(ArvLLRB* raiz, int chave, int valor){
    int resp; //Guarda a resposta da inser��o passada como par�metro e usada na recurs�o
    *raiz = insereNO(*raiz,chave,valor,&resp); //Fun��o respons�vel pela busca do local de inser��o do n�
    if((*raiz) != NULL) //�rvore est� vazia.
        (*raiz)->cor = BLACK; //Raiz � sempre preta

    return resp;
}

//Temos necessidade de outras 3 fun��es (al�m das rota��es) para restabelecer o balanceamento da �rvore
// e garantir que suas propriedades sejam respeitadas:
//-Mover um n� vermelho para a esquerda
//-Mover um n� vermelho para a direita
//-Arrumar o balanceamento

//Fun��o auxiliar usada no rebalanceamento da �rvore
//Trata 3 situa��es:
//- Se o filho direito � vermelho: rota��o � esquerda
//- Se o filho esquerdo e o neto da esquerda s�o vermelhos: rota��o � direita
//- Se ambos os filhos s�o vermelhos: trocar a cor do pai e dos filhos
struct NO* balancear(struct NO* H){
    if(cor(H->dir) == RED)//n� vermelho � sempre filho � esquerda
        H = rotacionaEsquerda(H);

    //Filho da esquerda e neto da esquerda s�o vermelhos
    if(H->esq != NULL && cor(H->esq) == RED && cor(H->esq->esq) == RED)
        H = rotacionaDireita(H);

    //2 filhos Vermelhos: troca cor!
    if(cor(H->esq) == RED && cor(H->dir) == RED)
        trocaCor(H);
    return H;
}

//Fun��o auxiliar para mover um n� vermelho para a esquerda e restabelecer o balanceamento da �rvore
//Essas fun��es movimentam um n� vermelho para a sub�rvore esquerda ou direita, dependendo da situa��o em que se encontra
struct NO* move2EsqRED(struct NO* H){
    trocaCor(H); //Troca a cor do n� e dos filhos
    if(cor(H->dir->esq) == RED){ //Filho a esquerda do filho direito � vermelho
        H->dir = rotacionaDireita(H->dir); //Aplica rota��o � direita no filho direito
        H = rotacionaEsquerda(H); //Rota��o � esquerda no pai e seus filhos
        trocaCor(H); //Troca a cor do n� e dos filhos
    }
    return H;
}

//Fun��o auxiliar para mover um n� vermelho para a direita e restabelecer o balanceamento da �rvore
//Essas fun��es movimentam um n� vermelho para a sub�rvore esquerda ou direita, dependendo da situa��o em que se encontra
struct NO* move2DirRED(struct NO* H){
    trocaCor(H); //Troca a cor do n� e dos filhos
    if(cor(H->esq->esq) == RED){ //Filho a esquerda do filho esquerdo � vermelho
        H = rotacionaDireita(H); //Aplicar rota��o � direita no pai
        trocaCor(H);//Troca a cor do n� e dos filhos
    }
    return H;
}

//Remo��o
// Existem 3 tipos de remo��o:
//- N� folha (sem filhos)
//- N� com 1 filho
//- N� com 2 filhos
//Ao voltar na recurs�o, verificar as propriedades de cada sub�rvore e aplicar as rota��es e mudan�as de cor necess�rias

struct NO* removerMenor(struct NO* H){
    if(H->esq == NULL){ //N�o tem filho � esquerda
        free(H);
        return NULL;
    }
    if(cor(H->esq) == BLACK && cor(H->esq->esq) == BLACK)
        H = move2EsqRED(H);

    H->esq = removerMenor(H->esq); //Indo pra esquerda
    return balancear(H);
}

// Fun��o auxiliar que procura pelo n� mais a esquerda (que � o menor n�)
struct NO* procuraMenor(struct NO* atual){
    struct NO *no1 = atual;
    struct NO *no2 = atual->esq;
    while(no2 != NULL){
        no1 = no2;
        no2 = no2->esq;
    }
    return no1;
}

//Fun��o respons�vel pela busca do n� a ser removido
struct NO* remove_NO(struct NO* H, int valor){
    if(valor < H->chave){ //Valor a ser removido � menor que o valor do pai?
        if(cor(H->esq) == BLACK && cor(H->esq->esq) == BLACK)
            H = move2EsqRED(H); //Move para esquerda

        H->esq = remove_NO(H->esq, valor); //Vou para a sub�rvore da esquerda
    }else{
        if(cor(H->esq) == RED) //No esquerdo � vermelho,
            H = rotacionaDireita(H);

        if(valor == H->chave && (H->dir == NULL)){ //N� folha. S� remove o n�
            free(H);
            return NULL;
        }

        if(cor(H->dir) == BLACK && cor(H->dir->esq) == BLACK)
            H = move2DirRED(H); //Move para direita

        if(valor == H->chave){
            struct NO* x = procuraMenor(H->dir); //Menor n� da sub�rvore da direita
            H->chave = x->chave; //Troco de lugar com o n� H
            H->dir = removerMenor(H->dir); //Remove o menor
        }else
            H->dir = remove_NO(H->dir, valor);
    }
    return balancear(H); //Antes de voltar na recurs�o, devemos balancear a �rvore
}

//Fun��o de gerenciamento da remo��o
int remove_ArvLLRB(ArvLLRB *raiz, int valor){
    if(consulta_ArvLLRB(raiz,valor)){ //Verifica primeiro se existe o elemento a ser removido na �rvore
        struct NO* h = *raiz;
        *raiz = remove_NO(h,valor); //Fun��o respons�vel pela busca do n� a ser removido
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
    int encontrouMapa=0, encontrouLLRB=0; //Vari�veis de controle
    double t = tmr.elapsed(); //Tempo de execu��o do trecho de c�digo em segundos (s)
    int N = 11000000; //N�mero de n�s da �rvore/Mapa. Range 500.000-15.000.000
    int num_rodadas = 500000; //N�mero de rodadas para pegar a m�dia de tempos e diminuir distor��es
    double somaMAPA = 0.0, somaLLRB = 0.0; //Tempos para cada implementa��o
    double *tempo_LLRB, *tempo_MAPA;
    tempo_LLRB = (double *)malloc(N * sizeof(double)); //Vetores para armazenar os tempos de inser��o de cada valor
    tempo_MAPA = (double *)malloc(N * sizeof(double));
    int res; //Resposta se a inser��o ocorreu corretamente (1) ou n�o (0)
    std::cout<<"N = "<<N<<" - Numero de rodadas = "<<num_rodadas<<std::endl;
    std::cout<<std::endl;

    //=============================Mapa============================================
    std::map<int, int> Mapa; //Para comparar com a implementa��o de �rvore LLRB
    std::cout<<"Inserindo elementos no Mapa..."<<std::endl;
    std::cout<<std::endl;
    for(int i=0;i<N;i++){ //Cria um Mapa adicionando os elementos na segu�ncia: 0, 2, 3, 4...N-1
        Mapa.insert(std::pair<int, int>(i, i));
        }


    std::cout<<"Buscando elementos e calculando tempo..."<<std::endl;
    std::cout<<std::endl;
    for(int j=0;j<num_rodadas;j++) //Fazer v�rias rodadas para tentar evitar muitas distor��es
    {
        tmr.reset();
        //======================Zera o contador. C�digo a ser medido deve estar depois dessa linha
        if(Mapa.find(j)!= Mapa.end())
        {
        //======================Para o contador. C�digo a ser medido deve estar antes dessa linha
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
    ArvLLRB *llrb = cria_ArvLLRB();//Cria �rvore LLRB
    std::cout<<"Inserindo elementos na LLRB..."<<std::endl;
    std::cout<<std::endl;
    for(int i=0;i<N;i++) //Insere elementos na �rvore
    {
        res = insere_ArvLLRB(llrb,i,i);
    }

    std::cout<<"Buscando elementos e calculando tempo..."<<std::endl;
    std::cout<<std::endl;
    for(int j=0;j<num_rodadas;j++)
    {
        tmr.reset();
        //======================Zera o contador. C�digo a ser medido deve estar depois dessa linha
        if(consulta_ArvLLRB(llrb,j)== 1) //Zero n�o encontrou, 1 encontrou
        {
        //======================Para o contador. C�digo a ser medido deve estar antes dessa linha
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
