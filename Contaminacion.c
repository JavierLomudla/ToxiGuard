#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "funciones.h"

int main() {
    ZonaContaminacion zonas[NUM_ZONAS];
    int numZonasActuales = 0;
    int opcion;

    // Inicializar las zonas
    for (int i = 0; i < NUM_ZONAS; i++) {
        zonas[i].nombreZona[0] = '\0';
        zonas[i].registros = NULL;
        zonas[i].numRegistros = 0;
        zonas[i].capacidadRegistros = 0;
    }

    // Cargar los datos desde el archivo al iniciar el programa
    cargarDatos(zonas, &numZonasActuales);

    do {
        opcion = menu();

        switch(opcion) {
            case 1:
                agregarDatos(zonas, &numZonasActuales);
                break;
            case 2:
                cambiarOMostrarDatos(zonas, numZonasActuales);
                break;
            case 3:
                predecirNivelesFuturos(zonas, numZonasActuales);
                break;
            case 4:
                crearRecomendaciones(zonas, numZonasActuales);
                break;
            case 5:
                crearAlertas(zonas, numZonasActuales);
                break;
            case 6:
                calcularPromediosHistoricos(zonas, numZonasActuales);
                break;
            case 7:
                generarReporteFinal(zonas, numZonasActuales);
                mostrarReporteEnPantalla();
                printf("\nGracias por usar el sistema.\n");
                break;
            default:
                printf("Opcion no valida.\n");
                break;
        }
    } while (opcion != 7);

    // Liberar memoria
    for (int i = 0; i < numZonasActuales; i++) {
        free(zonas[i].registros);
    }

    return 0;
}

