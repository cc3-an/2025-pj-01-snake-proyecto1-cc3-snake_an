#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "state.h"

/**
 * Función para agregar comida al tablero.
 * Esta función encuentra una posición vacía y coloca un '*' (comida) allí.
 * Devuelve 1 si tuvo éxito, de lo contrario 0.
 */
int add_food(game_state_t* state) {
    for (unsigned int i = 0; i < state->num_rows; i++) {
        for (unsigned int j = 0; j < strlen(state->board[i]); j++) {
            if (state->board[i][j] == ' ') {
                state->board[i][j] = '*';
                return 1;
            }
        }
    }
    return 0; // No hay espacio para comida
}

int main() {
    // Crear el estado del juego
    game_state_t* state = create_default_state();

    // Mostrar el tablero inicial
    printf("Tablero inicial:\n");
    print_board(state, stdout);

    // Inicializar las serpientes en el tablero
    state = initialize_snakes(state);

    // Ejecutar el bucle principal del juego
    printf("\nActualizando estado del juego...\n");
    for (int i = 0; i < 5; i++) { // Simular 5 pasos del juego
        update_state(state, add_food);
        print_board(state, stdout);
        printf("\n");
    }

    // Liberar memoria
    free_state(state);

    return 0;
}