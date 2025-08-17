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


//Dados
typedef struct{

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

    int heatmap[35][100];
    tDados dados;
    tGalinha galinha;
    tCarro carros[120];

} tJogo;
tJogo InicializarJogo(FILE *config_inicial, FILE *personagens);
tJogo LeArquivosJogo(FILE * config_inicial, FILE * personagens);
void InicializaMapaLocal(int linhas, int colunas,char mapa[linhas][colunas]);
void PreencherMapa(tJogo jogo, int linhas, int colunas, char mapa[linhas][colunas]);
void ImprimeMapa(int pontos, int vidas, int iteracoes, int linhas, int colunas, char mapa[linhas][colunas]);
int TerminarJogo(tJogo jogo);
tJogo ContinuarJogo(tJogo jogo);
char LeComando();
int ChecarColisao(char comando, int qtdCarros, tCarro carros[], tGalinha galinha);



//Validacao dos Arquivos
void geraCaminhoCompletoArquivos(char * diretorio, char * arquivo, char * caminho);
void AbreArquivos(char * argv, FILE **config_inicial, FILE **personagens);
void ValidaArquivos(char * argv, FILE *config_inicial, FILE *personagens);



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

    jogo = InicializarJogo(config_inicial, personagens);

    while (!TerminarJogo(jogo)){

        jogo = ContinuarJogo(jogo);

    }
 
    /* GerarArquivoEstatistica();
    GerarArquivoResumo();
    GerarArquivoRanking();
    GerarArquivoHeatmap(); */

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

    tJogo jogoL;

    
    int i;
    char leitura;
    char comando;
    int colidiu;
    
    comando = LeComando();

    colidiu = ChecarColisao(comando, jogo.dados.qtdTotalCarros, jogo.carros, jogo.galinha);
    
    if ((comando == 'w' || comando == 's' || comando == ' ') && colidiu){
        //printf("Colidiu\n");
        jogoL.galinha = AtualizaGalinhaColisao(jogo.galinha, jogoL.dados.qtdPistas);
        jogoL.dados = AtualizaDadosColisao(jogo.dados, comando);
        //printf("Atualizou Dados\n");
        for (i = 0; i < jogo.dados.qtdTotalCarros; i++){
            jogoL.carros[i] = AtualizaCarro(jogo.carros[i], jogo.dados.colunas);
        }
    }
    else if (comando == 'w' || comando == 's' || comando == ' '){

        for (i = 0; i < jogo.dados.qtdTotalCarros; i++){
            jogoL.carros[i] = AtualizaCarro(jogo.carros[i], jogo.dados.colunas);
        }
        jogoL.galinha = AtualizaGalinha(jogo.galinha, comando, jogo.dados.qtdPistas);
        jogoL.dados = AtualizaDados(jogo.dados, comando);
        
    }

    //printf("Altura Max: %d | Altura Max Morte: %d | Altura Min Morte: %d\n", jogoL.dados.alturaMax, jogoL.dados.alturaMaxMorte, jogoL.dados.alturaMinMorte);

    int linhas = 3 * jogoL.dados.qtdPistas - 1;
    char mapaLocal[linhas][jogoL.dados.colunas];

    InicializaMapaLocal(linhas, jogoL.dados.colunas, mapaLocal);        
    PreencherMapa(jogoL, linhas, jogoL.dados.colunas, mapaLocal);
    ImprimeMapa(jogoL.dados.pontos, jogoL.galinha.vida, jogoL.dados.iteracao, linhas, jogoL.dados.colunas, mapaLocal);

    return jogoL;

}

int ChecarColisao(char comando, int qtdCarros, tCarro carros[], tGalinha galinha){

    int pistaPrevGalinha = 0;

    //printf("Pista da Galinha: %d\n", galinha.pista);

    if (comando == 'w'){
        pistaPrevGalinha = galinha.pista-1;
    }
    else if (comando == 's'){
        pistaPrevGalinha = galinha.pista+1;
    }
    else if (comando == ' '){
        pistaPrevGalinha = galinha.pista;
    }

    //printf("Pista prev galinha: %d\n", pistaPrevGalinha);

    int idxVetor;
    int prevPosCarro;
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

            posGalinha = galinha.posX;

            if (carros[idxVetor].direcao == 'D'){
                prevPosCarro = carros[idxVetor].posX + carros[idxVetor].velocidade;
            }
            else if (carros[idxVetor].direcao == 'E'){
                prevPosCarro = carros[idxVetor].posX - carros[idxVetor].velocidade;
            }

            for (j = -1; j < 2; j++){
                posChecagemCarro = 0;
                posGalinha = galinha.posX + i;
               
                
                if (carros[idxVetor].direcao == 'D'){
                    posChecagemCarro = prevPosCarro - j;
                }
                else if (carros[idxVetor].direcao == 'E'){
                    posChecagemCarro = prevPosCarro + j;
                }
                
                //printf("PrevPos do carro+i: %d | posGalinha+j: %d\n", prevPosCarro, posGalinha);
                if (posChecagemCarro == posGalinha) return 1;
            }
        }
    }

    return 0;

}

char LeComando(){

    char buffer[10];

    if (fgets(buffer, sizeof(buffer), stdin) != NULL) { // mudar isso depois
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
    }  

    else if (comando == 's') {
        dadosL.altura -= 3;
        dadosL.numMovParaTras++;
        dadosL.numMovimentos++;
    }

    if (dadosL.altura > dadosL.alturaMax) dadosL.alturaMax = dadosL.altura;

    return dadosL;

}

tDados AtualizaDadosColisao(tDados dados, char comando){

    tDados dadosL = dados;

    if (comando == 'w') {
        dadosL.numMovimentos++;
        dadosL.alturaMorte = dadosL.altura;
    }
    else if (comando == 's') {
        dadosL.numMovimentos++;
        dadosL.numMovParaTras++;
        dadosL.alturaMorte = dadosL.altura;
    }

    if (dadosL.alturaMorte < dadosL.alturaMinMorte) dadosL.alturaMinMorte = dadosL.alturaMorte;
    if (dadosL.alturaMorte > dadosL.alturaMaxMorte) dadosL.alturaMaxMorte = dadosL.alturaMorte;

    dadosL.iteracao++;
    dadosL.pontos = 0;


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
tJogo InicializarJogo(FILE *config_inicial, FILE *personagens){
    
    tJogo jogo;

    
    jogo = LeArquivosJogo(config_inicial, personagens);
    
    jogo.dados.altura = 0;
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
    PreencherMapa(jogo, linhas, jogo.dados.colunas, mapaInicialL);
    ImprimeMapa(jogo.dados.pontos, jogo.galinha.vida, jogo.dados.iteracao, linhas, jogo.dados.colunas, mapaInicialL);
    
    //jogo = InicializarHeatmap(jogo);
    //GerarArquivoInicializacao();

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
    
    for (i = 0; i < linhas; i++){
        
        if ((i+1) % 3 == 0) {
            pistaAtual++;
            continue;
        }

        for (idxCarro = 0; idxCarro < jogo.dados.qtdTotalCarros; idxCarro++) {
            if (jogo.carros[idxCarro].pista != pistaAtual) continue;
                                                     // aqui se usa j+1 por ser uma comparacao com a posX do carro, que considera as margens
            int x = jogo.carros[idxCarro].posX - 1;

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
void ImprimeMapa(int pontos, int vidas, int iteracoes, int linhas, int colunas, char mapa[linhas][colunas]) { 
    
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