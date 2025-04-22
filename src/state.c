#include "state.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "snake_utils.h"

/* Tarea 1: Crea el estado inicial del juego */
game_state_t* create_default_state() {
    game_state_t* state = malloc(sizeof(game_state_t));
    state->num_rows = 18;
    state->board = malloc(state->num_rows * sizeof(char*));
    
    // Tablero predeterminado (18 filas x 20 columnas)
    char* default_board[] = {
        "####################",
        "#                  #",
        "# d>D    *         #",
        "#                  #",
        "#                  #",
        "#                  #",
        "#                  #",
        "#                  #",
        "#                  #",
        "#                  #",
        "#                  #",
        "#                  #",
        "#                  #",
        "#                  #",
        "#                  #",
        "#                  #",
        "#                  #",
        "####################"
    };
    
    for (int i = 0; i < state->num_rows; i++) {
        state->board[i] = strdup(default_board[i]);
    }
    
    // Inicializa la serpiente
    state->num_snakes = 1;
    state->snakes = malloc(sizeof(snake_t));
    state->snakes[0].tail_row = 2;
    state->snakes[0].tail_col = 2;
    state->snakes[0].head_row = 2;
    state->snakes[0].head_col = 4;
    state->snakes[0].live = true;
    
    return state;
}

/* Tarea 2: Libera la memoria */
void free_state(game_state_t* state) {
    for (int i = 0; i < state->num_rows; i++) {
        free(state->board[i]);
    }
    free(state->board);
    free(state->snakes);
    free(state);
}

/* Tarea 3: Imprime el tablero */
void print_board(game_state_t* state, FILE* fp) {
    for (int i = 0; i < state->num_rows; i++) {
        fprintf(fp, "%s\n", state->board[i]);
    }
}

/* Tarea 4.1: Funciones auxiliares */
bool is_tail(char c) {
    return (c == 'w' || c == 'a' || c == 's' || c == 'd');
}

bool is_head(char c) {
    return (c == 'W' || c == 'A' || c == 'S' || c == 'D' || c == 'x');
}

bool is_snake(char c) {
    return (is_tail(c) || is_head(c) || c == '^' || c == '<' || c == 'v' || c == '>');
}

char body_to_tail(char c) {
    switch (c) {
        case '^': return 'w';
        case '<': return 'a';
        case 'v': return 's';
        case '>': return 'd';
        default: return '?';
    }
}

char head_to_body(char c) {
    switch (c) {
        case 'W': return '^';
        case 'A': return '<';
        case 'S': return 'v';
        case 'D': return '>';
        default: return '?';
    }
}

unsigned int get_next_row(unsigned int cur_row, char c) {
    if (c == 'v' || c == 's' || c == 'S') return cur_row + 1;
    if (c == '^' || c == 'w' || c == 'W') return cur_row - 1;
    return cur_row;
}

unsigned int get_next_col(unsigned int cur_col, char c) {
    if (c == '>' || c == 'd' || c == 'D') return cur_col + 1;
    if (c == '<' || c == 'a' || c == 'A') return cur_col - 1;
    return cur_col;
}

/* Tarea 4.2: Obtiene el siguiente cuadro */
static char next_square(game_state_t* state, unsigned int snum) {
    snake_t snake = state->snakes[snum];
    char head = get_board_at(state, snake.head_row, snake.head_col);
    unsigned int next_row = get_next_row(snake.head_row, head);
    unsigned int next_col = get_next_col(snake.head_col, head);
    return get_board_at(state, next_row, next_col);
}

/* Tarea 4.3: Actualiza la cabeza */
static void update_head(game_state_t* state, unsigned int snum) {
    snake_t* snake = &(state->snakes[snum]);
    char head = get_board_at(state, snake->head_row, snake->head_col);
    
    // Nueva posición
    unsigned int new_row = get_next_row(snake->head_row, head);
    unsigned int new_col = get_next_col(snake->head_col, head);
    
    // Actualiza el tablero: cabeza antigua -> cuerpo
    set_board_at(state, snake->head_row, snake->head_col, head_to_body(head));
    
    // Actualiza la estructura
    snake->head_row = new_row;
    snake->head_col = new_col;
    set_board_at(state, new_row, new_col, head); // Mantiene el mismo carácter de cabeza
}

/* Tarea 4.4: Actualiza la cola */
static void update_tail(game_state_t* state, unsigned int snum) {
    snake_t* snake = &(state->snakes[snum]);
    char tail = get_board_at(state, snake->tail_row, snake->tail_col);
    
    // Nueva posición
    unsigned int new_row = get_next_row(snake->tail_row, tail);
    unsigned int new_col = get_next_col(snake->tail_col, tail);
    char new_tail = get_board_at(state, new_row, new_col);
    
    // Actualiza el tablero: cola antigua -> espacio vacío
    set_board_at(state, snake->tail_row, snake->tail_col, ' ');
    
    // Actualiza la estructura
    snake->tail_row = new_row;
    snake->tail_col = new_col;
    set_board_at(state, new_row, new_col, body_to_tail(new_tail));
}

/* Tarea 4.5: Actualiza el estado del juego */
void update_state(game_state_t* state, int (*add_food)(game_state_t* state)) {
    for (unsigned int i = 0; i < state->num_snakes; i++) {
        if (!state->snakes[i].live) continue;
        
        char next = next_square(state, i);
        if (next == ' ') {
            update_head(state, i);
            update_tail(state, i);
        } else if (next == '*') {
            update_head(state, i);
            add_food(state); // Genera nueva comida
        } else {
            // Choque con pared o serpiente
            state->snakes[i].live = false;
            set_board_at(state, state->snakes[i].head_row, state->snakes[i].head_col, 'x');
        }
    }
}

/* Tarea 5: Carga el tablero desde un archivo */
game_state_t* load_board(char* filename) {
    FILE* fp = fopen(filename, "r");
    if (!fp) return NULL;
    
    game_state_t* state = malloc(sizeof(game_state_t));
    state->num_rows = 0;
    state->board = NULL;
    
    char line[1024];
    while (fgets(line, sizeof(line), fp)) {
        state->num_rows++;
        state->board = realloc(state->board, state->num_rows * sizeof(char*));
        line[strcspn(line, "\n")] = '\0'; // Elimina el salto de línea
        state->board[state->num_rows - 1] = strdup(line);
    }
    
    fclose(fp);
    state->num_snakes = 0;
    state->snakes = NULL;
    return state;
}

/* Tarea 6.1: Encuentra la cabeza de la serpiente */
static void find_head(game_state_t* state, unsigned int snum) {
    snake_t* snake = &(state->snakes[snum]);
    unsigned int row = snake->tail_row;
    unsigned int col = snake->tail_col;
    char current = get_board_at(state, row, col);
    
    while (!is_head(current)) {
        row = get_next_row(row, current);
        col = get_next_col(col, current);
        current = get_board_at(state, row, col);
    }
    
    snake->head_row = row;
    snake->head_col = col;
}

/* Tarea 6.2: Inicializa las serpientes */
game_state_t* initialize_snakes(game_state_t* state) {
    // Contar serpientes (colas)
    state->num_snakes = 0;
    for (int i = 0; i < state->num_rows; i++) {
        for (int j = 0; j < strlen(state->board[i]); j++) {
            if (is_tail(state->board[i][j])) state->num_snakes++;
        }
    }
    
    state->snakes = malloc(state->num_snakes * sizeof(snake_t));
    int snum = 0;
    for (int i = 0; i < state->num_rows; i++) {
        for (int j = 0; j < strlen(state->board[i]); j++) {
            if (is_tail(state->board[i][j])) {
                state->snakes[snum].tail_row = i;
                state->snakes[snum].tail_col = j;
                state->snakes[snum].live = true;
                find_head(state, snum);
                snum++;
            }
        }
    }
    return state;
}
