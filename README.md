# firmware-macropad.ESP-32

Firmware pour un macropad basé sur une carte **Arduino Nano ESP32**. 

## 🎯 Caractéristiques

- ⌨️ **9 touches programmables** avec système de 3 layers (27 macros au total)
- 🔊 **Encodeur rotatif** pour le contrôle du volume et plus
- 📺 **Écran OLED** 0.91" (128x32) avec menu d'icônes
- ⚙️ **Menu de configuration** accessible
- 🌙 **Screensaver animé** avec effet de rebond
- 🚀 **Menu de démarrage** avec icônes

## 📋 Matériel Requis

- 1x Arduino Nano ESP32
- 1x Écran OLED I2C 0.91" (128x32, SSD1306)
- 1x Encodeur rotatif EC11
- 9x Switches mécaniques (Cherry MX ou similaire)

## 🚀 Démarrage Rapide

1. **Installer les bibliothèques** (via le gestionnaire Arduino) :
   - Adafruit GFX Library
   - Adafruit SSD1306
   - Wire (inclus)
   - USB (ESP32 natif)

2. **Ouvrir le firmware** :
   - Ouvrez `firmware_macropad.esp-32.0.3.ino` dans l'IDE Arduino

3. **Configurer la carte** :
   - Sélectionnez **Arduino Nano ESP32**
   - Sélectionnez le port COM approprié

4. **Téléverser** :
   - Cliquez sur le bouton Téléverser

5. **Personnaliser** :
   - Modifiez `config.h` pour personnaliser vos macros

## 📚 Documentation

- 📖 **[STRUCTURE.md](STRUCTURE.md)** - Documentation complète du firmware
- 🔧 **[config.h](config.h)** - Configuration des macros (à personnaliser)

## 🎮 Utilisation

### Touches
- **K1-K8** : Macros programmables
- **K9** : Change de layer (couche)

### Encodeur Rotatif
- **Rotation** : Contrôle volume / Scroll / Undo-Redo
- **Clic court** : Mute / Action selon mode
- **Clic long** : Change de mode
- **Clic très long (1.5s)** : Menu de configuration

## 🔧 Brochage

| Composant | Pin ESP32 |
|-----------|-----------|
| Encodeur A | A1 |
| Encodeur B | A0 |
| Encodeur SW | A2 |
| Touche 1 | D2 |
| Touche 2 | D3 |
| Touche 3 | D4 |
| Touche 4 | D5 |
| Touche 5 | D6 |
| Touche 6 | D7 |
| Touche 7 | D8 |
| Touche 8 | D9 |
| Touche 9 | D10 |
| OLED SDA | SDA |
| OLED SCL | SCL |

## 📝 Licence

GPL-3.0 - Voir [LICENSE](LICENSE) pour plus de détails.

## 👤 Auteur

ANCHER.P - 04/10/2025
