# Como compilar

Para compilar, basta criar uma pasta build e utilizar o CMake.

    mkdir build 
    cmake ..
    make

Após isso, basta executar o servidor e os clientes com

    ./Server
    ./Client

Também é possível jogar Offline:

    ./Offline
    
# Protocolo

**Cliente** tenta conectar servidor para participar do jogo:

"WHO AM I"

**Servidor** responde com *player number* do cliente:

"<number>"

**Cliente** envia informação das teclas:

"KEYS<player_number><key_up_bool><key_down_bool>"

**Servidor** envia configuração:

"CONF <xPlayer1> <yPlayer1> <xPlayer2> <yPlayer2> <xBall> <yBall> <scorePlayer1> <scorePlayer2>"