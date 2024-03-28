// Clase Riego
class Riego {
private:
    bool valvula_externa_activa;
    bool valvula_interna_activa;

public:
    Riego() : valvula_externa_activa(false), valvula_interna_activa(false) {}

    void activarValvulaExterna() {
        // Código para activar el relé de la válvula externa
        valvula_externa_activa = true;
    }

    void desactivarValvulaExterna() {
        // Código para desactivar el relé de la válvula externa
        valvula_externa_activa = false;
    }

    void activarValvulaInterna() {
        // Código para activar el relé de la válvula interna
        valvula_interna_activa = true;
    }

    void desactivarValvulaInterna() {
        // Código para desactivar el relé de la válvula interna
        valvula_interna_activa = false;
    }
};

