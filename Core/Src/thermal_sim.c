#include "thermal_sim.h"

uint16_t Thermal_ComputeNextTint(uint16_t tint_prec, uint16_t text_prec, uint16_t text_cur, uint8_t puis, uint8_t is_closed) {
    float a = is_closed ? COEFF_A_FERME : COEFF_A_OUVERT;
    float b = is_closed ? COEFF_B_FERME : COEFF_B_OUVERT;
    float d = is_closed ? COEFF_D_FERME : COEFF_D_OUVERT;

    float input_prec = (float)text_prec + (d * (float)puis);
    float input_cur  = (float)text_cur  + (d * (float)puis);

    float tint_next = ((float)tint_prec * a) + ((input_prec + input_cur) * b);
    return (uint16_t)tint_next;
}

void AD7991_FormatFrame(uint8_t channel, uint16_t data_centi, uint8_t *buffer) {
    /* 
     * Format AD7991 (16 bits) :
     * Octet 1 (MSB) : [Channel ID (4 bits)] [Data D11..D8 (4 bits)]
     * Octet 2 (LSB) : [Data D7..D0 (8 bits)]
     */
    buffer[0] = (uint8_t)((channel << 4) | ((data_centi >> 8) & 0x0F));
    buffer[1] = (uint8_t)(data_centi & 0xFF);
}