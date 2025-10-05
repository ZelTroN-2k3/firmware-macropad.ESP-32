# 📦 Contenu du Repository

Ce repository contient le firmware complet pour un macropad ESP32 avec écran OLED.

## 📁 Fichiers du Projet

### Firmware (Code Source)

| Fichier | Lignes | Description |
|---------|--------|-------------|
| `firmware_macropad.esp-32.0.3.ino` | 549 | **Fichier principal** - Point d'entrée, setup(), loop() |
| `config.h` | 95 | **Configuration des macros** - Personnaliser les touches |
| `iconmenu.h` | 140 | **Menu d'icônes** - Interface utilisateur principale |
| `icondata.h` | 149 | **Données graphiques** - Bitmaps des icônes |
| `key-shortcut.h` | 76 | **Raccourcis clavier** - Fonctions utilitaires |

**Total code source : 1009 lignes**

### Documentation

| Fichier | Lignes | Description |
|---------|--------|-------------|
| `README.md` | 84 | **Introduction** - Vue d'ensemble du projet |
| `STRUCTURE.md` | 197 | **Structure détaillée** - Guide complet du firmware |
| `EXAMPLES.md` | 418 | **Exemples de macros** - Configurations prêtes à l'emploi |
| `ARCHITECTURE.md` | 256 | **Architecture** - Diagrammes et flux d'exécution |
| `TROUBLESHOOTING.md` | 392 | **Dépannage** - Solutions aux problèmes courants |

**Total documentation : 1347 lignes**

### Autres

| Fichier | Description |
|---------|-------------|
| `LICENSE` | Licence GPL-3.0 |
| `.git/` | Historique Git |

## 🎯 Guide de Navigation

### Pour Démarrer
1. **README.md** - Commencez ici pour une vue d'ensemble
2. **STRUCTURE.md** - Comprenez la structure du projet
3. **EXAMPLES.md** - Trouvez des exemples de macros

### Pour Développer
1. **config.h** - Modifiez vos macros ici
2. **ARCHITECTURE.md** - Comprenez le fonctionnement interne
3. **firmware_macropad.esp-32.0.3.ino** - Code principal

### En Cas de Problème
1. **TROUBLESHOOTING.md** - Guide de dépannage complet
2. **ARCHITECTURE.md** - Flux d'exécution pour debug

## 🔧 Composants du Firmware

### Matériel Supporté
- ✅ Arduino Nano ESP32
- ✅ Écran OLED I2C SSD1306 (128x32)
- ✅ Encodeur rotatif EC11
- ✅ 9 touches mécaniques

### Fonctionnalités Principales
- ✅ 27 macros programmables (3 layers × 9 touches)
- ✅ Contrôle volume/scroll/undo via encodeur
- ✅ Menu d'icônes interactif
- ✅ Screensaver animé
- ✅ Menu de configuration
- ✅ USB HID (Clavier + Souris + Contrôles média)

### États du Système
- `STATE_NORMAL` - Mode de fonctionnement standard
- `STATE_ICON_MENU` - Menu principal avec icônes
- `STATE_SCREENSAVER` - Économiseur d'écran

### Modes de l'Encodeur
- `MODE_VOLUME` - Contrôle du volume
- `MODE_SCROLL` - Défilement souris
- `MODE_UNDO_REDO` - Annuler/Refaire

## 📊 Statistiques du Projet

```
Total Lignes de Code    : 1,009
Total Documentation     : 1,347
Total Fichiers Source   : 5
Total Fichiers Docs     : 5
Taille Repository       : ~112 KB
```

## 🚀 Installation Rapide

```bash
# 1. Cloner le repository
git clone https://github.com/ZelTroN-2k3/firmware-macropad.ESP-32.git

# 2. Ouvrir dans Arduino IDE
# Ouvrir firmware_macropad.esp-32.0.3.ino

# 3. Installer les bibliothèques requises
# - Adafruit GFX Library
# - Adafruit SSD1306

# 4. Sélectionner la carte
# Arduino Nano ESP32

# 5. Téléverser
# Cliquer sur "Téléverser"
```

## 🎨 Personnalisation

### Modifier les Macros
Éditez `config.h` :
```cpp
void executeMacroForLayer0(uint8_t id) {
  showMessage("Layer 0");
  switch (id) {
    case K1: /* Votre action */ break;
    // ...
  }
}
```

### Ajouter des Icônes
Éditez `icondata.h` :
```cpp
const unsigned char mon_icone[] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, /* ... */
};
```

### Changer le Brochage
Éditez les constantes dans le fichier principal :
```cpp
const uint8_t ENC_A = A1;
const uint8_t KEY_PINS[] = { D2, D3, ... };
```

## 📚 Bibliothèques Requises

| Bibliothèque | Version | Usage |
|--------------|---------|-------|
| Adafruit GFX | Dernière | Graphiques |
| Adafruit SSD1306 | Dernière | Driver écran OLED |
| Wire | Arduino natif | Communication I2C |
| USB* | ESP32 natif | Support HID |

*Inclus avec le package ESP32

## 🔗 Dépendances

```
firmware_macropad.esp-32.0.3.ino
├── USB.h (ESP32)
├── USBHIDMouse.h (ESP32)
├── USBHIDKeyboard.h (ESP32)
├── USBHIDConsumerControl.h (ESP32)
├── Wire.h (Arduino)
├── Adafruit_GFX.h
├── Adafruit_SSD1306.h
├── icondata.h (local)
├── key-shortcut.h (local)
├── config.h (local)
└── iconmenu.h (local)
```

## 🏗️ Architecture en Couches

```
┌─────────────────────────────────┐
│   Interface Utilisateur         │
│   (Écran OLED + Menu)           │
├─────────────────────────────────┤
│   Logique Applicative           │
│   (États, Layers, Macros)       │
├─────────────────────────────────┤
│   Abstraction Matérielle        │
│   (USB HID, I2C)                │
├─────────────────────────────────┤
│   Matériel                      │
│   (ESP32, OLED, Encodeur)       │
└─────────────────────────────────┘
```

## 🧪 Tests et Validation

### Checklist de Vérification
- [x] Code compile sans erreur
- [x] Toutes les dépendances documentées
- [x] Exemples de configuration fournis
- [x] Guide de dépannage complet
- [x] Architecture documentée
- [x] Brochage défini
- [x] Fonctionnalités testables

### Tests Recommandés
1. **Test Écran** - Vérifier affichage bootscreen
2. **Test Touches** - Appuyer sur chaque touche
3. **Test Encodeur** - Rotation et clic
4. **Test Layers** - Changer avec K9
5. **Test Macros** - Exécuter chaque macro
6. **Test Menu** - Navigation et configuration
7. **Test Screensaver** - Attendre 30s

## 📝 Historique des Versions

### v0.3 (Actuel)
- ✅ Firmware complet pour ESP32
- ✅ 3 layers de macros
- ✅ Menu d'icônes
- ✅ Screensaver animé
- ✅ Menu de configuration
- ✅ Documentation complète

## 🤝 Contribution

Pour contribuer :
1. Fork le repository
2. Créez une branche feature
3. Committez vos changements
4. Pushez vers la branche
5. Ouvrez une Pull Request

## 📄 Licence

GPL-3.0 License - Voir [LICENSE](LICENSE)

## 👤 Auteur

**ANCHER.P**
- Date : 04/10/2025
- Version : 0.3

## 🎯 Prochaines Étapes

### Pour l'Utilisateur
1. ✅ Télécharger le firmware
2. ✅ Lire README.md
3. ✅ Suivre STRUCTURE.md pour l'installation
4. ✅ Personnaliser config.h
5. ✅ Téléverser et tester

### Pour le Développeur
1. ✅ Comprendre ARCHITECTURE.md
2. ✅ Étudier les exemples dans EXAMPLES.md
3. ✅ Modifier le code source
4. ✅ Tester avec TROUBLESHOOTING.md
5. ✅ Contribuer au projet

## 🌟 Points Forts du Projet

- 📖 **Documentation complète** - Plus de 1300 lignes
- 🎨 **Personnalisable** - config.h facile à modifier
- 🛠️ **Bien structuré** - Code modulaire et commenté
- 🚀 **Prêt à l'emploi** - Exemples inclus
- 🔧 **Maintenable** - Architecture claire
- 💡 **Pédagogique** - Parfait pour apprendre

---

**Total du projet : 2356 lignes de code et documentation**

Pour toute question, consultez TROUBLESHOOTING.md ou créez une issue sur GitHub.
