#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "asserts.h"
#include "state.h"

/**
 * TAREA: Pruebas personalizadas.
 * - Pruebas unitarias para funciones auxiliares y lógicas del juego.
 */

/* Prueba para is_tail */
bool test_is_tail() {
    return assert_true("is_tail('w')", is_tail('w')) &&
           assert_false("is_tail('W')", is_tail('W'));
}

/* Prueba para is_head */
bool test_is_head() {
    return assert_true("is_head('W')", is_head('W')) &&
           assert_false("is_head('w')", is_head('w'));
}

/* Prueba para create_default_state */
bool test_create_default_state() {
    game_state_t* state = create_default_state();
    bool result = assert_true("Tablero inicial no es NULL", state != NULL) &&
                  assert_true("Número de filas es 18", state->num_rows == 18) &&
                  assert_true("Número inicial de serpientes es 1", state->num_snakes == 1);
    free_state(state);
    return result;
}

/* Prueba para initialize_snakes */
bool test_initialize_snakes() {
    game_state_t* state = create_default_state();
    state = initialize_snakes(state);
    bool result = assert_true("Número de serpientes después de inicializar es 1", state->num_snakes == 1) &&
                  assert_true("La serpiente está viva", state->snakes[0].live);
    free_state(state);
    return result;
}

/* Ejecutar todas las pruebas */
bool test_customs() {
    return test_is_tail() &&
           test_is_head() &&
           test_create_default_state() &&
           test_initialize_snakes();
}

/* Función principal */
int main() {
    if (!test_customs()) {
        printf("Algunas pruebas fallaron.\n");
        return 1;
    }
    printf("Todas las pruebas pasaron.\n");
    return 0;
}