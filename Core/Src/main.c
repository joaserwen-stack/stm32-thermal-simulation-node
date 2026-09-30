#include <stdint.h>
#include <stdbool.h>
#include "thermal_sim.h"

/* --- Registres STM32 simplifiés pour illustration --- */
typedef struct {
    volatile uint32_t CR1;
    volatile uint32_t CR2;
    volatile uint32_t OAR1;
    volatile uint32_t OAR2;
    volatile uint32_t DR;
    volatile uint32_t SR1;
    volatile uint32_t SR2;
} I2C_RegDef;

typedef struct {
    volatile uint32_t CR1;
    volatile uint32_t CNT;
    volatile uint32_t PSC;
    volatile uint32_t ARR;
    volatile uint32_t CCR2;
} TIM_RegDef;

#define TIM4_BASE       0x40000800
#define TIM4            ((TIM_RegDef *) TIM4_BASE)

/* --- Variables d'état système --- */
volatile uint8_t  fenetre_fermee = 1;      // 1 = Fermée, 0 = Ouverte
volatile uint32_t tim4_high_pulse = 0;     // Durée de l'état haut PC7
volatile uint8_t  puissance_puis = 0;      // 0 à 127

uint16_t Cent_Tint = 2000;                 // 20.00 °C
uint16_t Cent_Tint_prec = 2000;
uint16_t Cent_Text = 500;                  // 5.00 °C
uint16_t Cent_Text_prec = 500;

uint8_t i2c_tx_buffer[2];
volatile uint8_t i2c_tx_index = 0;
volatile uint8_t current_channel = AD7991_CH1_TINT;

/* --- Gestionnaire d'interruption externe (PC7 PWM & Bouton fenêtre) --- */
void EXTI_IRQHandler_Pin7_PC7(uint8_t pin_state) {
    if (pin_state == 1) {
        /* Front montant : démarrage du chronomètre */
        TIM4->CNT = 0;
    } else {
        /* Front descendant : capture de la durée et calcul du rapport cyclique */
        tim4_high_pulse = TIM4->CNT;
        puissance_puis = (uint8_t)(tim4_high_pulse / 16);
        if (puissance_puis > 127) {
            puissance_puis = 127;
        }
    }
}

void EXTI_IRQHandler_ButtonToggle(void) {
    /* Bascule d'état de la fenêtre */
    fenetre_fermee = !fenetre_fermee;
}

/* --- Gestionnaire d'interruption I2C Esclave (Événements) --- */
void I2C1_EV_IRQHandler_Handler(I2C_RegDef *i2c) {
    uint32_t sr1 = i2c->SR1;
    uint32_t sr2 = i2c->SR2;

    /* Détection d'adresse reconnue (ADDR) */
    if (sr1 & (1 << 1)) {
        (void)sr2; // Lecture SR2 pour acquitter le drapeau ADDR
        i2c_tx_index = 0;
        
        uint16_t data_to_send = (current_channel == AD7991_CH1_TINT) ? Cent_Tint : Cent_Text;
        AD7991_FormatFrame(current_channel, data_to_send, i2c_tx_buffer);
        
        // Alternance automatique de canal pour la lecture suivante
        current_channel = (current_channel == AD7991_CH1_TINT) ? AD7991_CH2_TEXT : AD7991_CH1_TINT;
    }

    /* Buffer de transmission vide (TXE) */
    if (sr1 & (1 << 7)) {
        if (i2c_tx_index < 2) {
            i2c->DR = i2c_tx_buffer[i2c_tx_index++];
        } else {
            i2c->DR = 0x00;
        }
    }
}

int main(void) {
    /* 
     * Initialisation horloge (16 MHz), GPIOs (PC7 en entrée FT),
     * TIM4 en compteur de base, et interface I2C1 en mode esclave.
     */
    while (1) {
        /* Boucle de calcul périodique (ex: chaque 500 ms) */
        Cent_Tint = Thermal_ComputeNextTint(Cent_Tint_prec, Cent_Text_prec, Cent_Text, puissance_puis, fenetre_fermee);
        
        Cent_Tint_prec = Cent_Tint;
        Cent_Text_prec = Cent_Text;
    }
}