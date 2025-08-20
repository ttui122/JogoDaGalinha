## Tutorial rápido de utilização do Script de Correção ##

Dentro da pasta `script` existem 3 outras pastas: 
  - `Casos`: contém os casos de teste com as entradas e as saídas para cada caso.
  - `Gabarito-obj`: contém os arquivos objetos (`.o`) do código correto feito pelo professor. Não é necessário se preocupar com ela.
  - `Respostas`: contém a resposta do aluno.
    - Em um caso em que essa pasta não exista. Você precisa criar `Respostas/NomeAluno/` e colocar o código resposta do trabalho dentro dela. O código dentro da pasta deve ser nomeado com o nome do aluno (`NomeAluno.c`), sem espaços e sem acentos, por exemplo `ThiagoOliveiraDosSantos.c`.

Abra um terminal na pasta `script` e execute o script com o comando:

```bash
./correcao.sh
```

**Importante:** caso seja exibido um erro de permissão, você deve executar o comando `chmod +x correcao.sh` para dar permissão de execução ao script.

Depois que executar, o script irá fazer o seguinte:

 1. Verificar se o aluno criou o arquivo fonte com o código corretamente.
 2. Compilar o código do aluno e verificar se existe erro de compilação.
 3. O script irá criar uma pasta `script/Resultados/NomeAluno/completo` com o código compilado `prog`, o resultado da compilação `result_compilation.txt` e o resultado da linkagem `result_linking.txt`. Além disso ele fará uma cópia da pasta `script/Casos` para dentro dessa pasta, porém sem as saídas.
 4. O script irá executar o programa `prog` compilado do aluno para cada caso da pasta `script/Resultados/NomeAluno/completo/Casos`.
 5. Verificar se os resultados da execução da pasta `script/Resultados/NomeAluno/completo/Casos` estão de acordo com os resultados dos casos da pasta `script/Casos/`.
 6. Dar uma nota final para o aluno gerando um arquivo chamado `script/Resultado.csv`, considerando os casos e suas partes corretas.

Então note que, toda vez que você quiser testar o seu código, o script deverá ser executado novamente para que os passos acima sejam refeitos. Se você quiser testar um caso manualmente com o código compilado pelo script você pode navegar para a pasta com o código compilado `script/Resultados/NomeAluno/completo` e executar o programa apontando para o caso em questão, por exemplo, para testar o caso 1 manualmente executaria `prog ./Casos/1`.

 ### Estrutura dos diretórios de casos
```
    script/
        Casos/
            1/
                saida/
                    estatisticas.txt
                    heatmap.txt
                    inicializacao.txt
                    ranking.txt
                    resumo.txt
                    saida.txt
                config_inicial.txt
                entrada.txt
                personagens.txt
            2/
                saida/
                    estatisticas.txt
                    heatmap.txt
                    inicializacao.txt
                    ranking.txt
                    resumo.txt
                    saida.txt
                config_inicial.txt
                entrada.txt
                personagens.txt
            .
            .
            .
            x/
                saida/
                    estatisticas.txt
                    heatmap.txt
                    inicializacao.txt
                    ranking.txt
                    resumo.txt
                    saida.txt
                config_inicial.txt
                entrada.txt
                personagens.txt
```

 ### Estrutura dos diretórios de Resultados
```
    Resultados/
        NomeAluno/
            completo/
                Casos/
                    1/
                        saida/
                        config_inicial.txt
                        entrada.txt
                        personagens.txt
                    2/
                        saida/
                        config_inicial.txt
                        entrada.txt
                        personagens.txt
                    .
                    .
                    .
                    x/
                        saida/
                        config_inicial.txt
                        entrada.txt
                        personagens.txt
                prog
                result_compilation.txt
                result_linking.txt
                NomeAluno.c
                NomeAluno.o
            resultado.txt
```
