#ifndef FUNCIONES_H
#define FUNCIONES_H

#define NUM_ZONAS 5
#define UMBRAL_CO2 50.0      //Los Umbrales son para las alertas tempranas de contaminacion
#define UMBRAL_SO2 20.0
#define UMBRAL_NO2 40.0
#define UMBRAL_PM25 35.0
#define LIMITE_CO2 500.0     // ppm (CO2 no tiene límite OMS exacto, usamos un estimado)
#define LIMITE_SO2 40.0      // µg/m3 (24 horas)
#define LIMITE_NO2 25.0      // µg/m3 (promedio anual)
#define LIMITE_PM25 15.0     // µg/m3 (promedio anual)


typedef struct {
    char fecha[12];
    int year;
    int month;
    int day;
    float co2;
    float so2;
    float no2;
    float pm25;
} DatoContaminacion;

typedef struct {
    char nombreZona[50];
    DatoContaminacion *registros; 
    int numRegistros;             
    int capacidadRegistros;       
} ZonaContaminacion;

int menu();
void parsearFecha(const char *fechaStr, int *year, int *month, int *day);
void cargarDatos(ZonaContaminacion zonas[], int *numZonasActuales);
void guardarTodosLosDatos(ZonaContaminacion zonas[], int numZonasActuales);
void agregarDatos(ZonaContaminacion zonas[], int *numZonasActuales);
void cambiarOMostrarDatos(ZonaContaminacion zonas[], int numZonasActuales);
void operacionesZonaMes(ZonaContaminacion zonas[], int numZonasActuales, int zonaIndex, int year, int month); // Added prototype
void crearRecomendacionesMes(ZonaContaminacion zonas[], int zonaIndex, int year, int month); // Added prototype
void crearAlertasMes(ZonaContaminacion zonas[], int zonaIndex, int year, int month); // Added prototype
void predecirNivelesFuturosMes(ZonaContaminacion zonas[], int zonaIndex, int year, int month); // Added prototype
void ingresarFactoresClimaticos(float *temperatura, float *viento, float *humedad);
void predecirNivelesFuturos(ZonaContaminacion zonas[], int numZonasActuales);
void crearRecomendaciones(ZonaContaminacion zonas[], int numZonasActuales);
void crearAlertas(ZonaContaminacion zonas[], int numZonasActuales);
void calcularPromediosHistoricos(ZonaContaminacion zonas[], int numZonas);
void generarReporteFinal(ZonaContaminacion zonas[], int numZonasActuales);
void mostrarReporteEnPantalla();

#endif