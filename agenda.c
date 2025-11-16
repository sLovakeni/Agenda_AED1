#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAMANHO_BLOCO_CONTROLE ( ( 6 * sizeof( int ) ) + ( 100 * sizeof( char ) ) )

#define REG_A ( *( int * )( pMemoria ) )
#define REG_B ( *( int * )( pMemoria + sizeof( int ) ) )
#define REG_C ( *( int * )( pMemoria + 2 * sizeof( int ) ) )
#define REG_D ( *( int * )( pMemoria + 3 * sizeof( int ) ) )

#define TAMANHO_TOTAL_ALOCADO ( *( int * )( pMemoria + 4 * sizeof( int ) ) )
#define CONTAGEM_REGISTROS ( *( int * )( pMemoria + 5 * sizeof( int ) ) )

#define BUFFER_ENTRADA_NOME ( ( char * )( pMemoria + 6 * sizeof( int ) ) )
#define BUFFER_ENTRADA_EMAIL ( ( char * )( pMemoria + 6 * sizeof( int ) + 50 * sizeof( char ) ) )

void * exibirMenu( void * pMemoria );
void * adicionarRegistro( void * pMemoria );
void * removerRegistro( void * pMemoria );
void buscarRegistro( void * pMemoria );
void listarRegistros( void * pMemoria );

int main() {
    void * pMemoria = malloc( TAMANHO_BLOCO_CONTROLE );

    if ( pMemoria == NULL ) {
        printf( "Falha crítica ao alocar memória inicial.\n" );
        return 1;
    }

    TAMANHO_TOTAL_ALOCADO = TAMANHO_BLOCO_CONTROLE;
    CONTAGEM_REGISTROS = 0;

    while ( 1 ) {
        pMemoria = exibirMenu( pMemoria );
    }

    return 0;
}

void * exibirMenu( void * pMemoria ) {
    printf( "\n--- AGENDA EM BUFFER ---\n" );
    printf( "1. Adicionar\n" );
    printf( "2. Remover\n" );
    printf( "3. Buscar\n" );
    printf( "4. Listar Todos\n" );
    printf( "5. Sair\n" );
    printf( "Escolha uma opcao: " );

    scanf( "%d", &REG_A );
    getchar();

    switch ( REG_A ) {
        case 1:
            pMemoria = adicionarRegistro( pMemoria );
            break;
        case 2:
            pMemoria = removerRegistro( pMemoria );
            break;
        case 3:
            buscarRegistro( pMemoria );
            break;
        case 4:
            listarRegistros( pMemoria );
            break;
        case 5:
            printf( "Saindo... liberando memoria.\n" );
            free( pMemoria );
            exit( 0 );
        default:
            printf( "Opcao invalida!\n" );
            break;
    }
    return pMemoria;
}

void * adicionarRegistro( void * pMemoria ) {
    printf( "-- Adicionar Registro --\n" );
    printf( "Nome: " );
    scanf( " %49[^\n]", BUFFER_ENTRADA_NOME );
    getchar();

    printf( "Idade: " );
    scanf( "%d", &REG_B );
    getchar();

    printf( "Email: " );
    scanf( " %49[^\n]", BUFFER_ENTRADA_EMAIL );
    getchar();

    REG_C = strlen( BUFFER_ENTRADA_NOME ) + 1;
    REG_D = strlen( BUFFER_ENTRADA_EMAIL ) + 1;

    REG_A = REG_C + sizeof( int ) + REG_D;

    void * pNovoBuffer = realloc( pMemoria, TAMANHO_TOTAL_ALOCADO + REG_A );

    if ( pNovoBuffer == NULL ) {
        printf( "ERRO: Falha ao alocar memória para novo registro! Nada foi adicionado.\n" );
        return pMemoria;
    }

    pMemoria = pNovoBuffer;

    void * pCursor = pMemoria + TAMANHO_TOTAL_ALOCADO;

    strcpy( pCursor, BUFFER_ENTRADA_NOME );
    pCursor += REG_C;
    *( int * )pCursor = REG_B;
    pCursor += sizeof( int );
    strcpy( pCursor, BUFFER_ENTRADA_EMAIL );

    CONTAGEM_REGISTROS++;
    TAMANHO_TOTAL_ALOCADO += REG_A;

    printf( "Registro adicionado com sucesso!\n" );
    return pMemoria;
}

void * removerRegistro( void * pMemoria ) {
    if ( CONTAGEM_REGISTROS == 0 ) {
        printf( "Agenda vazia. Nada para remover.\n" );
        return pMemoria;
    }

    printf( "-- Remover Registro --\n" );
    printf( "Digite o ID do registro a remover (0 a %d): ", CONTAGEM_REGISTROS - 1 );

    scanf( "%d", &REG_A );
    getchar();

    if ( REG_A < 0 || REG_A >= CONTAGEM_REGISTROS ) {
        printf( "ID invalido!\n" );
        return pMemoria;
    }

    void * pCursor = pMemoria + TAMANHO_BLOCO_CONTROLE;

    REG_B = 0;

    for ( REG_C = 0; REG_C < REG_A; REG_C++ ) {
        pCursor += strlen( ( char * )pCursor ) + 1;
        pCursor += sizeof( int );
        pCursor += strlen( ( char * )pCursor ) + 1;
    }

    void * pRegistroRemover = pCursor;

    REG_B = strlen( ( char * )pCursor ) + 1;
    pCursor += REG_B;
    REG_B += sizeof( int );
    pCursor += sizeof( int );
    REG_B += strlen( ( char * )pCursor ) + 1;
    void * pProximoRegistro = pCursor;


    REG_C = ( pMemoria + TAMANHO_TOTAL_ALOCADO ) - pProximoRegistro;

    if ( REG_C > 0 ) {
        memmove( pRegistroRemover, pProximoRegistro, REG_C );
    }

    CONTAGEM_REGISTROS--;
    TAMANHO_TOTAL_ALOCADO -= REG_B;
    void * pNovoBuffer = realloc( pMemoria, TAMANHO_TOTAL_ALOCADO );

    if ( pNovoBuffer == NULL && TAMANHO_TOTAL_ALOCADO > 0 ) {
        printf( "Aviso: Falha ao tentar reduzir o buffer. Memória não foi liberada.\n" );
        return pMemoria;
    }

    printf( "Registro ID %d removido.\n", REG_A );

    return pNovoBuffer;
}

void buscarRegistro( void * pMemoria ) {
    if ( CONTAGEM_REGISTROS == 0 ) {
        printf( "Agenda vazia.\n" );
        return;
    }

    printf( "-- Buscar Registro --\n" );
    printf( "Digite o nome (ou parte) para buscar: " );
    scanf( " %49[^\n]", BUFFER_ENTRADA_NOME );
    getchar();

    void * pCursor = pMemoria + TAMANHO_BLOCO_CONTROLE;
    REG_B = 0;

    for ( REG_A = 0; REG_A < CONTAGEM_REGISTROS; REG_A++ ) {

        if ( strstr( ( char * )pCursor, BUFFER_ENTRADA_NOME ) != NULL ) {

            printf( "--- ID: %d ---\n", REG_A );
            printf( "  Nome: %s\n", ( char * )pCursor );
            pCursor += strlen( ( char * )pCursor ) + 1;

            printf( "  Idade: %d\n", *( int * )pCursor );
            pCursor += sizeof( int );
            printf( "  Email: %s\n\n", ( char * )pCursor );
            pCursor += strlen( ( char * )pCursor ) + 1;

            REG_B = 1;
        } else {

            pCursor += strlen( ( char * )pCursor ) + 1;
            pCursor += sizeof( int );
            pCursor += strlen( ( char * )pCursor ) + 1;
        }
    }

    if ( REG_B == 0 ) {
        printf( "Nenhum registro encontrado com o nome: %s\n", BUFFER_ENTRADA_NOME );
    }
}

void listarRegistros( void * pMemoria ) {
    if ( CONTAGEM_REGISTROS == 0 ) {
        printf( "Agenda vazia.\n" );
        return;
    }

    printf( "--- LISTANDO TODOS OS REGISTROS (%d) ---\n", CONTAGEM_REGISTROS );

    void * pCursor = pMemoria + TAMANHO_BLOCO_CONTROLE;

    for ( REG_A = 0; REG_A < CONTAGEM_REGISTROS; REG_A++ ) {
        printf( "--- ID: %d ---\n", REG_A );

        printf( "  Nome: %s\n", ( char * )pCursor );
        pCursor += strlen( ( char * )pCursor ) + 1;

        printf( "  Idade: %d\n", *( int * )pCursor );
        pCursor += sizeof( int );

        printf( "  Email: %s\n\n", ( char * )pCursor );
        pCursor += strlen( ( char * )pCursor ) + 1;
    }
}
