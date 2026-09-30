# 🌡️ STM32 Hardware-in-the-Loop Thermal Simulation Node

## Présentation du Projet
Ce projet consiste en l'implémentation d'un nœud esclave communicant sur microcontrôleur **STM32L152RE (ARM Cortex-M3)**. 

Le système joue le rôle d'un simulateur de bâtiment physique (Hardware-in-the-Loop) connecté à une carte de régulation maîtresse :
- Il acquiert une consigne de puissance de chauffe via un signal **PWM** modulé.
- Il calcule en temps réel l'évolution de la température intérieure ($T_{int}$) à l'aide d'un modèle thermodynamique discret intégrant l'isolation, les apports énergétiques et l'état des ouvrants (fenêtre).
- Il émule le comportement matériel d'un convertisseur analogique-numérique commercial (**AD7991**) sur un bus **I2C**, répondant aux requêtes du maître avec alternance de canaux ($T_{int}$ et $T_{ext}$).

---

## Spécifications & Contraintes du Système

* **Consigne & Objectif Physique :** Modéliser et simuler numériquement l'inertie thermique d'une pièce soumise à des apports énergétiques (chauffage modulé) et des déperditions thermiques variables (ouvertures/fermetures de fenêtres).
* **Contrainte Temps Réel :** Répondre de manière non bloquante et déterministe aux requêtes du maître I2C via des interruptions matérielles ciblées (`I2C1_EV_IRQHandler`), garantissant l'intégrité du bus sans latence de traitement.
* **Entrées / Sorties Critiques :**
  - **PWM (Puissance) :** Acquisition du rapport cyclique sur la broche tolérante 5V (`PC7` - Five-Volt Tolerant), assurant l'interfaçage sécurisé avec la carte de commande externe.
  - **Bus I2C :** Émulation esclave fournissant des trames formatées type ADC 12 bits conformes aux spécifications de l'AD7991.

---

## Architecture Technique & Fonctionnalités Clés

* **Émulation I2C Esclave Bas-Niveau :** Gestion directe des événements matériels (`ADDR`, `TXE`) et formatage des trames 16 bits conformes au protocole AD7991 (identifiant de canal sur le nibble haut du MSB).
* **Acquisition de Puissance PWM :** Mesure précise de la durée de l'impulsion haute par réinitialisation et lecture de compteur (`TIM4->CNT`) sur interruption d'état.
* **Modélisation Thermique Discrète :** Calcul itératif basé sur la méthode d'Euler :
  $$Cent\_Tint_{k+1} = (Cent\_Tint_k \times a) + [(Cent\_Text + d \times PUIS)_k + (Cent\_Text + d \times PUIS)_{k+1}] \times b$$
  avec coefficient d'apport thermique $d = 25.15987$ et adaptation dynamique des coefficients de perte ($a, b$) selon l'état de la fenêtre.
* **Gestion d'Interruption & IHM :** Bascule d'état de l'isolation (Toggle logique) via interruption externe GPIO (`EXTI`) et restitution visuelle sur écran TFT.

---

## Stack Technique
* **Cible :** STM32L152RE (Nucleo-64, ARM Cortex-M3)
* **Langage & Outils :** C (Bare-metal / Registres / HAL), STM32CubeIDE
* **Protocoles & Périphériques :** I2C Slave, Timer (Counter / Input Capture), EXTI, GPIO