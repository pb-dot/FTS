## Installing the pkg :-

        sudo apt install ./FTS.deb

## Removing the pkg :-

        sudo apt purge FTS

## Norms of Usage :-

1. ----> **Go To https://github.com/pb-dot/FTS and Read the ReadMe.md**
2. ----> **The only difference is the repo focuses on building and runing from source files**
3. ----> **Ie U can think as This .deb pkg has only _exe names changed to runSomething rest same**
 
## How To Run :-

1. cd /opt/FTS

2. As per example setup Follow the below order(top 2 bottom) strictly <br>

        1st Terminal cd inside Server/build& run>  runServer  <Server_Port>
        2nd Terminal cd inside LoadBalancer/build& run>  runLB <lb_PORT> <config_File_PAth>
        3rd Terminal cd inside Switch/build& run>  runSwitch <SW_PORT> <config_File_PAth>
        4th Terminal cd inside Client/build& run>  runCLI <SW_IP> <SW_Port> <r/w> 
