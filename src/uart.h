// #include "uart.h"
#include <stdio.h>
#include <unistd.h>         //Used for UART
#include <fcntl.h>          //Used for UART
#include <termios.h>        //Used for UART


#ifndef UART_H_
#define UART_H_

/** definições de struct e funções */
typedef struct uart {
    int uart0_filestream;

    //"class" functions
    void (*close)();
    void (*envia_string)(char *);
    void (*envia_int)(int );
    void (*solicita_float)();
} uart;

void close();
void envia_string(char *);
void envia_int(int );
void solicita_float();
void configura_uart(uart *);
uart* new_uart();
void teste_uart();


/* singleton para UART */

uart * uart_object = NULL;

uart* new_uart()
{
    if(!uart_object) {
        uart_object = (uart*)malloc(sizeof(uart));

        uart_object->uart0_filestream = open("/dev/serial0", O_RDWR | O_NOCTTY | O_NDELAY);      //Open in non blocking read/write mode

        if (uart_object->uart0_filestream == -1) {
            printf("Erro - Não foi possível iniciar a UART.\n");
            return NULL;
        }
        else {
            printf("UART inicializada!\n");
        }

        configura_uart(uart_object);

        // métodos
        uart_object->close = close_uart;
        uart_object->envia_string = envia_string;
        uart_object->envia_int = envia_int;
        uart_object->solicita_float = solicita_float;
    }

    return uart_object;
}

void close_uart() {
    close(uart_object->uart0_filestream);
    free(uart_object);
}


void envia_string(char *mensagem) {
    unsigned char envio[200];
    int tamanho = 0;
    envio[0] = 0xB3;
    int matricula[] = {5, 4, 8, 1};

    while (mensagem[tamanho]) {
        envio[tamanho+2] = mensagem[tamanho];
        tamanho++;
    }
    envio[1] = tamanho;

    for(int i=0;i<4; i++) {
        envio[2+tamanho+i] = matricula[i];
    }

    int count = write(uart0_filestream, envio, tamanho+6);
    if (count < 0) {
        printf("UART TX error\n");
        exit(0);
    }
    printf("escrito.\n");
}

void envia_int(int numero) {
    unsigned char envio[200];// = {0xB3, 9, 'o','l','a',' ', 'g', 'e','n','t','e', 13, 5, 4, 8, 1};
    envio[0] = 0xB1;
    int matricula[] = {5, 4, 8, 1};

    memcpy(&envio[1], &numero, 4);

    for(int i=0;i<4; i++) {
        envio[5+i] = matricula[i];
    }

    int count = write(uart0_filestream, envio, 9);
    if (count < 0) {
        printf("UART TX error\n");
        exit(0);
    }
    printf("escrito.\n");
}

void solicita_float() {
    unsigned char envio[200] = {0xA1, 5, 4, 8, 1};

    int count = read(uart0_filestream, envio, 5);
    if (count < 0) {
        printf("UART TX error\n");
        // exit(0);
    }
    printf("escrito. %d\n", count);

    // return count;
}

void configura_uart(uart *u) {
    struct termios options;
    tcgetattr(u->uart0_filestream, &options);
    options.c_cflag = B9600 | CS8 | CLOCAL | CREAD;     //<Set baud rate
    options.c_iflag = IGNPAR;
    options.c_oflag = 0;
    options.c_lflag = 0;
    tcflush(u->uart0_filestream, TCIFLUSH);
    tcsetattr(u->uart0_filestream, TCSANOW, &options);
}


void teste_uart() {
    uart *teste = new_uart();
    teste->envia_string("Testando uart...");
    teste->envia_int(25);
    teste->close();
}

#endif /* UART_H_ */

