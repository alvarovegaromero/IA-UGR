#include "../Comportamientos_Jugador/jugador.hpp"
#include "motorlib/util.h"

#include <iostream>
#include <cmath>
#include <set>
#include <stack>
#include <queue>

// Este es el mÃ©todo principal que se piden en la practica.
// Tiene como entrada la informaciÃ³n de los sensores y devuelve la acciÃ³n a realizar.
// Para ver los distintos sensores mirar fichero "comportamiento.hpp"
Action ComportamientoJugador::think(Sensores sensores) {
	actual.fila        = sensores.posF;
	actual.columna     = sensores.posC;
	actual.orientacion = sensores.sentido;

	cout << "Fila: " << actual.fila << endl;
	cout << "Col : " << actual.columna << endl;
	cout << "Ori : " << actual.orientacion << endl;

	// Capturo los destinos
	cout << "sensores.num_destinos : " << sensores.num_destinos << endl;
	objetivos.clear();
	for (int i=0; i<sensores.num_destinos; i++){
		estado aux;
		aux.fila = sensores.destino[2*i]; //posiciones pares: filas
		aux.columna = sensores.destino[2*i+1]; //posiciones impares: columna
		objetivos.push_back(aux);
	}

	cout << hayPlan << endl << endl << endl;

	if(!hayPlan)
    {
        hayPlan = pathFinding(sensores.nivel, actual, objetivos, plan);
    }

    Action sigAccion;
    if(hayPlan and plan.size() > 0)
    {
        sigAccion = plan.front();
        plan.erase(plan.begin());
    }
    else{
        cout << "no se pudo encontrar un plan \n";
    }

  return sigAccion;
}

// Llama al algoritmo de busqueda que se usara en cada comportamiento del agente
// Level representa el comportamiento en el que fue iniciado el agente.
bool ComportamientoJugador::pathFinding (int level, const estado &origen, const list<estado> &destino, list<Action> &plan){
    estado un_objetivo;
    estado v_objetivos[num_objetivos]; //se usa en nivel 3 y 4

	switch (level){
		case 0: cout << "Demo\n";
						un_objetivo = objetivos.front();
						cout << "fila: " << un_objetivo.fila << " col:" << un_objetivo.columna << endl;
                        return pathFinding_Profundidad(origen,un_objetivo,plan);
						break;

		case 1: cout << "Optimo numero de acciones\n"; //casi igual que en caso 0 al haber 1 unico estado.
						un_objetivo = objetivos.front();
						cout << "fila: " << un_objetivo.fila << " col:" << un_objetivo.columna << endl;
                        return pathFinding_Anchura(origen,un_objetivo,plan);
						break;

		case 2: cout << "Optimo en coste 1 Objetivo\n";
						un_objetivo = objetivos.front();
						cout << "fila: " << un_objetivo.fila << " col:" << un_objetivo.columna << endl;
                        return pathFinding_CostoUniforme(origen,un_objetivo,plan);
						break;

		case 3: cout << "Optimo en coste 3 Objetivos\n"; //pasar directamente lista de objetivos
						for(int i = 0 ; i < num_objetivos ; i++)
						{
							v_objetivos[i] = objetivos.front();
							objetivos.pop_front();
							cout << "fila: " << v_objetivos[i].fila << " col:" << v_objetivos[i].columna << endl;
						}

						return pathFinding_AEstrella(origen, v_objetivos, plan);
						break;

		case 4: cout << "Algoritmo de busqueda usado en el reto\n"; //decidir si se modifica alg para trabajar con lista de alg oe starategia para ordenar objetiuvos
						// Incluir aqui la llamada al algoritmo de busqueda usado en el nivel 2
						cout << "No implementado aun\n";
						break;
	}
	return false;
}

//---------------------- ImplementaciÃ³n de la busqueda en profundidad ---------------------------

// Dado el codigo en caracter de una casilla del mapa dice si se puede
// pasar por ella sin riegos de morir o chocar.
bool EsObstaculo(unsigned char casilla){
	if (casilla=='P' or casilla=='M')
		return true;
	else
	  return false;
}

// Comprueba si la casilla que hay delante es un obstaculo. Si es un
// obstaculo devuelve true. Si no es un obstaculo, devuelve false y
// modifica st con la posiciÃ³n de la casilla del avance.
bool ComportamientoJugador::HayObstaculoDelante(estado &st){
	int fil=st.fila, col=st.columna;

  // calculo cual es la casilla de delante del agente
	switch (st.orientacion) {
		case 0: fil--; break;
		case 1: col++; break;
		case 2: fil++; break;
		case 3: col--; break;
	}

	// Compruebo que no me salgo fuera del rango del mapa
	if (fil<0 or fil>=mapaResultado.size()) return true;
	if (col<0 or col>=mapaResultado[0].size()) return true;

	// Miro si en esa casilla hay un obstaculo infranqueable
	if (!EsObstaculo(mapaResultado[fil][col])){
		// No hay obstaculo, actualizo el parametro st poniendo la casilla de delante.
    st.fila = fil;
		st.columna = col;
		return false;
	}
	else{
	  return true;
	}
}


struct nodo{
	estado st;
	list<Action> secuencia;
	int coste; //Para nivel 2 y 3
	int valor_heuristica; //Nivel 2 y 3


	bool operator< (const nodo& nod) const //Operador para ordenar priority queue. Primero los que más objetivos llevan y luego valor_heuristica
	{
	   	return this->valor_heuristica > nod.valor_heuristica;
	}
};

struct ComparaEstados{
	bool operator()(const estado &a, const estado &n) const{
		if ((a.fila > n.fila) or 
		   (a.fila == n.fila and a.columna > n.columna) or
	       (a.fila == n.fila and a.columna == n.columna and a.orientacion > n.orientacion))
			return true;
		else
			return false;
	}
};

// Sacar por la consola la secuencia del plan obtenido
void ComportamientoJugador::PintaPlan(list<Action> plan) {
	auto it = plan.begin();
	while (it!=plan.end()){
		if (*it == actFORWARD){
			cout << "A ";
		}
		else if (*it == actTURN_R){
			cout << "D ";
		}
		else if (*it == actTURN_L){
			cout << "I ";
		}
		else {
			cout << "- ";
		}
		it++;
	}
	cout << endl;
}


// Funcion auxiliar para poner a 0 todas las casillas de una matriz
void AnularMatriz(vector<vector<unsigned char> > &m){
	for (int i=0; i<m[0].size(); i++){
		for (int j=0; j<m.size(); j++){
			m[i][j]=0;
		}
	}
}

// Pinta sobre el mapa del juego el plan obtenido
void ComportamientoJugador::VisualizaPlan(const estado &st, const list<Action> &plan){
  AnularMatriz(mapaConPlan);
	estado cst = st;

	auto it = plan.begin();
	while (it!=plan.end()){
		if (*it == actFORWARD){
			switch (cst.orientacion) {
				case 0: cst.fila--; break;
				case 1: cst.columna++; break;
				case 2: cst.fila++; break;
				case 3: cst.columna--; break;
			}
			mapaConPlan[cst.fila][cst.columna]=1;
		}
		else if (*it == actTURN_R){
			cst.orientacion = (cst.orientacion+1)%4;
		}
		else {
			cst.orientacion = (cst.orientacion+3)%4;
		}
		it++;
	}
}

int ComportamientoJugador::interact(Action accion, int valor){
  return false;
}

////////////////////////////////////////////////
//					Nivel 0
////////////////////////////////////////////////

// Implementaciónn de la busqueda en profundidad.
// Entran los puntos origen y destino y devuelve la
// secuencia de acciones en plan, una lista de acciones.
bool ComportamientoJugador::pathFinding_Profundidad(const estado &origen, const estado &destino, list<Action> &plan)
{
	//Borro la lista
	cout << "Calculando plan\n";
	plan.clear();
	set<estado,ComparaEstados> Cerrados; // Lista de Cerrados
	stack<nodo> Abiertos;								 // Lista de Abiertos

  	nodo current;
	current.st = origen;
	current.secuencia.empty();

	Abiertos.push(current);

/*  Por si estuviese en la posición inicial - No funciona
	if((current.st.fila==destino.fila or current.st.columna == destino.columna))
	{
		current.secuencia.push_back(actIDLE);
	}
*/	

  while (!Abiertos.empty() and (current.st.fila!=destino.fila or current.st.columna != destino.columna)){

		Abiertos.pop();
		Cerrados.insert(current.st);

		// Generar descendiente de girar a la derecha
		nodo hijoTurnR = current;
		hijoTurnR.st.orientacion = (hijoTurnR.st.orientacion+1)%4;
		if (Cerrados.find(hijoTurnR.st) == Cerrados.end()){
			hijoTurnR.secuencia.push_back(actTURN_R);
			Abiertos.push(hijoTurnR);

		}

		// Generar descendiente de girar a la izquierda
		nodo hijoTurnL = current;
		hijoTurnL.st.orientacion = (hijoTurnL.st.orientacion+3)%4;
		if (Cerrados.find(hijoTurnL.st) == Cerrados.end()){
			hijoTurnL.secuencia.push_back(actTURN_L);
			Abiertos.push(hijoTurnL);
		}

		// Generar descendiente de avanzar
		nodo hijoForward = current;
		if (!HayObstaculoDelante(hijoForward.st)){
			if (Cerrados.find(hijoForward.st) == Cerrados.end()){
				hijoForward.secuencia.push_back(actFORWARD);
				Abiertos.push(hijoForward);
			}
		}

		// Tomo el siguiente valor de la Abiertos
		if (!Abiertos.empty()){
			current = Abiertos.top();
		}
	}

  cout << "Terminada la busqueda\n";

	if (current.st.fila == destino.fila and current.st.columna == destino.columna){
		cout << "Cargando el plan\n";
		plan = current.secuencia;
		cout << "Longitud del plan: " << plan.size() << endl;
		PintaPlan(plan);
		// ver el plan en el mapa
		VisualizaPlan(origen, plan);
		return true;
	}

	else {
		cout << "No encontrado plan\n";
	}

	return false;
}

////////////////////////////////////////////////
//					Nivel 1
////////////////////////////////////////////////

// Implementación de la busqueda en anchura .
// Entran los puntos origen y destino y devuelve la
// secuencia de acciones en plan, una lista de acciones.
bool ComportamientoJugador::pathFinding_Anchura(const estado &origen, const estado &destino, list<Action> &plan)
{
	//Borro la lista
	cout << "Calculando plan\n";
	plan.clear();
	set<estado,ComparaEstados> Cerrados; // Lista de Cerrados
	queue<nodo> Abiertos;								 // Lista de Abiertos

  	nodo current;
	current.st = origen;
	current.secuencia.empty();

	Abiertos.push(current);

  while (!Abiertos.empty() and (current.st.fila!=destino.fila or current.st.columna != destino.columna)){

		Abiertos.pop();
		Cerrados.insert(current.st);

		// Generar descendiente de girar a la derecha
		nodo hijoTurnR = current;
		hijoTurnR.st.orientacion = (hijoTurnR.st.orientacion+1)%4;
		if (Cerrados.find(hijoTurnR.st) == Cerrados.end()){
			hijoTurnR.secuencia.push_back(actTURN_R);
			Abiertos.push(hijoTurnR);

		}

		// Generar descendiente de girar a la izquierda
		nodo hijoTurnL = current;
		hijoTurnL.st.orientacion = (hijoTurnL.st.orientacion+3)%4;
		if (Cerrados.find(hijoTurnL.st) == Cerrados.end()){
			hijoTurnL.secuencia.push_back(actTURN_L);
			Abiertos.push(hijoTurnL);
		}

		// Generar descendiente de avanzar
		nodo hijoForward = current;
		if (!HayObstaculoDelante(hijoForward.st)){
			if (Cerrados.find(hijoForward.st) == Cerrados.end()){
				hijoForward.secuencia.push_back(actFORWARD);
				Abiertos.push(hijoForward);
			}
		}

		// Tomo el siguiente valor de la Abiertos
		if (!Abiertos.empty()){
			current = Abiertos.front();
		}
	}

  cout << "Terminada la busqueda\n";

	if (current.st.fila == destino.fila and current.st.columna == destino.columna){
		cout << "Cargando el plan\n";
		plan = current.secuencia;
		cout << "Longitud del plan: " << plan.size() << endl;
		PintaPlan(plan);
		// ver el plan en el mapa
		VisualizaPlan(origen, plan);
		return true;
	}
	else {
		cout << "No encontrado plan\n";
	}

	return false;
}

////////////////////////////////////////////////
//					Nivel 2
////////////////////////////////////////////////

struct ComparaEstadosCU //Comparador de estados para CU
{
	bool operator()(const estado &a, const estado &n) const{
		if ((a.fila > n.fila) 
		or (a.fila == n.fila and a.columna > n.columna) 
		or (a.fila == n.fila and a.columna == n.columna and a.orientacion > n.orientacion) 
		or (a.fila == n.fila and a.columna == n.columna and a.orientacion == n.orientacion and a.tieneBikini>n.tieneBikini) 
		or (a.fila == n.fila and a.columna == n.columna and a.orientacion == n.orientacion and a.tieneBikini==n.tieneBikini and a.tieneZapatillas>n.tieneZapatillas))
			return true;
		else
			return false;
	}
};

int ComportamientoJugador::Coste(int fila, int columna, bool tieneBikini, bool tieneZapatillas, int accion) //Calcular coste de una accione en un nodo
{
   	int cost = 0; //Por defecto las acciones son de coste 1
	char tipo_nodo = mapaResultado[fila][columna];

	if(accion != 3)
	{ //no es idle -> en este caso devuelve 0
		cost = 1; //por defecto es 1 para todas las casillas y acciones
	    if(tipo_nodo == 'A')
	    {
	    	if(accion==0) //ir pasa alante
	    	{
	    		cost=200;
	    		if(tieneBikini)
	    			cost=10;
	    	}
	    	else
	    	{
	    		cost=500;
	    		if(tieneBikini)
	    			cost=5;
	    	}
	    }

	    else if(tipo_nodo == 'B')
	    {
	    	if(accion==0)
	    	{
	    		cost=100;
	    		if(tieneZapatillas)
	    			cost=15;
	    	}
	    	else
	    	{
	    		cost=3;
	    		if(tieneZapatillas)
	    			cost=1;
	    	}
	    }

	    else if(tipo_nodo == 'T')
	    {
	    	cost=2;
	    }
	}

    return cost;
}


// Implementaciónn de la busqueda en Costo Uniforme.
// Entran los puntos origen y destino y devuelve la
// secuencia de acciones en plan, una lista de acciones.
bool ComportamientoJugador::pathFinding_CostoUniforme(const estado &origen, const estado &destino, list<Action> &plan)
{
	//Borro la lista
	cout << "Calculando plan\n";
	plan.clear();
	set<estado,ComparaEstadosCU> Cerrados; // Lista de Cerrados
	priority_queue<nodo> Abiertos;				 // Lista de Abiertos

    nodo current;
	current.st = origen;
	current.secuencia.empty();

	// Inicialización de los nuevos atributos (variables booleanas de bikini y zapatillas junto con el coste)
	current.st.tieneBikini = false;
	current.st.tieneZapatillas = false;
	current.coste = Coste(origen.fila, origen.columna, current.st.tieneBikini, current.st.tieneZapatillas, actIDLE);
	current.valor_heuristica = current.coste; //ya que es de costo uniforme

	Abiertos.push(current);

  while (!Abiertos.empty() and (current.st.fila!=destino.fila or current.st.columna != destino.columna)){

		Abiertos.pop();
		Cerrados.insert(current.st);

		// Comprobar la casilla si es de tipo bikini o zapatillas
		if (mapaResultado[current.st.fila][current.st.columna]=='K')
        {
            current.st.tieneBikini = true;

            if(current.st.tieneZapatillas)
            current.st.tieneZapatillas = false; // no se puede tener los dos items
        }

        if (mapaResultado[current.st.fila][current.st.columna]=='D')
        {
            current.st.tieneZapatillas = true;

            if(current.st.tieneBikini)
            current.st.tieneBikini = false; 
        }

		// Generar descendiente de girar a la derecha
		nodo hijoTurnR = current;
		hijoTurnR.st.orientacion = (hijoTurnR.st.orientacion+1)%4;
		hijoTurnR.coste += Coste(hijoTurnR.st.fila, hijoTurnR.st.columna, hijoTurnR.st.tieneBikini, hijoTurnR.st.tieneZapatillas, actTURN_R);
		hijoTurnR.valor_heuristica = hijoTurnR.coste;

		if (Cerrados.find(hijoTurnR.st) == Cerrados.end()){
			hijoTurnR.secuencia.push_back(actTURN_R);
			Abiertos.push(hijoTurnR);
		}

		// Generar descendiente de girar a la izquierda
		nodo hijoTurnL = current;
		hijoTurnL.st.orientacion = (hijoTurnL.st.orientacion+3)%4;
		hijoTurnL.coste += Coste(hijoTurnL.st.fila, hijoTurnL.st.columna, hijoTurnL.st.tieneBikini, hijoTurnL.st.tieneZapatillas, actTURN_L);
		hijoTurnL.valor_heuristica = hijoTurnL.coste;

		if (Cerrados.find(hijoTurnL.st) == Cerrados.end()){
			hijoTurnL.secuencia.push_back(actTURN_L);
			Abiertos.push(hijoTurnL);
		}

		// Generar descendiente de avanzar
		nodo hijoForward = current;
		hijoForward.coste += Coste(hijoForward.st.fila, hijoForward.st.columna, hijoForward.st.tieneBikini, hijoForward.st.tieneZapatillas, actFORWARD);
		hijoForward.valor_heuristica = hijoForward.coste;

		if (!HayObstaculoDelante(hijoForward.st)){
			if (Cerrados.find(hijoForward.st) == Cerrados.end()){
				hijoForward.secuencia.push_back(actFORWARD);
				Abiertos.push(hijoForward);
			}
		}

		if (!Abiertos.empty())
		{
			current = Abiertos.top();  // priority_queue no tiene front, si no top
		}
	}

  cout << "Terminada la busqueda\n";

	if (current.st.fila == destino.fila and current.st.columna == destino.columna){
		cout << "Cargando el plan\n";
		plan = current.secuencia;
		cout << "Longitud del plan: " << plan.size() << endl;
		PintaPlan(plan);
		// ver el plan en el mapa
		VisualizaPlan(origen, plan);
		return true;
	}
	else {
		cout << "No encontrado plan\n";
	}


	return false;
}

////////////////////////////////////////////////
//					Nivel 3
////////////////////////////////////////////////

int ComportamientoJugador::distManhattan(const estado &inicio, const estado &fin) //Devuelve la distManhattan entre dos puntos. Se usa como h(n) para estimar dist al final
{
	return abs(inicio.fila - fin.fila) + abs(inicio.columna - fin.columna);
}

int ComportamientoJugador::mejorDistManhattan(const estado &inicio, const estado destino[3], bool obje_visitados[3]) //TAmbién podria haberse hecho permutación de los nodos
{
	int valor = 1000000; //Valor muy alto para que se actualice por el valor más bajo

	for(int i = 0 ; i < num_objetivos ; i++)
	{
		if(!obje_visitados[i] && valor > distManhattan(inicio, destino[i])) //SI no se ha visitado ya
			valor = distManhattan(inicio, destino[i]);
	}

	return valor;
}

struct ComparaEstadosAEstrella //Comparador de estados para CU
{
	bool operator()(const estado &a, const estado &n) const
	{
		if ((a.fila > n.fila) or 
		  (a.fila == n.fila and a.columna > n.columna) or
	      (a.fila == n.fila and a.columna == n.columna and a.orientacion > n.orientacion) or
	      (a.fila == n.fila and a.columna == n.columna and a.orientacion == n.orientacion and a.tieneZapatillas>n.tieneZapatillas) or
	      (a.fila == n.fila and a.columna == n.columna and a.orientacion == n.orientacion and a.tieneZapatillas==n.tieneZapatillas and a.tieneBikini>n.tieneBikini) or
          (a.fila == n.fila and a.columna == n.columna and a.orientacion == n.orientacion and a.tieneZapatillas==n.tieneZapatillas and a.tieneBikini==n.tieneBikini and a.num_obj_visit>n.num_obj_visit))
			return true;
		else
			return false;
	}
};

// Implementación de la busqueda usando el algoritmo A*
// Entran los puntos origen y destino y devuelve la
// secuencia de acciones en plan, una lista de acciones.
bool ComportamientoJugador::pathFinding_AEstrella(const estado &origen, const estado destino[3], list<Action> &plan)
{

	//valor heuristica = f(n) = c(n) + h(n) en mi caso, aunque debería ser h(n)

	//Borro la lista
	cout << "Calculando plan\n";
	plan.clear();
	set<estado,ComparaEstadosAEstrella> Cerrados; // Lista de Cerrados
	priority_queue<nodo> Abiertos;				 // Lista de Abiertos

    nodo current;
	current.st = origen;
	current.secuencia.empty();

	// Inicialización de los nuevos atributos (variables booleanas de bikini y zapatillas junto con el coste)
	current.st.tieneBikini = false;
	current.st.tieneZapatillas = false;
	current.st.num_obj_visit = 0;
	current.coste = Coste(origen.fila, origen.columna, current.st.tieneBikini, current.st.tieneZapatillas, actIDLE);

	for(int i = 0 ; i < num_objetivos ; i++)
	{
		current.st.obj_visitados[i] = false;
	}

	//cout << mejorDistManhattan(current.st , destino, current.st.obj_visitados) << endl;
	current.valor_heuristica = current.coste + mejorDistManhattan(current.st , destino, current.st.obj_visitados); //ya que es de costo uniforme

	bool terminado = false;

	Abiertos.push(current);

  while (!Abiertos.empty() and !terminado)
  {

		Abiertos.pop();
		Cerrados.insert(current.st);

		// Comprobar la casilla si es de tipo bikini o zapatillas
		if (mapaResultado[current.st.fila][current.st.columna]=='K')
        {
            current.st.tieneBikini = true;

            if(current.st.tieneZapatillas)
            current.st.tieneZapatillas = false; // no se puede tener los dos items
        }

        if (mapaResultado[current.st.fila][current.st.columna]=='D')
        {
            current.st.tieneZapatillas = true;

            if(current.st.tieneBikini)
            current.st.tieneBikini = false; 
        }

	/*
        cout << current.st.fila << "	" << current.st.columna << "	" << current.st.num_obj_visit << 
    	"	b: " << current.st.tieneBikini << "	z: " << current.st.tieneZapatillas << endl;
    */

        for(int i = 0 ; i < num_objetivos ; i++) //Ver si algún objetivo se ha alcanzado
        {
        	if(current.st.fila == destino[i].fila and current.st.columna == destino[i].columna and !current.st.obj_visitados[i])
        	{
				current.st.obj_visitados[i] = true;
				current.st.num_obj_visit++;
        	}	
        }

        if(num_objetivos == current.st.num_obj_visit) //Comprobar si se han visitado todos los objetivos
        {
        	terminado = true;
		}

        else
        {
			// Generar descendiente de girar a la derecha
			nodo hijoTurnR = current;
			hijoTurnR.st.orientacion = (hijoTurnR.st.orientacion+1)%4;
			hijoTurnR.coste += Coste(hijoTurnR.st.fila, hijoTurnR.st.columna, hijoTurnR.st.tieneBikini, hijoTurnR.st.tieneZapatillas, actTURN_R);
			hijoTurnR.valor_heuristica = hijoTurnR.coste;
			hijoTurnR.valor_heuristica += mejorDistManhattan(hijoTurnR.st, destino, hijoTurnR.st.obj_visitados);		

			if (Cerrados.find(hijoTurnR.st) == Cerrados.end())
			{
				hijoTurnR.secuencia.push_back(actTURN_R);
				Abiertos.push(hijoTurnR);
			}

			// Generar descendiente de girar a la izquierda
			nodo hijoTurnL = current;
			hijoTurnL.st.orientacion = (hijoTurnL.st.orientacion+3)%4;
			hijoTurnL.coste += Coste(hijoTurnL.st.fila, hijoTurnL.st.columna, hijoTurnL.st.tieneBikini, hijoTurnL.st.tieneZapatillas, actTURN_L);
			hijoTurnL.valor_heuristica = hijoTurnL.coste;
			hijoTurnL.valor_heuristica += mejorDistManhattan(hijoTurnL.st , destino, hijoTurnL.st.obj_visitados);		

			if (Cerrados.find(hijoTurnL.st) == Cerrados.end())
			{
				hijoTurnL.secuencia.push_back(actTURN_L);
				Abiertos.push(hijoTurnL);
			}

			// Generar descendiente de avanzar
			nodo hijoForward = current;
			hijoForward.coste += Coste(hijoForward.st.fila, hijoForward.st.columna, hijoForward.st.tieneBikini, hijoForward.st.tieneZapatillas, actFORWARD);
			hijoForward.valor_heuristica = hijoForward.coste;
			hijoForward.valor_heuristica += mejorDistManhattan(hijoForward.st , destino, hijoForward.st.obj_visitados);		

			if (!HayObstaculoDelante(hijoForward.st))
			{
				if (Cerrados.find(hijoForward.st) == Cerrados.end()){
					hijoForward.secuencia.push_back(actFORWARD);
					Abiertos.push(hijoForward);
				}
			}

			if (!Abiertos.empty())
			{
				current = Abiertos.top();  // priority_queue no tiene front, si no top
			}
		}
	}

  cout << "Terminada la busqueda\n";

	if (terminado)
	{
		cout << "Cargando el plan\n";
		plan = current.secuencia;
		cout << "Longitud del plan: " << plan.size() << endl;
		PintaPlan(plan);
		// ver el plan en el mapa
		VisualizaPlan(origen, plan);
		return true;
	}
	else 
	{
		cout << "No encontrado plan\n";
	}


	return false;
}