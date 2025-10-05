# Architecture du Firmware

```
firmware_macropad.esp-32.0.3.ino (Main)
│
├── Bibliothèques Arduino/ESP32
│   ├── USB.h
│   ├── USBHIDMouse.h
│   ├── USBHIDKeyboard.h
│   ├── USBHIDConsumerControl.h
│   ├── Arduino.h
│   ├── Wire.h
│   ├── Adafruit_GFX.h
│   └── Adafruit_SSD1306.h
│
├── icondata.h
│   ├── bootscreen_128x32[]
│   ├── screensaver_frame_0/1/2/3[]
│   ├── icon_menu_*[]
│   ├── icon_layer_*[]
│   └── icon_*[]
│
├── key-shortcut.h
│   ├── openViaRun()
│   ├── sendAltTab()
│   ├── sendWinD()
│   ├── sendCombo_Ctrl()
│   └── sendCombo_CtrlShift()
│
├── config.h
│   ├── executeMacroForLayer0()
│   ├── executeMacroForLayer1()
│   ├── executeMacroForLayer2()
│   └── displayCustomAction()
│
└── iconmenu.h
    ├── drawIconMenu()
    ├── handleIconMenu()
    └── displayCustomScreen()
```

## Flux d'Exécution

### 1. Initialisation (setup())
```
setup()
  ├── Initialiser USB (Mouse, Keyboard, Consumer)
  ├── Configurer les pins (touches, encodeur)
  ├── Initialiser l'écran OLED
  ├── Afficher bootscreen
  ├── Configurer luminosité
  └── Afficher menu d'icônes
```

### 2. Boucle Principale (loop())
```
loop()
  ├── Vérifier timeout screensaver
  │
  ├── [STATE_NORMAL]
  │   ├── handleNormalMode()
  │   │   ├── Si dans menu → Gérer menu config
  │   │   └── Sinon → Gérer touches et encodeur
  │   └── Détection appui long → Menu config
  │
  ├── [STATE_ICON_MENU]
  │   ├── handleIconMenu()
  │   │   ├── Lire touches → fireMacro()
  │   │   └── Lire encodeur → Navigation menu
  │   └── Affichage icônes + layer
  │
  └── [STATE_SCREENSAVER]
      ├── Détecter activité → wakeUp()
      └── Animer screensaver
```

### 3. Gestion des Macros
```
Touche pressée → fireMacro(id)
  ├── Si K9 → Changer de layer
  └── Selon layer actuel:
      ├── executeMacroForLayer0(id)
      ├── executeMacroForLayer1(id)
      └── executeMacroForLayer2(id)
          └── switch(id)
              ├── case K1: action
              ├── case K2: action
              └── ...
```

## États du Système

```
┌─────────────────┐
│  STATE_NORMAL   │ ←──┐
│                 │    │
│ - Touches actives│    │ Appui encodeur
│ - Encodeur actif │    │ (très long)
│ - Menu config   │    │
└─────────────────┘    │
         │             │
         │ Timeout     │
         ↓             │
┌─────────────────┐    │
│STATE_ICON_MENU  │ ───┘
│                 │
│ - Menu icônes   │
│ - Sélection     │
│ - Navigation    │
└─────────────────┘
         │
         │ 30s inactivité
         ↓
┌─────────────────┐
│STATE_SCREENSAVER│
│                 │
│ - Animation     │
│ - Rebond        │
│ - Rotation      │
└─────────────────┘
         │
         │ Activité détectée
         ↓
    (retour menu)
```

## Modes de l'Encodeur

```
┌──────────────┐   Appui long   ┌──────────────┐   Appui long   ┌──────────────┐
│ MODE_VOLUME  │ ─────────────→ │ MODE_SCROLL  │ ─────────────→ │MODE_UNDO_REDO│
│              │                │              │                │              │
│ Rotation: ±  │                │ Rotation:    │                │ Rotation:    │
│   Volume     │                │   Scroll     │                │   Undo/Redo  │
│              │                │              │                │              │
│ Clic: Mute   │                │ Clic: Middle │                │ Clic: (rien) │
└──────────────┘                └──────────────┘                └──────────────┘
       ↑                                                                  │
       └──────────────────────────────────────────────────────────────────┘
                              Appui long (cycle)
```

## Système de Layers

```
Layer 0 (Défaut)          Layer 1 (Édition)         Layer 2 (Média)
┌──────────────┐          ┌──────────────┐          ┌──────────────┐
│ K1: Notepad  │          │ K1: Copier   │          │ K1: Play/Pause│
│ K2: CMD      │          │ K2: Coller   │          │ K2: Suivant  │
│ K3: (custom) │          │ K3: Couper   │          │ K3: Précédent│
│ K4: (custom) │          │ K4: Annuler  │          │ K4: Mute     │
│ K5: (custom) │          │ K5: Re-open  │          │ K5: Vol +    │
│ K6: (custom) │          │ K6: (custom) │          │ K6: Vol -    │
│ K7: (custom) │          │ K7: (custom) │          │ K7: (custom) │
│ K8: Alt+Tab  │          │ K8: Win+D    │          │ K8: (custom) │
│ K9: → Layer 1│          │ K9: → Layer 2│          │ K9: → Layer 0│
└──────────────┘          └──────────────┘          └──────────────┘
```

## Menu de Configuration

```
Menu Config (Appui très long sur encodeur)
┌────────────────────────────┐
│ > Luminosite              │ ← Sélectionné
│   Menu Principal          │
│   Sortir                  │
└────────────────────────────┘
         ↓ (sélection)
┌────────────────────────────┐
│ Tournez pour regler       │
│ Luminosite: 255           │
│ ▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓   │
└────────────────────────────┘
```

## Communication I2C

```
ESP32 ←─→ Écran OLED (SSD1306)
        │  Adresse: 0x3C
        │  Protocole: I2C
        │  Résolution: 128x32
        │
        └─ Wire (bibliothèque Arduino)
           ├── SDA (pin défini par ESP32)
           └── SCL (pin défini par ESP32)
```

## Communication USB HID

```
ESP32 ──→ Ordinateur
         │
         ├── USBHIDKeyboard
         │   └── Envoie touches et combinaisons
         │
         ├── USBHIDMouse
         │   └── Contrôle souris et scroll
         │
         └── USBHIDConsumerControl
             └── Contrôles média (volume, play/pause, etc.)
```

## Gestion de l'Alimentation et du Screensaver

```
Activité détectée
    ↓
lastActionTime = millis()
    ↓
loop() vérifie:
if (millis() - lastActionTime > 30000)
    ↓
STATE_SCREENSAVER activé
    ↓
drawScreensaver() animé
    ↓
Activité détectée → wakeUp()
    ↓
Retour à STATE_ICON_MENU
```

## Priorités des Événements

```
1. Menu Configuration (si actif)
   └── Gère encodeur et bouton indépendamment

2. Screensaver (si actif)
   └── Détecte activité pour sortir

3. Mode Normal / Icon Menu
   ├── Lecture touches (polling)
   ├── Lecture encodeur (transition table)
   └── Détection appui long encodeur
```

## Mécanisme Anti-rebond

```
Touches:
- État précédent stocké dans lastKeyState[]
- Comparaison avec état actuel
- Action seulement sur transition HIGH→LOW

Encodeur (bouton):
- lastClickMs stocke dernier clic
- Délai minimum: CLICK_DEBOUNCE_MS (300ms)
- Vérifie: (millis() - lastClickMs > 300)

Encodeur (rotation):
- Transition table pour décoder quadrature
- Accumulation des pas
- Action tous les STEPS_PER_DETENT (4)
```
