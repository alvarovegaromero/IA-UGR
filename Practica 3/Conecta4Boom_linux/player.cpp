#include <iostream>
#include <cstdlib>
#include <vector>
#include <queue>
#include "player.h"
#include "environment.h"

using namespace std;

const double masinf=9999999999.0, menosinf=-9999999999.0;

// Constructor
Player::Player(int jug){
    jugador_=jug;
}

// Actualiza el estado del juego para el jugador
void Player::Perceive(const Environment & env){
    actual_=env;
}

double Puntuacion(int jugador, const Environment &estado){
    double suma=0;

    for (int i=0; i<7; i++)
      for (int j=0; j<7; j++){
         if (estado.See_Casilla(i,j)==jugador){
            if (j<3)
               suma += j;
            else
               suma += (6-j);
         }
      }

    return suma;
}

// Funcion de valoracion para testear Poda Alfabeta
double ValoracionTest(const Environment &estado, int jugador){
    int ganador = estado.RevisarTablero();

    if (ganador==jugador)
       return 99999999.0; // Gana el jugador que pide la valoracion
    else if (ganador!=0)
            return -99999999.0; // Pierde el jugador que pide la valoracion
    else if (estado.Get_Casillas_Libres()==0)
            return 0;  // Hay un empate global y se ha rellenado completamente el tablero
    else
          return Puntuacion(jugador,estado);
}

// ------------------- Los tres metodos anteriores no se pueden modificar

////////////////////////////////////////////////////////////////////////
//          Convierte un número pasado como char a entero.
////////////////////////////////////////////////////////////////////////

int convert_to_int(char caracter) //Dado un número pasado como carácter, lo devuelve como entero
{
  return ((int)caracter);
}

////////////////////////////////////////////////////////////////////////
//          Procesamiento de la Valoración Vertical
////////////////////////////////////////////////////////////////////////


// Asignar valor tras conocer el número de casillas contiguas y si hay hueco encima de estas y en que fila está el hueco
double PuntuacionVertical(int num_casillas_contiguas, bool hueco, int fila) //fila es la fila superior a la última casilla del jugador
{
  double valor = 0;

  if(hueco) //Si hay hueco encima, también hay hueco en las que están inmediatamente encima
  {
    if(num_casillas_contiguas == 3) //Tenemos 3 casillas juntas y podemos poner la cuarta 
      valor = 100;
    else if(num_casillas_contiguas == 2 && fila + 1 < 7) //Tenemos dos casillas contiguas con hueco encima y entran 2 casillas más encima
      valor = 25;
    else if(fila + 2 < 7) //Tenemos una casilla con hueco encima y entran 3 casillas más encima
      valor = 10;
  }

  return valor;
}

//Calcular el número de casillas de jugador que tenemos juntas verticalmente y ver si encima de la ultima procesada (la primera por arriba) está vacío
double ValoracionVertical(const Environment &estado, int jugador, int fila, int columna)
{
  double valoracion;

  int casilla; //Casilla que procesaremos en el bucle

  int num_casillas_contiguas = 1; //Inicialmente tenemos la casilla pasada por parámetro

  bool sigo_procesando_arriba = true; //Sigo procesando mientras encuentre fichas del jugador que es pasado como parametro
  bool sigo_procesando_abajo = true;

  bool hueco_superior = false; //Si hay hueco encima de la ultima casilla del jugador procesada hacia arriba

  int i; // índice del bucle. Lo usaremos en la función de puntuación para ver si podemos llegar al 4 en raya verticalmente. 

  for(i = fila - 1 ; i >= 0 && sigo_procesando_abajo ; i--) //Mirar hacia abajo de la casilla pasada como parámetro
  {
    casilla = convert_to_int(estado.See_Casilla(i, columna));
    casilla %= 3;

    if(casilla == jugador)
      num_casillas_contiguas++;
    else
      sigo_procesando_abajo = false;

      //Debajo es imposible que haya huecos por las reglas del juego
  }

  for(i = fila + 1 ; i < 7 && sigo_procesando_arriba ; i++) //Mirar hacia arriba de la casilla pasada como parámetro
  {
    casilla = convert_to_int(estado.See_Casilla(i, columna));
    casilla %= 3;

    if(casilla == jugador)
      num_casillas_contiguas++;
    else
    {
      if(casilla == 0) //La casilla está vacia
        hueco_superior = true; //Hay hueco 

      sigo_procesando_arriba = false;
    }
  }

  //Pasamos i como parámetro ya que tiene la ultima fila procesada 
  valoracion = PuntuacionVertical(num_casillas_contiguas, hueco_superior, i);

  return valoracion;
}

////////////////////////////////////////////////////////////////////////
//          Procesamiento de la Valoración Horizontal
////////////////////////////////////////////////////////////////////////

// Asignar valor tras conocer el número de casillas contiguas y si hay hueco a la izq de estas y/o a la derecha para que se hiciese el 4 en raya en algún caso
double PuntuacionHorizontal(const Environment &estado, int num_casillas_contiguas, bool hueco_izq, bool hueco_dcha, int columna_izq, int columna_dcha, int fila, int jugador)
{ 
  double valor = 0;
  int casilla;

  if(hueco_izq || hueco_dcha) //Si podemos llegar al 4 en raya
  {
    if(num_casillas_contiguas == 3) //Si tenemos 3 casillas y tenemos algún hueco, podemos hacer el 4 en raya.
      valor = 100;
    else if(num_casillas_contiguas == 2)
    {
      if(hueco_izq && hueco_dcha) //Si hay huecos en ambos lados
        valor = 25;
      else //Solo hay huecos a un lado
      {
        if(hueco_izq && columna_izq-1 >= 0) //Solo hay hueco a la izq y podría haber hueco (podría me refiero a que entra dentro de los margenes del tablero)
        {
          casilla = convert_to_int(estado.See_Casilla(fila, columna_izq-1));
          casilla %= 3; 

          if(casilla == 0) //Si la casilla a la izq del hueco está vacia
            valor = 25;
          else if(casilla == jugador) //x-xx donde x es una casilla de alguien y - un hueco. 3 muy cercanas
            valor = 100; 
        }
        else if(columna_dcha + 1 < 7) //Solo hay huecos a la dcha y entra en el tablero
        {
          casilla = convert_to_int(estado.See_Casilla(fila, columna_dcha+1));
          casilla %= 3; 

          if(casilla == 0) //Si la casilla a la dcha del hueco está vacia
            valor = 25;
          else if(casilla == jugador) //xx-x donde x es una casilla de alguien y - un hueco. 3 muy cercanas
            valor = 100; 
        }
      } 
    } //(num_casillas_contiguas == 2)

    else //(num_casillas_contiguas == 1)
    {
      
      int libres_izq = 0; //num de casillas libres desde columna_izq - 1 hacia la izq (incluido) Paramos de contrar cuando salgamos del tablero o encontremos una casilla del rival
      int libres_dcha = 0; //num de casillas libres desde columna_dcha - 1 hacia la dcha
      int casillas_jug_izq = 0; //num de casillas desde columna_izq -1 hacia la izq
      int casillas_jug_dcha = 0;

      bool parar = false; //Parar bucle while cuando no encontremos hueco
      int casilla;

      if(hueco_izq)
      {
        for(int i = columna_izq-1 ; i >= 0 && !parar ; i--)
        {
          casilla = convert_to_int(estado.See_Casilla(fila, columna_dcha+1));
          casilla %= 3; 

          if(casilla == 0)
            libres_izq++;
          else if(casilla == jugador)
            casillas_jug_izq++;
          else
            parar = true;
        }
      }

      parar = false;

      if(hueco_dcha)
      {
        for(int i = columna_izq+1 ; i < 7 && !parar ; i++)
        {
          casilla = convert_to_int(estado.See_Casilla(fila, columna_dcha+1));
          casilla %= 3; 

          if(casilla == 0)
            libres_dcha++;
          else if(casilla == jugador)
            casillas_jug_dcha++;
          else
            parar = true;
        }
      }

      if((libres_izq + libres_dcha + casillas_jug_izq + casillas_jug_dcha) >= 3) //Se puede llegar al 4 en raya
      {
  
        if(columna_izq == 3) //Al ser 1, columna_izq es la columna es la de la casilla. 3 es el centro
          valor = 18;
        else if(columna_izq == 2 || columna_izq == 4)
          valor = 16;
        else if(columna_izq == 1 || columna_izq == 5)
          valor = 14;
        else
          valor = 12;

      }

    } //(num_casillas_contiguas == 1)

  } //(hueco_izq || hueco_dcha)

  return valor;
}

double ValoracionHorizontal(const Environment &estado, int jugador, int fila, int columna)
{
  double valoracion;
  int casilla;

  int num_casillas_contiguas = 1; //Inicialmente tenemos la casilla pasada por parámetro

  bool sigo_procesando_dcha = true; //Sigo procesando mientras encuentre fichas del jugador que es pasado como parametro
  bool sigo_procesando_izq = true;

  bool hueco_izq = false;
  bool hueco_dcha = false;

  int i, j; // índices de los bucles. Lo usaremos en la función de puntuación para ver si podemos llegar al 4 en raya horizontalmente. 

  for(i = columna - 1 ; i >= 0 && sigo_procesando_izq ; i--) //Mirar hacia la izquierda de la casilla pasada como parámetro
  {
    casilla = convert_to_int(estado.See_Casilla(fila, i));
    casilla %= 3;

    if(casilla == jugador)
      num_casillas_contiguas++;
    else
    {
      if(casilla == 0) //La casilla está vacia
        hueco_izq = true;

      sigo_procesando_izq = false;
    }

  }

  for(j = columna + 1 ; j < 7 && sigo_procesando_dcha ; j++) //Mirar hacia la derecha de la casilla pasada como parámetro
  {
    casilla = convert_to_int(estado.See_Casilla(fila, j));
    casilla %= 3;

    if(casilla == jugador)
      num_casillas_contiguas++;
    else
    {
      if(casilla == 0) //La casilla está vacia
        hueco_dcha = true; //Hay hueco a la derecha

      sigo_procesando_dcha = false;
    }
  }

  //i tiene la última columna procesada hacia la izq. La correspondiente al hueco_izq
  valoracion = PuntuacionHorizontal(estado, num_casillas_contiguas, hueco_izq, hueco_dcha, i, j, fila, jugador);

  return valoracion;
}

/*
double ValoracionDiagonalIzq(const Environment &estado, int jugador, int fila, int columa)
{
  double valoracion = 0;

  int casilla;

  return valoracion;
}

double ValoracionDiagonalDcha(const Environment &estado, int jugador, int fila, int columa)
{
  double valoracion = 0;

  int casilla;

  return valoracion;
}

double ValoracionDiagonal(const Environment &estado, int jugador, int fila, int columna)
{
  double valor = 0;

  valor += ValoracionDiagonalIzq(estado, jugador, fila, columna); // Un punto a la izq del pasado por parámetro, tendrá mayor altura que el pasado como parametro
  valor += ValoracionDiagonalDcha(estado, jugador, fila, columna); // Un punto a la izq del pasado por parámetro, tendrá menor altura que el pasado como parametro

  return valor;
}
*/

double Valoracion(const Environment &estado, int jugador, int fila, int columna)
{
  int sum = 0;

  sum += ValoracionVertical(estado, jugador, fila, columna); //Para cada punto, valoramos vertical, horizontal y diagonalmente como de bueno es
  sum += ValoracionHorizontal(estado, jugador, fila, columna);
  //sum += ValoracionDiagonal(estado, jugador, fila, columna);

  return sum;
}

// Funcion heuristica (ESTA ES LA QUE TENEIS QUE MODIFICAR)
double Valoracion(const Environment &estado, int jugador)
{
  double valor = 0;
  int casilla;
  int rival;

  if(jugador == 1)
    rival = 2;
  else
    rival = 1;

  //Vemos si se ha ganado en el Enviroment estado
  int ganador = estado.RevisarTablero();
  if(ganador == jugador) //Gana el jugador que llama a valoración
    return masinf; //mejor valor de un estado
  else if(ganador == rival) //Pierde el que llama a la función Valoración
    return menosinf;
  else if(estado.Get_Casillas_Libres()==0)  //Empate - Solo se consigue si estado.Get_Casillas_Libres()==0 
    return 0;

  //Casilla a casilla, calcularemos el valor del estado

  for(int i = 0 ; i < 7  ; i++) //8 es el número de casillas
    for(int j = 0 ; j < 7 ; j++)
    {
      casilla = convert_to_int(estado.See_Casilla(i,j)); //Nos quedamos con el número pero en vez de como char, como entero
      casilla %= 3; // (4,1) tienen el valor 1 y (2,5) tienen el valor 2 que son las casillas de J1 y J2 respectivamente.
 
      if(jugador == casilla) //Si la casilla pertenece a jugador, sumar la valoracion
        valor += Valoracion(estado, jugador, i, j); //Valoracion sobrecargado pasandole una casilla
      else if(casilla == rival) //Si pertenece al otro, restar la valoración
        valor -= Valoracion(estado, rival, i, j);
    }

  return valor;
}

// Esta funcion no se puede usar en la version entregable
// Aparece aqui solo para ILUSTRAR el comportamiento del juego
// ESTO NO IMPLEMENTA NI MINIMAX, NI PODA ALFABETA
void JuegoAleatorio(bool aplicables[], int opciones[], int &j){
    j=0;
    for (int i=0; i<8; i++){
        if (aplicables[i]){
           opciones[j]=i;
           j++;
        }
    }
}


double MiniMax(const Environment & estado, int jugador, int profundidad, const int PROF_MAX, Environment::ActionType & accion)
{
  if (profundidad == PROF_MAX || estado.JuegoTerminado()) //Si es nodo terminal o es el fin del juego
    return Valoracion(estado, jugador);

  Environment hijos[8];
  int num_hijos;
  double mejor;
  double valor = 0;
  Environment::ActionType accion_anterior;          

  if(jugador == estado.JugadorActivo()) //Nodo Max
  {
    mejor = menosinf; //Valor inicial para que se actualice con el mejor valor que encontremos

    num_hijos = estado.GenerateAllMoves(hijos);

    //También se podría hacer con GenerateNextMove;
    //Generamos los descendientes del estado actual y nos quedamos con la accion que tenga mejor valor asociado
    for(int i = 0; i < num_hijos; i++) 
    {
      valor = MiniMax(hijos[i], jugador, profundidad+1, PROF_MAX, accion_anterior); //Llamada recursiva, con "estado" cambiado y la profundidad

      if (valor > mejor) //Actualizar si es mejor valor y quedarse con la acción asociada a ese mejor valor
      {
        mejor = valor;
        accion = static_cast <Environment::ActionType> (hijos[i].Last_Action(estado.JugadorActivo()));      
      }
    }
  }

  else //Es nodo MIN - Lo mismo que con MAX, pero actualizamos si es "peor valor" (para max)
  {
    mejor = masinf;

    num_hijos = estado.GenerateAllMoves(hijos);

    for(int i = 0; i < num_hijos; i++) 
    {
      valor = MiniMax(hijos[i], jugador, profundidad+1, PROF_MAX, accion_anterior);

      if (valor < mejor)
      {
        mejor = valor;
        accion = static_cast <Environment::ActionType> (hijos[i].Last_Action(estado.JugadorActivo()));      
      }
    }
  }

  return mejor;
}

//Adaptamos el minimax para que pode dados los valores alfa y beta 
double Poda_AlfaBeta(const Environment & estado, int jugador, int profundidad, const int PROF_MAX, Environment::ActionType & accion, double alpha, double beta)
{
if (profundidad == PROF_MAX || estado.JuegoTerminado()) //Si es nodo terminal o es el fin del juego
    return Valoracion(estado, jugador);

  Environment hijos[8];
  int num_hijos;
  double valor = 0;
  Environment::ActionType accion_anterior;          

  if(jugador == estado.JugadorActivo()) //Nodo Max
  {
    num_hijos = estado.GenerateAllMoves(hijos);

    //También se podría hacer con GenerateNextMove;
    //Generamos los descendientes del estado actual y nos quedamos con la accion que tenga mejor valor asociado
    for(int i = 0; i < num_hijos; i++) 
    {
      valor = Poda_AlfaBeta(hijos[i], jugador, profundidad+1, PROF_MAX, accion_anterior, alpha, beta); //Llamada recursiva, con "estado" cambiado y la profundidad

      if (valor > alpha)
      {
        alpha = valor;
        accion = static_cast <Environment::ActionType> (hijos[i].Last_Action(estado.JugadorActivo()));      
      }
      if(alpha >= beta) //COndicion de poda
        break;
    }

    return alpha;
  }

  else //Es nodo MIN - Lo mismo que con MAX, pero actualizamos si es "peor valor" (para max)
  {
    num_hijos = estado.GenerateAllMoves(hijos);

    for(int i = 0; i < num_hijos; i++) 
    {
      valor = Poda_AlfaBeta(hijos[i], jugador, profundidad+1, PROF_MAX, accion_anterior,alpha, beta);

      if (valor < beta)
      {
        beta = valor;
        accion = static_cast <Environment::ActionType> (hijos[i].Last_Action(estado.JugadorActivo()));      
      }
      if(alpha >= beta) //COndicion de poda
        break;
    }

    return beta;
  }
}

// Invoca el siguiente movimiento del jugador
Environment::ActionType Player::Think(){
    const int PROFUNDIDAD_MINIMAX = 6;  // Umbral maximo de profundidad para el metodo MiniMax
    const int PROFUNDIDAD_ALFABETA = 8; // Umbral maximo de profundidad para la poda Alfa_Beta

    Environment::ActionType accion; // acción que se va a devolver
    bool aplicables[8]; // Vector bool usado para obtener las acciones que son aplicables en el estado actual. La interpretacion es
                        // aplicables[0]==true si PUT1 es aplicable
                        // aplicables[1]==true si PUT2 es aplicable
                        // aplicables[2]==true si PUT3 es aplicable
                        // aplicables[3]==true si PUT4 es aplicable
                        // aplicables[4]==true si PUT5 es aplicable
                        // aplicables[5]==true si PUT6 es aplicable
                        // aplicables[6]==true si PUT7 es aplicable
                        // aplicables[7]==true si BOOM es aplicable



    double valor; // Almacena el valor con el que se etiqueta el estado tras el proceso de busqueda.
    double alpha = menosinf, beta = masinf; // Cotas de la poda AlfaBeta

    int n_act; //Acciones posibles en el estado actual


    n_act = actual_.possible_actions(aplicables); // Obtengo las acciones aplicables al estado actual en "aplicables"
    int opciones[10];

    // Muestra por la consola las acciones aplicable para el jugador activo
    //actual_.PintaTablero();
    cout << " Acciones aplicables ";
    (jugador_==1) ? cout << "Verde: " : cout << "Azul: ";
    for (int t=0; t<8; t++)
      if (aplicables[t]) //SI se puede hacer la accion t
         cout << " " << actual_.ActionStr( static_cast< Environment::ActionType > (t)  ); 
    cout << endl;


    //--------------------- COMENTAR Desde aqui
    /*
    cout << "\n\t";
    int n_opciones=0;
    JuegoAleatorio(aplicables, opciones, n_opciones);

    if (n_act==0){
      (jugador_==1) ? cout << "Verde: " : cout << "Azul: ";
      cout << " No puede realizar ninguna accion!!!\n";
      //accion = Environment::actIDLE;
    }
    else if (n_act==1){
           (jugador_==1) ? cout << "Verde: " : cout << "Azul: ";
            cout << " Solo se puede realizar la accion "
                 << actual_.ActionStr( static_cast< Environment::ActionType > (opciones[0])  ) << endl;
            accion = static_cast< Environment::ActionType > (opciones[0]);

         }
         else { // Hay que elegir entre varias posibles acciones
            int aleatorio = rand()%n_opciones;
            cout << " -> " << actual_.ActionStr( static_cast< Environment::ActionType > (opciones[aleatorio])  ) << endl;
            accion = static_cast< Environment::ActionType > (opciones[aleatorio]);
         }
    */
    //--------------------- COMENTAR Hasta aqui


    //--------------------- AQUI EMPIEZA LA PARTE A REALIZAR POR EL ALUMNO ------------------------------------------------

    // 1.Opcion: Minimax 
    //valor = MiniMax(actual_, jugador_, 0, PROFUNDIDAD_MINIMAX, accion);
    //cout << "Valor MiniMax: " << valor << "  Accion: " << actual_.ActionStr(accion) << endl;

    // 2.Opcion: Poda AlfaBeta
    valor = Poda_AlfaBeta(actual_, jugador_, 0, PROFUNDIDAD_ALFABETA, accion, alpha, beta);
    cout << "Valor MiniMax: " << valor << "  Accion: " << actual_.ActionStr(accion) << endl;


  return accion;
}


