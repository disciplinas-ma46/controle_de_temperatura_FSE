
// controle.h
/**
 * O software da bancada de testes deve implementar duas estratégias de controle simples: On/Off e PID. O uso de cada estratégia também é definida pelo usuário do sistema através de 2 mecanismos.
 *
 * 1) Através de botão switch físico;
 * 2) Através de entrada de teclado no terminal.
*/

#ifndef Controle_H_
#define Controle_H_

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

/** definições de struct e funções */
typedef struct Controle {

    // atributos para on/off
    double histerese;

    //"class" functions
    void (*close)();
    double (*calcular_porcentagem)(double temperatura_referencia, double temperatura_interna);
    
} Controle;

Controle * new_controle_on_off();
Controle * new_controle_PID();
void teste_controle();
void close_on_off();
void close_PID();
double calcular_porcentagem_on_off();
double calcular_porcentagem_PID();

Controle * controle_on_off = NULL;
Controle * controle_PID = NULL;


Controle * new_controle_on_off() {
    if(!controle_on_off) {
        controle_on_off = (Controle*)malloc(sizeof(Controle));

        controle_on_off->histerese = 4.0;

        // métodos
        controle_on_off->close = close_on_off;
        controle_on_off->calcular_porcentagem = calcular_porcentagem_on_off;
    }

    return controle_on_off;
}

Controle * new_controle_PID() {
    if(!controle_PID) {
        controle_PID = (Controle*)malloc(sizeof(Controle));

        // métodos
        controle_PID->close = close_PID;
        controle_PID->calcular_porcentagem = calcular_porcentagem_PID;
    }

    return controle_PID;
}


// métodos do Controle On/Off
double calcular_porcentagem_on_off(double temperatura_referencia, double temperatura_interna) {
    double h = controle_on_off->histerese;
    double diferenca = temperatura_interna - temperatura_referencia;

    // se a diferença da temperatura interna e de referência estiver dentro de 
    // uma margem de aceitação (histerese) nada é ativado (0%)
    if(diferenca >= 0 && diferenca < h) {
        return 0.0;
    }
    if(diferenca < 0 && -diferenca < h) {
        return 0.0;
    }

    return diferenca > 0 ? 100.0 : -100.0;

}

void close_on_off() {
    free(controle_on_off);
}


// métodos do Controle PID
double calcular_porcentagem_PID(double temperatura_referencia, double temperatura_interna) {
    // return controle_PID->temperatura_referencia;
    return 0.0;
}

void close_PID() {
    free(controle_PID);
}

void teste_controle() {
    double porcentagem;
    printf("teste de controle on/off\n");
    Controle * p = new_controle_on_off();
    // parametros para cáclulo: temperatura_referencia, temperatura_interna
    porcentagem = p->calcular_porcentagem(40.0, 60.0);
    printf("esse valor deve ser 100: %f \n", porcentagem);
    porcentagem = p->calcular_porcentagem(51.4, 32.0);
    printf("esse valor deve ser -100: %f \n", porcentagem);
    porcentagem = p->calcular_porcentagem(50.0, 53.0);
    printf("esse valor deve ser 0: %f \n", porcentagem);
    porcentagem = p->calcular_porcentagem(20.0, 16.5);
    printf("esse valor deve ser 0: %f \n", porcentagem);
    porcentagem = p->calcular_porcentagem(22.7, 22.7);
    printf("esse valor deve ser 0: %f \n", porcentagem);
    

    // Controle * t = new_controle_PID();
    // printf("Valor t: %f\n", t->ler_tr());
    // t->atualizar_tr();
    // printf("Valor t: %f\n", t->ler_tr());

    

}



#endif /* Controle_H_ */

