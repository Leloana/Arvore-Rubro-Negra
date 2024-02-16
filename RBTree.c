#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TAM 8

typedef int TIPOCHAVE;

typedef struct _node {
    TIPOCHAVE chave;//conteudo
    
    struct _node* pai;//Aponta para o pai, é necessário para herdar cor e outras funcionalidades
    
    //Filhos padrao de arvore binaria
    struct _node* esq;
    struct _node* dir;

    int color; // 0 para preto, 1 para vermelho

} Node;

typedef struct _root {
    Node* root;//Identifica aquele nó como raiz
} Root;


//Funcao para criar no de valor dado como "value"
Node* createNode(int value, Root* Tree){
    Node* newNode = calloc(1, sizeof(Node));
    //Inicia valores
    newNode->chave = value;
    newNode->pai = NULL;
    newNode->esq = NULL;
    newNode->dir = NULL;

    newNode->color = 1; // Nos adicionados sao vermelhos no inicio

    return newNode;
}

// Função para a rotação à esquerda
void Rotateesq(Root* Tree, Node* node){
    Node* FilhoDir = node->dir;

    node->dir = FilhoDir->esq;

    if (FilhoDir->esq != NULL)FilhoDir->esq->pai = node;
    FilhoDir->pai = node->pai;
    
    if (node->pai == NULL)Tree->root = FilhoDir;
    else if (node == node->pai->esq)node->pai->esq = FilhoDir;
    else node->pai->dir = FilhoDir;

    FilhoDir->esq = node;
    node->pai = FilhoDir;
}

// Função para a rotação à direita
void Rotatedir(Root* Tree, Node* node){
    Node* FilhoEsq = node->esq;

    node->esq = FilhoEsq->dir;

    if (FilhoEsq->dir != NULL)FilhoEsq->dir->pai = node;
    FilhoEsq->pai = node->pai;

    if (node->pai == NULL) Tree->root = FilhoEsq;//caso seja a raiz 
    else if (node == node->pai->dir)node->pai->dir = FilhoEsq;
    else node->pai->esq = FilhoEsq;
    
    FilhoEsq->dir = node;
    node->pai = FilhoEsq;
}
/*Checa se as condicoes de RED-BLACK tree se manteem apos a inserção,
caso nao esteja, faz rotações e colore os nós novamente
    -"Tree" é a raiz principal
    -"node" é o nó que foi inserido
*/
void AjeitaTree(Root* Tree, Node* node){
    while (node != Tree->root && node->pai->color == 1){//Enquanto o pai for vermelho(propriedade 4)
        // Node* Pai = node->pai;
        if (node->pai == node->pai->pai->esq){//Pai à esquerda
            Node* tio = node->pai->pai->dir;
            //========= CASO 1 ESQ =======================================
            if (tio != NULL && tio->color == 1){//Se tio for vermelho
                node->pai->color = 0;//Pai recebe preto
                tio->color = 0;//tio recebe preto
                if(node->pai->pai == Tree->root)node->pai->pai->color = 0;//se avo for raiz recebe preto
                else node->pai->pai->color = 1;//se nao recebe vermelho

                node = node->pai->pai;//node vai receber avo para continuar busca
            }
            //========= CASO 2 ESQ =======================================
            else{
                if (node == node->pai->dir){//Checa se node esta a direita
                    node = node->pai;//node recebe pai
                    Rotateesq(Tree, node);//rotaciona pau a esquerda
                }
                //========= CASO 3 ESQ =======================================
                node->pai->color = 0;//Pai recebe preto
                node->pai->pai->color = 1;//avo recebe vermelho
                Rotatedir(Tree, node->pai->pai);//rotaciona avo à direita
            }

        } else{//Mesmos casos porem o pai esta à direita
            Node* tio = node->pai->pai->esq;

            //========= CASO 1 DIR =======================================
            if (tio != NULL && tio->color == 1){
                node->pai->color = 0;
                tio->color = 0;
                node->pai->pai->color = 1;

                node = node->pai->pai;
            } 
            //========= CASO 2 DIR =======================================
            else{
                if (node == node->pai->esq){
                    node = node->pai;
                    Rotatedir(Tree, node);//rotaciona no a direita
                }
                //========= CASO 3 DIR =======================================
                node->pai->color = 0;
                node->pai->pai->color = 1;
                Rotateesq(Tree, node->pai->pai);//rotaciona avo a esquerda
            }
        }
    }
    Tree->root->color = 0;//Sempre define raiz como preta
}
//"Tree" é a raiz da arvore e value é o valor que sera inserido
void insert(Root* Tree, int value){
    Node* newNode = createNode(value, Tree);//Cria o novo a nó a ser adicionado

    Node* pai = NULL;//declara nó pai que sera usado nas comparaçoes
    Node* atual = Tree->root;//"atual" é o ponteiro que percorrerá a arvore
    // Busca o local para inserir o novo nó como em uma árvore binária de busca
    while (atual != NULL) {//Se atual == NULL significa que chegou ao fim da arvore
        pai = atual;
        if (value < atual->chave)atual = atual->esq;
        else if(value > atual->chave) atual = atual->dir;
        else{
            printf("\nValor ja inserido!!\n");
            return;
        }

    }
    //atual agora é igual a NULL
    //pai é igual ao pai de current
    newNode->pai = pai;
    if (pai == NULL) {// Arvore é vazia
        Tree->root = newNode;//Raiz recebe o novo nó
        newNode->color = 0; // Raiz é preta
    } else{//Se houver arvore
        if (value < pai->chave)pai->esq = newNode;
        else pai->dir = newNode;
    }
    //Faz rotaçoes/ coloracoes necessárias para manter balanceamento da arvore
    AjeitaTree(Tree, newNode);
}
/*Função que checa se a árvore cumpre as regras RED-BLACK após a remoção de um
nó preto, caso nao, a arvoré é arrumada
    -"Tree" é a raiz atual(sao criadas novas raizes na recursao)
    -"pontTree"é o ponteiro para a raiz principal da arvore
    -"removido" é o nó que será removido
    -"paiRemovido" é o pai do nó que será removido
*/
void deleteFixup(Root* Tree,Root* pontTree, Node* removido, Node* paiRemovido){
    Node* irmao;
    if((removido->pai != NULL)){//Caso removido nao seja a raiz
        if(removido == paiRemovido->esq){//se removido for nó a esquerda
            irmao = paiRemovido->dir;//irmao de removido(o que queremos deletar)

            if(irmao->color == 1){//se o irmao for vermelho
                irmao->color = 0;//irmao fica preto
                paiRemovido->color = 1;//pai fica vermelho
                Rotateesq(pontTree, paiRemovido);//rotaciona
                irmao = paiRemovido->dir;//irmao fica a direira de pai
            }
            //se nao houver sobrinho a esquerda e a direita ou se eles forem pretos
            if((irmao->esq == NULL || irmao->esq->color == 0) &&(irmao->dir == NULL || irmao->dir->color == 0)){
                irmao->color = 1;//muda cor do irmao
                removido = paiRemovido;
                paiRemovido = removido->pai;//paiRemovido recebe avo de removido
            } else{
                if(irmao->dir == NULL || irmao->dir->color == 0){//se o irmao for nulo ou preto
                    irmao->esq->color = 0;//sobrinho esquerdo fica preto
                    irmao->color = 1;//irmao fica vermelho
                    Rotatedir(pontTree, irmao);//rotaciona o Irmao para a direita
                    irmao = paiRemovido->dir;///irmao fica a direita de pai
                }
                irmao->color = paiRemovido->color;//irmao descende cor do pai
                paiRemovido->color = 0;//pai fica preto
                irmao->dir->color = 0;//sobrinho a direita fica preto
                Rotateesq(pontTree, paiRemovido);//Pai rotacionaa esquerda
                removido = Tree->root;//removido vira raiz
            }
        } 
        else{//se removido for nó da direita
            irmao = paiRemovido->esq;//irmao é no da esquerda
            if(irmao->color == 1){//se irmao for vermelho
                irmao->color = 0;//irmao muda para preto
                paiRemovido->color = 1;//pai fica vermelho
                Rotatedir(pontTree, paiRemovido);//rotaciona Pai a direita
                irmao = paiRemovido->esq;//atualiza irmao apos rotacao
            }
            //se nao houver sobrinho a esquerda e a direita ou se eles forem pretos
            if((irmao->dir == NULL || irmao->dir->color == 0) && (irmao->esq == NULL || irmao->esq->color == 0)){
                irmao->color = 1;//mesmos passos do anterior invertendo lado das rotaçoes
                removido = paiRemovido;
                paiRemovido = removido->pai;
            } else{
                if(irmao->esq == NULL || irmao->esq->color == 0){
                    irmao->dir->color = 0;
                    irmao->color = 1;
                    Rotateesq(pontTree, irmao);
                    irmao = paiRemovido->esq;
                }
                irmao->color = paiRemovido->color;
                paiRemovido->color = 0;
                irmao->esq->color = 0;
                Rotatedir(pontTree, paiRemovido);
                removido = Tree->root;
            }
        }
    }
    if (removido != NULL){
        removido->color = 0;//se o no removido nao for NULL ele fica preto
    }
    //apos isso ocorrera a remoção real do nó
}
/*Deleta o nó de valor "value" da arvore
    -"Tree" é a raiz atual(arvores auxiliares sao criadas na recursao)
    -"pontTree" é a raiz real da árvore, é passado como parametro para
    realizar as rotações em "deletFixup"
    -"value" é o valor a ser deletado 
*/
void deleteNode(Root* Tree,Root* pontTree, int value){
    Node* atual = Tree->root;//"atual" é o ponteiro que percorrerá a arvore
    if((atual == pontTree->root) && (!atual->dir) && (!atual->esq)){//neste caso atual é o ultimo no da arvore
        free(atual);
        pontTree->root = NULL;
        return;
    }
    while (atual != NULL && atual->chave != value) {//Se atual == NULL significa que chegou ao fim da arvore
        if (value < atual->chave)atual = atual->esq;
        else atual = atual->dir;
    }
    if(!atual){
        printf("\nValor nao encontrado!!\n");//Caso atual seja == NULL o valor nao existe na arvore
        return;//sai do remove
    }

    if((atual->dir == NULL) && (atual->esq == NULL)){//se for no folha
        if(atual->color == 0)deleteFixup(Tree,pontTree,atual, atual->pai);//se remover uma folha preta precisa corrigir
        
        if(atual->pai->esq == atual)atual->pai->esq = NULL;//zera os ponteiros do pai
        else atual->pai->dir = NULL;

        free(atual);//Caso seja uma folha 
    }
    else if((atual->dir != NULL) || (atual->esq != NULL)){//Caso Tenha algum filho
        Node* aux;
        if(atual->dir) aux = atual->dir;//aux recebe o no que existir
        else aux = atual->esq;

        while(aux->esq != NULL)aux = aux->esq;//devo procurar o menor node da parte direita
        atual->chave = aux->chave;//valor a ser deletado recebe menor valor a sua direita
        aux->chave = value;//aux recebe o valor a ser deletado

        Root auxTree;
        if(atual->dir) auxTree.root = atual->dir;//aux recebe o no que existir
        else auxTree.root = atual->esq;

        deleteNode(&auxTree,pontTree,value);
    }
}

// Função para imprimir a árvore em ordem
void printfTree(Node* node){
    if (node != NULL) {
        printfTree(node->esq);
        printf("%d ", node->chave);
        printfTree(node->dir);
    }
}
/*
Cria arquivo Dot para a vizualização
    -Nó ROXO = esquerda
    -Nó Vermelho = direita
    -1 = VERMELHO
    -0 = PRETO

    ex: nó roxo de nome "33_0"
    esta a ESQUERDA de seu pai, tem valor 33 e cor PRETA
*/
void DOT_Tree(Root* Head, char* name){
    if(Head->root == NULL)return;
    Node* Tree = Head->root;

    FILE* DOT = fopen(name, "a+");
    fprintf(DOT, "\n\t\t\"%d_%d\";",Tree->chave, Tree->color);
    if(Tree->esq != NULL){//mesma logica de printfTree
        fprintf(DOT, "\n\t\t\"%d_%d\" -> \"%d_%d\";\n",Tree->chave,Tree->color,Tree->esq->chave,Tree->esq->color);
        fprintf(DOT, "\t\t\"%d_%d\"  [style=filled, fillcolor=purple, fontcolor=black];\n",Tree->esq->chave,Tree->esq->color);//Nos a esquerda sao roxos
        
        Root aux;
        aux.root = Tree->esq;
        
        DOT_Tree(&aux, name);
    }

    if(Tree->dir != NULL){
        fprintf(DOT, "\n\t\t\"%d_%d\"-> \"%d_%d\";\n",Tree->chave,Tree->color,Tree->dir->chave,Tree->dir->color);
        fprintf(DOT, "\t\t\"%d_%d\"  [style=filled, fillcolor=red, fontcolor=black];\n",Tree->dir->chave,Tree->dir->color);//Nos a direita sao vermelhos
        Root aux;
        aux.root = Tree->dir;
        
        DOT_Tree(&aux, name);
    }

    fclose(DOT);
}
//funcoes para abrir arquivo dot
FILE* AbreEscritaDot(char* fn){
    FILE* dot = fopen(fn, "w");

    fprintf(dot, "\tdigraph BinaryTree {\n");
    
    fclose(dot);
    return dot;
}

void FechaEscritaDOT(char* fn){
    FILE* DOT = fopen(fn,"a+");
    fprintf(DOT,"\n\t}");
    fclose(DOT);
}

int main(void){
    Root cabeca;
    cabeca.root = NULL;
    srand(time(NULL));
    // Inserção de valores de exemplo
    int Vector[TAM];

    // printf("\nInserts:");
    // for(int i = 0; i < TAM; i++){
    //     Vector[i] = rand() % 50 ;
    //     printf(" %d", Vector[i]);
    // }

    // for(int i = 0; i < TAM; i++)insert(&cabeca, Vector[i]);
    // 
    insert(&cabeca,4);
    insert(&cabeca,25);
    insert(&cabeca,24);
    insert(&cabeca,23);
    insert(&cabeca,7);
    insert(&cabeca,23);
    insert(&cabeca,12);
    insert(&cabeca,32);

    // Impressão em ordem
    printf("\nÁrvore Rubro-Negra em ordem: ");
    printfTree(cabeca.root);
    printf("\n");
    FILE* DOT = AbreEscritaDot("arvore.DOT");
    DOT_Tree(&cabeca, "arvore.DOT");
    FechaEscritaDOT("arvore.DOT");

    // deleteNode(&cabeca,&cabeca,Vector[1]);
    // deleteNode(&cabeca,&cabeca,Vector[2]);
    // deleteNode(&cabeca,&cabeca,Vector[3]);

    
    deleteNode(&cabeca,&cabeca,4);
    deleteNode(&cabeca,&cabeca,25);
    deleteNode(&cabeca,&cabeca,24);


    FILE* DOT2 = AbreEscritaDot("mudancas.DOT");
    DOT_Tree(&cabeca, "mudancas.DOT");  
    FechaEscritaDOT("mudancas.DOT");
    return 0;
}