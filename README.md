# Firmware Avancé pour Macropad Personnalisé v1.0.4

Ce projet contient le firmware pour un macropad polyvalent, personnalisable et **entièrement configurable via une interface web moderne**. 
Il est basé sur une carte **Arduino Nano ESP32** et offre des fonctionnalités avancées pour une productivité accrue.
des modes pour l'encodeur et un menu de configuration intégré.

![GitHub Issues or Pull Requests](https://github.com/ZelTroN-2k3/firmware-macropad.ESP-32/blob/main/Images%20macropad-ESP32/image-01.jpg)
---
![GitHub Issues or Pull Requests](https://github.com/ZelTroN-2k3/firmware-macropad.ESP-32/blob/main/Images%20macropad-ESP32/image-02.jpg)

## 📂 Structure des Fichiers

* `firmware_macropad.ino` : Le cœur du projet, gérant la logique principale, les états et le serveur web.
* `webpage.h` : Contient le code de l'interface web (HTML/CSS/JS) basée sur le framework **Bootstrap 5**.
* `key-shortcut.h` : Regroupe les fonctions de bas niveau pour simuler des raccourcis clavier.
* `icondata.h` : Bibliothèque contenant les données de toutes les icônes utilisées par l'écran OLED.
* `iconmenu.h` : Gère l'affichage et la logique du menu de démarrage.
* `debug.h` : Module optionnel pour le débogage via le port Série.
* `base64.h` : Utilitaire pour l'encodage des icônes pour la fenêtre d'aide.
* `config.json` (sur l'appareil) : Fichier créé sur la mémoire interne de l'ESP32 qui stocke toutes vos macros.

---

## 📋 Fonctionnalités

* **Interface Web Bootstrap 5** : Une interface web moderne et responsive pour une configuration intuitive.
    * **Thèmes Clair/Sombre** : Un interrupteur pour basculer entre un thème clair et sombre, avec mémorisation du choix.
    * **Aide Intégrée** : Une fenêtre d'aide affiche la liste de toutes les icônes disponibles avec leur aperçu visuel.
    * **Affichage par Onglets** : La configuration des 3 *layers* est organisée en onglets pour plus de clarté.
    * **Footer Informatif** : Affiche en temps réel l'adresse IP, la version du firmware, l'auteur, et l'heure/date actuelles.
* **Stockage sur Mémoire Interne (SPIFFS)** : Les macros sont sauvegardées sur la mémoire de l'ESP32.
* **Mise à Jour en Temps Réel** : Les modifications sont appliquées instantanément sans recompiler.
* **Écran d'Information au Démarrage** : Affiche l'adresse IP, la date et l'heure pendant 5 secondes.
* **Icônes de Statut** : Le menu principal affiche l'état de la connexion Wi-Fi et du fichier de configuration.
* **9 touches mécaniques** programmables sur **3 couches (Layers)**.
* **Encodeur rotatif multifonction** (Volume, Défilement, Annuler/Rétablir).
* **Économiseur d'écran "Champ d'Étoiles"**.
* **Mise en veille automatique** de l'écran.

---

## 🛠️ Composants et Câblage

### Matériel requis
* 1 x Carte **Arduino Nano ESP32**.
* 9 x Switches mécaniques.
* 1 x Encodeur rotatif **EC11**.
* 1 x Écran **OLED I2C 0.91"**.
* ...

### Schéma de câblage
* **Touches (K1 à K9)** : `D2` à `D10`
* **Encodeur** : `A1` (A), `A0` (B), `A2` (SW)
* **Écran OLED** : `A4` (SDA), `A5` (SCL)

---

## 🚀 Installation et Personnalisation

### 1. Prérequis logiciels

* **Arduino IDE**.
* **Support pour les cartes ESP32**.
* **Bibliothèques requises** (à installer via le Gestionnaire de bibliothèques) :
    * `Adafruit GFX Library`
    * `Adafruit SSD1306`
    * `ArduinoJson` (par Benoit Blanchon)
    * `AsyncTCP`
    * `ESPAsyncWebServer`

### 2. Configuration Initiale (Première Utilisation)

1.  **Entrez vos identifiants Wi-Fi** : Dans le fichier `firmware_macropad.ino`, modifiez ces lignes avec les informations de votre réseau :
    ```cpp
    const char* ssid = "VOTRE_SSID_WIFI";
    const char* password = "VOTRE_MOT_DE_PASSE_WIFI";
    ```
2.  **Sélectionnez le Schéma de Partition** : Dans l'Arduino IDE, allez dans `Outils > Partition Scheme` et choisissez une option qui inclut un système de fichiers, par exemple **"... with SPIFFS"**.

### 3. Téléversement du code

1.  Ouvrez le fichier du firmware (`.ino`) dans l'Arduino IDE.
2.  Sélectionnez la carte **"Arduino Nano ESP32"** et le bon port COM.
3.  Cliquez sur "Téléverser".

### 4. Personnalisation via l'Interface Web

La personnalisation se fait entièrement via la page web hébergée sur le macropad.

1.  **Trouvez l'Adresse IP** : Au démarrage, l'IP s'affiche sur l'écran OLED et dans le Moniteur Série.
2.  **Accédez à l'Interface** : Ouvrez un navigateur web (Chrome, Firefox, etc.) et entrez cette IP.
    * **Note :** L'ordinateur doit être connecté à Internet pour charger le style Bootstrap.
3.  **Configurez vos Macros** :
    * **Commande** : L'action à exécuter (voir syntaxe ci-dessous).
    * **Libellé affiché** : Le texte qui s'affichera sur l'OLED lors de l'action.
    * **Nom de l'icône** : Le nom de la variable de l'icône à afficher (cliquez sur l'icône `?` pour voir la liste complète).

#### Syntaxe des Macros (avec préfixes)

Utilisez un préfixe dans le champ "Commande" pour définir le type d'action :

* `run:<programme>` : Lance un programme. Ex: `run:calc.exe`
* `key:<touche>` : Simule `Ctrl + <touche>`. Ex: `key:c` (pour Copier)
* `keyshift:<touche>` : Simule `Ctrl + Shift + <touche>`. Ex: `keyshift:t`
* `media:<action>` : Contrôle Média. Actions : `play`, `next`, `prev`. Ex: `media:play`
* `text:<votre texte>` : Écrit une chaîne de caractères. Ex: `text:Bonjour !`

Si aucun préfixe n'est utilisé, `run:` est appliqué par défaut.

* **Modifier les icônes** : Ouvrez le fichier **`icondata.h`** pour ajouter ou modifier les tableaux de bitmaps de vos icônes.

* **Activer/Désactiver le mode Débogage** : Ouvrez le fichier **`debug.h`** et commentez ou décommentez la ligne `#define DEBUG_MODE_ENABLED` pour activer ou désactiver complètement cette fonctionnalité.
