#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//Galinha
typedef struct{

    int vida;
    int posX;
    int posY;
    int pista;
    char matriz[2][3];

} tGalinha;
tGalinha LeGalinha(FILE * config_inicial, tGalinha galinha, int pista);
tGalinha LeMatrizGalinha(FILE * personagens);
tGalinha AtualizaGalinha(tGalinha galinha, char comando, int ultimaPista);
tGalinha AtualizaGalinhaColisao(tGalinha galinha, int qdtPistas);



//Carros
typedef struct{

    int velocidade;
    int pista;
    int posX;
    int index; 
    char direcao;
    char matriz[2][3];

} tCarro;
tCarro LeCarro(FILE * config_inicial, int pistaAtual, tCarro carroBase, int index);
tCarro LeMatrizCarro(FILE * personagens, int animacao);
tCarro AtualizaCarro(tCarro carro, int colunas);


//Colisoes
typedef struct{
    
    int pista;
    int indice;
    int iteracao;
    int ordenada;

} tColisao;


//Dados
typedef struct{

    int qtdColisoes;
    tColisao colisoes[20];
    int pontos;
    int iteracao;
    int numMovimentos;
    int numMovParaTras;
    int altura;
    int alturaMorte;
    int alturaMaxMorte;
    int alturaMinMorte;
    int alturaMax;
    int qtdPistas;
    int qtdTotalCarros;
    int colunas;
    int animacao;
    
} tDados;
tDados LeDados(FILE * config_inicial);
tDados AtualizaDados(tDados dados, char comando);
tDados AtualizaDadosColisao(tDados dados, char comando);



//Jogo
typedef struct{

    char caminho[1100];
    int heatmap[35][100];
    tDados dados;
    tGalinha galinha;
    tCarro carros[120];

} tJogo;
tJogo InicializarJogo(FILE *config_inicial, FILE *personagens, char * caminho);
tJogo LeArquivosJogo(FILE * config_inicial, FILE * personagens);
void InicializaMapaLocal(int linhas, int colunas,char mapa[linhas][colunas]);
void PreencherMapa(tJogo jogo, int linhas, int colunas, char mapa[linhas][colunas]);
void ImprimeMapa(int pontos, int vidas, int iteracoes, int linhas, int colunas, char mapa[linhas][colunas]);
int TerminarJogo(tJogo jogo);
tJogo ContinuarJogo(tJogo jogo);
char LeComando();
int ChecarColisao(char comando, int qtdCarros, tCarro carros[], tGalinha galinha, int iteracao, int qtdColisoes, tColisao[], char * caminho);



//Validacao dos Arquivos
void geraCaminhoCompletoArquivos(char * diretorio, char * arquivo, char * caminho);
void AbreArquivos(char * argv, FILE **config_inicial, FILE **personagens);
void ValidaArquivos(char * argv, FILE *config_inicial, FILE *personagens);


//Geracao de Arquivos
void GerarArquivoInicializacao(tDados dados, tGalinha galinha, int linhas, int colunas, char mapa[linhas][colunas], char * caminho);
void GerarArquivoEstatisticas(tJogo jogo);
void GerarArquivoResumo(int fimJogo, int iteracao, int pista, int posX, int posY, tCarro carro, char * caminho);
void GerarArquivoRanking(tJogo jogo);
void OrdenaVetorColisoes(int qtdColisoes, tColisao colisoes[]);


int main(int argc, char * argv[]){
    tJogo jogo;

    FILE * config_inicial;
    FILE * personagens;

    if (argc == 1) {
        printf("ERRO: Informe o diretorio com os arquivos de configuracao.");
        exit(1);
    }
    AbreArquivos(argv[1], &config_inicial, &personagens);

    ValidaArquivos(argv[1], config_inicial, personagens);

    jogo = InicializarJogo(config_inicial, personagens, argv[1]);

    strcpy(jogo.caminho, argv[1]);

    //printf("CAMINHO: %s", jogo.caminho);

    //printf("Inicalizou...\n");

    while (!TerminarJogo(jogo)){

        jogo = ContinuarJogo(jogo);

    }

    GerarArquivoResumo(1, jogo.dados.iteracao, 0, 0, 0, jogo.carros[0], jogo.caminho); // preciso apenas do 1, de fim de jogo
    GerarArquivoEstatisticas(jogo);
    GerarArquivoRanking(jogo);
    //printf("Gerou Arquivo Ranking");
    //GerarArquivoHeatmap();

}

//############## FUNCIONAMENTO DO JOGO ##############
int TerminarJogo(tJogo jogo){

    if (jogo.galinha.pista == 1) {
        printf("Parabens! Voce atravessou todas as pistas e venceu!\n");
        return 1;
    }
    else if (jogo.galinha.vida == 0) {
        printf("Voce perdeu todas as vidas! Fim de jogo.\n");
        return 1;
    }

    return 0;

    }

tJogo ContinuarJogo(tJogo jogo){

    tJogo jogoL = jogo;
    
    int i;
    char leitura;
    char comando;
    int colidiu;
    
    comando = LeComando();

    if (comando == 'w' || comando == 's' || comando == ' '){

        for (i = 0; i < jogo.dados.qtdTotalCarros; i++){
            jogoL.carros[i] = AtualizaCarro(jogo.carros[i], jogo.dados.colunas);
        }
    
        colidiu = ChecarColisao(comando, jogo.dados.qtdTotalCarros, jogoL.carros, jogo.galinha, jogo.dados.iteracao, jogo.dados.qtdColisoes, jogo.dados.colisoes, jogo.caminho);
    
        if (colidiu){
            
            jogoL.galinha = AtualizaGalinhaColisao(jogo.galinha, jogo.dados.qtdPistas);
            jogoL.dados = AtualizaDadosColisao(jogo.dados, comando);
            jogoL.dados.qtdColisoes++;
    
        }
        else{
    
            jogoL.galinha = AtualizaGalinha(jogo.galinha, comando, jogo.dados.qtdPistas);
            jogoL.dados = AtualizaDados(jogo.dados, comando);
            
        }

        int linhas = 3 * jogo.dados.qtdPistas - 1;
        char mapaLocal[linhas][jogo.dados.colunas];
    
        InicializaMapaLocal(linhas, jogoL.dados.colunas, mapaLocal);        
        PreencherMapa(jogoL, linhas, jogoL.dados.colunas, mapaLocal);
        ImprimeMapa(jogoL.dados.pontos, jogoL.galinha.vida, jogoL.dados.iteracao, linhas, jogoL.dados.colunas, mapaLocal);

    }



    return jogoL;

}

int ChecarColisao(char comando, int qtdCarros, tCarro carros[], tGalinha galinha, int iteracao, int qtdColisoes, tColisao colisoes[qtdColisoes], char * caminho){

    int pistaPrevGalinha = 0;
    int posYPrevGalinha = 0;

    //printf("Pista da Galinha: %d\n", galinha.pista);

    if (comando == 'w'){
        pistaPrevGalinha = galinha.pista-1;
        posYPrevGalinha = galinha.posY-3;
    }
    else if (comando == 's'){
        pistaPrevGalinha = galinha.pista+1;
        posYPrevGalinha = galinha.posY+3;
    }
    else if (comando == ' '){
        pistaPrevGalinha = galinha.pista;
        posYPrevGalinha = galinha.posY;
    }

    //printf("Pista prev galinha: %d\n", pistaPrevGalinha);

    int idxVetor;
    int posChecagemCarro;
    int posGalinha;
    int i, j;
    
    for (idxVetor = 0; idxVetor < qtdCarros; idxVetor++){
        //printf("Entrou no for");
        if (carros[idxVetor].pista != pistaPrevGalinha){
            //printf("Pista: %d\n", carros[idxVetor].pista);    
            continue;
        }
        
        //printf("Passou pelo continnue");
        
        for (i = -1; i < 2; i++){
            for (j = -1; j < 2; j++){
                posGalinha = galinha.posX + i;
                
                if (carros[idxVetor].direcao == 'D'){
                    posChecagemCarro = carros[idxVetor].posX + j;
                }
                else if (carros[idxVetor].direcao == 'E'){
                    posChecagemCarro = carros[idxVetor].posX + j;
                }

                
                //printf("PosChecagem do carro: %d | posGalinha: %d\n", posChecagemCarro, posGalinha);
                if (posChecagemCarro == posGalinha) {

                    colisoes[qtdColisoes].pista = pistaPrevGalinha;
                    colisoes[qtdColisoes].indice = carros[idxVetor].index+1;
                    colisoes[qtdColisoes].iteracao = iteracao;

                    //printf("PISTA COLISAO1: %d | INDICE COLISAO1: %d | ITERACAO COLISAO: %d\n", colisoes[qtdColisoes].pista, colisoes[qtdColisoes].indice = carros[idxVetor].index+1, colisoes[qtdColisoes].iteracao);

                    GerarArquivoResumo(0, iteracao+1, pistaPrevGalinha, galinha.posX, posYPrevGalinha, carros[idxVetor], caminho);
                    return 1;
                    
                }
            }
        }
    }

    return 0;

}

char LeComando(){

    char buffer[10];

    if (fgets(buffer, sizeof(buffer), stdin) != NULL) { // mudar isso depois, so pega um caractere por vez e e ruim em geral
        return buffer[0]; 
    }

    return '\0'; 

}
tDados AtualizaDados(tDados dados, char comando){

    tDados dadosL = dados;

    dadosL.iteracao++;
    
    if (comando == 'w') {
        dadosL.altura += 3;
        dadosL.pontos++;
        dadosL.numMovimentos++;
        if (dadosL.altura == (3 * dadosL.qtdPistas - 1)) dadosL.pontos += 10;
    }  

    else if (comando == 's') {
        if (dadosL.altura != 2) dadosL.altura -= 3;
        dadosL.numMovParaTras++;
        dadosL.numMovimentos++;
    }

    if (dadosL.altura > dadosL.alturaMax) dadosL.alturaMax = dadosL.altura;

    return dadosL;

}
tDados AtualizaDadosColisao(tDados dados, char comando){

    tDados dadosL = dados;

    dadosL.alturaMorte = dadosL.altura;

    if (comando == 'w') {
        dadosL.numMovimentos++;
        dadosL.alturaMorte = dadosL.altura + 3;
    }
    else if (comando == 's') {
        dadosL.numMovimentos++;
        dadosL.numMovParaTras++;
        dadosL.alturaMorte = dadosL.altura - 3;
    }

    //printf("ALTURA MORTE: %d\n", dadosL.alturaMorte);

    if (dadosL.alturaMorte < dadosL.alturaMinMorte) dadosL.alturaMinMorte = dadosL.alturaMorte;
    if (dadosL.alturaMorte > dadosL.alturaMaxMorte) dadosL.alturaMaxMorte = dadosL.alturaMorte;

    dadosL.iteracao++;
    dadosL.pontos = 0;
    dadosL.altura = 2;

    return dadosL;

}
tGalinha AtualizaGalinha(tGalinha galinha, char comando, int ultimaPista){

    tGalinha galinhaL = galinha;
    

    switch (comando) {
    case 'w':
        //comandos galinha pra frente;
        galinhaL.posY -= 3;
        galinhaL.pista--;
        break;
    case 's':
        //comandos galinha pra tras;
        if (galinha.pista == ultimaPista) {
            galinhaL.posY = galinha.posY;
            galinhaL.pista = galinha.pista;
        }
        else {
            galinhaL.posY += 3;
            galinhaL.pista++;
        }
        break;
        case ' ':
        //comandos galnha ficar parada;
        galinhaL.posY = galinha.posY;
        galinhaL.pista = galinha.pista;
        break;
        default:
        break;
    }
    
    return galinhaL;
    
}
tGalinha AtualizaGalinhaColisao(tGalinha galinha, int ultimaPista){

    tGalinha galinhaL = galinha;

    galinhaL.pista = ultimaPista;
    galinhaL.vida--;
    galinhaL.posY = (3 * ultimaPista - 1) - 1;

    return galinhaL;

}
tCarro AtualizaCarro(tCarro carro, int colunas){

    tCarro carroL = carro;

    if (carro.direcao == 'D'){
        carroL.posX += carro.velocidade;  
    }
    else if (carro.direcao == 'E'){
        carroL.posX -= carro.velocidade;
    }
    
    if (carroL.posX > colunas) carroL.posX %= colunas; // se tiver erro possivel que e aqui
    
    else if (carroL.posX < 1) carroL.posX += colunas;

    return carroL;

}

//############## LEITURA DOS ARQUIVOS ###############
tJogo LeArquivosJogo(FILE * config_inicial, FILE * personagens){
    
    tJogo jogo;

    tGalinha galinhaMatriz = LeMatrizGalinha(personagens);

    jogo.dados = LeDados(config_inicial);
    jogo.dados.qtdTotalCarros = 0;
    
    
    int pistaAtual = 1, idxVetor = 0, qtdCarros, i, j;
    char direcao, lixo;
    tCarro carroBase;

    carroBase = LeMatrizCarro(personagens, jogo.dados.animacao);

    while (pistaAtual < jogo.dados.qtdPistas){

        fscanf(config_inicial, "%c", &direcao); 

        if (direcao == '\n') pistaAtual++;     // linha de chegada e pistas vazias
        else {

            carroBase.direcao = direcao;

            fscanf(config_inicial, " %d %d", &carroBase.velocidade, &qtdCarros); // escaneia cada linha, gerando um vetor de carros, com os primeiros elementos sendo os carros da primeira pista   
            
            for (i = 0; i < qtdCarros; i++){          
                jogo.carros[idxVetor] = LeCarro(config_inicial, pistaAtual, carroBase, i);                          
                idxVetor++; 
                jogo.dados.qtdTotalCarros++;
            }

            fscanf(config_inicial, "%c", &lixo);
            
            pistaAtual++;

        }
    }

    /* printf("DESENHO CARRO\n");
    for (i = 0; i < 2; i++){
        for (j = 0; j < 3; j++){
            printf("%c", jogo.carros[0].matriz[i][j]);
        }
        printf("\n");
    } */


    jogo.galinha = LeGalinha(config_inicial, galinhaMatriz, jogo.dados.qtdPistas);

    fclose(config_inicial);
    fclose(personagens);
    
    return jogo;

}
tDados LeDados(FILE * config_inicial){
    
    tDados dados;
    char lixo;

    fscanf(config_inicial, "%d\n", &dados.animacao); // primeira linha
    fscanf(config_inicial, "%d %d", &dados.colunas, &dados.qtdPistas); // segunda linha
    fscanf(config_inicial, "%c", &lixo);

    /* printf("Dados da Animação: %d\n", dados.animacao);
    printf("COLUNAS: %d | PISTAS: %d\n----------------------------\n", dados.colunas, dados.qtdPistas); */

    return dados;

}
tGalinha LeGalinha(FILE * config_inicial, tGalinha galinha, int pista){

    tGalinha galinhaL = galinha;
    galinhaL.pista = pista;
    galinhaL.posY = (3 * pista - 1) - 1;

    char letra;

    fscanf(config_inicial, "%c %d %d", &letra, &galinhaL.posX, &galinhaL.vida);

    //printf("----------------------------\nGALINHA:\nVida: %d | posX: %d\n", galinhaL.vida, galinhaL.posX);

    return galinhaL;

}
tGalinha LeMatrizGalinha(FILE * personagens){

    tGalinha galinha;

    int i, j;
    char lixo;

    //printf("DESENHO GALINHA\n");
    for (i = 0; i < 2; i++) {
        for (j = 0; j < 3; j++)
        {
            fscanf(personagens, "%c", &galinha.matriz[i][j]);  // erro quando tem um " " em algum elemento do desenho
            //printf("%c", galinha.matriz[i][j]);
        }
        fscanf(personagens, "%c", &lixo); // \n
        //printf("%c", lixo);
    }

    return galinha;

}
tCarro LeCarro(FILE * config_inicial, int pistaAtual, tCarro carroBase, int index){
    
    tCarro carro;

    carro = carroBase;
    carro.pista = pistaAtual;
    carro.index = index;
    fscanf(config_inicial, " %d", &carro.posX);

    //printf("velocidade: %d | posX: %d | pista: %d | direcao: %c | index: %d\n", carro.velocidade, carro.posX, carro.pista, carro.direcao, carro.index);

    return carro;

}
tCarro LeMatrizCarro(FILE * personagens, int animacao){

    tCarro carro;

    int i, j;
    char lixo;

    if (animacao == 0) {
        for (i = 0; i < 2; i++) {
            for (j = 0; j < 3; j++) {
                fscanf(personagens, "%c", &carro.matriz[i][j]); // nao testei, mas possivelmente pode dar erro quando tem um " " em algum elemento do desenho
                //printf("%c", carro.matriz[i][j]);
            }
            fscanf(personagens, "%c", &lixo); // \n
            //printf("%c", lixo);
        }
    }
    else {

        printf ("### FAZER ANIMACAO ###\n");
    }

    return carro;

}

//############## GERENCIAMENTO DO MAPA ##############
tJogo InicializarJogo(FILE *config_inicial, FILE *personagens, char * caminho){
    
    tJogo jogo;
    
    
    jogo = LeArquivosJogo(config_inicial, personagens);
    //printf("Leu arquivos jogo....\n");

    jogo.dados.qtdColisoes = 0;
    jogo.dados.altura = 2;
    jogo.dados.alturaMax = 0;
    jogo.dados.alturaMorte = 0;
    jogo.dados.alturaMaxMorte = 0;
    jogo.dados.alturaMinMorte = 999;
    jogo.dados.iteracao = 0;
    jogo.dados.numMovimentos = 0;
    jogo.dados.numMovParaTras = 0;
    jogo.dados.pontos = 0;  
    
    int linhas = 3 * jogo.dados.qtdPistas - 1;
    char mapaInicialL[linhas][jogo.dados.colunas];

    InicializaMapaLocal(linhas, jogo.dados.colunas, mapaInicialL);
    //printf("Inicializou mapa...\n");
    PreencherMapa(jogo, linhas, jogo.dados.colunas, mapaInicialL);
    //printf("Preencheu mapa...\n");
    ImprimeMapa(jogo.dados.pontos, jogo.galinha.vida, jogo.dados.iteracao, linhas, jogo.dados.colunas, mapaInicialL);
    
    //jogo = InicializarHeatmap(jogo);

    int temArquivoInicializacao = 0;

    GerarArquivoInicializacao(jogo.dados, jogo.galinha, linhas, jogo.dados.colunas, mapaInicialL, caminho);

    return jogo;

}
void PreencherMapa(tJogo jogo, int linhas, int colunas, char mapa[linhas][colunas]){
    
    int pistaAtual = 1;
    int idxCarro = 0;
    int i, j;

    i = jogo.galinha.posY - 1;
    j = jogo.galinha.posX - 1;
    
    mapa[i][j-1] = jogo.galinha.matriz[0][0];
    mapa[i][j] = jogo.galinha.matriz[0][1];
    mapa[i][j+1] = jogo.galinha.matriz[0][2];      // não estou fazendo a checagem do limite
    mapa[i+1][j-1] = jogo.galinha.matriz[1][0];
    mapa[i+1][j] = jogo.galinha.matriz[1][1];
    mapa[i+1][j+1] = jogo.galinha.matriz[1][2];

    //printf("Preencheu galinha na matriz....\n");
    
    for (i = 0; i < linhas; i++){
        
        if ((i+1) % 3 == 0) {
            pistaAtual++;
            continue;
        }

        for (idxCarro = 0; idxCarro < jogo.dados.qtdTotalCarros; idxCarro++) {

            if (jogo.carros[idxCarro].pista != pistaAtual) continue;

            int esquerda = ((jogo.carros[idxCarro].posX - 1) - 1 + colunas) % colunas;
            int direita  = ((jogo.carros[idxCarro].posX - 1) + 1) % colunas;

            mapa[i][esquerda] = jogo.carros[0].matriz[0][0];
            mapa[i][jogo.carros[idxCarro].posX - 1] = jogo.carros[0].matriz[0][1];
            mapa[i][direita] = jogo.carros[0].matriz[0][2];

            mapa[i+1][esquerda] = jogo.carros[0].matriz[1][0];
            mapa[i+1][jogo.carros[idxCarro].posX - 1] = jogo.carros[0].matriz[1][1];
            mapa[i+1][direita] = jogo.carros[0].matriz[1][2];
                
        }
        
        i++;
        //printf("Preencheu carro na matriz.....\n");


    }  
}
void InicializaMapaLocal(int linhas, int colunas, char mapa[linhas][colunas]){
    
    int i, j;

    for (i = 0; i < linhas; i++){
        for (j = 0; j < colunas; j++){
            if ((i+1) % 3 == 0 && (j+1) % 3 != 0) mapa[i][j] = '-';
            else mapa[i][j] = ' ';
            //printf("%c", jogoL.mapaBase[i][j]);
        }
        //printf("\n");
    }

}
void ImprimeMapa(int pontos, int vidas, int iteracoes, int linhas, int colunas, char mapa[linhas][colunas]){ 
    
    int i, j;

    printf("Pontos: %d | Vidas: %d | Iteracoes: %d\n", pontos, vidas, iteracoes);

    for (j = 0; j < colunas + 2; j++){
        if (j == 0 || j == colunas+1) printf("|");
        else printf("-");
    }
    printf("\n");

    for (i = 0; i < linhas; i++){
        for (j = 0; j < colunas + 2; j++){
            if (j == 0 || j == colunas+1) printf("|");
            else printf("%c", mapa[i][j-1]);
        }
        printf("\n");
    }

    // linha inferior
    for (j = 0; j < colunas + 2; j++) {
        if (j == 0 || j == colunas+1) 
            printf("|");
        else 
            printf("-");
    }
    printf("\n");

}

//##############  CRIACAO DE ARQUIVOS  ##############
void GerarArquivoInicializacao(tDados dados, tGalinha galinha, int linhas, int colunas, char mapa[linhas][colunas], char * caminho){

    FILE * inicializacao;

    char caminhoIni[1100];

    sprintf(caminhoIni, "%s/saida/inicializacao.txt", caminho);

    inicializacao = fopen(caminhoIni, "w"); // adicionar o / para correcao

    int i, j;

    for (j = 0; j < colunas + 2; j++){
        if (j == 0 || j == colunas+1) fprintf(inicializacao, "|");
        else fprintf(inicializacao, "-");
    }
    fprintf(inicializacao, "\n");

    for (i = 0; i < linhas; i++){
        for (j = 0; j < colunas + 2; j++){
            if (j == 0 || j == colunas+1) fprintf(inicializacao, "|");
            else fprintf(inicializacao, "%c", mapa[i][j-1]);
        }
        fprintf(inicializacao, "\n");
    }

    // linha inferior
    for (j = 0; j < colunas + 2; j++) {
        if (j == 0 || j == colunas+1) fprintf(inicializacao, "|");
        else fprintf(inicializacao, "-");
    }
    fprintf(inicializacao, "\n");

    fprintf(inicializacao, "A posicao central da galinha iniciara em (%d %d).", galinha.posX, galinha.posY);

    fclose(inicializacao);

}

void GerarArquivoEstatisticas(tJogo jogo){

    FILE * estatisticas;

    char caminhoEst[1100];

    sprintf(caminhoEst, "%s/saida/estatistica.txt", jogo.caminho);

    estatisticas = fopen(caminhoEst, "w"); // adicionar / antes de saida para o correcao

    if (jogo.dados.alturaMinMorte == 999) jogo.dados.alturaMinMorte = 0;

    fprintf(estatisticas, "Numero total de movimentos: %d\n", jogo.dados.numMovimentos); 
    fprintf(estatisticas, "Altura maxima que a galinha chegou: %d\n", jogo.dados.alturaMax);
    fprintf(estatisticas, "Altura maxima que a galinha foi atropelada: %d\n", jogo.dados.alturaMaxMorte);
    fprintf(estatisticas, "Altura minima que a galinha foi atropelada: %d\n", jogo.dados.alturaMinMorte);
    fprintf(estatisticas, "Numero de movimentos na direcao oposta: %d", jogo.dados.numMovParaTras);

    fclose(estatisticas);

}

void GerarArquivoResumo(int fimJogo, int iteracao, int pista, int posX, int posY, tCarro carro, char * caminho){

    FILE * resumo;

    char caminhoResumo[1100];

    sprintf(caminhoResumo, "%s/saida/resumo.txt", caminho);

    resumo = fopen(caminhoResumo, "a"); // adcionar a barra depois para correcao

    if (fimJogo) {
        fprintf(resumo, "[%d] Fim de jogo", iteracao);
    }
    else {
        fprintf(resumo, "[%d] Na pista %d o carro %d atropelou a galinha na posicao (%d,%d).\n", iteracao, pista, carro.index+1, posX, posY);
    }

    fclose(resumo);

}

void GerarArquivoRanking(tJogo jogo){

    FILE * ranking;

    char caminhoRanking[1100];

    sprintf(caminhoRanking, "%s/saida/ranking.txt", jogo.caminho);

    ranking = fopen(caminhoRanking, "w"); // adicionar / para correcao

    int i;

    /* for (i = 0; i < jogo.dados.qtdColisoes; i++){
        printf("PISTA COLISAO: %d | INDICE COLISAO: %d | ITERACAO COLISAO %d\n", jogo.dados.colisoes[i].pista, jogo.dados.colisoes[i].indice, jogo.dados.colisoes[i].iteracao);
    }
 */
    OrdenaVetorColisoes(jogo.dados.qtdColisoes, jogo.dados.colisoes);

    fprintf(ranking, "id_pista,id_carro,iteracao\n");

    int cont = 0;

    while (cont < jogo.dados.qtdColisoes){
        fprintf(ranking, "%d,%d,%d\n", jogo.dados.colisoes[cont].pista, jogo.dados.colisoes[cont].indice, jogo.dados.colisoes[cont].iteracao+1);
        cont++;
    }

    fclose(ranking);

}

void OrdenaVetorColisoes(int qtdColisoes, tColisao colisoes[]){

    int i, j;
    int cont = 0;

    tColisao colisaoAuxiliar;
    
    
    while (cont < qtdColisoes){
        
        int menorPista = 13;

        int menorColisao = 21;

        for (i = 0; i < qtdColisoes; i++){

            int pista = colisoes[i].pista;
    
            if (colisoes[i].ordenada != 1 && pista < menorPista){
                menorColisao = i;
                menorPista = pista;
            } 
            else if (pista == menorPista) {
    
                int indice = colisoes[i].indice;
    
                if (colisoes[i].ordenada != 1 && indice < colisoes[menorColisao].indice){
                    menorColisao = i;
                    menorPista = colisoes[i].pista;
                }
                else if (indice == colisoes[menorColisao].indice){

                    int iteracao = colisoes[i].iteracao;

                    if (colisoes[i].ordenada != 1 && iteracao > colisoes[menorColisao].iteracao){
                        menorColisao = i;
                    }
                }
            }
        }

        colisoes[menorColisao].ordenada = 1;

        colisaoAuxiliar = colisoes[cont];
        colisoes[cont] = colisoes[menorColisao];
        colisoes[menorColisao] = colisaoAuxiliar;

        cont++;

    }
}

//############## VALIDACAO DE ARQUIVOS ##############
void AbreArquivos(char * argv, FILE **config_inicial, FILE **personagens){
    char caminhoConfig[1100];
    char caminhoPersonagens[1100];
    
    char arquivoConfig[] = "/config_inicial.txt";
    char arquivoPersonagens[] = "/personagens.txt";

    geraCaminhoCompletoArquivos(argv, arquivoConfig, caminhoConfig);
    geraCaminhoCompletoArquivos(argv, arquivoPersonagens, caminhoPersonagens);

    *config_inicial = fopen(caminhoConfig, "r");
    *personagens = fopen(caminhoPersonagens, "r");

}
void geraCaminhoCompletoArquivos(char * diretorio, char * arquivo, char * caminho){

    strcpy(caminho, diretorio);
    strcat(caminho, arquivo);

}
void ValidaArquivos(char * argv, FILE *config_inicial, FILE *personagens){    
    int comprimento = strlen(argv);
    
    if (comprimento >= 1000) {
        printf("ERRO: Diretorio inválido (mais de 1000 caracteres)");
    }

    if (!config_inicial){

        printf("Erro ao abrir o arquivo ""config_inicial.txt"" no diretorio: %s", argv);
        exit(1);

    }
    
    if (!personagens){

        printf("Erro ao abrir o arquivo ""personagens.txt"" no diretorio: %s", argv);
        exit(1);
        
    }
}