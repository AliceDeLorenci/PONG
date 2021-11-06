# Online Pong Game

Este jogo foi desenvolvido para o trabalho da disciplina de Redes de Computadores (SSC0641) da Universidade de São Paulo, ministrada pelo Prof. Dr. Rodolfo Ipolito Meneguette. Os membros do grupo são:
- Alessandro de Freitas Guerreiro (11233891)
- Alice Valença De Lorenci (11200289)
- Lucas Furco Granela (11299978)
- Matheus Yasuo Ribeiro Utino (11233689)
- Vinícius Silva Montanari (11233709)

## Requisitos de sistema

O programa requer o compilador gcc e a versão C++17 da linguagem C++. Ademais, é compátivel com os seguintes sistemas operacionais:
- Ubuntu 20.04.2 LTS
- ...

## Como compilar

Para compilar, basta criar uma pasta build e utilizar o CMake:

    mkdir build && cd build
    cmake ..
    make

Após isso, basta executar o servidor e os clientes:

    ./Server
    ./Client

Também é possível jogar Offline:

    ./Offline
    
## Dependências

* Debian: `sudo apt install freeglut3-dev libpng-dev libspdlog-dev -y`

## Protocolo de comunicação

Foi adotada uma arquitetura de rede centralizada, em que os clientes comunicam-se diretamente com o servidor, responsável pelo processamento do jogo.

A comunicação é iniciada por um cliente, que estabelece uma conexão TCP com o servidor a fim de participar do jogo. Se a conexão for realizada com sucesso, o servidor responde com o número de jogador que foi atribuído a esse cliente. A conexão TCP é utilizada apenas para fins de controle, em que é essencial garantir o recebimento da mensagem.

Uma vez estabelecida a conexão TCP, deve ser estabelecida uma conexão UDP, por meio da qual transitarão os dados do jogo. Para isso, o cliente envia uma mensagem UDP ao servidor informando o seu número de jogador.
 
### Conexão

- **Cliente** inicia conexão TCP com o servidor para participar do jogo.
- **Servidor** responde com *player number* do cliente:

        "\<number>"
- **Cliente** inicia conexão UDP:

        "I AM \<player number>"

### Fluxo de dados 

- **Cliente** envia informação das teclas:

        "KEYS\<player_number>\<key_up_bool>\<key_down_bool>"

- **Servidor** envia configuração do jogo:

        "\<xPlayer1> \<yPlayer1> \<xPlayer2> \<yPlayer2> \<xBall> \<yBall> \<scorePlayer1> \<scorePlayer2>"

### Encerramento do jogo

Fechando uma das janelas, seja do servidor, seja de um dos clientes, é enviada aos demais uma mensagem anunciando o fim do jogo aos demais componenetes por meio da conexão TCP:
        
        "EXIT"

## Debug

Instead of just using `cmake ..` use `cmake -DCMAKE_BUILD_TYPE:STRING=Debug ..`. Don't forget to delete the cache when switching between the build types.
