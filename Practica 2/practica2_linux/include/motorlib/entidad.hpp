#ifndef ENTIDAD_H
#define ENTIDAD_H

#include "obj3dlib.hpp"
#include "comportamientolib.hpp"
#include "motorlib/util.h"

// Macro clase para englobar entidades de juego
// Contiene la informacion para pintar y ejecutar en el juego las entidades
// Las entidades son el jugador y los npcs

enum Tipo {jugador, npc};
enum SubTipo {jugador_, aldeano, __NONE__};

class Entidad {
    private:
	  Tipo tipo;
	  SubTipo subtipo;
	  Orientacion orient;
	  unsigned int f, c;
    unsigned int num_destinos;
    vector<unsigned int> destino;
    vector<bool> alcanzados;
    bool completoLosObjetivos;


	  Objeto3D * modelo;
	  Comportamiento* comportamiento;

	  //Colision
	  bool colision = false;
	  int colisiones = 0;

	  //reset
	  bool reset = false;

	  bool mensaje = false;

	  //vida
	  int vida = 3000;
    int muertes_innecesarias = 0;

    //bateria
    int bateria = 3000;

	  bool hitbox = true;
	  int desactivado = 0;

	  int last_action = 3;
	  int misiones = 0;
	  bool done = false;
	  double tiempo = 0;
    double tiempoMaximo = 300 * CLOCKS_PER_SEC;
    int tiempo_sig_accion = 0;
    int bateria_sig_accion = 0;
    int nivel = 1;

    bool tiene_zapatillas = false;
    bool tiene_bikini = false;

	  vector <vector< unsigned char> > visionAux;

	public:
	  Entidad(Tipo paramTipo, SubTipo paramSubTipo, Orientacion paramOrient, unsigned int paramF, unsigned int paramC, Objeto3D * mod, Comportamiento * comp, int numberdest, vector<unsigned int> CasillasObjetivo, int v){
          tipo = paramTipo;
          subtipo = paramSubTipo;
          orient = paramOrient;
          f = paramF;
          c = paramC;
          modelo = mod;
          comportamiento = comp;
          num_destinos = numberdest;
          destino = CasillasObjetivo;
          for (int i=0; i<num_destinos; i++)
            alcanzados.push_back(false);
          vida = v;
          completoLosObjetivos = false;
        }

	  ~Entidad() {delete modelo; delete comportamiento;}

	  void draw(int modo) {if(hitbox) {modelo -> draw(modo);}}
	  Action think(int acc, vector <vector< unsigned char> > vision, int level);

	  //Programar funciones para la interacción con el entorno
	  void setPosicion(unsigned int paramF, unsigned int paramC) {f = paramF; c = paramC;}
	  //void setObjetivo(unsigned int paramF, unsigned int paramC) {destF = paramF; destC = paramC;}
	  void setOrientacion(Orientacion paramOrient) {orient = paramOrient;}

    int isMemberObjetivo(unsigned int paramF, unsigned int paramC);
    bool allObjetivosAlcanzados();
    bool allLessOneObjetivosAlcanzados();
    void setObjetivos(vector<unsigned int> v);
    void actualizarAlcanzados();
    void anularAlcanzados();
    unsigned int getNumObj(){return num_destinos;}
    unsigned int getObjFil(int pos);
    unsigned int getObjCol(int pos);


	  Tipo getTipo() {return tipo;}
	  SubTipo getSubTipo() {return subtipo;}

	  unsigned char getSubTipoChar();

	  Orientacion getOrientacion() {return orient;}
	  unsigned int getFil() {return f;}
	  unsigned int getCol() {return c;}
	  bool getHitbox() {return hitbox;}

	  bool interact(Action accion, int valor);

	  void resetEntidad();
	  void seAostio() {colision = true; colisiones++;}
	  void notify() {mensaje = true;}
    void setCompletoLosObjetivos(){completoLosObjetivos=true;}
    bool SeHanConseguidoLosObjetivos(){return completoLosObjetivos;}

	  void getLastAction(int accion) {last_action = accion;}
    int putLastAction() {return last_action;}

	  vector< vector< unsigned char> > getMapaResultado() {return comportamiento -> getMapaResultado();}
	  vector< vector< unsigned char> > getMapaEntidades() {return comportamiento -> getMapaEntidades();}
    vector< vector< unsigned char> > getMapaPlan() {return comportamiento -> getMapaPlan();}

	  bool vivo() {return vida > 0;}

	  string toString();

	  void setVision(vector <vector< unsigned char> > vision) {visionAux = vision;}

	  void perderPV(int valor) {vida -= valor;}
	  void ganarPV(int valor) {vida += valor;}
    int getInstantesPendientes(){return vida;}

	  int getColisiones() {return colisiones;}
    int getMuertesI() {return muertes_innecesarias;}

	  void setTiempo(double valor) {tiempo=valor;}
	  double getTiempo() {return tiempo;}
    double getTiempoMaximo(){return tiempoMaximo;}
	  void addTiempo(double valor) {tiempo+=valor;}

	  int getMisiones() {return misiones;}

	  bool isActived() {return desactivado!=-1;}
	  void Active() {desactivado=0;}

	  void setMensajeOff() {mensaje = false;}
	  void setColisionOff() {colision = false;}
	  void setResetOff() {reset = false;}
	  void setHitbox(bool valor) {hitbox=valor;}

    bool fin() {return done;}
	  void resetFin() {done = false;}

    bool ready(){return tiempo_sig_accion == 0 and bateria>0;}
    void decTiempo_sig_accion(){if (tiempo_sig_accion>0) tiempo_sig_accion--;}
    void fixTiempo_sig_accion(unsigned char celda);
    int getTsig_accion(){return tiempo_sig_accion;}

    int getBateria() const {return bateria;}
    void setBateria(int valor){bateria=valor;}
    void fixBateria_default(){bateria_sig_accion=0;}
    int fixBateria_sig_accion(unsigned char celda, Action accion);
    void decBateria_sig_accion();
    int getBsig_accion(){return bateria_sig_accion;}
    void increaseBateria(int valor){bateria+=valor;if(bateria>3000) bateria=3000;}

    bool Has_Zapatillas(){return tiene_zapatillas;}
    bool Has_Bikini(){return tiene_bikini;}
    void Cogio_Zapatillas(bool valor){tiene_zapatillas=valor;}
    void Cogio_Bikini(bool valor){tiene_bikini=valor;}

    void set_Nivel(int level){ nivel = level;}
    int get_Nivel(){ return nivel;}
};
#endif
