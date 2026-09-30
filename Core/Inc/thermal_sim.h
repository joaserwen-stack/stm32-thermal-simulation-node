#ifndef THERMAL_SIM_H
#define THERMAL_SIM_H

#include <stdint.h>

/* --- Paramètres du modèle thermique (Euler discret) --- */
#define COEFF_D_FERME       25.15987f
#define COEFF_D_OUVERT      25.15987f
#define COEFF_A_FERME       0.99f
#define COEFF_B_FERME       0.005f
#define COEFF_A_OUVERT      0.95f
#define COEFF_B_OUVERT      0.02f

/* --- Adressage et canaux AD7991 --- */
#define AD7991_I2C_ADDR     (0x28 << 1)
#define AD7991_CH1_TINT     0x00
#define AD7991_CH2_TEXT     0x01

/* --- Prototypes --- */
uint16_t Thermal_ComputeNextTint(uint16_t tint_prec, uint16_t text_prec, uint16_t text_cur, uint8_t puis, uint8_t is_closed);
void AD7991_FormatFrame(uint8_t channel, uint16_t data_centi, uint8_t *buffer);

#endif /* THERMAL_SIM_H */