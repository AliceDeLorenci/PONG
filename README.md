# Como compilar

Para compilar, basta criar uma pasta build e utilizar o CMake.

    mkdir build
    cd build
    cmake ..
    make

Após isso, basta executar o servidor e os clientes com

    ./Server
    ./Client

Também é possível jogar Offline:

    ./Offline
    
# Protocolo

**Cliente** inicia conexão TCP com o servidor para participar do jogo:

**Servidor** responde com *player number* do cliente:

TCP: "<number>"

**Cliente** inicia conexão UDP (para que o servidor saiba o endereço):

UDP: "I AM <number>"

**Cliente** envia informação das teclas:

"KEYS<player_number><key_up_bool><key_down_bool>"

**Servidor** envia configuração:

"CONF <xPlayer1> <yPlayer1> <xPlayer2> <yPlayer2> <xBall> <yBall> <scorePlayer1> <scorePlayer2>"

# Quitting

User quits on Server:
- ```(Server) OnUserDestroy()```: sends messages to clients announcing exit
- ```(Server) listener thread```: end
- ```(Clinet) OnUserUpdate()```: return false 