# Como compilar

Terminal 1 - Diretório *Server*

    make 
    make run

Terminal 2 - Diretório *Client*
    
    make EXEC=client1
    make run EXEC=client1

Terminal 3 - Diretório *Client*

    make EXEC=client2
    make run EXEC=client2

**OBS:** Não sei porque, mas dá uns erros estranhos no socket quando só roda duas vezes o mesmo executável.

# Protocolo

**Cliente** tenta conectar servidor para participar do jogo:

"WHO AM I"

**Servidor** responde com *player number* do cliente:

"<number>"

**Cliente** envia informação das teclas:

"KEYS<player_number><key_up_bool><key_down_bool>"

**Servidor** envia configuração:

"CONF <xPlayer1> <yPlayer1> <xPlayer2> <yPlayer2> <xBall> <yBall> <scorePlayer1> <scorePlayer2>"