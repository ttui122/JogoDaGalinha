#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_COLISOES 20
#define TAMANHO_MAX_CAMINHO 1100
#define MAX_LINHAS 35
#define MAX_COLUNAS 100
#define MAX_FRAMES 4
#define QTD_MAX_CARROS 120

//Colisoes
typedef struct{
    
    int pista;
    int indice;
    int iteracao;

} tColisao;
//Galinha ----------------------------------------------------------------------------------------------------------------------------------
typedef struct{

    int vida;
    int posX;
    int posXInicial;
    int posY;
    int pista;
    char matriz[2][3];

} tGalinha;    
tGalinha LeGalinha(FILE * config_inicial, tGalinha galinha, int pista);
tGalinha LeMatrizGalinha(FILE * personagens);
tGalinha AtualizaGalinha(tGalinha galinha, char comando, int ultimaPista);
tGalinha AtualizaColisaoGalinha(tGalinha galinha, int qdtPistas);
//Carros -----------------------------------------------------------------------------------------------------------------------------------
typedef struct{

    int velocidade;
    int pista;
    int posX;
    int index; 
    char direcao;
    char matriz[MAX_FRAMES][2][3];

} tCarro;    
tCarro LeCarro(FILE * config_inicial, int pistaAtual, tCarro carroBase, int index);
tCarro LeMatrizCarro(FILE * personagens, int animacao);
tCarro AtualizaCarro(tCarro carro, int colunas);
//Dados ------------------------------------------------------------------------------------------------------------------------------------
typedef struct{

    int qtdColisoes;
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
    int linhas;
    int qtdTotalCarros;
    int colunas;
    int animacao;
    tColisao colisoes[MAX_COLISOES];
    
} tDados;
tDados LeDados(FILE * config_inicial);
tDados AtualizaDados(tDados dados, char comando);
tDados AtualizaColisaoDados(tDados dados, char comando);
//Jogo -------------------------------------------------------------------------------------------------------------------------------------
typedef struct{

    char caminho[TAMANHO_MAX_CAMINHO];
    int heatmap[MAX_LINHAS][MAX_COLUNAS];
    tDados dados;
    tGalinha galinha;
    tCarro carros[QTD_MAX_CARROS];

} tJogo;
tJogo InicializarJogo(FILE *config_inicial, FILE *personagens, char * caminho);
tJogo InstanciaJogo(FILE * config_inicial, FILE * personagens);
tJogo InicializaHeatmapJogo(tJogo jogo);
tJogo AtualizaHeatmapJogo(tJogo jogo);
tJogo AtualizaHeatmapColisaoJogo(tJogo jogo, int posYColisao);
tJogo ContinuarJogo(tJogo jogo);
int TerminarJogo(tJogo jogo);
//Funcionamento ----------------------------------------------------------------------------------------------------------------------------
void InicializaMapaLocal(int linhas, int colunas,char mapa[linhas][colunas]);
void PreencherMapa(tJogo jogo, int linhas, int colunas, char mapa[linhas][colunas]);
void ImprimeMapa(tJogo jogo, int linhas, int colunas, char mapa[linhas][colunas]);
char LeComando();
int ChecarColisao(char comando, int qtdColisoes, tCarro carros[], tColisao colisoes[], tJogo jogo);
//Validacao dos Arquivos -------------------------------------------------------------------------------------------------------------------
void geraCaminhoCompletoArquivos(char * diretorio, char * arquivo, char * caminho);
void AbreArquivos(char * argv, FILE **config_inicial, FILE **personagens);
void ValidaArquivos(char * argv, FILE *config_inicial, FILE *personagens);
//Geracao de Arquivos ----------------------------------------------------------------------------------------------------------------------
void GerarArquivoInicializacao(tJogo jogo, int linhas, int colunas, char mapa[linhas][colunas], char * caminho);
void GerarArquivoEstatisticas(tJogo jogo);
void GerarArquivoResumo(int fimJogo, int iteracao, int pista, int posX, int posY, tCarro carro, char * caminho);
void GerarArquivoRanking(tJogo jogo);
void OrdenaVetorColisoes(int qtdColisoes, tColisao colisoes[]);
void GerarArquivoHeatmap(tJogo jogo);

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
    
    while (!TerminarJogo(jogo)){

        jogo = ContinuarJogo(jogo);

    }

    GerarArquivoResumo(1, jogo.dados.iteracao, 0, 0, 0, jogo.carros[0], jogo.caminho); // preciso apenas do 1, de fim de jogo
    GerarArquivoEstatisticas(jogo);
    GerarArquivoRanking(jogo);
    GerarArquivoHeatmap(jogo);

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
    char comando;
    int colidiu;
    int parar;
    
    comando = LeComando();

    if (comando == 'p') {
        if (parar == 0) parar = 1;
        else if (parar == 1) parar = 0;
    }

    if (parar != 1){
        for (i = 0; i < jogo.dados.qtdTotalCarros; i++){
            jogoL.carros[i] = AtualizaCarro(jogo.carros[i], jogo.dados.colunas);
        }    
    }
    
    colidiu = ChecarColisao(comando, jogoL.dados.qtdColisoes, jogoL.carros, jogo.dados.colisoes, jogo); // checa a colisao antes de atualizar a galinha
                                                                                                        // retorna um inteiro que representa o Y em que a galinha
    if (colidiu > 0){                                                                                   // sofreu a colisao para ser usado em outras funcoes
                                                                                                        // em caso de colisao, senao retorna 0
        if (jogo.dados.animacao == 1){
            
            int pistaColisao = colidiu/3 + 1;
            
            for (i = 0; i < jogo.dados.qtdTotalCarros; i++){           // para o caso bonus, se houve colisao na pista a velocidade dos carros nela descresce em 1
                if (jogoL.carros[i].pista != pistaColisao) continue;
                
                if (jogoL.carros[i].velocidade != 1) jogoL.carros[i].velocidade--;
            }
        }
        
        jogoL = AtualizaHeatmapColisaoJogo(jogoL, colidiu);
        
        jogoL.galinha = AtualizaColisaoGalinha(jogo.galinha, jogo.dados.qtdPistas); // realiza as atualizacoes no caso de colisao
        jogoL.dados = AtualizaColisaoDados(jogo.dados, comando);
        jogoL = AtualizaHeatmapJogo(jogoL);
        jogoL.dados.qtdColisoes++;
    }
    else{
        
        jogoL.galinha = AtualizaGalinha(jogo.galinha, comando, jogo.dados.qtdPistas);    // realiza as atualizacoes no caso de nao colisao
        jogoL.dados = AtualizaDados(jogo.dados, comando);
        
        jogoL = AtualizaHeatmapJogo(jogoL);
        
    }
    
    char mapaLocal[jogo.dados.linhas][jogo.dados.colunas];

    InicializaMapaLocal(jogo.dados.linhas, jogo.dados.colunas, mapaLocal);        
    PreencherMapa(jogoL, jogo.dados.linhas, jogo.dados.colunas, mapaLocal);     // imprime o mapa na tela
    ImprimeMapa(jogoL, jogo.dados.linhas, jogo.dados.colunas, mapaLocal);


    return jogoL;

}
int ChecarColisao(char comando, int qtdColisoes, tCarro carros[], tColisao colisoes[], tJogo jogo){

    int pistaPrevGalinha = 0; // duas variaveis que representam a previsao de onde a galinha estara com a mudanca,
    int posYPrevGalinha = 0;  // nao precisando mudar o valor real dela
    int posXPrevGalinha = 0;

    tGalinha galinha = jogo.galinha;
    tDados dados = jogo.dados;


    if (comando == 'w'){
        pistaPrevGalinha = galinha.pista-1;
        posYPrevGalinha = galinha.posY-3;
        posXPrevGalinha = galinha.posX;
    }
    else if (comando == 's'){
        pistaPrevGalinha = galinha.pista+1;
        posYPrevGalinha = galinha.posY+3;
        posXPrevGalinha = galinha.posX;
    }
    else if (comando == ' '){
        pistaPrevGalinha = galinha.pista;
        posYPrevGalinha = galinha.posY;
        posXPrevGalinha = galinha.posX;
    }
    else if (comando == 'a'){
        pistaPrevGalinha = galinha.pista;
        posYPrevGalinha = galinha.posY;
        posXPrevGalinha = galinha.posX-1;
    }   
    else if (comando == 'd'){
        pistaPrevGalinha = galinha.pista;
        posYPrevGalinha = galinha.posY;
        posXPrevGalinha = galinha.posX+1;
    }

    int idxVetor;
    int posChecagemCarro;
    int posGalinha;
    int i, j;
    
    for (idxVetor = 0; idxVetor < dados.qtdTotalCarros; idxVetor++){

        if (carros[idxVetor].pista != pistaPrevGalinha) continue;

        for (i = -1; i < 2; i++){
            for (j = -1; j < 2; j++){                                    // loopa pelos carros na mesma pista que a pista prevista que a galinha vai estar
                posGalinha = posXPrevGalinha + i;                        // e checa se alguma das tres partes do carro esta na mesma posicao que alguma 
                                                                         // das tres partes da galinha. Basta ver apenas a parte cima ou de baixo, nao precisa das duas
                posChecagemCarro = carros[idxVetor].posX + j;

                if (posChecagemCarro == posGalinha) {

                    colisoes[qtdColisoes].pista = pistaPrevGalinha;
                    colisoes[qtdColisoes].indice = carros[idxVetor].index+1; // armazena os dados no vetor colisoes que e usado para o ranking
                    colisoes[qtdColisoes].iteracao = dados.iteracao;

                    GerarArquivoResumo(0, dados.iteracao+1, pistaPrevGalinha, galinha.posX, posYPrevGalinha, carros[idxVetor], jogo.caminho);
                    
                    return posYPrevGalinha;
                    
                }
            }
        }
    }

    return 0;

}
char LeComando(){

    char comando;

    do {    
        scanf("%c", &comando);
    } while (comando != 'w' && comando != 's' && comando != ' ' && comando != 'a' && comando != 'd' && comando != 'p'); // repete o scanf ate receber um comando que seja valido

    return comando;

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
        if (dadosL.altura != 2) {
            dadosL.altura -= 3;
            dadosL.numMovParaTras++;
        } 
        dadosL.numMovimentos++;
    }

    if (dadosL.altura > dadosL.alturaMax) dadosL.alturaMax = dadosL.altura;

    return dadosL;

}
tDados AtualizaColisaoDados(tDados dados, char comando){

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
    case 'w':   //comandos galinha pra frente;
        galinhaL.posY -= 3;
        galinhaL.pista--;
        break;
    case 's':   //comandos galinha pra tras;
        if (galinha.pista == ultimaPista) {
            galinhaL.posY = galinha.posY;
            galinhaL.pista = galinha.pista;
        }
        else {
            galinhaL.posY += 3;
            galinhaL.pista++;
        }
        break;
    case ' ':   //comandos galinha ficar parada;
        galinhaL.posY = galinha.posY;
        galinhaL.pista = galinha.pista;
        break;
    case 'a':
        galinhaL.posY = galinha.posY;
        galinhaL.pista = galinha.pista;
        galinhaL.posX--;
        break;
    case 'd':
        galinhaL.posY = galinha.posY;
        galinhaL.pista = galinha.pista;
        galinhaL.posX++;
        break;
    default:
        break;
    }
    
    return galinhaL;
    
}
tGalinha AtualizaColisaoGalinha(tGalinha galinha, int ultimaPista){

    tGalinha galinhaL = galinha;

    galinhaL.pista = ultimaPista;
    galinhaL.vida--;
    galinhaL.posY = (3 * ultimaPista - 1) - 1;
    galinhaL.posX = galinha.posXInicial;

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
    
    if (carroL.posX > colunas) carroL.posX %= colunas;  
                                                            // faz o centro do carro voltar para dentro do mapa ao ultrapassar a margem
    else if (carroL.posX < 1) carroL.posX += colunas;

    return carroL;

}
tJogo AtualizaHeatmapJogo(tJogo jogo){

    tJogo jogoL = jogo;

    int posXGalinha = jogo.galinha.posX;
    int posYGalinha = jogo.galinha.posY;

    int celEsquerdaX = posXGalinha - 2;                 // incrementa em 1 no heatmap no respectivo espaco por onde a galinha passou para cada pedaco dela 
    int celCentroX = posXGalinha - 1;                   // se for menor que o limite de 99 e nao tiver acontecido uma colisao ali
    int celDireitaX = posXGalinha;

    if (jogoL.heatmap[posYGalinha][celEsquerdaX] < 99 && jogoL.heatmap[posYGalinha][celEsquerdaX] != -1) jogoL.heatmap[posYGalinha][celEsquerdaX] += 1;
    if (jogoL.heatmap[posYGalinha][celCentroX] < 99 && jogoL.heatmap[posYGalinha][celCentroX] != -1) jogoL.heatmap[posYGalinha][celCentroX] += 1;
    if (jogoL.heatmap[posYGalinha][celDireitaX] < 99 && jogoL.heatmap[posYGalinha][celDireitaX] != -1) jogoL.heatmap[posYGalinha][celDireitaX] += 1;
    
    if (jogoL.heatmap[posYGalinha-1][celEsquerdaX] < 99 && jogoL.heatmap[posYGalinha-1][celEsquerdaX] != -1) jogoL.heatmap[posYGalinha-1][celEsquerdaX] += 1;
    if (jogoL.heatmap[posYGalinha-1][celCentroX] < 99 && jogoL.heatmap[posYGalinha-1][celCentroX] != -1) jogoL.heatmap[posYGalinha-1][celCentroX] += 1;
    if (jogoL.heatmap[posYGalinha-1][celDireitaX] < 99 && jogoL.heatmap[posYGalinha-1][celDireitaX] != -1) jogoL.heatmap[posYGalinha-1][celDireitaX] += 1;

    return jogoL;

}
tJogo AtualizaHeatmapColisaoJogo(tJogo jogo, int posYColisao){

    tJogo jogoL = jogo;

    int i, j;
    int linhas = jogo.dados.linhas;
    int colunas = jogo.dados.colunas;

    for (i = 0; i < linhas; i++){                     // preenche toda a pista onde houve colisao com -1 no heatmap, para depois na impressao
        for (j = 0; j < colunas; j++){                // do arquivo se saiba onde preencher com asteriscos

            if (i == posYColisao - 1) jogoL.heatmap[i][j] = -1;
            else if (i == posYColisao) jogoL.heatmap[i][j] = -1;

        } 
    }

    return jogoL;

}

//############## LEITURA DOS ARQUIVOS ###############
tJogo InstanciaJogo(FILE * config_inicial, FILE * personagens){

    tJogo jogo;


    tGalinha galinhaMatriz = LeMatrizGalinha(personagens); // Desenho da galinha


    jogo.dados = LeDados(config_inicial);
    jogo.dados.linhas = 3 * jogo.dados.qtdPistas - 1;     // Dados do jogo   
    jogo.dados.altura = 2;                                
    jogo.dados.alturaMinMorte = 999;
    
    // Carro ---------------------------------------
    int pistaAtual = 1, idxVetor = 0, qtdCarros, i;
    char direcao, lixo;
    tCarro carroBase;

    carroBase = LeMatrizCarro(personagens, jogo.dados.animacao); // Desenho do carro

    while (pistaAtual < jogo.dados.qtdPistas){

        fscanf(config_inicial, "%c", &direcao); 

        if (direcao == '\n') pistaAtual++;  // linha de chegada e pistas vazias
        else {

            carroBase.direcao = direcao;

            fscanf(config_inicial, " %d %d", &carroBase.velocidade, &qtdCarros);                                                     
                                                                                              // escaneia cada linha, gerando um vetor de carros, com os primeiros
            for (i = 0; i < qtdCarros; i++){                                                  // elementos sendo os carros da primeira pista
                jogo.carros[idxVetor] = LeCarro(config_inicial, pistaAtual, carroBase, i);    // e assim por diante                         
                idxVetor++; 
                jogo.dados.qtdTotalCarros++;
            }

            fscanf(config_inicial, "%*[^\n]");
            fscanf(config_inicial, "%c", &lixo);
            
            pistaAtual++;

        }
    }

    jogo.galinha = LeGalinha(config_inicial, galinhaMatriz, jogo.dados.qtdPistas); // Le os dados da galinha

    fclose(config_inicial);
    fclose(personagens);
    
    return jogo;

}
tDados LeDados(FILE * config_inicial){
    
    tDados dados = {0};
    char lixo;

    fscanf(config_inicial, "%d\n", &dados.animacao); // primeira linha
    fscanf(config_inicial, "%d %d", &dados.colunas, &dados.qtdPistas); // segunda linha
    fscanf(config_inicial, "%c", &lixo);

    return dados;

}
tGalinha LeGalinha(FILE * config_inicial, tGalinha galinha, int pista){

    tGalinha galinhaL = galinha;
    galinhaL.pista = pista;
    galinhaL.posY = (3 * pista - 1) - 1;

    char letra; // G

    fscanf(config_inicial, "%c %d %d", &letra, &galinhaL.posXInicial, &galinhaL.vida);

    galinhaL.posX = galinhaL.posXInicial;

    return galinhaL;

}
tGalinha LeMatrizGalinha(FILE * personagens){

    tGalinha galinha;

    int i, j;
    char lixo;

    for (i = 0; i < 2; i++) {
        for (j = 0; j < 3; j++)
        {
            fscanf(personagens, "%c", &galinha.matriz[i][j]);
        }
        fscanf(personagens, "%c", &lixo); // \n
    }

    return galinha;

}
tCarro LeCarro(FILE * config_inicial, int pistaAtual, tCarro carroBase, int index){
    
    tCarro carro;

    carro = carroBase;
    carro.pista = pistaAtual;
    carro.index = index;
    fscanf(config_inicial, " %d", &carro.posX);

    return carro;

}
tCarro LeMatrizCarro(FILE * personagens, int animacao){

    tCarro carro;

    int i, j, frame;
    char lixo;

    if (animacao == 0) {            // checa se o caso pede por animacao, se nao, pega apenas o primero desenho e o guarda no frame 0
        for (i = 0; i < 2; i++) {
            for (j = 0; j < 3; j++) {
                fscanf(personagens, "%c", &carro.matriz[0][i][j]); 
            }
            fscanf(personagens, "%c", &lixo); // \n
        }
    }
    else {                          // se sim, repete o processo de ler o desenho 4 vezes e armazena cada um em seu respectivo frame

        for (frame = 0; frame < 4; frame++){
            for (i = 0; i < 2; i++) {
                for (j = 0; j < 3; j++) {
                    fscanf(personagens, "%c", &carro.matriz[frame][i][j]); 
                }
                fscanf(personagens, "%c", &lixo); // \n
            }

        }

    }

    return carro;

}

//############## GERENCIAMENTO DO MAPA ##############
tJogo InicializarJogo(FILE *config_inicial, FILE *personagens, char * caminho){
    
    tJogo jogo;
    
    jogo = InstanciaJogo(config_inicial, personagens);

    char mapaInicialL[jogo.dados.linhas][jogo.dados.colunas];

    InicializaMapaLocal(jogo.dados.linhas, jogo.dados.colunas, mapaInicialL);
    PreencherMapa(jogo, jogo.dados.linhas, jogo.dados.colunas, mapaInicialL);
    ImprimeMapa(jogo, jogo.dados.linhas, jogo.dados.colunas, mapaInicialL);
    
    jogo = InicializaHeatmapJogo(jogo);

    GerarArquivoInicializacao(jogo, jogo.dados.linhas, jogo.dados.colunas, mapaInicialL, caminho);

    return jogo;

}
void InicializaMapaLocal(int linhas, int colunas, char mapa[linhas][colunas]){
    
    int i, j;

    for (i = 0; i < linhas; i++){
        for (j = 0; j < colunas; j++){                                  // Cria o "molde" do mapa, sem bordas e com as pistas vazias
            if ((i+1) % 3 == 0 && (j+1) % 3 != 0) mapa[i][j] = '-';
            else mapa[i][j] = ' ';
        }
    }
    
}
void PreencherMapa(tJogo jogo, int linhas, int colunas, char mapa[linhas][colunas]){
    
    int pistaAtual = 1;
    int idxCarro = 0;
    int i, j;
    int iteracao = jogo.dados.iteracao;
    int frame;
    
    if (jogo.dados.animacao) frame = iteracao % 4;    // checa se e animacao, se for calcula o frame da animacao do carro a partir da iteracao atual
    else frame = 0;                                   // se nao, utiliza sempre o mesmo frame, o primeiro

    i = jogo.galinha.posY - 1;     // calcula a posicao da galinha dentro da matriz do mapa
    j = jogo.galinha.posX - 1;
    
    mapa[i][j-1] = jogo.galinha.matriz[0][0];
    mapa[i][j] = jogo.galinha.matriz[0][1];
    mapa[i][j+1] = jogo.galinha.matriz[0][2];       // Desenha a galinha no mapa
    mapa[i+1][j-1] = jogo.galinha.matriz[1][0];
    mapa[i+1][j] = jogo.galinha.matriz[1][1];
    mapa[i+1][j+1] = jogo.galinha.matriz[1][2];
    
    for (i = 0; i < linhas; i++){
        
        if ((i+1) % 3 == 0) {
            pistaAtual++;               // Pula as linhas que são divisao de pista e incrementa a pista atual do preenchimento
            continue;
        }

        for (idxCarro = 0; idxCarro < jogo.dados.qtdTotalCarros; idxCarro++) {

            if (jogo.carros[idxCarro].pista != pistaAtual) continue;

            int centro = jogo.carros[idxCarro].posX - 1;         // calcula a posicao que cada parte do carro vai assumir levando em consideracao
            int direita  = (centro + 1) % colunas;               // que ele retorna ao inicio ou fim, dependendo da direcao, quando ultrapassa a margem
            int esquerda;
            if (centro <= 0) esquerda = centro - 1 + colunas;
            else esquerda = centro - 1;


            mapa[i][esquerda] = jogo.carros[0].matriz[frame][0][0];
            mapa[i][centro] = jogo.carros[0].matriz[frame][0][1];
            mapa[i][direita] = jogo.carros[0].matriz[frame][0][2];
                                                                            // Loopa pelos carros que estao na pista que esta sendo preenchida
            mapa[i+1][esquerda] = jogo.carros[0].matriz[frame][1][0];       //  e os aloca na posicao da matriz com base na sua posicao
            mapa[i+1][centro] = jogo.carros[0].matriz[frame][1][1];
            mapa[i+1][direita] = jogo.carros[0].matriz[frame][1][2];
                
        }

        i++;
        
    }  
}
void ImprimeMapa(tJogo jogo, int linhas, int colunas, char mapa[linhas][colunas]){ 
    
    int i, j;
    int pontos = jogo.dados.pontos;
    int vidas = jogo.galinha.vida;
    int iteracoes = jogo.dados.iteracao;

    printf("Pontos: %d | Vidas: %d | Iteracoes: %d\n", pontos, vidas, iteracoes); // Imprime o cabecalho

    for (j = 0; j < colunas + 2; j++){
        if (j == 0 || j == colunas+1) printf("|");     // Imprime a margem superior
        else printf("-");
    }
    printf("\n");

    for (i = 0; i < linhas; i++){
        for (j = 0; j < colunas + 2; j++){
            if (j == 0 || j == colunas+1) printf("|"); // Imprime o mapa, ja preenchido
            else printf("%c", mapa[i][j-1]);
        }
        printf("\n");
    }

    for (j = 0; j < colunas + 2; j++) {                 // Imprime a margem inferior
        if (j == 0 || j == colunas+1) 
            printf("|");
        else 
            printf("-");
    }
    printf("\n");

}

//##############  CRIACAO DE ARQUIVOS  ##############
tJogo InicializaHeatmapJogo(tJogo jogo){ 

    tJogo jogoL = jogo;

    int i, j;                                   // Como o heatmap ja foi inicializado com todo campo igual a zero em InstaciaJogo,
    int linhas = jogo.dados.linhas;             // apenas atribui o calor da galinha na posicao incial
    int colunas = jogo.dados.colunas;

    int posX = jogo.galinha.posX - 1;

    for (i = 0; i < linhas; i++){
        for (j = 0; j < colunas; j++){
            if ((i == linhas-1 || i == linhas - 2) && (j == posX || j == posX + 1 || j == posX - 1)) jogoL.heatmap[i][j] = 1;
        }
    }

    return jogoL;

}
void GerarArquivoHeatmap(tJogo jogo){
    
    FILE * heatmap;

    char caminhoHeatmap[TAMANHO_MAX_CAMINHO];

    sprintf(caminhoHeatmap, "%s/saida/heatmap.txt", jogo.caminho);

    heatmap = fopen(caminhoHeatmap, "w");

    int i, j;   
    int linhas = jogo.dados.linhas;
    int colunas = jogo.dados.colunas;

    for (i = 0; i < linhas; i++){
        for (j = 0; j < colunas; j++){
            if (jogo.heatmap[i][j] == -1) fprintf(heatmap, " * ");
            else fprintf(heatmap, "%2d ", jogo.heatmap[i][j]);
        }
        fprintf(heatmap, "\n");
    }

    fclose(heatmap);

}
void GerarArquivoInicializacao(tJogo jogo, int linhas, int colunas, char mapa[linhas][colunas], char * caminho){

    FILE * inicializacao;

    char caminhoIni[TAMANHO_MAX_CAMINHO];

    sprintf(caminhoIni, "%s/saida/inicializacao.txt", caminho);

    inicializacao = fopen(caminhoIni, "w");

    int i, j;

    for (j = 0; j < colunas + 2; j++){                              
        if (j == 0 || j == colunas+1) fprintf(inicializacao, "|");  // Imprime a margem de cima no arquivo 
        else fprintf(inicializacao, "-");
    }
    fprintf(inicializacao, "\n");

    for (i = 0; i < linhas; i++){
        for (j = 0; j < colunas + 2; j++){
            if (j == 0 || j == colunas+1) fprintf(inicializacao, "|");  // Imprime o mapa preenchido arquivo
            else fprintf(inicializacao, "%c", mapa[i][j-1]);
        }
        fprintf(inicializacao, "\n");
    }

    for (j = 0; j < colunas + 2; j++) {
        if (j == 0 || j == colunas+1) fprintf(inicializacao, "|");  // Imprime a margem de baixo no arquivo
        else fprintf(inicializacao, "-");
    }
    fprintf(inicializacao, "\n");

    fprintf(inicializacao, "A posicao central da galinha iniciara em (%d %d).", jogo.galinha.posX, jogo.galinha.posY);

    fclose(inicializacao);

}
void GerarArquivoEstatisticas(tJogo jogo){

    FILE * estatisticas;

    char caminhoEst[TAMANHO_MAX_CAMINHO];

    sprintf(caminhoEst, "%s/saida/estatistica.txt", jogo.caminho);

    estatisticas = fopen(caminhoEst, "w");

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

    char caminhoResumo[TAMANHO_MAX_CAMINHO];

    sprintf(caminhoResumo, "%s/saida/resumo.txt", caminho);

    resumo = fopen(caminhoResumo, "a");

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

    char caminhoRanking[TAMANHO_MAX_CAMINHO];

    sprintf(caminhoRanking, "%s/saida/ranking.txt", jogo.caminho);

    ranking = fopen(caminhoRanking, "w");

    OrdenaVetorColisoes(jogo.dados.qtdColisoes, jogo.dados.colisoes);

    fprintf(ranking, "id_pista,id_carro,iteracao\n");

    int cont = 0;

    while (cont < jogo.dados.qtdColisoes){
        fprintf(ranking, "%d,%d,%d\n", jogo.dados.colisoes[cont].pista, jogo.dados.colisoes[cont].indice, jogo.dados.colisoes[cont].iteracao+1);
        cont++;
    }

    fclose(ranking);

}
void OrdenaVetorColisoes(int qtdColisoes, tColisao colisoes[]) {
    
    int i, j, idxMenor;
    tColisao colisaoAuxiliar;

    for (i = 0; i < qtdColisoes - 1; i++) {                 
        
        idxMenor = i;

        for (j = i + 1; j < qtdColisoes; j++) {

            if (colisoes[j].pista < colisoes[idxMenor].pista) idxMenor = j;
            else if (colisoes[j].pista == colisoes[idxMenor].pista && colisoes[j].indice < colisoes[idxMenor].indice) idxMenor = j;
            
            else if (colisoes[j].pista == colisoes[idxMenor].pista && colisoes[j].indice == colisoes[idxMenor].indice && colisoes[j].iteracao > colisoes[idxMenor].iteracao) idxMenor = j;
        }

        if (idxMenor != i) {
            colisaoAuxiliar = colisoes[i];
            colisoes[i] = colisoes[idxMenor];
            colisoes[idxMenor] = colisaoAuxiliar;
        }
    } 
}

//############## VALIDACAO DE ARQUIVOS ##############
void AbreArquivos(char * argv, FILE **config_inicial, FILE **personagens){
    char caminhoConfig[TAMANHO_MAX_CAMINHO];
    char caminhoPersonagens[TAMANHO_MAX_CAMINHO];
    
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
