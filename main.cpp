#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <list>
#include <queue>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <filesystem>
#include <iterator>
#include <chrono> // Biblioteca para medir el tiempo

namespace fs = std::filesystem;

struct Node {
    int location;
    int capacity;
    int intValue;
    std::string varcharValue;
    Node* next;

    // Constructor para ubicación y capacidad
    Node(int loc, int cap) : location(loc), capacity(cap), intValue(0), varcharValue(""), next(nullptr) {}

    // Constructor para valores enteros
    Node(int loc, int val, bool isInt) : location(loc), capacity(4), intValue(val), varcharValue(""), next(nullptr) {}

    // Constructor para valores varchar
    Node(int loc, std::string val, int cap) : location(loc), capacity(cap), intValue(0), varcharValue(val), next(nullptr) {}
};

class LinkedList {
private:
    Node* head;
    Node* varcharHead;
    int locationCounter;

public:
    LinkedList() : head(nullptr), varcharHead(nullptr), locationCounter(1) {}

    void addLocationCapacity(int location, int capacity) {
        Node* newNode = new Node(location, capacity);
        newNode->next = head;
        head = newNode;
    }

    void addIntValue(int value) {
        Node* newNode = new Node(locationCounter, value, true);
        locationCounter += 4;
        if (!head) {
            head = newNode;
        } else {
            Node* temp = head;
            while (temp->next) temp = temp->next;
            temp->next = newNode;
        }
    }

    void addVarcharValue(std::string value, int capacity) {
        Node* newNode = new Node(locationCounter, value, capacity);
        locationCounter += capacity;
        if (!varcharHead) {
            varcharHead = newNode;
        } else {
            Node* temp = varcharHead;
            while (temp->next) temp = temp->next;
            temp->next = newNode;
        }
    }

    void saveToFile(const std::string& filename) {
        std::ofstream file(filename, std::ios::app);  // Abrir en modo append
        if (!file.is_open()) {
            std::cout << "Error al abrir el archivo!" << std::endl;
            return;
        }

        int currentLocation = 1;

        // Guardar ubicaciones y capacidades de varchar
        Node* temp = varcharHead;
        while (temp) {
            file << "(" << currentLocation << ") " << temp->location << "," << temp->capacity << " ";
            currentLocation += 1;
            temp = temp->next;
        }

        // Guardar valores int
        temp = head;
        while (temp) {
            if (temp->varcharValue.empty() && temp->intValue != 0) {
                file << "(" << currentLocation << ") " << temp->intValue << " ";
                currentLocation += 4;
            }
            temp = temp->next;
        }

        // Guardar valores varchar
        temp = varcharHead;
        while (temp) {
            file << "(" << currentLocation << ") " << temp->varcharValue << " ";
            currentLocation += temp->capacity;
            temp = temp->next;
        }

        file << std::endl;  // Agregar una nueva línea después de cada registro
        file.close();
    }

    void printList() {
        int currentLocation = 1;

        // Imprimir ubicaciones y capacidades de varchar
        Node* temp = varcharHead;
        while (temp) {
            std::cout << "(" << currentLocation << ") " << temp->location << "," << temp->capacity << " ";
            currentLocation += 1;
            temp = temp->next;
        }

        // Imprimir valores int
        temp = head;
        while (temp) {
            if (temp->varcharValue.empty() && temp->intValue != 0) {
                std::cout << "(" << currentLocation << ") " << temp->intValue << " ";
                currentLocation += 4;
            }
            temp = temp->next;
        }

        // Imprimir valores varchar
        temp = varcharHead;
        while (temp) {
            std::cout << "(" << currentLocation << ") " << temp->varcharValue << " ";
            currentLocation += temp->capacity;
            temp = temp->next;
        }
        std::cout << std::endl;
    }
};

std::vector<std::pair<std::string, std::string>> parseSchema(const std::string& schemaFile) {
    std::ifstream file(schemaFile);
    std::vector<std::pair<std::string, std::string>> schema;
    std::string line;

    if (file.is_open()) {
        while (getline(file, line)) {
            std::stringstream ss(line);
            std::string token;
            while (getline(ss, token, '#')) {
                std::string name = token;
                getline(ss, token, '#');
                std::string type = token;
                schema.push_back({name, type});
            }
        }
        file.close();
    } else {
        std::cout << "Error al abrir el archivo esquema!" << std::endl;
    }

    return schema;
}

class ArbolBPlus {
    struct Nodo {
        bool esHoja;
        std::vector<int> llaves;
        std::vector<Nodo*> hijos;
        Nodo* siguiente;
        Nodo() : esHoja(true), siguiente(nullptr) {}
    };

    Nodo* raiz;
    int cantidadNodos;

public:
    ArbolBPlus(int D) : cantidadNodos(2 * D), raiz(nullptr) {}

    void insertar(int llave) {
        if (!raiz) {
            raiz = new Nodo();
            raiz->llaves.push_back(llave);
        } else {
            Nodo* nuevoHijo = nullptr;
            int nuevaLlave = insertarInterno(raiz, llave, nuevoHijo);
            if (nuevoHijo) {
                Nodo* nuevaRaiz = new Nodo();
                nuevaRaiz->esHoja = false;
                nuevaRaiz->llaves.push_back(nuevaLlave);
                nuevaRaiz->hijos.push_back(raiz);
                nuevaRaiz->hijos.push_back(nuevoHijo);
                raiz = nuevaRaiz;
            }
        }
    }

    void eliminar(int llave) {
        if (raiz) {
            eliminarInterno(raiz, llave);
            if (raiz->llaves.empty() && !raiz->esHoja) {
                Nodo* antiguaRaiz = raiz;
                raiz = raiz->hijos.front();
                delete antiguaRaiz;
            }
        }
    }

    void imprimir() const {
        if (raiz) imprimirInterno(raiz);
    }

    bool buscar(int llave) const {
        return buscarInterno(raiz, llave);
    }

private:
    int insertarInterno(Nodo* nodo, int llave, Nodo*& nuevoHijo) {
        if (nodo->esHoja) {
            nodo->llaves.insert(std::upper_bound(nodo->llaves.begin(), nodo->llaves.end(), llave), llave);
            if (nodo->llaves.size() <= cantidadNodos) {
                nuevoHijo = nullptr;
                return 0;
            } else {
                nuevoHijo = new Nodo();
                nuevoHijo->esHoja = true;
                int mitad = nodo->llaves.size() / 2;
                nuevoHijo->llaves.assign(nodo->llaves.begin() + mitad, nodo->llaves.end());
                nodo->llaves.resize(mitad);
                nuevoHijo->siguiente = nodo->siguiente;
                nodo->siguiente = nuevoHijo;
                return nuevoHijo->llaves.front();
            }
        } else {
            int i = std::upper_bound(nodo->llaves.begin(), nodo->llaves.end(), llave) - nodo->llaves.begin();
            int nuevaLlave = insertarInterno(nodo->hijos[i], llave, nuevoHijo);
            if (nuevoHijo) {
                nodo->llaves.insert(nodo->llaves.begin() + i, nuevaLlave);
                nodo->hijos.insert(nodo->hijos.begin() + i + 1, nuevoHijo);
                if (nodo->llaves.size() <= cantidadNodos) {
                    nuevoHijo = nullptr;
                    return 0;
                } else {
                    nuevoHijo = new Nodo();
                    nuevoHijo->esHoja = false;
                    int mitad = nodo->llaves.size() / 2;
                    nuevoHijo->llaves.assign(nodo->llaves.begin() + mitad + 1, nodo->llaves.end());
                    nuevoHijo->hijos.assign(nodo->hijos.begin() + mitad + 1, nodo->hijos.end());
                    nodo->llaves.resize(mitad);
                    nodo->hijos.resize(mitad + 1);
                    return nodo->llaves[mitad];
                }
            } else {
                return 0;
            }
        }
    }

    bool eliminarInterno(Nodo* nodo, int llave) {
        if (nodo->esHoja) {
            auto it = std::lower_bound(nodo->llaves.begin(), nodo->llaves.end(), llave);
            if (it != nodo->llaves.end() && *it == llave) {
                nodo->llaves.erase(it);
                return true;
            }
            return false;
        } else {
            int i = std::upper_bound(nodo->llaves.begin(), nodo->llaves.end(), llave) - nodo->llaves.begin();
            if (i > 0 && llave < nodo->llaves[i - 1]) --i;
            bool eliminado = eliminarInterno(nodo->hijos[i], llave);
            if (eliminado) {
                if (nodo->hijos[i]->llaves.size() < (cantidadNodos - 1) / 2) {
                    Nodo* izquierdo = (i > 0) ? nodo->hijos[i - 1] : nullptr;
                    Nodo* derecho = (i + 1 < nodo->hijos.size()) ? nodo->hijos[i + 1] : nullptr;
                    if (izquierdo && izquierdo->llaves.size() > (cantidadNodos - 1) / 2) {
                        nodo->hijos[i]->llaves.insert(nodo->hijos[i]->llaves.begin(), nodo->llaves[i - 1]);
                        nodo->llaves[i - 1] = izquierdo->llaves.back();
                        izquierdo->llaves.pop_back();
                    } else if (derecho && derecho->llaves.size() > (cantidadNodos - 1) / 2) {
                        nodo->hijos[i]->llaves.push_back(nodo->llaves[i]);
                        nodo->llaves[i] = derecho->llaves.front();
                        derecho->llaves.erase(derecho->llaves.begin());
                    } else if (izquierdo) {
                        izquierdo->llaves.push_back(nodo->llaves[i - 1]);
                        izquierdo->llaves.insert(izquierdo->llaves.end(), nodo->hijos[i]->llaves.begin(), nodo->hijos[i]->llaves.end());
                        delete nodo->hijos[i];
                        nodo->hijos.erase(nodo->hijos.begin() + i);
                        nodo->llaves.erase(nodo->llaves.begin() + i - 1);
                    } else if (derecho) {
                        nodo->hijos[i]->llaves.push_back(nodo->llaves[i]);
                        nodo->hijos[i]->llaves.insert(nodo->hijos[i]->llaves.end(), derecho->llaves.begin(), derecho->llaves.end());
                        delete nodo->hijos[i + 1];
                        nodo->hijos.erase(nodo->hijos.begin() + i + 1);
                        nodo->llaves.erase(nodo->llaves.begin() + i);
                    }
                }
                return true;
            }
            return false;
        }
    }

    void imprimirInterno(Nodo* nodo) const {
        std::queue<std::pair<Nodo*, int>> q;
        std::vector<std::vector<Nodo*>> niveles;

        q.push({nodo, 0});
        while (!q.empty()) {
            Nodo* actual = q.front().first;
            int nivel = q.front().second;
            q.pop();

            if (nivel >= niveles.size()) {
                niveles.push_back({});
            }
            niveles[nivel].push_back(actual);

            if (!actual->esHoja) {
                for (Nodo* hijo : actual->hijos) {
                    q.push({hijo, nivel + 1});
                }
            }
        }

        int maxAncho = 0;
        for (const auto& nivel : niveles) {
            int ancho = nivel.size() * 4;
            for (Nodo* n : nivel) {
                ancho += n->llaves.size() * 4;
            }
            if (ancho > maxAncho) {
                maxAncho = ancho;
            }
        }

        for (const auto& nivel : niveles) {
            int padding = (maxAncho - nivel.size() * 4) / (nivel.size() + 1);
            for (Nodo* n : nivel) {
                std::cout << std::string(padding, ' ');
                for (int llave : n->llaves) {
                    std::cout << llave << ' ';
                }
                std::cout << std::string(padding, ' ');
            }
            std::cout << "\n";
        }
    }

    bool buscarInterno(Nodo* nodo, int llave) const {
        if (!nodo) return false;
        if (nodo->esHoja) {
            return std::find(nodo->llaves.begin(), nodo->llaves.end(), llave) != nodo->llaves.end();
        } else {
            int i = std::upper_bound(nodo->llaves.begin(), nodo->llaves.end(), llave) - nodo->llaves.begin();
            if (i > 0 && llave < nodo->llaves[i - 1]) --i;
            return buscarInterno(nodo->hijos[i], llave);
        }
    }
};

// Función para obtener la ruta completa del archivo destino
std::string obtenerRutaDestino(const std::string& identificador) {
    std::string rutaBase = "disco/";
    std::string plato = identificador.substr(4, 1);  // Extraer PL1, por ejemplo
    std::string superficie = identificador.substr(8, 1);  // Extraer SU1, por ejemplo
    std::string pista = identificador.substr(12, 1);  // Extraer PI1, por ejemplo

    // Construir la ruta completa basada en los identificadores
    rutaBase += "plato" + plato + "/";
    rutaBase += "superficie" + superficie + "/";
    rutaBase += "pista" + pista + "/";

    // Nombre del archivo destino
    rutaBase += identificador + ".txt";
    std::cout << "Ruta destino: " << rutaBase << std::endl;

    return rutaBase;
}

class Disco {
private:
    int nroPlato;
    int nroSuperficie;
    int nroPistaxSuperficie;
    int nroSectorxPista;
    int byteSector;
    int capacidadDisco;
    /*Bloques*/
    int cantBloque;
    int capBloque;
    int sectorxBloque;
    int cantSector;
public:
    Disco(int nroPlato, int nroSuperficie, int nroPistaxSuperficie, int nroSectorxPista, int byteSector, int cantBloque) {
        this->nroPlato = nroPlato;
        this->nroSuperficie = nroSuperficie;
        this->nroPistaxSuperficie = nroPistaxSuperficie;
        this->nroSectorxPista = nroSectorxPista;
        this->byteSector = byteSector;
        this->capacidadDisco = byteSector * nroSectorxPista * nroPistaxSuperficie * nroSuperficie * nroPlato;
        this->cantBloque = cantBloque;
        while (this->capacidadDisco % (this->cantBloque * this->byteSector) != 0) {
            this->cantBloque++;
        }
        this->capBloque = this->capacidadDisco / this->cantBloque;
        this->sectorxBloque = this->capBloque / this->byteSector;
        this->cantSector = this->cantBloque * this->sectorxBloque;
    }

    int getNroPlato() { return nroPlato; }
    int getNroSuperficie() { return nroSuperficie; }
    int getNroPistaxSuperficie() { return nroPistaxSuperficie; }
    int getNroSectorxPista() { return nroSectorxPista; }
    int getByteSector() { return byteSector; }
    int getCantBloque() { return cantBloque; }
    int getCapacidadDisco() { return capacidadDisco; }
    int getCapBloque() { return capBloque; }
    int getSectorxBloque() { return sectorxBloque; }
    int getCantSector() { return cantSector; }  

    void verInformacionDisco() {
        std::cout << "-----------------------------------" << std::endl;
        std::cout << "Capacidad por Sector: " << byteSector << " bytes" << std::endl;
        std::cout << "Capacidad Total del disco: ";
        long long capacidadDiscoGB = capacidadDisco / (1024 * 1024 * 1024);
        if (capacidadDiscoGB > 1) {
            std::cout << capacidadDiscoGB << " GB" << std::endl;
        } else {
            std::cout << capacidadDisco << " bytes" << std::endl;
        }
        std::cout << "Cantidad de Bloques: " << cantBloque << std::endl;
        std::cout << "Capacidad por Bloque: " << capBloque << " bytes" << std::endl;
        std::cout << "Sectores por Bloque: " << sectorxBloque << std::endl;
        std::cout << "Total de Sectores: " << cantSector << std::endl;
    }

    void crearDisco () {
        std::string carpetaRaiz = "disco";

        fs::create_directory(carpetaRaiz);

        int sectorCount = 0;

        for(int i = 0; i < nroPlato; i++) {
            std::stringstream carpetaPlatoSS;
            carpetaPlatoSS << carpetaRaiz << "/plato" << i + 1;
            std::string carpetaPlato = carpetaPlatoSS.str();
            fs::create_directory(carpetaPlato);

            for(int j = 0; j < 2; j++) {
                std::stringstream carpetaSuperficieSS;
                carpetaSuperficieSS << carpetaPlato << "/superficie" << j + 1;
                std::string carpetaSuperficie = carpetaSuperficieSS.str();
                fs::create_directory(carpetaSuperficie);

                for(int k = 0; k < nroPistaxSuperficie; k++) {
                    std::stringstream carpetaPistaSS;
                    carpetaPistaSS << carpetaSuperficie << "/pista" << k + 1;
                    std::string carpetaPista = carpetaPistaSS.str();
                    fs::create_directory(carpetaPista); 

                    for(int l = 0; l < nroSectorxPista; l++) {
                        std::stringstream archivoSS;
                        archivoSS << carpetaPista << "/D#PL" << i + 1 << "#SU" << j + 1 << "#PI" << k + 1 << "#SE" << l + 1 << ".txt";
                        std::string archivo = archivoSS.str();

                        std::ofstream outFile(archivo);
                        if (outFile.is_open()) {
                            outFile << "";
                            outFile.close();
                        } else {
                            std::cerr << "Error al crear el archivo " << archivo << std::endl;
                        }

                        sectorCount++;
                    }
                }
            }
        }

        // Crear la carpeta bloques
        std::string carpetaBloques = carpetaRaiz + "/bloques";
        fs::create_directory(carpetaBloques);

        // Crear los archivos de bloques y asignar los sectores
        for (int b = 0; b < cantBloque; b++) {
            std::stringstream bloqueArchivoSS;
            bloqueArchivoSS << carpetaBloques << "/" << b + 1 << ".txt";
            std::string bloqueArchivo = bloqueArchivoSS.str();

            std::ofstream bloqueOutFile(bloqueArchivo);
            if (bloqueOutFile.is_open()) {
                for (int s = 0; s < sectorxBloque; s++) {
                    int sectorIndex = b * sectorxBloque + s;
                    int platoIndex = sectorIndex / (nroSuperficie * nroPistaxSuperficie * nroSectorxPista);
                    int superficieIndex = (sectorIndex / (nroPistaxSuperficie * nroSectorxPista)) % nroSuperficie;
                    int pistaIndex = (sectorIndex / nroSectorxPista) % nroPistaxSuperficie;
                    int sectorIndexInPista = sectorIndex % nroSectorxPista;

                    std::stringstream sectorSS;
                    sectorSS << "D#PL" << platoIndex + 1 << "#SU" << superficieIndex + 1 << "#PI" << pistaIndex + 1 << "#SE" << sectorIndexInPista + 1;
                    std::string sector = sectorSS.str();

                    bloqueOutFile << sector << std::endl;
                }
                bloqueOutFile.close();
            } else {
                std::cerr << "Error al crear el archivo de bloque " << bloqueArchivo << std::endl;
            }
        }
    }
};

void insertarConClave(ArbolBPlus& arbol) {
    std::ifstream archivoClave("disco/indice/Clave.txt");
    if (!archivoClave.is_open()) {
        std::cerr << "Error al abrir el archivo Clave.txt" << std::endl;
        return;
    }

    std::string lineaClave;
    std::getline(archivoClave, lineaClave);
    archivoClave.close();

    std::istringstream ss(lineaClave);
    std::string campo;
    std::vector<std::string> campos;

    while (std::getline(ss, campo, ',')) {
        campos.push_back(campo);
    }

    std::string idStr, direccion;
    std::cout << "Agregar " << campos[0] << ": ";
    std::cin >> idStr;
    direccion = "1.txt";

    int id = std::stoi(idStr);
    std::ofstream archivoSalida("disco/indice/" + idStr + ".txt");
    archivoSalida << idStr << "," << direccion << std::endl;
    archivoSalida.close();

    //////////////////////////////////////

    // Ruta del archivo 1.txt
    std::string rutaArchivo = "disco/bloques/1.txt";

    // Palabra clave que vamos a buscar y el texto a insertar
    std::string palabraClave = "D#PL1#SU1#PI1#SE1";

    // Vector para almacenar las líneas del archivo
    std::vector<std::string> lineasArchivo;

    // Abrir el archivo 1.txt en modo lectura
    std::ifstream archivoEntrada(rutaArchivo);

    if (archivoEntrada.is_open()) {
        std::string linea;
        bool encontrada = false;

        // Leer el archivo línea por línea
        while (std::getline(archivoEntrada, linea)) {
            lineasArchivo.push_back(linea);

            // Buscar la palabra clave en la línea actual
            if (!encontrada && linea.find(palabraClave) != std::string::npos) {
                // Insertar el texto debajo de la línea con la palabra clave
                lineasArchivo.push_back(idStr);
                encontrada = true; // Marcar como encontrada para evitar más inserciones
            }
        }

        archivoEntrada.close();

        // Reabrir el archivo 1.txt en modo escritura para sobrescribirlo
        std::ofstream archivoSalida(rutaArchivo);

        if (archivoSalida.is_open()) {
            // Escribir todas las líneas de vuelta al archivo 1.txt
            for (const auto& linea : lineasArchivo) {
                archivoSalida << linea << std::endl;
            }

            archivoSalida.close();
            std::cout << "Texto agregado correctamente al archivo 1.txt." << std::endl;

            // Obtener la ruta del archivo destino
            std::string rutaDestino = obtenerRutaDestino(palabraClave);

            // Abrir el archivo destino para escritura
            std::ofstream archivoDestino(rutaDestino, std::ios::app);

            if (archivoDestino.is_open()) {
                // Escribir el contenido nuevo en el archivo destino
                archivoDestino << idStr << std::endl;

                archivoDestino.close();
                std::cout << "Texto agregado correctamente al archivo destino: " << rutaDestino << std::endl;
            } else {
                std::cerr << "No se pudo abrir el archivo destino para escritura." << std::endl;
            }

        } else {
            std::cerr << "No se pudo abrir el archivo 1.txt para escritura." << std::endl;
        }

    } else {
        std::cerr << "No se pudo abrir el archivo 1.txt para lectura." << std::endl;
    }

    auto inicio = std::chrono::high_resolution_clock::now(); // Medir tiempo

    arbol.insertar(id);

    auto fin = std::chrono::high_resolution_clock::now(); // Medir tiempo
    std::chrono::duration<double> duracion = fin - inicio;
    std::cout << "Tiempo para insertar nuevo registro: " << duracion.count() << " segundos.\n";

}

void eliminarYBorrarArchivo(ArbolBPlus& arbol, int llave) {
    auto inicio = std::chrono::high_resolution_clock::now(); // Medir tiempo
    arbol.eliminar(llave);

    std::string nombreArchivo = "disco/indice/" + std::to_string(llave) + ".txt";
    if (fs::exists(nombreArchivo)) {
        fs::remove(nombreArchivo);
        std::cout << "Archivo Eliminado" << std::endl;
    } else {
        std::cerr << "Archivo " << nombreArchivo << " no encontrado." << std::endl;
    }

    auto fin = std::chrono::high_resolution_clock::now(); // Medir tiempo
    std::chrono::duration<double> duracion = fin - inicio;
    std::cout << "Tiempo para eliminar registro: " << duracion.count() << " segundos.\n";
}

int buscarYMostrardireccion(ArbolBPlus& arbol, int id) {
    //id para buscar
    int llave = id;
    auto inicio = std::chrono::high_resolution_clock::now(); // Medir tiempo
    bool encontrado = arbol.buscar(llave);

    if (encontrado) {
        std::string nombreArchivo = "disco/indice/" + std::to_string(llave) + ".txt";
        if (fs::exists(nombreArchivo)) {
            std::ifstream archivoEntrada(nombreArchivo);
            std::string direccion((std::istreambuf_iterator<char>(archivoEntrada)), std::istreambuf_iterator<char>());
            int ID = std::stoi(direccion);
            std::cout << "La direccion es: " << ID << std::endl;
            return ID;
        } else {
            std::cerr << "Archivo " << nombreArchivo << " no encontrado." << std::endl;
        }
    } else {
        std::cout << "El valor " << llave << " no fue encontrado en el arbol." << std::endl;
    }

    auto fin = std::chrono::high_resolution_clock::now(); // Medir tiempo
    std::chrono::duration<double> duracion = fin - inicio;
    std::cout << "Tiempo para buscar y mostrar direccion: " << duracion.count() << " segundos.\n";
    return -1;
}

///////////////////////////////////////////////////////////////////
//Capaz Para eliminar el insertarDatos()
///////////////////////////////////////////////////////////////////
void insertarDatos() {
    // Ruta del archivo 1.txt
    std::string rutaArchivo = "disco/bloques/1.txt";

    // Palabra clave que vamos a buscar y el texto a insertar
    std::string palabraClave = "D#PL1#SU1#PI1#SE1";
    std::string textoAGuardar;
    std::cout << "Ingresa un texto: ";
    std::cin.ignore();  // Limpiar el búfer de entrada
    std::getline(std::cin, textoAGuardar);

    // Vector para almacenar las líneas del archivo
    std::vector<std::string> lineasArchivo;

    // Abrir el archivo 1.txt en modo lectura
    std::ifstream archivoEntrada(rutaArchivo);

    if (archivoEntrada.is_open()) {
        std::string linea;
        bool encontrada = false;

        // Leer el archivo línea por línea
        while (std::getline(archivoEntrada, linea)) {
            lineasArchivo.push_back(linea);

            // Buscar la palabra clave en la línea actual
            if (!encontrada && linea.find(palabraClave) != std::string::npos) {
                // Insertar el texto debajo de la línea con la palabra clave
                lineasArchivo.push_back(textoAGuardar);
                encontrada = true; // Marcar como encontrada para evitar más inserciones
            }
        }

        archivoEntrada.close();

        // Reabrir el archivo 1.txt en modo escritura para sobrescribirlo
        std::ofstream archivoSalida(rutaArchivo);

        if (archivoSalida.is_open()) {
            // Escribir todas las líneas de vuelta al archivo 1.txt
            for (const auto& linea : lineasArchivo) {
                archivoSalida << linea << std::endl;
            }

            archivoSalida.close();
            std::cout << "Texto agregado correctamente al archivo 1.txt." << std::endl;

            // Obtener la ruta del archivo destino
            std::string rutaDestino = obtenerRutaDestino(palabraClave);

            // Abrir el archivo destino para escritura
            std::ofstream archivoDestino(rutaDestino);

            if (archivoDestino.is_open()) {
                // Escribir el contenido nuevo en el archivo destino
                archivoDestino << textoAGuardar << std::endl;

                archivoDestino.close();
                std::cout << "Texto agregado correctamente al archivo destino: " << rutaDestino << std::endl;
            } else {
                std::cerr << "No se pudo abrir el archivo destino para escritura." << std::endl;
            }

        } else {
            std::cerr << "No se pudo abrir el archivo 1.txt para escritura." << std::endl;
        }

    } else {
        std::cerr << "No se pudo abrir el archivo 1.txt para lectura." << std::endl;
    }

}

void crearIndice() {
    std::string carpetaDisco = "disco";
    std::string carpetaIndice = carpetaDisco + "/indice";
    std::string archivoClave = carpetaIndice + "/Clave.txt";

    // Crear la carpeta "disco" si no existe
    if (!fs::exists(carpetaDisco)) {
        fs::create_directory(carpetaDisco);
        std::cout << "Carpeta 'disco' creada exitosamente." << std::endl;
    } else {
        std::cout << "Carpeta 'disco' ya existe." << std::endl;
    }

    // Crear la carpeta "indice" si no existe
    if (!fs::exists(carpetaIndice)) {
        fs::create_directory(carpetaIndice);
        std::cout << "Carpeta 'indice' creada exitosamente." << std::endl;
    } else {
        std::cout << "Carpeta 'indice' ya existe." << std::endl;
    }

    // Crear el archivo Clave.txt con el contenido "id,direccion"
    std::ofstream archivo(archivoClave);
    if (archivo.is_open()) {
        archivo << "id,direccion" << std::endl;
        archivo.close();
    } else {
        std::cerr << "Error al crear el archivo Clave.txt" << std::endl;
    }
}

void ClikClak() {
    int opcionClickClak;
    ArbolBPlus arbolBPlus(1);
    crearIndice();
    //insertarDatosDesdeCSV(arbolBPlus, disco);
    do {
        std::cout << "-----------------------------------" << std::endl;
        std::cout << "(1) Insertar Datos" << std::endl;
        std::cout << "(2) Buscar Datos" << std::endl;
        std::cout << "(3) Eliminar Datos" << std::endl;
        std::cout << "(4) Imprimir Arbol" << std::endl;
        std::cout << "(0) Salir" << std::endl;
        std::cout << "Ingrese su opcion: ";
        std::cin >> opcionClickClak;
        switch (opcionClickClak) {
            case 1:
                insertarConClave(arbolBPlus);
                break;
            case 2:
                //buscarDatos();
                break;
            case 3:
                //eliminarConClave(arbolBPlus);
                break;
            case 4:
                arbolBPlus.imprimir();
                break;
            case 0:
                std::cout << "Saliendo del programa..." << std::endl;
                break;
            default:
                std::cout << "Opcion no valida" << std::endl;
                break;
        }
    } while (opcionClickClak != 0);

}

// Clase para representar una página
class Bloque {
private:
    std::string nombre;  // Nombre de la página (e.g., "1.txt")
    std::string rutaCompleta;  // Ruta completa del archivo
    int almacenamientoTotal;  // Almacenamiento total de la página en bytes
    int almacenamientoUsado;  // Almacenamiento usado actualmente en bytes

public:
    Bloque(const std::string& nombre, int almacenamientoTotal)
        : nombre(nombre), almacenamientoTotal(almacenamientoTotal), almacenamientoUsado(0) {
        rutaCompleta = "disco/bloques/" + nombre;
    }

    // Función para añadir información a la página
    void agregarInformacion(const std::string& informacion) {
        int bytexregistro = 50;
        //std::ofstream archivo(rutaCompleta, std::ios::app);
        //archivo << informacion << std::endl;
        //almacenamientoUsado += informacion.length();  // Suponiendo que 1 byte por carácter
        almacenamientoUsado += bytexregistro;
        //archivo.close();
    }

    // Función para calcular el porcentaje de almacenamiento usado
    double porcentajeAlmacenamientoUsado() const {
        if (almacenamientoTotal == 0) {
            return 100.0;  // Si la página no tiene almacenamiento, está llena
        } else {
            return static_cast<double>(almacenamientoUsado) / almacenamientoTotal * 100.0;
        }
    }

    // Getter para el nombre de la página
    std::string obtenerNombre() const {
        return nombre;
    }

    // Getter para la ruta completa de la página
    std::string obtenerRutaCompleta() const {
        return rutaCompleta;
    }
};

// Clase para manejar la lista de páginas
class GestorBloques {
private:
    std::vector<Bloque> bloques;  // Lista de páginas

public:
    // Función para crear la carpeta "Paginas"
    void crearCarpetas() {
        fs::create_directory("Bloques");
    }

    // Función para crear las páginas según la cantidad y almacenamiento especificados por el usuario
    void crearBloques(int cantidadBloques, int almacenamientoPorBloque) {
        for (int i = 1; i <= cantidadBloques; ++i) {
            std::string nombreBloque = std::to_string(i) + ".txt";
            Bloque bloque(nombreBloque, almacenamientoPorBloque);
            bloques.push_back(bloque);
        }
    }

    // Función para añadir información a una página específica
    void agregarInformacionAPagina(int indicePagina, const std::string& informacion) {
        if (indicePagina >= 0 && indicePagina < bloques.size()) {
            bloques[indicePagina].agregarInformacion(informacion);
            std::cout << "Informacion agregada a la pagina " << indicePagina + 1 << "." << std::endl;
        } else {
            std::cout << "Indice de página invalido." << std::endl;
        }
    }

    // Función para mostrar el porcentaje de almacenamiento disponible de cada página
    void FileSpaceMap() const {
        std::cout << "Porcentaje de disponibilidad de cada pagina:" << std::endl;
        for (size_t i = 0; i < bloques.size(); ++i) {
            std::cout << "Pagina " << i + 1 << ": ";
            double porcentajeUsado = bloques[i].porcentajeAlmacenamientoUsado();
            if (porcentajeUsado == 100.0) {
                std::cout << "100% (llena)" << std::endl;
            } else {
                std::cout << std::fixed << std::setprecision(2) << porcentajeUsado << "% usado, "
                          << 100.0 - porcentajeUsado << "% disponible." << std::endl;
            }
        }
    }

    // Función principal para manejar todo el proceso
    //Crear las paginas, pero eso ya esta en el disco
    void ejecutar(Disco &disco) {
        int cantidadBloques = disco.getCantBloque();
        int almacenamientoPorBloque = disco.getCapBloque();
        crearBloques(cantidadBloques, almacenamientoPorBloque);
    }

    friend class BufferManager;
};

// Estructura para almacenar la información de cada campo del esquema
struct Campo {
    std::string nombre;
    std::string tipo_dato;
    int cantidad_bytes;
};

// Función para leer el esquema desde el archivo "esquema.txt"
std::vector<Campo> leerEsquema(const std::string& archivo_esquema) {
    std::vector<Campo> campos;
    std::ifstream file(archivo_esquema);
    if (file.is_open()) {
        std::string linea;
        std::getline(file, linea);
        std::istringstream ss(linea);
        std::string token;
        while (std::getline(ss, token, '#')) {
            Campo campo;
            campo.nombre = token;
            std::getline(ss, campo.tipo_dato, '#');
            ss >> campo.cantidad_bytes;
            // Saltar el espacio en blanco después de la cantidad de bytes
            ss.ignore();
            campos.push_back(campo);
        }
        file.close();
    } else {
        std::cerr << "No se pudo abrir el archivo de esquema." << std::endl;
    }
    return campos;
}

class BufferPool {
private:
    struct Frame {
        int frameID;
        int pageID;
        bool dirtyBit;
        bool pinned;
        int pinCount;
        bool refBit;
    };

    std::vector<Frame> frames;
    std::string tablaFilename;

public:
    BufferPool() : tablaFilename("TablaBufferPool.txt") {}

    void MenuBufferPool() {
        int numFrames = 3;
        frames.resize(numFrames);

        for (int i = 0; i < numFrames; ++i) {
            frames[i] = {i, 0, false, false, 0, false};
        }
        CrearTabla();
    }

    void CrearTabla() {
        std::ofstream tablaFile(tablaFilename);
        if (tablaFile.is_open()) {
            tablaFile << "Frame ID\tPage ID\t\tDirty Bit\tPinned\t\tPin Count\tRef Bit\n";
            for (const auto& frame : frames) {
                tablaFile << frame.frameID << "\t\t"
                          << frame.pageID << "\t\t"
                          << frame.dirtyBit << "\t\t"
                          << frame.pinned << "\t\t"
                          << frame.pinCount << "\t\t"
                          << frame.refBit << "\n";
            }
            tablaFile.close();
            std::cout << "Tabla creada exitosamente en " << tablaFilename << std::endl;
        } else {
            std::cerr << "No se pudo crear el archivo: " << tablaFilename << std::endl;
        }
    }

    void MostrarTabla() {
        std::ifstream tablaFile(tablaFilename);
        if (tablaFile.is_open()) {
            std::string line;
            while (std::getline(tablaFile, line)) {
                std::cout << line << std::endl;
            }
            tablaFile.close();
        } else {
            std::cerr << "No se pudo abrir el archivo: " << tablaFilename << std::endl;
        }
    }

    void ActualizarTabla() {
        CrearTabla();
        MostrarTabla();
    }

    friend class BufferManager;
};

class BufferManager {
private:
    struct Consulta {
        int pageID;
        char accion;
        std::string contenido;
        bool completada;
    };
    GestorBloques& gestorBloques;
    BufferPool& bufferPool;
    bool usarLRU;
    int punteroReloj;
    std::list<int> listaLRU;
    std::unordered_map<int, std::list<int>::iterator> mapaLRU;
    std::unordered_map<int, std::queue<Consulta>> colasDeConsultas;

public:
    BufferManager(BufferPool& pool, GestorBloques& gestor) : bufferPool(pool), gestorBloques(gestor), punteroReloj(0)  {
        int opcion;
        std::cout << "Algoritmo de reemplazo: LRU ";
        usarLRU = true;
    }

    void MenuBufferManager(Disco& disco) {
        MenuConsultas(disco);
    }

    void MenuConsultas(Disco& disco) {
        ArbolBPlus arbolBPlus(2);
        crearIndice();
        int opcion;
        int opcionUser;
        int userID;
        int pageID;
        std::string respuesta;
        std::string nombrePagina;
        std::string nombreBloque;
        do {
            std::cout << "Menu iniciar sesion" << std::endl;
            std::cout << "(1) Iniciar sesion" << std::endl;
            std::cout << "(0) Salir" << std::endl;
            std::cout << "Ingrese su opcion: ";
            std::cin >> opcionUser;
            switch (opcionUser) {
                case 1:
                    std::cout << "Ingresar userID: ";
                    std::cin >> userID;
                    do {
                        std::ifstream archivoBloque;
                        std::string contenido;
                        BufferPool::Frame* frame;
                        std::cout << "Menu de Consultas:" << std::endl;
                        /////////////////
                        /*
                        std::cout << "(1) Insertar Datos" << std::endl;
                        std::cout << "(2) Buscar Datos" << std::endl;
                        std::cout << "(3) Eliminar Datos" << std::endl;
                        std::cout << "(0) Salir" << std::endl;
                        */
                        /////////////////
                        std::cout << "(1) Consultar Datos" << std::endl;
                        std::cout << "(2) Imprimir tabla" << std::endl;
                        std::cout << "(3) Ver porcentaje de disponibilidad de cada pagina" << std::endl;
                        std::cout << "(4) Mostrar los datos de algun frame de la tabla" << std::endl;
                        std::cout << "(5) Imprimir las colas de consultas" << std::endl;
                        std::cout << "(6) Liberar o despinear algun frame" << std::endl;
                        std::cout << "(7) Agregar archivo .csv" << std::endl;
                        std::cout << "(8) Imprimir Arbol" << std::endl;
                        std::cout << "(0) Salir" << std::endl;
                        std::cout << "Opcion: ";
                        std::cin >> opcion;

                        switch (opcion) {
                            case 1:
                                int opcionDatos;
                                do {
                                    std::cout << "(1) Insertar Datos" << std::endl;
                                    std::cout << "(2) Buscar Datos" << std::endl;
                                    std::cout << "(3) Eliminar Datos" << std::endl;
                                    std::cout << "(0) Salir" << std::endl;
                                    std::cout << "Opcion: ";
                                    std::cin >> opcionDatos;
                                    switch (opcionDatos) {
                                        case 1: {
                                            int idBusqueda;
                                            std::cout << "ID: ";
                                            std::cin >> idBusqueda;
                                            bool comprobar = arbolBPlus.buscar(idBusqueda);
                                            if (comprobar) {
                                                std::cout << "El ID ya existe" << std::endl;
                                            } else {
                                                pageID = 1;
                                                frame = BuscarFramePorPagina(pageID);
                                                if (!frame) {
                                                    frame = BuscarFrameVacio();
                                                    if (!frame) {
                                                        if (usarLRU) {
                                                            LRU(pageID);
                                                            continue;
                                                        } else {
                                                            AlgoritmoClock(pageID);
                                                        }
                                                        frame = BuscarFramePorPagina(pageID);
                                                    } else {
                                                        frame->pageID = pageID;
                                                        frame->pinCount = 0;
                                                        frame->dirtyBit = false;
                                                        bufferPool.ActualizarTabla();
                                                    }
                                                }
                                                frame->pinCount++;
                                                if (usarLRU) {
                                                    ActualizarLRU(pageID);
                                                } else {
                                                    frame->refBit = true;
                                                }
                                                bufferPool.ActualizarTabla();
                                                char accion = 'e';
                                                ManejarAccionPagina(archivoBloque, frame, pageID, contenido, accion, arbolBPlus, userID, idBusqueda);
                                                }
                                            break;
                                        }
                                        case 2: {
                                            int idBusqueda;
                                            std::cout << "ID: ";
                                            std::cin >> idBusqueda;
                                            pageID = buscarYMostrardireccion(arbolBPlus, idBusqueda);
                                            if (pageID == -1){
                                                break;
                                            } else {
                                                frame = BuscarFramePorPagina(pageID);
                                                if (!frame) {
                                                    frame = BuscarFrameVacio();
                                                    if (!frame) {
                                                        if (usarLRU) {
                                                            LRU(pageID);
                                                            continue;
                                                        } else {
                                                            AlgoritmoClock(pageID);
                                                        }
                                                        frame = BuscarFramePorPagina(pageID);
                                                    } else {
                                                        frame->pageID = pageID;
                                                        frame->pinCount = 0;
                                                        frame->dirtyBit = false;
                                                        bufferPool.ActualizarTabla();
                                                    }
                                                }
                                                frame->pinCount++;
                                                if (usarLRU) {
                                                    ActualizarLRU(pageID);
                                                } else {
                                                    frame->refBit = true;
                                                }
                                                bufferPool.ActualizarTabla();
                                                char accion = 'l';
                                                ManejarAccionPaginaLectura(frame, pageID, contenido, accion, arbolBPlus, idBusqueda);
                                            }
                                            break;
                                        }
                                        case 3:
                                            int idBusqueda;
                                            std::cout << "ID: ";
                                            std::cin >> idBusqueda;
                                            pageID = buscarYMostrardireccion(arbolBPlus, idBusqueda);
                                            
                                            if (pageID == -1) {
                                                break;
                                            } else {
                                                if (comprobarUserID(pageID, userID, idBusqueda)) {
                                                    frame = BuscarFramePorPagina(pageID);
                                                    if (!frame) {
                                                        frame = BuscarFrameVacio();
                                                        if (!frame) {
                                                            if (usarLRU) {
                                                                LRU(pageID);
                                                                continue;
                                                            } else {
                                                                AlgoritmoClock(pageID);
                                                            }
                                                            frame = BuscarFramePorPagina(pageID);
                                                        } else {
                                                            frame->pageID = pageID;
                                                            frame->pinCount = 0;
                                                            frame->dirtyBit = false;
                                                            bufferPool.ActualizarTabla();
                                                        }
                                                    }
                                                    frame->pinCount++;
                                                    if (usarLRU) {
                                                        ActualizarLRU(pageID);
                                                    } else {
                                                        frame->refBit = true;
                                                    }
                                                    bufferPool.ActualizarTabla();
                                                    char accion = 'l';
                                                    std::cout << "pageID: " << pageID << std::endl;
                                                    ManejarAccionPaginaEliminar(frame, pageID, contenido, accion, arbolBPlus, idBusqueda);
                                                    eliminarYBorrarArchivo(arbolBPlus, idBusqueda);
                                                } else {
                                                    std::cout << "No tiene permisos para realizar esta operacion" << std::endl;
                                                }
                                            }
                                            break;
                                        case 0:
                                            break;
                                        default:
                                            std::cout << "Opcion no valida. Por favor, ingrese una opcion valida" << std::endl;
                                            break;
                                    }
                                } while (opcionDatos != 0);
                                break;
                            case 2:
                                bufferPool.MostrarTabla();
                                break;
                            case 3:
                                gestorBloques.FileSpaceMap();
                                break;
                            case 4:
                                int frameID;
                                std::cout << "Ingrese el ID del frame: ";
                                std::cin >> frameID;
                                imprimirFrame(frameID);
                                break;
                            case 5:
                                ImprimirColasDeConsultas();
                                break;
                            case 6:
                                PreguntarLiberarODespinear();
                                break;
                            case 7:
                                insertarDatosDesdeCSV2(arbolBPlus, *&frame, disco);
                                break;
                            case 8:
                                arbolBPlus.imprimir();
                                break;
                            case 0:
                                break;
                            default:
                                std::cout << "Opcion no valida. Por favor, ingrese una opcion valida (1, 2, 3 o 0)." << std::endl;
                                break;
                        }
                    } while (opcion != 0);
                case 0:
                    break;
                default:
                    std::cout << "Opcion no valida." << std::endl;
                    break;
            }
        } while (opcionUser != 0);
    }

    void insertarDatosDesdeCSV2(ArbolBPlus& arbolBPlus, BufferPool::Frame* frame, Disco& disco) {
        const int espacioDisponibleSectorMax = 250; // Espacio máximo por sector
        const int espacioRegistro = 50; // Tamaño de cada registro
        const int espacioDisponibleBloqueMax = 750; // Espacio máximo por bloque
        int espacioDisponibleSector = espacioDisponibleSectorMax;
        int espacioDisponibleBloque = espacioDisponibleBloqueMax;

        int bloqueDisponible = 1;
        int platoDisponible = 1;
        int superficieDisponible = 1;
        int pistaDisponible = 1;
        int sectorDisponible = 1;

        std::string rutaCSV = "clikclak.csv";
        std::ifstream archivoCSV(rutaCSV);
        if (!archivoCSV.is_open()) {
            std::cerr << "No se pudo abrir el archivo CSV." << std::endl;
            return;
        }

        // Leer el esquema desde el archivo
        std::vector<Campo> esquema = leerEsquema("esquemaF.txt");
        std::string line;

        while (std::getline(archivoCSV, line)) {
            frame = BuscarFramePorPagina(bloqueDisponible);
            if (!frame) {
                frame = BuscarFrameVacio();
                if (!frame) {
                    if (usarLRU) {
                        LRU(bloqueDisponible);
                    } else {
                        AlgoritmoClock(bloqueDisponible);
                    }
                    frame = BuscarFramePorPagina(bloqueDisponible);
                } else {
                    frame->pageID = bloqueDisponible;
                    frame->pinCount = 0;
                    frame->dirtyBit = false;
                    bufferPool.ActualizarTabla();
                }
            }
            frame->pinCount++;
            if (usarLRU) {
                ActualizarLRU(bloqueDisponible);
            } else {
                frame->refBit = true;
            }
            bufferPool.ActualizarTabla();
            char accion = 'e';
            std::istringstream ss(line);
            std::string token;
            std::string contenido;
            std::string idStr;
            std::string direccion;

            // Leer cada campo basado en el esquema
            for (const Campo& campo : esquema) {
                std::string valor;
                if (std::getline(ss, token, ',')) {
                    valor = token;
                    if (campo.nombre == "postId") {
                        idStr = valor;
                    }
                }

                // Asegurarse de que `valor` tenga el tamaño adecuado
                if (campo.tipo_dato == "int") {
                    valor.resize(campo.cantidad_bytes + 6, ' ');
                } else {
                    valor.resize(campo.cantidad_bytes, ' ');
                }

                contenido += valor;
            }

            // Comprobar espacio disponible en el sector
            if (espacioDisponibleSector < espacioRegistro) {
                // Mover al siguiente sector
                sectorDisponible++;
                espacioDisponibleSector = espacioDisponibleSectorMax; // Reiniciar espacio disponible para el nuevo sector
            }

            if (espacioDisponibleBloque < espacioRegistro) {
                bloqueDisponible++;
                espacioDisponibleBloque = espacioDisponibleBloqueMax; // Reiniciar espacio disponible para el nuevo bloque
            }

            if (sectorDisponible > disco.getNroSectorxPista()) {
                // Mover a la siguiente pista si todos los sectores están llenos
                sectorDisponible = 1;
                pistaDisponible++;
            }

            if (pistaDisponible > disco.getNroPistaxSuperficie()) {
                // Mover a la siguiente superficie si todas las pistas están llenas
                pistaDisponible = 1;
                superficieDisponible++;
            }

            if (superficieDisponible > disco.getNroSuperficie()) {
                // Mover al siguiente plato si todas las superficies están llenas
                superficieDisponible = 1;
                platoDisponible++;
            }

            if (platoDisponible > disco.getNroPlato()) {
                // Mover al siguiente bloque si todos los platos están llenos
                superficieDisponible = 1;
                platoDisponible = 1;
            }

            if (bloqueDisponible > disco.getCantBloque()) {
                std::cerr << "Se alcanzó el número máximo de bloques." << std::endl;
                break;
            }

            // Construir la dirección
            direccion = "D#PL" + std::to_string(platoDisponible) + "#SU" + std::to_string(superficieDisponible) + "#PI" + std::to_string(pistaDisponible) + "#SE" + std::to_string(sectorDisponible);

            // Actualizar el buffer pool
            frame->dirtyBit = true;
            bufferPool.ActualizarTabla();
            gestorBloques.agregarInformacionAPagina(frame->pageID - 1, contenido);

            // Actualizar el archivo de índice
            std::ofstream archivoIndice("disco/indice/" + idStr + ".txt");
            if (archivoIndice.is_open()) {
                archivoIndice << bloqueDisponible << std::endl;
                archivoIndice.close();
            } else {
                std::cerr << "Error al abrir el archivo de índice para escritura." << std::endl;
            }

            // Actualizar el archivo correspondiente
            std::string rutaArchivo = "disco/bloques/" + std::to_string(bloqueDisponible) + ".txt";
            std::vector<std::string> lineasArchivo;
            std::ifstream archivoEntrada(rutaArchivo);

            if (archivoEntrada.is_open()) {
                std::string linea;
                bool encontrada = false;

                while (std::getline(archivoEntrada, linea)) {
                    lineasArchivo.push_back(linea);
                    if (!encontrada && linea.find(direccion) != std::string::npos) {
                        lineasArchivo.push_back(contenido);
                        encontrada = true;
                        espacioDisponibleSector -= espacioRegistro;
                        espacioDisponibleBloque -= espacioRegistro;
                    }
                }

                archivoEntrada.close();

                std::ofstream archivoSalida(rutaArchivo);
                if (archivoSalida.is_open()) {
                    for (const auto& linea : lineasArchivo) {
                        archivoSalida << linea << std::endl;
                    }
                    archivoSalida.close();
                    std::cout << "Texto agregado correctamente al archivo " << bloqueDisponible << ".txt." << std::endl;

                    // Obtener la ruta del archivo destino
                    std::string rutaDestino = obtenerRutaDestino(direccion);
                    std::ofstream archivoDestino(rutaDestino, std::ios::app);

                    if (archivoDestino.is_open()) {
                        archivoDestino << contenido << std::endl;
                        archivoDestino.close();
                        std::cout << "Texto agregado correctamente al archivo destino: " << rutaDestino << std::endl;
                    } else {
                        std::cerr << "No se pudo abrir el archivo destino para escritura." << std::endl;
                    }

                } else {
                    std::cerr << "No se pudo abrir el archivo " << bloqueDisponible << ".txt para escritura." << std::endl;
                }

            } else {
                std::cerr << "No se pudo abrir el archivo " << bloqueDisponible << ".txt para lectura." << std::endl;
            }

            // Medir tiempo de inserción
            auto inicio = std::chrono::high_resolution_clock::now();
            arbolBPlus.insertar(std::stoi(idStr));
            auto fin = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> duracion = fin - inicio;
            std::cout << "Tiempo para insertar nuevo registro: " << duracion.count() << " segundos.\n";

            // Actualizar el frame
            frame->pinCount--;
            ActualizarDirtyBit(bloqueDisponible);
            bufferPool.ActualizarTabla();
        }
    }


    void insertarDatosDesdeCSV(ArbolBPlus& arbolBPlus, BufferPool::Frame* frame, Disco& disco) {
        int bloqueDisponible = 1;
        int platoDiscponible = 1;
        int superficieDisponible = 1;
        int pistaDisponible = 1;
        int sectorDisponible = 1;
        std::string rutaCSV = "clikclak.csv";
        std::ifstream archivoCSV(rutaCSV);
        if (!archivoCSV.is_open()) {
            std::cerr << "No se pudo abrir el archivo CSV." << std::endl;
            return;
        }

        // Leer el esquema desde el archivo
        std::vector<Campo> esquema = leerEsquema("esquemaF.txt");
        std::string line;
        int pageID = 1;

        while (std::getline(archivoCSV, line)) {
            frame = BuscarFramePorPagina(pageID);
            if (!frame) {
                frame = BuscarFrameVacio();
                if (!frame) {
                    if (usarLRU) {
                        LRU(pageID);
                    } else {
                        AlgoritmoClock(pageID);
                    }
                    frame = BuscarFramePorPagina(pageID);
                } else {
                    frame->pageID = pageID;
                    frame->pinCount = 0;
                    frame->dirtyBit = false;
                    bufferPool.ActualizarTabla();
                }
            }
            frame->pinCount++;
            if (usarLRU) {
                ActualizarLRU(pageID);
            } else {
                frame->refBit = true;
            }
            bufferPool.ActualizarTabla();
            char accion = 'e';
            std::istringstream ss(line);
            std::string token;
            std::string contenido;
            std::string idStr;
            std::string direccion = "1";

            // Leer cada campo basado en el esquema
            for (const Campo& campo : esquema) {
                std::string valor;
                if (std::getline(ss, token, ',')) {
                    valor = token;
                    if (campo.nombre == "postId") {
                        idStr = valor;
                    }
                }

                // Asegurarse de que `valor` tenga el tamaño adecuado
                if (campo.tipo_dato == "int") {
                    valor.resize(campo.cantidad_bytes + 6, ' ');
                } else {
                    valor.resize(campo.cantidad_bytes, ' ');
                }

                contenido += valor;
            }

            // Actualizar el buffer pool
            frame->dirtyBit = true;
            bufferPool.ActualizarTabla();
            gestorBloques.agregarInformacionAPagina(frame->pageID - 1, contenido);

            // Actualizar el archivo de índice
            std::ofstream archivoIndice("disco/indice/" + idStr + ".txt");
            if (archivoIndice.is_open()) {
                archivoIndice << direccion << std::endl;
                archivoIndice.close();
            } else {
                std::cerr << "Error al abrir el archivo de índice para escritura." << std::endl;
            }

            // Actualizar el archivo 1.txt
            std::string rutaArchivo = "disco/bloques/" + std::to_string(pageID) + ".txt";
            std::string palabraClave = "D#PL1#SU1#PI1#SE1";
            std::vector<std::string> lineasArchivo;
            std::ifstream archivoEntrada(rutaArchivo);

            if (archivoEntrada.is_open()) {
                std::string linea;
                bool encontrada = false;

                while (std::getline(archivoEntrada, linea)) {
                    lineasArchivo.push_back(linea);
                    if (!encontrada && linea.find(palabraClave) != std::string::npos) {
                        lineasArchivo.push_back(contenido);
                        encontrada = true;
                    }
                }

                archivoEntrada.close();

                std::ofstream archivoSalida(rutaArchivo);
                if (archivoSalida.is_open()) {
                    for (const auto& linea : lineasArchivo) {
                        archivoSalida << linea << std::endl;
                    }
                    archivoSalida.close();
                    std::cout << "Texto agregado correctamente al archivo 1.txt." << std::endl;

                    // Obtener la ruta del archivo destino
                    std::string rutaDestino = obtenerRutaDestino(palabraClave);
                    std::ofstream archivoDestino(rutaDestino, std::ios::app);

                    if (archivoDestino.is_open()) {
                        archivoDestino << contenido << std::endl;
                        archivoDestino.close();
                        std::cout << "Texto agregado correctamente al archivo destino: " << rutaDestino << std::endl;
                    } else {
                        std::cerr << "No se pudo abrir el archivo destino para escritura." << std::endl;
                    }

                } else {
                    std::cerr << "No se pudo abrir el archivo 1.txt para escritura." << std::endl;
                }

            } else {
                std::cerr << "No se pudo abrir el archivo 1.txt para lectura." << std::endl;
            }

            // Medir tiempo de inserción
            auto inicio = std::chrono::high_resolution_clock::now();
            arbolBPlus.insertar(std::stoi(idStr));
            auto fin = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> duracion = fin - inicio;
            std::cout << "Tiempo para insertar nuevo registro: " << duracion.count() << " segundos.\n";

            // Actualizar el frame
            frame->pinCount--;
            ActualizarDirtyBit(pageID);
            bufferPool.ActualizarTabla();
        }
    }

    bool comprobarUserID(int pageID, int userId, int idBusqueda) {
        std::ifstream archivoBloque("disco/bloques/" + std::to_string(pageID) + ".txt");
        if (!archivoBloque.is_open()) {
            std::cerr << "Error al abrir el archivo " << pageID << ".txt" << std::endl;
            // Manejar el error apropiadamente, como lanzar una excepción o devolver un valor de error
            return false;
        }
        
        std::string linea;
        bool encontrado = false;

        while (std::getline(archivoBloque, linea)) {
            std::istringstream iss(linea);
            std::vector<std::string> tokens((std::istream_iterator<std::string>(iss)), std::istream_iterator<std::string>());

            if (tokens.size() >= 2) {
                std::string primerToken = tokens[0];
                std::string segundoToken = tokens[1];

                // Comparar los tokens con idBusqueda y userId
                if (primerToken == std::to_string(idBusqueda) && segundoToken == std::to_string(userId)) {
                    encontrado = true;
                    break;
                }
            }
        }

        archivoBloque.close();
        return encontrado;
    }
    
    void ManejarAccionPaginaLectura(BufferPool::Frame* frame, int pageID, std::string& contenido, char& accion, ArbolBPlus& arbolBPlus, int idBusqueda) {
        std::string respuesta = "si";
        if (accion == 'l') {
            std::ifstream archivoBloque("disco/bloques/" + std::to_string(pageID) + ".txt");
            if (!archivoBloque.is_open()) {
                std::cerr << "Error al abrir el archivo " << pageID << ".txt" << std::endl;
                // Manejar el error apropiadamente, como lanzar una excepción o devolver un valor de error
                return;
            }
            std::string linea;

            std::unordered_set<std::string> numerosEncontrados;

            while (std::getline(archivoBloque, linea)) {
                // Encontrar el número antes de la primera coma
                std::istringstream iss(linea);
                std::string token;
                if (std::getline(iss, token, ' ')) {
                    // token contiene el número antes de la primera coma
                    if (token == std::to_string(idBusqueda)) {
                        std::cout << linea << std::endl;
                    }
                    // Agregar el número encontrado al conjunto
                    numerosEncontrados.insert(token);
                }
            }

            archivoBloque.close();
            //archivoBloque.clear();
            //archivoBloque.seekg(0, std::ios::beg);
        } else {
            std::cout << "No se puede realizar esta accion" << std::endl;
        }
        if (respuesta == "si") {
            frame->pinCount--;
            ActualizarDirtyBit(pageID);
            bufferPool.ActualizarTabla();
        } else {
            Consulta consulta = {pageID, accion, contenido, false};
            colasDeConsultas[pageID].push(consulta);
            ActualizarDirtyBit(pageID);
            bufferPool.ActualizarTabla();
        }
    }

    void ManejarAccionPaginaEliminar(BufferPool::Frame* frame, int pageID, std::string& contenido, char& accion, ArbolBPlus& arbolBPlus, int idBusqueda) {
        std::string respuesta = "si";
        if (accion == 'l') {
            // Abrir el archivo principal
            std::ifstream archivoBloque("disco/bloques/" + std::to_string(pageID) + ".txt");
            if (!archivoBloque.is_open()) {
                std::cerr << "Error al abrir el archivo " << pageID << ".txt" << std::endl;
                return;
            }

            // Archivo temporal para almacenar el contenido modificado
            std::ofstream archivoTempB("disco/bloques/temp_" + std::to_string(pageID) + ".txt");
            if (!archivoTempB.is_open()) {
                std::cerr << "Error al crear el archivo temporal" << std::endl;
                archivoBloque.close();
                return;
            }

            std::string linea;
            std::string direccion;

            // Leer el archivo y buscar la línea con el ID
            while (std::getline(archivoBloque, linea)) {
                std::istringstream iss(linea);
                std::string token;
                if (std::getline(iss, token, ' ')) {
                    if (token != std::to_string(idBusqueda)) {
                        archivoTempB << linea << std::endl;
                    }
                }
            }

            direccion = "D#PL1#SU1#PI1#SE1";

            archivoBloque.close();
            archivoTempB.close();

            // Reemplazar el archivo original con el archivo temporal
            if (std::remove(("disco/bloques/" + std::to_string(pageID) + ".txt").c_str()) != 0) {
                std::cerr << "Error al eliminar el archivo original" << std::endl;
                return;
            }

            if (std::rename(("disco/bloques/temp_" + std::to_string(pageID) + ".txt").c_str(),
                            ("disco/bloques/" + std::to_string(pageID) + ".txt").c_str()) != 0) {
                std::cerr << "Error al renombrar el archivo temporal" << std::endl;
                return;
            }

            // Si se encontró una dirección, eliminar la línea correspondiente en el archivo de destino
            if (!direccion.empty()) {
                // Convertir la dirección en la ruta del archivo destino
                std::string rutaArchivoDestino = "disco/";

                // Reemplazar los prefijos en la dirección para formar la ruta
                std::istringstream dirStream(direccion);
                std::string token;
                std::cout << "DIRECCION: " << direccion << std::endl;
                while (std::getline(dirStream, token, '#')) {
                    if (token.substr(0, 2) == "PL") {
                        rutaArchivoDestino += "plato" + token.substr(2) + "/";
                    } else if (token.substr(0, 2) == "SU") {
                        rutaArchivoDestino += "superficie" + token.substr(2) + "/";
                    } else if (token.substr(0, 2) == "PI") {
                        rutaArchivoDestino += "pista" + token.substr(2) + "/";
                    } else if (token.substr(0, 2) == "SE") {
                        rutaArchivoDestino += direccion + ".txt";
                    }
                }

                // Abrir el archivo de destino
                std::ifstream archivoDestino(rutaArchivoDestino);
                if (!archivoDestino.is_open()) {
                    std::cerr << "Error al abrir el archivo destino " << rutaArchivoDestino << std::endl;
                    return;
                }

                // Crear archivo temporal para el archivo destino
                std::ofstream archivoTempDestino("disco/temp_" + direccion + ".txt");
                if (!archivoTempDestino.is_open()) {
                    std::cerr << "Error al crear el archivo temporal destino" << std::endl;
                    archivoDestino.close();
                    return;
                }

                // Leer el archivo destino y copiar las líneas excepto la que contiene el ID
                while (std::getline(archivoDestino, linea)) {
                    std::istringstream iss(linea);
                    std::string token;
                    if (std::getline(iss, token, ' ')) {
                        if (token != std::to_string(idBusqueda)) {
                            archivoTempDestino << linea << std::endl;
                        }
                    }
                }

                archivoDestino.close();
                archivoTempDestino.close();

                // Reemplazar el archivo destino original con el archivo temporal
                if (std::remove(rutaArchivoDestino.c_str()) != 0) {
                    std::cerr << "Error al eliminar el archivo destino original" << std::endl;
                    return;
                }

                if (std::rename(("disco/temp_" + direccion + ".txt").c_str(), rutaArchivoDestino.c_str()) != 0) {
                    std::cerr << "Error al renombrar el archivo temporal destino" << std::endl;
                    return;
                }
            }
        } else {
            std::cout << "No se puede realizar esta acción" << std::endl;
        }

        frame->dirtyBit = true;
        bufferPool.ActualizarTabla();
        if (respuesta == "si") {
            frame->pinCount--;
            ActualizarDirtyBit(pageID);
            bufferPool.ActualizarTabla();
        } else {
            Consulta consulta = {pageID, accion, contenido, false};
            colasDeConsultas[pageID].push(consulta);
            ActualizarDirtyBit(pageID);
            bufferPool.ActualizarTabla();
        }
    }

    void ManejarAccionPagina(std::ifstream& archivoBloque, BufferPool::Frame* frame, int pageID, std::string& contenido, char& accion, ArbolBPlus& arbolBPlus, int userId, int idBusqueda) {
        std::string respuesta;
        while (true) {
            if (accion == 'l') {
                std::string line;
                while (std::getline(archivoBloque, line)) {
                    std::cout << line << std::endl;
                }
                archivoBloque.clear();
                archivoBloque.seekg(0, std::ios::beg);
            } else if (accion == 'e') {
                int opcion;
                std::cout << "Longitud Fija" << std::endl;
                if (longitudFija("esquemaF.txt", *frame, bufferPool, arbolBPlus, userId, idBusqueda)) {
                    std::cout << "Contenido guardado exitosamente." << std::endl;
                }
            } else {
                std::cerr << "Acción no valida." << std::endl;
                continue;
            }

            respuesta = "si";
            if (respuesta == "si") {

                frame->pinCount--;
                ActualizarDirtyBit(pageID);
                bufferPool.ActualizarTabla();
            } else {
                Consulta consulta = {pageID, accion, contenido, false};
                colasDeConsultas[pageID].push(consulta);
                ActualizarDirtyBit(pageID);
                bufferPool.ActualizarTabla();
            }
            break;
        }
    }

    void ImprimirColasDeConsultas() {
        std::cout << "Colas de Consultas:\n";
        for (const auto& entry : colasDeConsultas) {
            int pageID = entry.first;
            BufferPool::Frame* frame = BuscarFramePorPagina(pageID);
            if (frame) {
                std::cout << "Pagina ID: " << pageID << "\n";
                std::queue<Consulta> copiaCola = entry.second;
                while (!copiaCola.empty()) {
                    const Consulta& consulta = copiaCola.front();
                    std::cout << "  Accion: " << consulta.accion 
                            << ", Contenido: " << (consulta.contenido.empty() ? "(vacio)" : consulta.contenido)
                            << ", Completada: " << (consulta.completada ? "Si" : "No") << "\n";
                    copiaCola.pop();
                }
            }
        }
    }

    void imprimirFrame(int frameID) {
        BufferPool::Frame* frame = BuscarFrame(frameID);
        if (frame) {
            std::cout << "Frame ID: " << frame->frameID << std::endl;
            std::cout << "Page ID: " << frame->pageID << std::endl;
            std::cout << "Dirty Bit: " << frame->dirtyBit << std::endl;
            std::cout << "Pinned: " << frame->pinned << std::endl;
            std::cout << "Pin Count: " << frame->pinCount << std::endl;
            std::cout << "Ref Bit: " << frame->refBit << std::endl;
        } else {
            std::cout << "No se encontro el frame con ID " << frameID << std::endl;
        }
    }

    bool longitudFija(const std::string& esquemaArchivo, BufferPool::Frame& frame, BufferPool& bufferPool, ArbolBPlus& arbolBPlus, int userId, int idBusqueda) {
        int userID = userId;
        // Leer el esquema desde el archivo
        std::vector<Campo> esquema = leerEsquema(esquemaArchivo);
        std::string contenido;
        std::string respuesta;
        std::string idStr, direccion;

        for (const Campo& campo : esquema) {
            std::string valor;
            if(campo.nombre == "postId"){
                valor = std::to_string(idBusqueda);
                idStr = valor;
            }else if(campo.nombre == "pDate"){
                // Obtener la fecha y hora actuales
                std::time_t t = std::time(nullptr);  // Obtiene el tiempo actual
                std::tm* tm = std::localtime(&t);    // Convierte a la estructura tm

                // Usar std::ostringstream para construir la cadena de fecha
                std::ostringstream fechaStream;
                fechaStream << std::setw(2) << std::setfill('0') << tm->tm_mday   // Día
                            << std::setw(2) << std::setfill('0') << (tm->tm_mon + 1) // Mes (tm_mon comienza en 0)
                            << (tm->tm_year + 1900);                            // Año (tm_year es el año desde 1900)
                // Convertir el contenido del stream a std::string
                valor = fechaStream.str();
            } else if(campo.nombre == "userId"){
                valor = std::to_string(userID);
            } else {
                std::cout << "Ingrese el valor para " << campo.nombre << " (" << campo.tipo_dato << ", " << campo.cantidad_bytes << " bytes): ";
                std::cin >> valor;
                if(campo.nombre == "postId"){
                    idStr = valor;
                }
            }
            // Aseguramos que el valor ocupe exactamente campo.cantidad_bytes caracteres
            if(campo.tipo_dato == "int") {
                valor.resize(campo.cantidad_bytes + 6, ' ');
            } else {
                valor.resize(campo.cantidad_bytes, ' ');
            }
            contenido += valor;
        }

        std::cout << "Desea guardar el contenido? (si/no): ";
        std::cin >> respuesta;

        if (respuesta == "si") {
            frame.dirtyBit = true;
            bufferPool.ActualizarTabla();
            gestorBloques.agregarInformacionAPagina(frame.pageID - 1, contenido);
            ////////////////////////////////////////////
            std::ifstream archivoClave("disco/indice/Clave.txt");
            if (!archivoClave.is_open()) {
                std::cerr << "Error al abrir el archivo Clave.txt" << std::endl;
                ///return;
            }
            direccion = "1";

            int id = std::stoi(idStr);
            std::ofstream archivoSalida("disco/indice/" + idStr + ".txt");
            archivoSalida << direccion << std::endl;
            archivoSalida.close();
            ////////////////////////////////////////////
            // Ruta del archivo 1.txt
                std::string rutaArchivo = "disco/bloques/1.txt";

                // Palabra clave que vamos a buscar y el texto a insertar
                std::string palabraClave = "D#PL1#SU1#PI1#SE1";

                // Vector para almacenar las líneas del archivo
                std::vector<std::string> lineasArchivo;

                // Abrir el archivo 1.txt en modo lectura
                std::ifstream archivoEntrada(rutaArchivo);

                if (archivoEntrada.is_open()) {
                    std::string linea;
                    bool encontrada = false;

                    // Leer el archivo línea por línea
                    while (std::getline(archivoEntrada, linea)) {
                        lineasArchivo.push_back(linea);

                        // Buscar la palabra clave en la línea actual
                        if (!encontrada && linea.find(palabraClave) != std::string::npos) {
                            // Insertar el texto debajo de la línea con la palabra clave
                            lineasArchivo.push_back(contenido);
                            encontrada = true; // Marcar como encontrada para evitar más inserciones
                        }
                    }

                    archivoEntrada.close();

                    // Reabrir el archivo 1.txt en modo escritura para sobrescribirlo
                    std::ofstream archivoSalida(rutaArchivo);

                    if (archivoSalida.is_open()) {
                        // Escribir todas las líneas de vuelta al archivo 1.txt
                        for (const auto& linea : lineasArchivo) {
                            archivoSalida << linea << std::endl;
                        }

                        archivoSalida.close();
                        std::cout << "Texto agregado correctamente al archivo 1.txt." << std::endl;

                        // Obtener la ruta del archivo destino
                        std::string rutaDestino = obtenerRutaDestino(palabraClave);

                        // Abrir el archivo destino para escritura
                        std::ofstream archivoDestino(rutaDestino, std::ios::app);

                        if (archivoDestino.is_open()) {
                            // Escribir el contenido nuevo en el archivo destino
                            archivoDestino << contenido << std::endl;

                            archivoDestino.close();
                            std::cout << "Texto agregado correctamente al archivo destino: " << rutaDestino << std::endl;
                        } else {
                            std::cerr << "No se pudo abrir el archivo destino para escritura." << std::endl;
                        }

                    } else {
                        std::cerr << "No se pudo abrir el archivo 1.txt para escritura." << std::endl;
                    }

                } else {
                    std::cerr << "No se pudo abrir el archivo 1.txt para lectura." << std::endl;
                }

                auto inicio = std::chrono::high_resolution_clock::now(); // Medir tiempo

                arbolBPlus.insertar(id);

                auto fin = std::chrono::high_resolution_clock::now(); // Medir tiempo
                std::chrono::duration<double> duracion = fin - inicio;
                std::cout << "Tiempo para insertar nuevo registro: " << duracion.count() << " segundos.\n";
        }
        return false;
    }

    void longitudVariable(const std::string& esquemaArchivo, std::string nombrePagina, std::string nombreBloque, BufferPool::Frame& frame, BufferPool& bufferPool) {
        LinkedList list;
        std::vector<std::pair<std::string, std::string>> schema = parseSchema("esquemaV.txt");

        for (const auto& field : schema) {
            std::string name = field.first;
            std::string type = field.second;

            if (type == "varchar") {
                // 2da Version
                int capacity;
                std::string value;
                std::cout << "Ingrese el valor para " << name << ": ";
                std::cin.ignore();
                std::getline(std::cin, value);
                capacity = value.size();
                //std::cout << "Valor de capacity:" << capacity << std::endl;
                list.addVarcharValue(value, capacity);
            } else if (type == "int") {
                int value;
                std::cout << "Ingrese el valor para " << name << ": ";
                std::cin >> value;
                list.addIntValue(value);
            }
        }

        std::cout << "Desea guardar el contenido? (si/no): ";
        std::string respuesta;

        std::cin >> respuesta;
        if (respuesta == "si") {
            frame.dirtyBit = true;
            bufferPool.ActualizarTabla();
            list.saveToFile(nombrePagina);
            list.saveToFile(nombreBloque);
            //gestorPaginas.agregarInformacionAPagina(frame.pageID - 1, nombreBloque);
        }

        // Imprimir lista para verificación
        list.printList();
    }

    void ActualizarLRU(int pageID) {
        if (mapaLRU.find(pageID) != mapaLRU.end()) {
            listaLRU.erase(mapaLRU[pageID]);
        }
        listaLRU.push_front(pageID);
        mapaLRU[pageID] = listaLRU.begin();
    }

    void LRU(int pageID) {
        while (!listaLRU.empty()) {
            int lruPageID = listaLRU.back();
            listaLRU.pop_back();
            mapaLRU.erase(lruPageID);
            BufferPool::Frame* frame = BuscarFramePorPagina(lruPageID);
            if (frame && !frame->pinned && frame->pinCount == 0) {
                frame->pageID = pageID;
                frame->pinCount = 0;
                frame->dirtyBit = false;
                frame->pinned = false;
                frame->refBit = false;
                ActualizarLRU(pageID);
                bufferPool.ActualizarTabla();
                return;
            }
        }
        std::cerr << "No se pudo encontrar una pagina disponible para reemplazar, es necesario despinear o liberar alguna pagina." << std::endl;
    }

    void AlgoritmoClock(int pageID) {
        int framesChecked = 0;
        while (framesChecked < bufferPool.frames.size()) {
            BufferPool::Frame& frame = bufferPool.frames[punteroReloj];
            if (!frame.refBit && !frame.pinned && frame.pinCount == 0) {
                frame.pageID = pageID;
                frame.dirtyBit = false;
                frame.pinned = false;
                frame.pinCount = 0;
                frame.refBit = true;
                bufferPool.ActualizarTabla();
                punteroReloj = (punteroReloj + 1) % bufferPool.frames.size();
                return;
            } else {
                frame.refBit = false;
                punteroReloj = (punteroReloj + 1) % bufferPool.frames.size();
                framesChecked++;
            }
        }
        std::cerr << "Frame actual no disponibles para reemplazar, consulta el siguiente frame." << std::endl;
        PreguntarLiberarODespinear();
    }

    BufferPool::Frame* BuscarFrame(int frameId) {
        for (auto& frame : bufferPool.frames) {
            if (frame.frameID == frameId) {
                return &frame;
            }
        }
        return nullptr;
    }

    BufferPool::Frame* BuscarFramePorPagina(int pageID) {
        for (auto& frame : bufferPool.frames) {
            if (frame.pageID == pageID) {
                return &frame;
            }
        }
        return nullptr;
    }

    BufferPool::Frame* BuscarFrameVacio() {
        for (auto& frame : bufferPool.frames) {
            if (frame.pageID == 0) {
                return &frame;
            }
        }
        return nullptr;
    }

    void PreguntarLiberarODespinear() {
        std::string respuesta;
        std::cout << "Desea liberar algun frame? (si/no): ";
        std::cin >> respuesta;
        if (respuesta == "si") {
            int frameID;
            std::cout << "Ingrese el ID del frame: ";
            std::cin >> frameID;
            BufferPool::Frame* frame = BuscarFrame(frameID);
            if (frame && frame->pinCount > 0) {
                frame->pinCount--;
                colasDeConsultas[frame->pageID].pop();
                bufferPool.ActualizarTabla();
            } else {
                std::cerr << "Frame no encontrado o pinCount ya es 0." << std::endl;
            }
        }

        std::cout << "Desea despinear (desanclar) algun frame? (si/no): ";
        std::cin >> respuesta;
        if (respuesta == "si") {
            int frameID;
            std::cout << "Ingrese el ID del frame: ";
            std::cin >> frameID;
            BufferPool::Frame* frame = BuscarFrame(frameID);
            if (frame && frame->pinned) {
                frame->pinned = false;
                bufferPool.ActualizarTabla();
            } else {
                std::cerr << "Frame no encontrado o no esta pineado." << std::endl;
            }
        }
    }

    void ActualizarDirtyBit(int pageID) {
        BufferPool::Frame* frame = BuscarFramePorPagina(pageID);
        if (!frame) return;

        std::queue<Consulta>& cola = colasDeConsultas[pageID];
        std::queue<Consulta> copiaCola = cola;
        bool tieneEscritura = false;
        while (!copiaCola.empty()) {
            if (copiaCola.front().accion == 'e') {
                tieneEscritura = true;
                break;
            }
            copiaCola.pop();
        }
        frame->dirtyBit = tieneEscritura;
        bufferPool.ActualizarTabla();
    }
};

void gestorPaginas(Disco& disco) {
    GestorBloques gestor;
    gestor.ejecutar(disco);
    BufferPool bufferPool;
    bufferPool.MenuBufferPool();
    bufferPool.MostrarTabla();
    BufferManager bufferManager(bufferPool, gestor);
    bufferManager.MenuBufferManager(disco);
}

void menuDisco() {
    int nroPlato, nroSuperficie, nroPistaxSuperficie, nroSectorxPista, byteSector, cantBloque;
    int capacidadDisco;
    std::cout << "Especificaciones del Disco:" << std::endl;
    std::cout << "Disco Personalizado:" << std::endl;
    std::cout << "Nro de Platos: ";
    std::cin >> nroPlato;
    nroSuperficie = 2;
    std::cout << "Nro de Pistas x Superficie: ";
    std::cin >> nroPistaxSuperficie;
    std::cout << "Nro de Sectores x Pista: ";
    std::cin >> nroSectorxPista;
    std::cout << "Byte por Sector: ";
    std::cin >> byteSector;
    std::cout << "Cantidad Aprox de Bloques: ";
    std::cin >> cantBloque;
    Disco disco(nroPlato, nroSuperficie, nroPistaxSuperficie, nroSectorxPista, byteSector, cantBloque);
    disco.crearDisco();
    disco.verInformacionDisco();
    gestorPaginas(disco);
}

void DiscoPorDefecto() {
    std::cout << "Disco Por Defecto:" << std::endl;
    int nroPlato = 2;
    int nroSuperficie = 2;
    int nroPistaxSuperficie = 2;
    int nroSectorxPista = 3;
    int byteSector = 250;
    int cantBloque = 8;
    Disco disco(nroPlato, nroSuperficie, nroPistaxSuperficie, nroSectorxPista, byteSector, cantBloque);
    disco.crearDisco();
    disco.verInformacionDisco();
    gestorPaginas(disco);
}

void menu() {
    std::cout << "Megatron..." << std::endl;
    std::cout << "Menu:" << std::endl;
    int opcionMenu;
    do {
        std::cout << "(1) Crear Disco" << std::endl;
        std::cout << "(2) Consultas" << std::endl;
        std::cout << "(0) Salir" << std::endl;
        std::cout << "Ingrese su opcion: ";
        std::cin >> opcionMenu;
        switch (opcionMenu) {   
            case 1:
                std::cout << "Disco..." << std::endl;
                menuDisco();
                opcionMenu = 0;
            case 2:
                std::cout << "Consultas..." << std::endl;
                DiscoPorDefecto();
                opcionMenu = 0;
            case 0:
                std::cout << "Saliendo del programa..." << std::endl;
                break;
            default:
                std::cout << "Opcion invalida. Por favor ingrese una opcion valida" << std::endl;
        }
    } while (opcionMenu != 0);
}

int main() {
    menu();
    return 0;
}
