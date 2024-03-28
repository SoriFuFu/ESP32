#ifndef TEMPORIZADOR_H
#define TEMPORIZADOR_H

#include <LiquidCrystal.h>

class Temporizador
{
private:
  unsigned long tiempoInicial;
  unsigned int tiempoPausado;
  unsigned int duracionTotal;
  bool enPausa;

public:
  Temporizador() {}

  // Método para iniciar el conteo regresivo con una duración específica en milisegundos
void iniciarConteoRegresivo(unsigned long duracionMilisegundos) {
    duracionTotal = duracionMilisegundos;
    tiempoInicial = millis();
    enPausa = false;
}

// Método para pausar el conteo regresivo
void pausarConteo() {
    if (!enPausa) {
        tiempoPausado = millis();
        enPausa = true;
    }
}



  unsigned long tiempoTranscurrido()
  {
    if (!enPausa)
    {
      return millis() - tiempoInicial;
    }
    else
    {
      return tiempoPausado - tiempoInicial;
    }
  }

  unsigned long tiempoEnPausa()
  {
    if (enPausa)
    {
      return millis() - tiempoPausado;
    }
    else
    {
      return 0;
    }
  }

  void pausar()
  {
    if (!enPausa)
    {
      tiempoPausado = millis();
      enPausa = true;
    }
  }

  void reanudar()
  {
    if (enPausa)
    {
      unsigned long tiempoPausa = millis() - tiempoPausado;
      tiempoInicial += tiempoPausa;
      enPausa = false;
    }
  }

  void reiniciar()
  {
    tiempoInicial = millis();
    enPausa = false;
  }

  bool terminado()
  {
    if (!enPausa)
    {
      return (millis() - tiempoInicial) >= duracionTotal;
    }
    else
    {
      return false;
    }
  }
};
#endif
