#ifndef COMPORTAMIENTOJUGADOR_H
#define COMPORTAMIENTOJUGADOR_H

#include "comportamientos/comportamiento.hpp"

#include <list>

struct estado {
  int fila;
  int columna;
  int orientacion;
  bool tieneBikini; //Estado sabe si tiene o no bikini/zapatillas
  bool tieneZapatillas;
  bool obj_visitados[3];  //Para nivel 3 
  int num_obj_visit;
  //Para el nivel 3: valor heuristica = f(n) = c(n) + h(n) en mi caso, aunque debería ser h(n)  
};

class ComportamientoJugador : public Comportamiento {
  public:
    ComportamientoJugador(unsigned int size) : Comportamiento(size) { //nivel 4
      // Inicializar Variables de Estado
      hayPlan = false;
      num_objetivos = 3; //num_objetivos para el nivel 3. EN nivel 0, 1 y 2 no se usa

    }

    ComportamientoJugador(std::vector< std::vector< unsigned char> > mapaR) : Comportamiento(mapaR) { //resto
      // Inicializar Variables de Estado
      hayPlan = false;
      num_objetivos = 3; //num_objetivos para el nivel 4

    }

    ComportamientoJugador(const ComportamientoJugador & comport) : Comportamiento(comport){}
    ~ComportamientoJugador(){}

    Action think(Sensores sensores);
    int interact(Action accion, int valor);
    void VisualizaPlan(const estado &st, const list<Action> &plan);
    ComportamientoJugador * clone(){return new ComportamientoJugador(*this);}

  private:
    // Declarar Variables de Estado
    estado actual;
    list<estado> objetivos;
    list<Action> plan;
    bool hayPlan;
    int num_objetivos;

    // MÃ©todos privados de la clase
    bool pathFinding(int level, const estado &origen, const list<estado> &destino, list<Action> &plan); //llama a resto de pathfinding
    bool pathFinding_Profundidad(const estado &origen, const estado &destino, list<Action> &plan);
    bool pathFinding_Anchura(const estado &origen, const estado &destino, list<Action> &plan);
    bool pathFinding_CostoUniforme(const estado &origen, const estado &destino, list<Action> &plan);
    bool pathFinding_AEstrella(const estado &origen, const estado destino[3], list<Action> &plan); //destino es vector de 3, ya que se le pasan 3 objetivos

    int distManhattan(const estado &inicio, const estado &fin);
    int mejorDistManhattan(const estado &inicio, const estado destino[3], bool visitados[3]);


    void PintaPlan(list<Action> plan);
    bool HayObstaculoDelante(estado &st);

    int Coste(int fila, int columna, bool tieneBikini, bool tieneZapatillas, int accion);  

};

#endif