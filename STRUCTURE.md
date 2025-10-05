# Structure du Firmware Macropad ESP32

Ce document explique la structure du firmware et comment personnaliser votre macropad.

## 📁 Structure des Fichiers

### Fichiers Principaux

1. **firmware_macropad.esp-32.0.3.ino** - Fichier principal Arduino
   - Initialisation du matériel (écran OLED, encodeur, touches)
   - Boucle principale et gestion des états
   - Gestion du screensaver
   - Menu de configuration

2. **config.h** - Configuration des macros
   - **C'EST ICI QUE VOUS PERSONNALISEZ LES ACTIONS DES TOUCHES**
   - Trois fonctions pour les 3 layers (couches) :
     - `executeMacroForLayer0()` - Layer 0 (défaut)
     - `executeMacroForLayer1()` - Layer 1
     - `executeMacroForLayer2()` - Layer 2

3. **key-shortcut.h** - Fonctions utilitaires pour les raccourcis clavier
   - `openViaRun()` - Ouvre une application via Win+R
   - `sendAltTab()` - Envoie Alt+Tab
   - `sendWinD()` - Affiche le bureau (Win+D)
   - `sendCombo_Ctrl()` - Envoie Ctrl+touche
   - `sendCombo_CtrlShift()` - Envoie Ctrl+Shift+touche

4. **icondata.h** - Données des icônes (bitmaps)
   - Bootscreen 128x32
   - Icônes du menu 16x16
   - Icônes des layers 16x16
   - Frames du screensaver animé

5. **iconmenu.h** - Module du menu d'icônes
   - Gestion du menu principal avec icônes
   - Navigation avec l'encodeur rotatif

## 🔧 Matériel Requis

- **Microcontrôleur** : Arduino Nano ESP32
- **Écran** : OLED I2C 0.91" (128x32, driver SSD1306)
- **Encodeur** : Encodeur rotatif EC11
- **Touches** : 9x switches mécaniques (Cherry MX ou similaire)

## 📌 Brochage (Pins)

### Encodeur Rotatif
- `A1` - Signal A (ENC_A)
- `A0` - Signal B (ENC_B)
- `A2` - Bouton (ENC_SW)

### 9 Touches Programmables
- `D2` - Touche 1 (K1)
- `D3` - Touche 2 (K2)
- `D4` - Touche 3 (K3)
- `D5` - Touche 4 (K4)
- `D6` - Touche 5 (K5)
- `D7` - Touche 6 (K6)
- `D8` - Touche 7 (K7)
- `D9` - Touche 8 (K8)
- `D10` - Touche 9 (K9)

### Écran OLED (I2C)
- `SDA` - Données I2C
- `SCL` - Horloge I2C
- Adresse : `0x3C`

## 🎮 Fonctionnalités

### Système de Layers (Couches)
- **3 couches** disponibles (0, 1, 2) = **27 macros programmables**
- La **touche K9** change de couche
- Chaque couche peut avoir des actions différentes pour les touches K1-K8

### Modes de l'Encodeur
1. **MODE_VOLUME** - Contrôle du volume système
2. **MODE_SCROLL** - Défilement de la souris
3. **MODE_UNDO_REDO** - Annuler/Refaire

### Actions de l'Encodeur
- **Rotation** : Action selon le mode actif
- **Clic court** : Action spécifique au mode (ex: Mute en mode volume)
- **Clic long (500ms)** : Change de mode
- **Clic très long (1.5s)** : Ouvre le menu de configuration

### Menu de Configuration
Accessible par appui très long sur l'encodeur :
- **Luminosité** - Réglage de la luminosité de l'écran
- **Menu Principal** - Retour au menu d'icônes
- **Sortir** - Sortie du menu

### Screensaver
- S'active après **30 secondes** d'inactivité
- Animation de rebond avec rotation
- Se désactive au moindre mouvement/touche

## ✏️ Comment Personnaliser les Macros

### Éditer config.h

Ouvrez `config.h` et modifiez les fonctions pour chaque layer :

```cpp
void executeMacroForLayer0(uint8_t id) {
  showMessage("Layer 0");
  switch (id) {
    case K1:
      openViaRun("notepad.exe");
      displayCustomAction("Notepad", icon_notpad_16x16);
      break;
    
    case K2:
      showMessage("Copier");
      sendCombo_Ctrl('c');
      break;
    
    // ... autres touches
  }
}
```

### Fonctions Utiles

#### Affichage
- `showMessage("Texte")` - Affiche un message
- `displayCustomAction("Label", icon)` - Affiche un label avec une icône

#### Raccourcis Clavier
- `sendCombo_Ctrl('c')` - Ctrl+C
- `sendCombo_CtrlShift('t')` - Ctrl+Shift+T
- `sendAltTab()` - Alt+Tab
- `sendWinD()` - Win+D (Bureau)

#### Lancement d'Applications
- `openViaRun("notepad.exe")` - Lance via Win+R

#### Contrôles Média
```cpp
Consumer.press(HID_USAGE_CONSUMER_PLAY_PAUSE);
Consumer.release();
```

Options disponibles :
- `HID_USAGE_CONSUMER_PLAY_PAUSE`
- `HID_USAGE_CONSUMER_SCAN_NEXT`
- `HID_USAGE_CONSUMER_SCAN_PREVIOUS`
- `HID_USAGE_CONSUMER_VOLUME_INCREMENT`
- `HID_USAGE_CONSUMER_VOLUME_DECREMENT`
- `HID_USAGE_CONSUMER_MUTE`

## 📚 Bibliothèques Requises

Installez ces bibliothèques via le gestionnaire de bibliothèques Arduino :

1. **Adafruit GFX Library** - Graphiques pour l'écran
2. **Adafruit SSD1306** - Driver pour écran OLED
3. **USB** (ESP32 natif) - Support USB HID
4. **Wire** (Arduino natif) - Communication I2C

## 🚀 Compilation et Upload

1. Ouvrez `firmware_macropad.esp-32.0.3.ino` dans l'IDE Arduino
2. Sélectionnez la carte : **Arduino Nano ESP32**
3. Sélectionnez le port COM approprié
4. Cliquez sur **Téléverser**

## 🔍 Dépannage

### L'écran ne s'allume pas
- Vérifiez l'adresse I2C (0x3C ou 0x3D)
- Vérifiez les connexions SDA/SCL

### Les touches ne fonctionnent pas
- Vérifiez que les pins sont bien en INPUT_PULLUP
- Vérifiez le câblage (touches connectées à GND quand pressées)

### L'encodeur ne répond pas
- Vérifiez le câblage des pins A, B et SW
- Vérifiez que les pins sont en INPUT_PULLUP

## 📝 Notes

- Les touches doivent être connectées entre le pin et GND (logique inversée)
- L'écran OLED doit être alimenté en 3.3V ou 5V selon le modèle
- Le screensaver se déclenche après 30 secondes d'inactivité

## 🎨 Personnalisation des Icônes

Pour créer vos propres icônes 16x16 :
1. Utilisez un éditeur bitmap (GIMP, Photoshop, etc.)
2. Convertissez en tableau C avec un outil comme [LCD Assistant](http://en.radzio.dxp.pl/bitmap_converter/)
3. Ajoutez le tableau dans `icondata.h`

## 📄 Licence

Ce firmware est distribué sous licence GPL-3.0. Voir le fichier LICENSE pour plus de détails.
