#include "funciones.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>  // Para toupper, para convertir la entrada del usuario

// --- Menu function ---
int menu() {
    int opcion;
    printf("\n___ TOXI-GUARD ___\n");
    printf("\n___ Sistema de Gestion Ambiental ___\n");
    printf("1. Ingresar nuevos datos\n");
    printf("2. Mostrar Datos Especificos por Zona\n");
    printf("3. Prediccion general de niveles futuros\n");
    printf("4. Ver recomendaciones generales\n");
    printf("5. Ver alertas totales\n");
    printf("6. Resumen historico de datos\n");
    printf("7. Guardar y salir\n________________________________________\n");
    printf("Opcion>> ");
    scanf("%d", &opcion);
    return opcion;
}

// Ayuda a analizar fecha en formato YYYY/MM/DD, toda la buena vibra
void parsearFecha(const char *fechaStr, int *year, int *month, int *day) {
    sscanf(fechaStr, "%d-%d-%d", year, month, day);
}

//Cargador de datos
void cargarDatos(ZonaContaminacion zonas[], int *numZonasActuales) {
    FILE *archivo = fopen("datos.txt", "r");
    if (!archivo) {
        perror("Error al abrir el archivo 'datos.txt'. Se creara/actualizara al guardar.");
        return;
    }

    char linea[256];
    fgets(linea, sizeof(linea), archivo); // Skip encabezado

    *numZonasActuales = 0; // Resetea zonas actuales

    while (fgets(linea, sizeof(linea), archivo)) {
        char fechaStr[12], zonaStr[50];
        float co2, so2, no2, pm25;

        if (sscanf(linea, "%[^,], %[^,], %f, %f, %f, %f", fechaStr, zonaStr, &co2, &so2, &no2, &pm25) == 6) {
            int zonaIndex = -1;

            for (int i = 0; i < *numZonasActuales; i++) {
                if (strcmp(zonas[i].nombreZona, zonaStr) == 0) {
                    zonaIndex = i;
                    break;
                }
            }

            if (zonaIndex == -1) {
                if (*numZonasActuales < NUM_ZONAS) {
                    zonaIndex = *numZonasActuales;
                    strncpy(zonas[zonaIndex].nombreZona, zonaStr, sizeof(zonas[zonaIndex].nombreZona) - 1);
                    zonas[zonaIndex].nombreZona[sizeof(zonas[zonaIndex].nombreZona) - 1] = '\0';
                    zonas[zonaIndex].numRegistros = 0;
                    zonas[zonaIndex].capacidadRegistros = 10; //capacidad inicial
                    zonas[zonaIndex].registros = (DatoContaminacion *)malloc(sizeof(DatoContaminacion) * zonas[zonaIndex].capacidadRegistros);
                    if (zonas[zonaIndex].registros == NULL) {
                        perror("Error: Fallo de asignacion de memoria para registros de zona");
                        continue;
                    }
                    (*numZonasActuales)++;
                } else {
                    printf("Advertencia: Maximo de zonas (%d) alcanzado. Ignorando datos de '%s'.\n", NUM_ZONAS, zonaStr);
                    continue;
                }
            }

            if (zonaIndex != -1) {
                if (zonas[zonaIndex].numRegistros >= zonas[zonaIndex].capacidadRegistros) {
                    zonas[zonaIndex].capacidadRegistros *= 2; // Double capacity
                    DatoContaminacion *temp = (DatoContaminacion *)realloc(zonas[zonaIndex].registros, sizeof(DatoContaminacion) * zonas[zonaIndex].capacidadRegistros);
                    if (temp == NULL) {
                        perror("Error: Fallo de reasignacion de memoria para registros de zona");
                        continue;
                    }
                    zonas[zonaIndex].registros = temp;
                }

                DatoContaminacion nuevoDato;
                strncpy(nuevoDato.fecha, fechaStr, sizeof(nuevoDato.fecha) - 1);
                nuevoDato.fecha[sizeof(nuevoDato.fecha) - 1] = '\0';
                parsearFecha(fechaStr, &nuevoDato.year, &nuevoDato.month, &nuevoDato.day);
                nuevoDato.co2 = co2;
                nuevoDato.so2 = so2;
                nuevoDato.no2 = no2;
                nuevoDato.pm25 = pm25;

                zonas[zonaIndex].registros[zonas[zonaIndex].numRegistros] = nuevoDato;
                zonas[zonaIndex].numRegistros++;
            }
        } else {
            printf("Advertencia: Formato de linea invalido en 'datos.txt': %s", linea);
        }
    }
    fclose(archivo);
}

// Salvador-inador
void guardarTodosLosDatos(ZonaContaminacion zonas[], int numZonasActuales) {
    FILE *archivo = fopen("datos.txt", "w"); // Open in write mode (truncates/overwrites)
    if (!archivo) {
        perror("Error al guardar el archivo de datos");
        return;
    }

    fprintf(archivo, "Fecha, Zona, CO2, SO2, NO2, PM2.5\n"); // Write header

    for (int i = 0; i < numZonasActuales; i++) {
        for (int j = 0; j < zonas[i].numRegistros; j++) {
            fprintf(archivo, "%s, %s, %.2f, %.2f, %.2f, %.2f\n",
                    zonas[i].registros[j].fecha,
                    zonas[i].nombreZona,
                    zonas[i].registros[j].co2,
                    zonas[i].registros[j].so2,
                    zonas[i].registros[j].no2,
                    zonas[i].registros[j].pm25);
        }
    }
    fclose(archivo);
    printf("Todos los datos han sido guardados en 'datos.txt'.\n");
}

//Agregador-complejador-inador
void agregarDatos(ZonaContaminacion zonas[], int *numZonasActuales) {
    char zonaStr[50];
    char fechaStr[12];
    float co2, so2, no2, pm25;
    int c;

    printf("\n--- Agregar nuevos datos ---\n");

    do {
        printf("Ingrese nombre de la zona (Norte, Sur, Este, Oeste, Centro): ");
        scanf(" %[^\n]", zonaStr);
        if (strlen(zonaStr) == 0) {
            printf("Zona no puede estar vacia.\n");
        }
    } while (strlen(zonaStr) == 0);

    do {
        printf("Ingrese la fecha (YYYY-MM-DD): ");
        scanf(" %[^\n]", fechaStr);
        if (strlen(fechaStr) != 10 || fechaStr[4] != '-' || fechaStr[7] != '-') {
            printf("Formato de fecha invalido. Debe serYYYY-MM-DD.\n");
        }
    } while (strlen(fechaStr) != 10 || fechaStr[4] != '-' || fechaStr[7] != '-');

    do {
        printf("Ingrese valor de CO2: ");
        scanf("%f", &co2);
    } while (co2 < 0);

    do {
        printf("Ingrese valor de SO2: ");
        scanf("%f", &so2);
    } while (so2 < 0);

    do {
        printf("Ingrese valor de NO2: ");
        scanf("%f", &no2);
    } while (no2 < 0);

    do {
        printf("Ingrese valor de PM2.5: ");
        scanf("%f", &pm25);
    } while (pm25 < 0);

    int zonaIndex = -1;
    for (int i = 0; i < *numZonasActuales; i++) {
        if (strcmp(zonas[i].nombreZona, zonaStr) == 0) {
            zonaIndex = i;
            break;
        }
    }

    if (zonaIndex == -1) {
        if (*numZonasActuales < NUM_ZONAS) {
            zonaIndex = *numZonasActuales;
            strncpy(zonas[zonaIndex].nombreZona, zonaStr, sizeof(zonas[zonaIndex].nombreZona) - 1);
            zonas[zonaIndex].nombreZona[sizeof(zonas[zonaIndex].nombreZona) - 1] = '\0';
            zonas[zonaIndex].numRegistros = 0;
            zonas[zonaIndex].capacidadRegistros = 10;
            zonas[zonaIndex].registros = (DatoContaminacion *)malloc(sizeof(DatoContaminacion) * zonas[zonaIndex].capacidadRegistros);
            if (zonas[zonaIndex].registros == NULL) {
                perror("Error: Fallo de asignacion de memoria para nueva zona");
                return;
            }
            (*numZonasActuales)++;
            printf("Nueva zona '%s' agregada.\n", zonaStr);
        } else {
            printf("No hay espacio para agregar una nueva zona. Maximo de %d zonas alcanzado.\n", NUM_ZONAS);
            return;
        }
    }

    if (zonas[zonaIndex].numRegistros >= zonas[zonaIndex].capacidadRegistros) {
        zonas[zonaIndex].capacidadRegistros *= 2;
        DatoContaminacion *temp = (DatoContaminacion *)realloc(zonas[zonaIndex].registros, sizeof(DatoContaminacion) * zonas[zonaIndex].capacidadRegistros);
        if (temp == NULL) {
            perror("Error: Fallo de reasignacion de memoria al agregar dato");
            return;
        }
        zonas[zonaIndex].registros = temp;
    }

    DatoContaminacion nuevoDato;
    strncpy(nuevoDato.fecha, fechaStr, sizeof(nuevoDato.fecha) - 1);
    nuevoDato.fecha[sizeof(nuevoDato.fecha) - 1] = '\0';
    parsearFecha(fechaStr, &nuevoDato.year, &nuevoDato.month, &nuevoDato.day);
    nuevoDato.co2 = co2;
    nuevoDato.so2 = so2;
    nuevoDato.no2 = no2;
    nuevoDato.pm25 = pm25;

    zonas[zonaIndex].registros[zonas[zonaIndex].numRegistros] = nuevoDato;
    zonas[zonaIndex].numRegistros++;

    printf("Datos agregados exitosamente a la memoria.\n");


    FILE *archivo = fopen("datos.txt", "a");
    if (archivo) {
        fseek(archivo, 0, SEEK_END);
        if (ftell(archivo) > 0) {
            fseek(archivo, -1, SEEK_END);
            if (fgetc(archivo) != '\n') { 
                fprintf(archivo, "\n");
            }
        }
        fprintf(archivo, "%s, %s, %.2f, %.2f, %.2f, %.2f\n", fechaStr, zonaStr, co2, so2, no2, pm25);
        fclose(archivo);
        printf("Datos guardados en el archivo 'datos.txt'.\n");
    } else {
        printf("No se pudo abrir el archivo para guardar. Los datos solo se guardaron en memoria.\n");
    }
    while ((c = getchar()) != '\n' && c != EOF);
}

//Ver datos por periodo de tiepo especifico y zona
// Aqui es donde pasa la magia
void cambiarOMostrarDatos(ZonaContaminacion zonas[], int numZonasActuales) {
    char nombreZona[50];
    int currentYear = 0, currentMonth = 0;
    int zonaIndex = -1;
    char opcionNavegacion;
    int c;

    while (1) {
        printf("\nIngrese el nombre de la zona (Norte, Sur, Este, Oeste, Centro) o 0 para salir: ");
        while ((c = getchar()) != '\n' && c != EOF); // Clear buffer before reading string
        scanf(" %[^\n]", nombreZona);
        if (strcmp(nombreZona, "0") == 0) {
            printf("\nSaliendo de la vista de datos por zona.\n");
            return;
        }

        zonaIndex = -1;
        for (int i = 0; i < numZonasActuales; i++) {
            if (strcmp(zonas[i].nombreZona, nombreZona) == 0) {
                zonaIndex = i;
                break;
            }
        }
        if (zonaIndex == -1) {
            printf("\nZona no encontrada.\n");
            continue;
        }
        if (zonas[zonaIndex].numRegistros == 0) {
            printf("No hay datos para la zona %s.\n", nombreZona);
            continue;
        }
        // Seleccionar mes/ano inicial (ultimo disponible)
        currentYear = zonas[zonaIndex].registros[zonas[zonaIndex].numRegistros - 1].year;
        currentMonth = zonas[zonaIndex].registros[zonas[zonaIndex].numRegistros - 1].month;

        while (1) {
            printf("\n--- Datos de %s para %04d-%02d ---\n", nombreZona, currentYear, currentMonth);
            printf("%-12s | %-6s | %-6s | %-6s | %-6s\n", "Fecha", "CO2", "SO2", "NO2", "PM2.5");
            printf("-------------------------------------------------------------\n");
            int registrosMostradosCount = 0;
            for (int j = 0; j < zonas[zonaIndex].numRegistros; j++) {
                if (zonas[zonaIndex].registros[j].year == currentYear && zonas[zonaIndex].registros[j].month == currentMonth) {
                    printf("%-12s | %-6.2f | %-6.2f | %-6.2f | %-6.2f\n",
                        zonas[zonaIndex].registros[j].fecha,
                        zonas[zonaIndex].registros[j].co2,
                        zonas[zonaIndex].registros[j].so2,
                        zonas[zonaIndex].registros[j].no2,
                        zonas[zonaIndex].registros[j].pm25);
                    registrosMostradosCount++;
                }
            }
            if (registrosMostradosCount == 0) {
                printf("No hay datos recopilados para esta epoca en %s.\n", nombreZona);
            }
            printf("\nNavegacion: < (Mes Anterior) | > (Mes Siguiente) | R (Cambiar Region) | O (Operaciones) | 0 (Salir) | 1 (Todos los datos de la zona) \n");
            printf("Opcion: ");
            scanf(" %c", &opcionNavegacion);
            opcionNavegacion = toupper(opcionNavegacion);
            if (opcionNavegacion == '1')
            {
                printf("\n--- Todos los datos para la zona: %s ---\n", zonas[zonaIndex].nombreZona);
                printf("%-12s | %-6s | %-6s | %-6s | %-6s\n", "Fecha", "CO2", "SO2", "NO2", "PM2.5");
                printf("-------------------------------------------------------------\n");

                for (int j = 0; j < zonas[zonaIndex].numRegistros; j++)
                {
                    printf("%-12s | %-6.2f | %-6.2f | %-6.2f | %-6.2f\n",
                           zonas[zonaIndex].registros[j].fecha,
                           zonas[zonaIndex].registros[j].co2,
                           zonas[zonaIndex].registros[j].so2,
                           zonas[zonaIndex].registros[j].no2,
                           zonas[zonaIndex].registros[j].pm25);
                }

                // Retorna inmediatamente al menú principal
                return;
            }
            if (opcionNavegacion == '<') {
                // Buscar el mes anterior con datos
                int prevYear = -1, prevMonth = -1;
                int maxYear = -1, maxMonth = -1;
                for (int j = 0; j < zonas[zonaIndex].numRegistros; j++) {
                    int y = zonas[zonaIndex].registros[j].year;
                    int m = zonas[zonaIndex].registros[j].month;
                    if (maxYear == -1 || y > maxYear || (y == maxYear && m > maxMonth)) {
                        maxYear = y;
                        maxMonth = m;
                    }
                    if ((y < currentYear) || (y == currentYear && m < currentMonth)) {
                        if (prevYear == -1 || y > prevYear || (y == prevYear && m > prevMonth)) {
                            prevYear = y;
                            prevMonth = m;
                        }
                    }
                }
                if (prevYear != -1) {
                    currentYear = prevYear;
                    currentMonth = prevMonth;
                } else if (maxYear != -1) {
                    currentYear = maxYear;
                    currentMonth = maxMonth;
                }
            } else if (opcionNavegacion == '>') {
                // Buscar el mes siguiente con datos
                int nextYear = -1, nextMonth = -1;
                int minYear = -1, minMonth = -1;
                for (int j = 0; j < zonas[zonaIndex].numRegistros; j++) {
                    int y = zonas[zonaIndex].registros[j].year;
                    int m = zonas[zonaIndex].registros[j].month;
                    if (minYear == -1 || y < minYear || (y == minYear && m < minMonth)) {
                        minYear = y;
                        minMonth = m;
                    }
                    if ((y > currentYear) || (y == currentYear && m > currentMonth)) {
                        if (nextYear == -1 || y < nextYear || (y == nextYear && m < nextMonth)) {
                            nextYear = y;
                            nextMonth = m;
                        }
                    }
                }
                if (nextYear != -1) {
                    currentYear = nextYear;
                    currentMonth = nextMonth;
                } else if (minYear != -1) {
                    currentYear = minYear;
                    currentMonth = minMonth;
                }
            } else if (opcionNavegacion == 'R') {
                break; // Cambiar de region
            } else if (opcionNavegacion == 'O') {
                // Submenu de operaciones usando la zona y epoca seleccionadas
                operacionesZonaMes(zonas, numZonasActuales, zonaIndex, currentYear, currentMonth);
            } else if (opcionNavegacion == '0') {
                printf("\nSaliendo de la vista de datos por zona.\n");
                return;
            } else {
                printf("Opcion de navegacion invalida.\n");
                printf("\nPresione Enter para continuar...");
                while ((c = getchar()) != '\n' && c != EOF);
                getchar();
            }
        }
    }
}

// Submenu de operaciones para la zona y mes/ano seleccionados
void operacionesZonaMes(ZonaContaminacion zonas[], int numZonasActuales, int zonaIndex, int year, int month) {
    char opcion;
    while (1) {
        printf("\n--- Operaciones para %s en %04d-%02d ---\n", zonas[zonaIndex].nombreZona, year, month);
        printf("1. Recomendaciones\n2. Alertas\n3. Prediccion de niveles futuros\n0. Volver\nOpcion: ");
        scanf(" %c", &opcion);
        if (opcion == '1') {
            crearRecomendacionesMes(zonas, zonaIndex, year, month);
        } else if (opcion == '2') {
            crearAlertasMes(zonas, zonaIndex, year, month);
        } else if (opcion == '3') {
            predecirNivelesFuturosMes(zonas, zonaIndex, year, month);
        } else if (opcion == '0') {
            return;
        } else {
            printf("Opcion invalida.\n");
        }
    }
}

// Recomendaciones solo para el mes/ano seleccionado
void crearRecomendacionesMes(ZonaContaminacion zonas[], int zonaIndex, int year, int month) {
    int count = 0;
    float sumaCO2 = 0, sumaSO2 = 0, sumaNO2 = 0, sumaPM25 = 0;
    for (int j = 0; j < zonas[zonaIndex].numRegistros; j++) {
        if (zonas[zonaIndex].registros[j].year == year && zonas[zonaIndex].registros[j].month == month) {
            sumaCO2 += zonas[zonaIndex].registros[j].co2;
            sumaSO2 += zonas[zonaIndex].registros[j].so2;
            sumaNO2 += zonas[zonaIndex].registros[j].no2;
            sumaPM25 += zonas[zonaIndex].registros[j].pm25;
            count++;
        }
    }
    if (count == 0) {
        printf("No hay datos para recomendaciones en esta epoca.\n");
        return;
    }
    float promCO2 = sumaCO2 / count;
    float promSO2 = sumaSO2 / count;
    float promNO2 = sumaNO2 / count;
    float promPM25 = sumaPM25 / count;
    printf("\n--- Recomendaciones para %s en %04d-%02d ---\n", zonas[zonaIndex].nombreZona, year, month);
    if (promCO2 > UMBRAL_CO2) printf(" - Reducir emisiones de CO2: usar transporte publico, evitar fogatas.\n");
    if (promSO2 > UMBRAL_SO2) printf(" - Evitar actividades industriales intensas sin control.\n");
    if (promNO2 > UMBRAL_NO2) printf(" - Usar mascarillas y ventilar espacios cerrados.\n");
    if (promPM25 > UMBRAL_PM25) printf(" - Limitar actividades al aire libre en dias criticos.\n");
    if (promCO2 <= UMBRAL_CO2 && promSO2 <= UMBRAL_SO2 && promNO2 <= UMBRAL_NO2 && promPM25 <= UMBRAL_PM25)
        printf("Los niveles de contaminacion estan dentro de rangos aceptables.\n");
}

// Alertas solo para el mes/ano seleccionado
void crearAlertasMes(ZonaContaminacion zonas[], int zonaIndex, int year, int month) {
    int count = 0, alertas = 0;
    printf("\n--- Alertas para %s en %04d-%02d ---\n", zonas[zonaIndex].nombreZona, year, month);
    for (int j = 0; j < zonas[zonaIndex].numRegistros; j++) {
        if (zonas[zonaIndex].registros[j].year == year && zonas[zonaIndex].registros[j].month == month) {
            if (zonas[zonaIndex].registros[j].co2 > UMBRAL_CO2) {
                printf("Alerta en %s: CO2 alto (%.2f ppm)\n", zonas[zonaIndex].registros[j].fecha, zonas[zonaIndex].registros[j].co2);
                alertas++;
            }
            if (zonas[zonaIndex].registros[j].so2 > UMBRAL_SO2) {
                printf("Alerta en %s: SO2 alto (%.2f ppm)\n", zonas[zonaIndex].registros[j].fecha, zonas[zonaIndex].registros[j].so2);
                alertas++;
            }
            if (zonas[zonaIndex].registros[j].no2 > UMBRAL_NO2) {
                printf("Alerta en %s: NO2 alto (%.2f ppm)\n", zonas[zonaIndex].registros[j].fecha, zonas[zonaIndex].registros[j].no2);
                alertas++;
            }
            if (zonas[zonaIndex].registros[j].pm25 > UMBRAL_PM25) {
                printf("Alerta en %s: PM2.5 alto (%.2f ug/m3)\n", zonas[zonaIndex].registros[j].fecha, zonas[zonaIndex].registros[j].pm25);
                alertas++;
            }
            count++;
        }
    }
    if (count == 0) {
        printf("No hay datos para alertas en esta epoca.\n");
    } else if (alertas == 0) {
        printf("No se detectaron niveles criticos de contaminacion en este periodo.\n");
    }
}

// Prediccion solo para el mes/ano seleccionado
void predecirNivelesFuturosMes(ZonaContaminacion zonas[], int zonaIndex, int year, int month) {
    float temperatura, viento, humedad;
    printf("\n--- Factores climaticos actuales ---\n");
    ingresarFactoresClimaticos(&temperatura, &viento, &humedad);
    int count = 0;
    float promCO2 = 0, promSO2 = 0, promNO2 = 0, promPM25 = 0;
    for (int j = 0; j < zonas[zonaIndex].numRegistros; j++) {
        if (zonas[zonaIndex].registros[j].year == year && zonas[zonaIndex].registros[j].month == month) {
            promCO2 += zonas[zonaIndex].registros[j].co2;
            promSO2 += zonas[zonaIndex].registros[j].so2;
            promNO2 += zonas[zonaIndex].registros[j].no2;
            promPM25 += zonas[zonaIndex].registros[j].pm25;
            count++;
        }
    }
    if (count == 0) {
        printf("No hay datos para prediccion en esta epoca.\n");
        return;
    }
    promCO2 /= count;
    promSO2 /= count;
    promNO2 /= count;
    promPM25 /= count;
    float ajuste = 1.0;
    if (temperatura > 30) ajuste += 0.05;
    else if (temperatura < 10) ajuste -= 0.03;
    if (humedad > 70) ajuste += 0.04;
    else if (humedad < 30) ajuste -= 0.02;
    if (viento > 50) ajuste -= 0.06;
    else if (viento < 10) ajuste += 0.05;
    float predCO2 = promCO2 * ajuste;
    float predSO2 = promSO2 * ajuste;
    float predNO2 = promNO2 * ajuste;
    float predPM25 = promPM25 * ajuste;
    printf("Prediccion CO2: %.2f ppm, Prediccion SO2: %.2f ppb, Prediccion NO2: %.2f ppb, Prediccion PM2.5: %.2f ug/m3\n",
        predCO2, predSO2, predNO2, predPM25);
}


// Estas funciones se mantienen del codigo del Luis
// Mucha gracia papa

void ingresarFactoresClimaticos(float *temperatura, float *viento, float *humedad) {
    printf("Ingrese la temperatura actual (C): ");
    while (scanf("%f", temperatura) != 1 || *temperatura < -50 || *temperatura > 60) {
        printf("Valor invalido. Intente de nuevo: ");
        while (getchar() != '\n');
    }

    printf("Ingrese la velocidad del viento actual (km/h): ");
    while (scanf("%f", viento) != 1 || *viento < 0 || *viento > 300) {
        printf("Valor invalido. Intente de nuevo: ");
        while (getchar() != '\n');
    }

    printf("Ingrese el nivel de humedad actual (%%): ");
    while (scanf("%f", humedad) != 1 || *humedad < 0 || *humedad > 100) {
        printf("Valor invalido. Intente de nuevo: ");
        while (getchar() != '\n');
    }
}
void predecirNivelesFuturos(ZonaContaminacion zonas[], int numZonasActuales) {
    float temperatura, viento, humedad;

    printf("\n--- Factores climaticos actuales ---\n");
    ingresarFactoresClimaticos(&temperatura, &viento, &humedad);

    printf("\n--- Prediccion de Niveles Futuros ---\n");

    for (int i = 0; i < numZonasActuales; i++) {
        if (zonas[i].numRegistros == 0) continue;

        int ultimos = (zonas[i].numRegistros >= 5) ? 5 : zonas[i].numRegistros;

        float promCO2 = 0, promSO2 = 0, promNO2 = 0, promPM25 = 0;

        for (int j = zonas[i].numRegistros - ultimos; j < zonas[i].numRegistros; j++) {
            promCO2 += zonas[i].registros[j].co2;
            promSO2 += zonas[i].registros[j].so2;
            promNO2 += zonas[i].registros[j].no2;
            promPM25 += zonas[i].registros[j].pm25;
        }

        promCO2 /= ultimos;
        promSO2 /= ultimos;
        promNO2 /= ultimos;
        promPM25 /= ultimos;

        float ajuste = 1.0;

        if (temperatura > 30) ajuste += 0.05;
        else if (temperatura < 10) ajuste -= 0.03;

        if (humedad > 70) ajuste += 0.04;
        else if (humedad < 30) ajuste -= 0.02;

        if (viento > 50) ajuste -= 0.06;
        else if (viento < 10) ajuste += 0.05;

        float predCO2 = promCO2 * ajuste;
        float predSO2 = promSO2 * ajuste;
        float predNO2 = promNO2 * ajuste;
        float predPM25 = promPM25 * ajuste;

        printf("Zona: %s\n", zonas[i].nombreZona);
        printf("Prediccion CO2: %.2f ppm, Prediccion SO2: %.2f ppb, Prediccion NO2: %.2f ppb, Prediccion PM2.5: %.2f ug/m3\n",
                predCO2, predSO2, predNO2, predPM25);
    }
}

void crearRecomendaciones(ZonaContaminacion zonas[], int numZonasActuales) {
    char nombreZona[50];
    printf("\nIngrese el nombre de la zona para obtener recomendaciones: ");
    // Clear the input buffer before reading string
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
    scanf(" %[^\n]", nombreZona);

    int encontrada = 0;
    for (int i = 0; i < numZonasActuales; i++) {
        if (strcmp(zonas[i].nombreZona, nombreZona) == 0) {
            encontrada = 1;
            if (zonas[i].numRegistros == 0) {
                printf("No hay datos para generar recomendaciones para esta zona.\n");
                return;
            }

            float sumaCO2 = 0, sumaSO2 = 0, sumaNO2 = 0, sumaPM25 = 0;
            for (int j = 0; j < zonas[i].numRegistros; j++) {
                sumaCO2 += zonas[i].registros[j].co2;
                sumaSO2 += zonas[i].registros[j].so2;
                sumaNO2 += zonas[i].registros[j].no2;
                sumaPM25 += zonas[i].registros[j].pm25;
            }

            float promCO2 = sumaCO2 / zonas[i].numRegistros;
            float promSO2 = sumaSO2 / zonas[i].numRegistros;
            float promNO2 = sumaNO2 / zonas[i].numRegistros;
            float promPM25 = sumaPM25 / zonas[i].numRegistros;

            printf("\n--- Recomendaciones para la zona %s ---\n", nombreZona);

            if (promCO2 > UMBRAL_CO2) {
                printf(" - Reducir emisiones de CO2: usar transporte publico, evitar fogatas.\n");
            }
            if (promSO2 > UMBRAL_SO2) {
                printf(" - Evitar actividades industriales intensas sin control.\n");
            }
            if (promNO2 > UMBRAL_NO2) {
                printf(" - Usar mascarillas y ventilar espacios cerrados.\n");
            }
            if (promPM25 > UMBRAL_PM25) {
                printf(" - Limitar actividades al aire libre en dias criticos.\n");
            }

            if (promCO2 <= UMBRAL_CO2 && promSO2 <= UMBRAL_SO2 && promNO2 <= UMBRAL_NO2 && promPM25 <= UMBRAL_PM25) {
                printf("Los niveles de contaminacion estan dentro de rangos aceptables.\n");
            }

            break;
        }
    }

    if (!encontrada) {
        printf("Zona no encontrada para recomendaciones.\n");
    }
}

void crearAlertas(ZonaContaminacion zonas[], int numZonasActuales) {
    char nombreZona[50];
    printf("\nIngrese el nombre de la zona para obtener alertas: ");
    // Clear the input buffer before reading string
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
    scanf(" %[^\n]", nombreZona);

    int encontrada = 0;
    for (int i = 0; i < numZonasActuales; i++) {
        if (strcmp(zonas[i].nombreZona, nombreZona) == 0) {
            encontrada = 1;

            printf("\n--- Alertas para la zona %s ---\n", nombreZona);
            int alertas = 0;

            for (int j = 0; j < zonas[i].numRegistros; j++) {
                if (zonas[i].registros[j].co2 > UMBRAL_CO2) {
                    printf("Alerta en %s: CO2 alto (%.2f ppm)\n", zonas[i].registros[j].fecha, zonas[i].registros[j].co2);
                    alertas++;
                }
                if (zonas[i].registros[j].so2 > UMBRAL_SO2) {
                    printf("Alerta en %s: SO2 alto (%.2f ppm)\n", zonas[i].registros[j].fecha, zonas[i].registros[j].so2);
                    alertas++;
                }
                if (zonas[i].registros[j].no2 > UMBRAL_NO2) {
                    printf("Alerta en %s: NO2 alto (%.2f ppm)\n", zonas[i].registros[j].fecha, zonas[i].registros[j].no2);
                    alertas++;
                }
                if (zonas[i].registros[j].pm25 > UMBRAL_PM25) {
                    printf("Alerta en %s: PM2.5 alto (%.2f ug/m3)\n", zonas[i].registros[j].fecha, zonas[i].registros[j].pm25);
                    alertas++;
                }
            }

            if (alertas == 0) {
                printf("No se detectaron niveles criticos de contaminacion.\n");
            }

            break;
        }
    }

    if (!encontrada) {
        printf("Zona no encontrada para alertas.\n");
    }
}


void calcularPromediosHistoricos(ZonaContaminacion zonas[], int numZonas) {
    printf("\n--- Promedios Historicos de Contaminacion (Ultimos 30 dias) ---\n");

    for (int i = 0; i < numZonas; i++) {
        ZonaContaminacion zona = zonas[i];
        int totalRegistros = zona.numRegistros;

        if (totalRegistros == 0) {
            printf("\nZona: %s\nSin registros disponibles.\n", zona.nombreZona);
            continue;
        }

        int dias = totalRegistros < 30 ? totalRegistros : 30;

        float suma_co2 = 0, suma_so2 = 0, suma_no2 = 0, suma_pm25 = 0;

        // Tomamos los últimos 'dias' registros
        for (int j = totalRegistros - dias; j < totalRegistros; j++) {
            suma_co2 += zona.registros[j].co2;
            suma_so2 += zona.registros[j].so2;
            suma_no2 += zona.registros[j].no2;
            suma_pm25 += zona.registros[j].pm25;
        }

        float prom_co2 = suma_co2 / dias;
        float prom_so2 = suma_so2 / dias;
        float prom_no2 = suma_no2 / dias;
        float prom_pm25 = suma_pm25 / dias;

        printf("\nZona: %s\n", zona.nombreZona);
        printf("-------------------------------------------------\n");
        printf("Promedio CO2   : %7.2f ppm   %s\n", prom_co2, prom_co2 > LIMITE_CO2 ? "(Excede OMS)" : "");
        printf("Promedio SO2   : %7.2f ug/m3 %s\n", prom_so2, prom_so2 > LIMITE_SO2 ? "(Excede OMS)" : "");
        printf("Promedio NO2   : %7.2f ug/m3 %s\n", prom_no2, prom_no2 > LIMITE_NO2 ? "(Excede OMS)" : "");
        printf("Promedio PM2.5 : %7.2f ug/m3 %s\n", prom_pm25, prom_pm25 > LIMITE_PM25 ? "(Excede OMS)" : "");
    }
}

void generarReporteFinal(ZonaContaminacion zonas[], int numZonasActuales) {
    FILE *reporte = fopen("reporte.txt", "w");
    if (!reporte) {
        printf("No se pudo crear el archivo reporte.txt\n");
        return;
    }

    fprintf(reporte, "--- REPORTE FINAL DE ALERTAS Y RECOMENDACIONES ---\n\n");

    for (int i = 0; i < numZonasActuales; i++) {
        if (zonas[i].numRegistros == 0) continue;

        float sumaCO2 = 0, sumaSO2 = 0, sumaNO2 = 0, sumaPM25 = 0;
        for (int j = 0; j < zonas[i].numRegistros; j++) {
            sumaCO2 += zonas[i].registros[j].co2;
            sumaSO2 += zonas[i].registros[j].so2;
            sumaNO2 += zonas[i].registros[j].no2;
            sumaPM25 += zonas[i].registros[j].pm25;
        }

        float promCO2 = sumaCO2 / zonas[i].numRegistros;
        float promSO2 = sumaSO2 / zonas[i].numRegistros;
        float promNO2 = sumaNO2 / zonas[i].numRegistros;
        float promPM25 = sumaPM25 / zonas[i].numRegistros;

        fprintf(reporte, "Zona: %s\n", zonas[i].nombreZona);

        int hayAlerta = 0;
        if (promCO2 > UMBRAL_CO2 || promSO2 > UMBRAL_SO2 || promNO2 > UMBRAL_NO2 || promPM25 > UMBRAL_PM25) {
            fprintf(reporte, "ALERTAS:\n");
            if (promCO2 > UMBRAL_CO2) {
                fprintf(reporte, " - CO2 elevado (Promedio: %.2f ppm)\n", promCO2);
                hayAlerta = 1;
            }
            if (promSO2 > UMBRAL_SO2) {
                fprintf(reporte, " - SO2 elevado (Promedio: %.2f ppb)\n", promSO2);
                hayAlerta = 1;
            }
            if (promNO2 > UMBRAL_NO2) {
                fprintf(reporte, " - NO2 elevado (Promedio: %.2f ppb)\n", promNO2);
                hayAlerta = 1;
            }
            if (promPM25 > UMBRAL_PM25) {
                fprintf(reporte, " - PM2.5 elevado (Promedio: %.2f ug/m3)\n", promPM25);
                hayAlerta = 1;
            }
        }

        if (!hayAlerta) {
            fprintf(reporte, "Sin alertas significativas en promedio.\n");
        }

        fprintf(reporte, "RECOMENDACIONES:\n");

        if (promCO2 > UMBRAL_CO2) {
            fprintf(reporte, " - Reducir uso de vehiculos y quemas.\n");
        }
        if (promSO2 > UMBRAL_SO2) {
            fprintf(reporte, " - Controlar emisiones industriales.\n");
        }
        if (promNO2 > UMBRAL_NO2) {
            fprintf(reporte, " - Usar mascarilla en exteriores.\n");
        }
        if (promPM25 > UMBRAL_PM25) {
            fprintf(reporte, " - Limitar actividades al aire libre.\n");
        }

        if (promCO2 <= UMBRAL_CO2 && promSO2 <= UMBRAL_SO2 && promNO2 <= UMBRAL_NO2 && promPM25 <= UMBRAL_PM25) {
            fprintf(reporte, " - Los niveles son adecuados. Mantener buenas practicas ambientales.\n");
        }

        fprintf(reporte, "\n");
    }

    fclose(reporte);
    printf("Reporte final generado en 'reporte.txt'.\n");
}

void mostrarReporteEnPantalla() {
    FILE *reporte = fopen("reporte.txt", "r");
    if (!reporte) {
        printf("No se pudo abrir el archivo reporte.txt\n");
        printf("Por favor, genere el reporte primero (opcion 6).\n");
        return;
    }

    char linea[256];
    printf("\n--- CONTENIDO DE reporte.txt ---\n\n");
    while (fgets(linea, sizeof(linea), reporte)) {
        printf("%s", linea);
    }

    fclose(reporte);
}