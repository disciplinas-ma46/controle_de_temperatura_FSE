
// temperatura_referencia.h
/**
 * Como requisitos, o usuário do sistema pode definir a temperatura de referência (TR) 
 *   ou temperatura desejada de duas maneiras:
 * 1) Através de um Potenciômetro;
 * 2) Através de entrada de teclado no terminal.
*/

#ifndef TR_H_
#define TR_H_

#include <stdio.h>
#include <unistd.h>

/** definições de struct e funções */
typedef struct TR {
    // não ler diretamente, usar ler_tr()
    int temperatura_referencia;

    //"class" functions
    void (*close)();
    float (*ler_tr)();
    void (*atualizar_tr)(float );

} TR;
TR * new_potenciometro_tr();
TR * new_teclado_tr();
float ler_tr_p();
void atualizar_tr_p();
void close_p();
float ler_tr_t();
void atualizar_tr_t();
void close_t();
void teste_tr();


TR * potenciometro_tr = NULL;
TR * teclado_tr = NULL;


TR * new_potenciometro_tr() {
    if(!potenciometro_tr) {
        potenciometro_tr = (TR*)malloc(sizeof(TR));

        potenciometro_tr->temperatura_referencia = 20.5;

        // métodos
        potenciometro_tr->close = close_p;
        potenciometro_tr->ler_tr = ler_tr_p;
        potenciometro_tr->atualizar_tr = atualizar_tr_p;
    }

    return potenciometro_tr;
}

TR * new_teclado_tr() {
    if(!teclado_tr) {
        teclado_tr = (TR*)malloc(sizeof(TR));

        teclado_tr->temperatura_referencia = 20.5;

        // métodos
        teclado_tr->close = close_t;
        teclado_tr->ler_tr = ler_tr_t;
        teclado_tr->atualizar_tr = atualizar_tr_t;
    }

    return teclado_tr;
}


// métodos do potenciometro
float ler_tr_p() {
    potenciometro_tr->atualizar_tr();
    return potenciometro_tr->temperatura_referencia;
}
void atualizar_tr_p() {
    potenciometro_tr->temperatura_referencia += 1;
}
void close_p() {
    free(potenciometro_tr);
}


// métodos do teclado
float ler_tr_t() {
    return teclado_tr->temperatura_referencia;
}
void atualizar_tr_t() {
    printf("Digite a nova temperatura de referencia: ");
    scanf("%f", teclado_tr->temperatura_referencia);
    // fix validar entrada
    // printf("Valor lido: %f", teclado_tr->temperatura_referencia);
}
void close_t() {
    free(teclado_tr);
}

void teste_tr() {
    TR * t = new_teclado_tr();
    printf("Valor t: %f", t->ler_tr());
    t->atualizar_tr();
    printf("Valor t: %f", t->ler_tr());

    TR * p = new_potenciometro_tr();
    printf("Valor p: %f", t->ler_tr());
    t->atualizar_tr();
    printf("Valor p: %f", t->ler_tr());

}



#endif /* TR_H_ */

