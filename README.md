# Firmware Avancé pour Macropad Personnalisé v1.0.3

Ce projet contient le firmware pour un macropad polyvalent et personnalisable, 
basé sur une carte **Arduino Nano ESP32** (ou une autre carte ESP32-S3). Il est conçu pour offrir des fonctionnalités avancées telles que la gestion de couches, 
des modes pour l'encodeur et un menu de configuration intégré.

*(N'hésitez pas à remplacer ce texte par une photo de votre macropad une fois assemblé !)*

## 📂 Structure des Fichiers

Le firmware est organisé en plusieurs fichiers pour une meilleure lisibilité et une maintenance simplifiée :

* `firmware_macropad.ino` : Le fichier principal qui orchestre tous les états du macropad (menu de démarrage, mode normal, configuration, etc.).
* `config.h` : **Votre fichier de configuration.** C'est ici que vous définissez toutes les actions de vos touches (macros).
* `key-shortcut.h` : Un module qui regroupe toutes les fonctions de raccourcis clavier (`openViaRun`, `sendAltTab`, etc.).
* `icondata.h` : Contient les données brutes (bitmaps) de toutes vos icônes personnalisées.
* `debug.h` : Contient le mode de débogage via le port Série, activable à la demande.

---

## 📋 Fonctionnalités

* **Menu de Démarrage à Icônes** : Un menu graphique au démarrage permet de choisir un "profil" ou de lancer une action rapide.
* **9 touches mécaniques** programmables.
* **Gestion multi-couches (Layers)** : Multipliez vos macros en basculant entre différents ensembles de raccourcis.
* **Encodeur rotatif multifonction** avec des modes commutables :
    * **Mode Volume** : Ajuste le volume du système. Appui court pour Mute.
    * **Mode Défilement (Scroll)** : Fait défiler les pages verticalement. Appui court pour simuler un clic de molette.
    * **Mode Annuler/Rétablir** : Simule `Ctrl+Z` / `Ctrl+Y`.
* **Menu de configuration intégré** : Accessible via un appui très long, il permet de régler la luminosité de l'écran ou de revenir au menu de démarrage.
* **Économiseur d'écran (Screensaver)** : Après une période d'inactivité, une icône animée qui tourne et rebondit s'affiche.
* **Mise en veille automatique** de l'écran pour prolonger sa durée de vie.
* **Mode de débogage Série** : Permet de tester les macros et de changer les couches via l'ordinateur.
* **Écran OLED 0.91"** affichant des informations contextuelles et des animations.

---

## 🛠️ Composants et Câblage

### Matériel requis
* 1 x Carte **Arduino Nano ESP32** (ou une autre carte à base d'ESP32-S3 avec USB natif).
* 9 x Switches mécaniques (par ex. Cherry MX, Gateron, Kailh).
* 1 x Encodeur rotatif **EC11** avec bouton pressoir.
* 1 x Écran **OLED I2C 0.91"** (128x32 pixels, driver SSD1306).
* Des capuchons de touches (keycaps), un boîtier et du fil pour le câblage.

### Schéma de câblage

*(N'hésitez pas à insérer ici une image de votre schéma électrique !)*

Voici le câblage correspondant aux broches définies dans le code :

* **Touches (K1 à K9)** :
    * `D2, D3, D4, D5, D6, D7, D8, D9, D10`
* **Encodeur Rotatif** :
    * Signal A : `A1`
    * Signal B : `A0`
    * Bouton (SW) : `A2`
* **Écran OLED (I2C)** :
    * SDA : `A4`
    * SCL : `A5`

*(Chaque touche et bouton de l'encodeur doit être connecté entre sa broche et la masse (GND)).*

---

## 🚀 Installation et Personnalisation

### 1. Prérequis logiciels

* **Arduino IDE** : [Téléchargez et installez la dernière version](https://www.arduino.cc/en/software).
* **Support pour l'ESP32** : Ajoutez le support pour les cartes **"Arduino ESP32 Boards"** via le "Gestionnaire de cartes" de l'IDE.
* **Bibliothèques requises** : Installez les bibliothèques suivantes via le "Gestionnaire de bibliothèques" :
    * `Adafruit GFX Library`
    * `Adafruit SSD1306`
* Les fonctionnalités clavier/souris (`USBHIDKeyboard`, etc.) sont **incluses nativement** avec le support de la carte ESP32 et ne nécessitent pas de bibliothèque externe.

### 2. Téléversement du code

1.  Ouvrez le fichier du firmware (`.ino`) dans l'Arduino IDE.
2.  Sélectionnez la carte **"Arduino Nano ESP32"**.
3.  Connectez votre carte et sélectionnez le port COM correspondant.
4.  Cliquez sur le bouton "Téléverser".

### 3. Comment utiliser le macropad

* **Menu de démarrage** : Tournez l'encodeur pour sélectionner un profil, cliquez pour valider.
* **Changer de couche (Layer)** : Appuyez sur la **touche 9**.
* **Changer de mode d'encodeur** : Maintenez le bouton de l'encodeur enfoncé pendant **0.5 seconde** (appui long).
* **Ouvrir le menu de configuration** : Maintenez le bouton de l'encodeur enfoncé pendant **1.5 seconde** (appui très long).
    * Dans ce menu, vous pouvez choisir **"Menu Principal"** pour revenir à l'écran de sélection d'icônes.
* **Utiliser le mode débogage** : Ouvrez le **Moniteur Série** dans l'Arduino IDE (réglez-le sur **115200 bauds**) et tapez `help` pour voir la liste des commandes.

### 4. Personnalisation

* **Modifier les macros** : Ouvrez le fichier **`config.h`**.
* **Modifier les actions du menu de démarrage** : Ouvrez le fichier **`.ino`** et modifiez le `switch (selectedIconIndex)` dans la fonction `loop()`.
* **Modifier les icônes** : Ouvrez le fichier **`icondata.h`**.
* **Activer/Désactiver le mode Débogage** : Ouvrez le fichier **`debug.h`**.

    *Exemple : Assigner "Ouvrir le Bloc-notes" à la touche 1 de la couche 0.*

    ```cpp
    void executeMacroForLayer0(uint8_t id) {
      showMessage("Layer 0");
      switch (id) {
        case K1:
        // D'abord, on exécute la macro qui prend du temps
        openViaRun("notepad3.exe");
        displayCustomAction("NOTEPAD.3", icon_notpad_16x16);
        break;
        case K2: showMessage("Macro 2.0"); /* TODO */ break;
        // ...
      }
    }
    ```

* **Modifier les icônes** : Ouvrez le fichier **`icondata.h`** pour ajouter ou modifier les tableaux de bitmaps de vos icônes.


* **Activer/Désactiver le mode Débogage** : Ouvrez le fichier **`debug.h`** et commentez ou décommentez la ligne `#define DEBUG_MODE_ENABLED` pour activer ou désactiver complètement cette fonctionnalité.
