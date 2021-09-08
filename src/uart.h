// #include "uart.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>         //Used for UART
#include <fcntl.h>          //Used for UART
#include <termios.h>        //Used for UART
#include "crc16.h"

#ifndef UART_H_
#define UART_H_

/** definições de struct e funções */
typedef struct uart {
    int uart0_filestream;

    //"class" functions
    void (*close)();
    float (*solicita_TI)();

    void (*envia_string)(char *);
    void (*envia_int)(int );
    void (*solicita_float)();
} uart;

void close_uart();
void envia_string(char *);
void envia_int(int );
void solicita_float();
void configura_uart(uart *);
uart* new_uart();
void teste_uart();
float solicita_TI();

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
        uart_object->solicita_TI = solicita_TI;

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

    int count = write(uart_object->uart0_filestream, envio, tamanho+6);
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

    int count = write(uart_object->uart0_filestream, envio, 9);
    if (count < 0) {
        printf("UART TX error\n");
        exit(0);
    }
    printf("escrito.\n");
}

void solicita_float() {
    unsigned char envio[200] = {0xA1, 5, 4, 8, 1};

    int count = read(uart_object->uart0_filestream, envio, 5);
    if (count < 0) {
        printf("UART TX error\n");
        // exit(0);
    }
    printf("escrito. %d\n", count);

    // return count;
}

float solicita_TI() {
    // unsigned char envio[200] = {0x01, 0x23, 0xC1, 5, 4, 8, 1, 0x4F, 0xAD};
    unsigned char envio[200] = {0x01, 0x23, 0xC1, 0x60, 0x89};
    int tamanho = 3;
    short cod_crc = calcula_CRC(envio, tamanho);
    // unsigned char * char_crc = (unsigned char *) &cod_crc);

    // envio[tamanho] = ([];

    memcpy(&envio[tamanho], &cod_crc, 2);
    envio[tamanho+2] = envio[tamanho];
    envio[tamanho] = envio[tamanho+1];
    envio[tamanho+1] = envio[tamanho+2];
    tamanho += 2;
    // envio[7] = (unsigned char)(  )

    printf("[crc %x] Codigo de envio: ", cod_crc);
    for(int i=0;i<tamanho;i++) {
		printf("%x ", envio[i]);
	}
    printf("\n");


    int count = read(uart_object->uart0_filestream, envio, tamanho);
    if (count < 0) {
        printf("UART TX error: erro ao solicitar TI\n");
        return 0.0;
        // exit(0);
    }
    // printf("TI solicitado\n");

    // fix definir outro tempo, pois será usado um SIGALARM a cada 1 segundo
    sleep(1);

    //----- CHECK FOR ANY RX BYTES -----
    // Read up to 255 characters from the port if they are there
    unsigned char rx_buffer[256];
    int rx_length = read(uart_object->uart0_filestream, (void*)rx_buffer, 255);      //Filestream, buffer to store in, number of bytes to read (max)
    if (rx_length < 0)
    {
        printf("Erro na leitura de buffer\n"); //An error occured (will occur if there are no bytes)
    }
    else if (rx_length == 0)
    {
        printf("Nenhum dado disponível.\n"); //No data waiting
    }
    else
    {
        //Bytes received
        rx_buffer[rx_length] = '\0';
        // printf("%i Bytes lidos : %s\n", rx_length, rx_buffer);
        float temperatura_interna;
        memcpy(&temperatura_interna, rx_buffer, 4*sizeof(unsigned char));
        return temperatura_interna;
    }

    return 0.0;
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
    // teste->envia_string("Testando uart...");
    // teste->envia_int(25);
    float ti = teste->solicita_TI();
    printf("Temperatura interna lida: %f \n", ti);
    teste->close();
}

#endif /* UART_H_ */

