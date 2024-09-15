#include <iostream>
#include <thread>
#include <chrono>
#include <atomic>
#include <mutex>
#include <iomanip>
#include <sstream>
using namespace std;

mutex mtx;
atomic<int> cursorPosX(23);  // Posición X inicial del cursor
string valorX = "    ";
string valorY = "    ";

// Función para actualizar el reloj en tiempo real
void actualizarReloj(atomic<bool>& ejecutando, int tim) {
    while (ejecutando) {
        {
            lock_guard<mutex> lock(mtx);
            cout << "\033[2;22H" << "\033[K" << time(0) - tim << flush;
            // Actualizar la línea de entrada
            cout << "\033[1;22H" << "[" << valorX << "][" << valorY << "]" << flush;
            // Mover el cursor de vuelta a la posición de entrada
            cout << "\033[1;" << cursorPosX << "H" << flush;
        }
        this_thread::sleep_for(chrono::seconds(1)); // Actualizar cada segundo
    }
}

// Función para formatear el valor como una cadena de 4 caracteres
string formatearValor(int valor) {
    ostringstream ss;
    ss << setw(4) << valor;
    return ss.str();
}

int main() {
    int tim = time(0);
    int x, y;
    bool primerValor = true;

    atomic<bool> ejecutando(true);  // Variable atómica para controlar el hilo
    thread hiloReloj(actualizarReloj, ref(ejecutando), tim);  // Lanzar hilo para el reloj

    cout << "Ingresa dos valores: [    ][    ]" << endl;
    cout << "Tiempo transcurrido: " << time(0) - tim << endl;

    while (true) {
        {
            lock_guard<mutex> lock(mtx);
            if (primerValor) {
                cursorPosX = 23;  // Columna para el primer valor
            } else {
                cursorPosX = 29;  // Columna para el segundo valor
            }
            cout << "\033[1;" << cursorPosX << "H" << flush;
        }

        if (primerValor) {
            cin >> x;
            valorX = formatearValor(x);
            primerValor = false;
        } else {
            cin >> y;
            valorY = formatearValor(y);
            primerValor = true;
        }

        // Actualizar la visualización de los valores
        {
            lock_guard<mutex> lock(mtx);
            cout << "\033[1;22H" << "[" << valorX << "][" << valorY << "]" << flush;
            cout << "\033[1;" << cursorPosX << "H" << flush;
        }
    }

    // Finaliza el hilo cuando se termine el programa
    ejecutando = false;
    hiloReloj.join();  // Esperar que el hilo termine
}