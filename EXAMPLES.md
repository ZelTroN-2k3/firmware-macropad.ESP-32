# Exemples de Configuration de Macros

Ce fichier contient des exemples de macros que vous pouvez ajouter dans `config.h`.

## 📋 Exemples de Base

### Ouvrir des Applications

```cpp
// Ouvrir Notepad
case K1:
  openViaRun("notepad.exe");
  displayCustomAction("Notepad", icon_notpad_16x16);
  break;

// Ouvrir Chrome
case K2:
  openViaRun("chrome.exe");
  showMessage("Chrome");
  break;

// Ouvrir Calculatrice
case K3:
  openViaRun("calc.exe");
  showMessage("Calculatrice");
  break;

// Ouvrir Explorateur de fichiers
case K4:
  openViaRun("explorer.exe");
  showMessage("Explorateur");
  break;
```

### Raccourcis Clavier Communs

```cpp
// Copier
case K1:
  showMessage("Copier");
  sendCombo_Ctrl('c');
  break;

// Coller
case K2:
  showMessage("Coller");
  sendCombo_Ctrl('v');
  break;

// Couper
case K3:
  showMessage("Couper");
  sendCombo_Ctrl('x');
  break;

// Annuler
case K4:
  showMessage("Annuler");
  sendCombo_Ctrl('z');
  break;

// Refaire
case K5:
  showMessage("Refaire");
  sendCombo_Ctrl('y');
  break;

// Tout sélectionner
case K6:
  showMessage("Tout selectionner");
  sendCombo_Ctrl('a');
  break;

// Rechercher
case K7:
  showMessage("Rechercher");
  sendCombo_Ctrl('f');
  break;

// Enregistrer
case K8:
  showMessage("Enregistrer");
  sendCombo_Ctrl('s');
  break;
```

### Navigation et Fenêtres

```cpp
// Alt+Tab (Changer de fenêtre)
case K1:
  showMessage("Alt+Tab");
  sendAltTab();
  break;

// Win+D (Afficher le bureau)
case K2:
  showMessage("Bureau");
  sendWinD();
  break;

// Fermer fenêtre (Alt+F4)
case K3:
  showMessage("Fermer fenetre");
  Keyboard.press(KEY_LEFT_ALT);
  Keyboard.press(KEY_F4);
  delay(50);
  Keyboard.releaseAll();
  break;

// Rouvrir onglet fermé (Ctrl+Shift+T)
case K4:
  showMessage("Re-ouvrir onglet");
  sendCombo_CtrlShift('t');
  break;

// Nouvel onglet (Ctrl+T)
case K5:
  showMessage("Nouvel onglet");
  sendCombo_Ctrl('t');
  break;

// Fermer onglet (Ctrl+W)
case K6:
  showMessage("Fermer onglet");
  sendCombo_Ctrl('w');
  break;
```

## 🎵 Contrôles Média

```cpp
// Play/Pause
case K1:
  showMessage("Play/Pause");
  Consumer.press(HID_USAGE_CONSUMER_PLAY_PAUSE);
  Consumer.release();
  break;

// Piste suivante
case K2:
  showMessage("Suivant");
  Consumer.press(HID_USAGE_CONSUMER_SCAN_NEXT);
  Consumer.release();
  break;

// Piste précédente
case K3:
  showMessage("Precedent");
  Consumer.press(HID_USAGE_CONSUMER_SCAN_PREVIOUS);
  Consumer.release();
  break;

// Mute
case K4:
  showMessage("Mute");
  Consumer.press(HID_USAGE_CONSUMER_MUTE);
  Consumer.release();
  break;

// Volume +
case K5:
  showMessage("Volume +");
  Consumer.press(HID_USAGE_CONSUMER_VOLUME_INCREMENT);
  Consumer.release();
  break;

// Volume -
case K6:
  showMessage("Volume -");
  Consumer.press(HID_USAGE_CONSUMER_VOLUME_DECREMENT);
  Consumer.release();
  break;
```

## 💻 Développement / Programmation

```cpp
// Commenter ligne (Ctrl+/)
case K1:
  showMessage("Commenter");
  Keyboard.press(KEY_LEFT_CTRL);
  Keyboard.press('/');
  delay(50);
  Keyboard.releaseAll();
  break;

// Formater code (Shift+Alt+F dans VS Code)
case K2:
  showMessage("Formater");
  Keyboard.press(KEY_LEFT_SHIFT);
  Keyboard.press(KEY_LEFT_ALT);
  Keyboard.press('f');
  delay(50);
  Keyboard.releaseAll();
  break;

// Terminal (Ctrl+`)
case K3:
  showMessage("Terminal");
  Keyboard.press(KEY_LEFT_CTRL);
  Keyboard.press('`');
  delay(50);
  Keyboard.releaseAll();
  break;

// Debug (F5)
case K4:
  showMessage("Debug");
  Keyboard.press(KEY_F5);
  delay(50);
  Keyboard.releaseAll();
  break;

// Build (Ctrl+Shift+B)
case K5:
  showMessage("Build");
  sendCombo_CtrlShift('b');
  break;
```

## 🎨 Édition Graphique / Photo

```cpp
// Zoom + (Ctrl++)
case K1:
  showMessage("Zoom +");
  Keyboard.press(KEY_LEFT_CTRL);
  Keyboard.press('+');
  delay(50);
  Keyboard.releaseAll();
  break;

// Zoom - (Ctrl+-)
case K2:
  showMessage("Zoom -");
  Keyboard.press(KEY_LEFT_CTRL);
  Keyboard.press('-');
  delay(50);
  Keyboard.releaseAll();
  break;

// Zoom 100% (Ctrl+0)
case K3:
  showMessage("Zoom 100%");
  sendCombo_Ctrl('0');
  break;
```

## 🌐 Navigation Web

```cpp
// Actualiser page (F5)
case K1:
  showMessage("Actualiser");
  Keyboard.press(KEY_F5);
  delay(50);
  Keyboard.releaseAll();
  break;

// Actualiser force (Ctrl+F5)
case K2:
  showMessage("Actualiser force");
  Keyboard.press(KEY_LEFT_CTRL);
  Keyboard.press(KEY_F5);
  delay(50);
  Keyboard.releaseAll();
  break;

// Page précédente (Alt+Gauche)
case K3:
  showMessage("Page precedente");
  Keyboard.press(KEY_LEFT_ALT);
  Keyboard.press(KEY_LEFT_ARROW);
  delay(50);
  Keyboard.releaseAll();
  break;

// Page suivante (Alt+Droite)
case K4:
  showMessage("Page suivante");
  Keyboard.press(KEY_LEFT_ALT);
  Keyboard.press(KEY_RIGHT_ARROW);
  delay(50);
  Keyboard.releaseAll();
  break;

// Nouvel onglet privé (Ctrl+Shift+N dans Chrome)
case K5:
  showMessage("Navigation privee");
  sendCombo_CtrlShift('n');
  break;
```

## 📝 Productivité

```cpp
// Capture d'écran (Win+Shift+S)
case K1:
  showMessage("Capture ecran");
  Keyboard.press(KEY_LEFT_GUI);
  Keyboard.press(KEY_LEFT_SHIFT);
  Keyboard.press('s');
  delay(50);
  Keyboard.releaseAll();
  break;

// Gestionnaire de tâches (Ctrl+Shift+Esc)
case K2:
  showMessage("Gest. taches");
  Keyboard.press(KEY_LEFT_CTRL);
  Keyboard.press(KEY_LEFT_SHIFT);
  Keyboard.press(KEY_ESC);
  delay(50);
  Keyboard.releaseAll();
  break;

// Verrouiller PC (Win+L)
case K3:
  showMessage("Verrouiller");
  Keyboard.press(KEY_LEFT_GUI);
  Keyboard.press('l');
  delay(50);
  Keyboard.releaseAll();
  break;

// Emoji (Win+.)
case K4:
  showMessage("Emoji");
  Keyboard.press(KEY_LEFT_GUI);
  Keyboard.press('.');
  delay(50);
  Keyboard.releaseAll();
  break;
```

## 🎮 Gaming

```cpp
// Quicksave (F5 dans beaucoup de jeux)
case K1:
  showMessage("Quicksave");
  Keyboard.press(KEY_F5);
  delay(50);
  Keyboard.releaseAll();
  break;

// Quickload (F9 dans beaucoup de jeux)
case K2:
  showMessage("Quickload");
  Keyboard.press(KEY_F9);
  delay(50);
  Keyboard.releaseAll();
  break;

// Screenshot (F12 dans Steam)
case K3:
  showMessage("Screenshot");
  Keyboard.press(KEY_F12);
  delay(50);
  Keyboard.releaseAll();
  break;
```

## 🔤 Texte et Saisie

```cpp
// Taper du texte prédéfini
case K1:
  showMessage("Email");
  Keyboard.print("votre.email@example.com");
  break;

// Taper URL
case K2:
  showMessage("URL");
  Keyboard.print("https://www.exemple.com");
  break;

// Signature
case K3:
  showMessage("Signature");
  Keyboard.print("Cordialement,\nVotre Nom");
  break;
```

## 💡 Conseils

1. **Délais** : Ajoutez `delay(50)` ou `delay(100)` entre les actions pour assurer la fiabilité
2. **Messages** : Utilisez `showMessage()` pour un feedback visuel
3. **Icônes** : Utilisez `displayCustomAction()` pour afficher une icône avec le texte
4. **Tests** : Testez vos macros une par une pour vérifier qu'elles fonctionnent

## 🔗 Ressources

- **Codes des touches spéciales** : Consultez la documentation Arduino Keyboard
- **HID Consumer codes** : Pour les contrôles média
- **Modifier keys** : KEY_LEFT_CTRL, KEY_LEFT_SHIFT, KEY_LEFT_ALT, KEY_LEFT_GUI

## 📌 Structure Complète d'une Macro Layer

```cpp
void executeMacroForLayer0(uint8_t id) {
  showMessage("Layer 0");
  switch (id) {
    case K1: /* Votre action */ break;
    case K2: /* Votre action */ break;
    case K3: /* Votre action */ break;
    case K4: /* Votre action */ break;
    case K5: /* Votre action */ break;
    case K6: /* Votre action */ break;
    case K7: /* Votre action */ break;
    case K8: /* Votre action */ break;
  }
}
```

N'oubliez pas le `break;` à la fin de chaque case !
