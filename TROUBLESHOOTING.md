# Guide de Dépannage

Ce guide vous aide à résoudre les problèmes courants avec le firmware macropad ESP32.

## 🔍 Diagnostic Rapide

### L'écran OLED ne s'allume pas

**Symptôme** : L'écran reste noir, rien ne s'affiche.

**Solutions possibles** :

1. **Vérifier l'adresse I2C**
   ```cpp
   // Dans firmware_macropad.esp-32.0.3.ino, ligne ~62
   const uint8_t OLED_ADDR = 0x3C;  // Essayez 0x3D si 0x3C ne fonctionne pas
   ```

2. **Scanner l'adresse I2C**
   - Utilisez un scanner I2C (exemple Arduino)
   - Les adresses courantes : 0x3C ou 0x3D

3. **Vérifier les connexions**
   - SDA → Pin SDA de l'ESP32
   - SCL → Pin SCL de l'ESP32
   - VCC → 3.3V ou 5V (selon écran)
   - GND → GND

4. **Vérifier l'alimentation**
   - Certains écrans nécessitent 5V
   - D'autres fonctionnent en 3.3V
   - Consultez la fiche technique de votre écran

### Les touches ne répondent pas

**Symptôme** : Appuyer sur les touches ne fait rien.

**Solutions possibles** :

1. **Vérifier le câblage**
   - Chaque touche doit connecter le pin à GND quand pressée
   - Utilisez des switches normalement ouverts (NO)

2. **Vérifier les pins**
   ```cpp
   // Dans firmware_macropad.esp-32.0.3.ino, ligne ~74
   const uint8_t KEY_PINS[] = { D2, D3, D4, D5, D6, D7, D8, D9, D10 };
   ```
   - Assurez-vous que ces pins correspondent à votre câblage

3. **Tester avec le moniteur série**
   ```cpp
   // Ajoutez dans handleNormalMode() pour debug:
   for (uint8_t i = 0; i < NUM_KEYS; i++) {
     Serial.print("K"); Serial.print(i); Serial.print(": ");
     Serial.println(digitalRead(KEY_PINS[i]));
   }
   ```

4. **Vérifier les pull-up**
   - Les pins sont configurés en INPUT_PULLUP
   - État repos : HIGH
   - État pressé : LOW

### L'encodeur rotatif ne fonctionne pas

**Symptôme** : Tourner l'encodeur ne change rien.

**Solutions possibles** :

1. **Vérifier le câblage**
   - Pin A de l'encodeur → A1 (ENC_A)
   - Pin B de l'encodeur → A0 (ENC_B)
   - Pin commun → GND
   - Pin SW (bouton) → A2 (ENC_SW)

2. **Inverser A et B si le sens est inversé**
   ```cpp
   const uint8_t ENC_A = A0;  // Inverser avec ENC_B
   const uint8_t ENC_B = A1;
   ```

3. **Ajuster la sensibilité**
   ```cpp
   const uint8_t STEPS_PER_DETENT = 4;  // Essayez 2 ou 1 si trop lent
   ```

4. **Tester l'encodeur**
   ```cpp
   // Dans loop(), ajoutez pour debug:
   Serial.print("A: "); Serial.print(digitalRead(ENC_A));
   Serial.print(" B: "); Serial.println(digitalRead(ENC_B));
   ```

### Le bouton de l'encodeur ne répond pas

**Symptôme** : Cliquer sur l'encodeur ne fait rien.

**Solutions possibles** :

1. **Vérifier le câblage**
   - SW → A2 (ENC_SW)
   - Commun → GND

2. **Ajuster le délai anti-rebond**
   ```cpp
   const unsigned long CLICK_DEBOUNCE_MS = 300;  // Essayez 200 ou 100
   ```

3. **Vérifier les durées d'appui**
   ```cpp
   const unsigned long LONG_PRESS_DURATION = 500;       // Appui long
   const unsigned long VERY_LONG_PRESS_DURATION = 1500; // Menu config
   ```

### Les macros ne fonctionnent pas correctement

**Symptôme** : Les macros envoient les mauvaises touches ou ne fonctionnent pas.

**Solutions possibles** :

1. **Vérifier le layout clavier**
   - Le firmware suppose un clavier QWERTY US
   - Adaptez les caractères pour votre layout

2. **Ajouter des délais**
   ```cpp
   case K1:
     Keyboard.press(KEY_LEFT_CTRL);
     delay(50);  // Ajoutez un délai
     Keyboard.press('c');
     delay(50);  // Ajoutez un délai
     Keyboard.releaseAll();
     break;
   ```

3. **Vérifier que la touche existe**
   ```cpp
   // Touches spéciales disponibles :
   KEY_LEFT_CTRL, KEY_LEFT_SHIFT, KEY_LEFT_ALT, KEY_LEFT_GUI
   KEY_RIGHT_CTRL, KEY_RIGHT_SHIFT, KEY_RIGHT_ALT, KEY_RIGHT_GUI
   KEY_UP_ARROW, KEY_DOWN_ARROW, KEY_LEFT_ARROW, KEY_RIGHT_ARROW
   KEY_BACKSPACE, KEY_TAB, KEY_RETURN, KEY_ESC, KEY_INSERT
   KEY_DELETE, KEY_PAGE_UP, KEY_PAGE_DOWN, KEY_HOME, KEY_END
   KEY_F1 à KEY_F12
   ```

### Le screensaver ne s'active pas

**Symptôme** : L'écran reste toujours allumé.

**Solutions possibles** :

1. **Vérifier le timeout**
   ```cpp
   const unsigned long SCREENSAVER_TIMEOUT = 30000;  // 30 secondes en ms
   ```

2. **Vérifier lastActionTime**
   - S'assurer que `wakeUp()` est appelé correctement
   - Vérifier que `lastActionTime` est mis à jour

3. **Désactiver temporairement**
   ```cpp
   // Pour tester, augmentez le timeout
   const unsigned long SCREENSAVER_TIMEOUT = 3600000;  // 1 heure
   ```

### Les layers ne changent pas

**Symptôme** : Appuyer sur K9 ne change pas de layer.

**Solutions possibles** :

1. **Vérifier fireMacro()**
   ```cpp
   void fireMacro(uint8_t id) {
     if (id == K9) {  // Vérifier cette condition
       currentLayer = (currentLayer + 1) % NUM_LAYERS;
       // ...
     }
   }
   ```

2. **Afficher le layer actuel**
   ```cpp
   // Ajoutez dans drawIconMenu():
   display.setCursor(0, 0);
   display.print("L"); display.print(currentLayer);
   ```

3. **Vérifier NUM_LAYERS**
   ```cpp
   const uint8_t NUM_LAYERS = 3;  // Doit être 3
   ```

## 🔧 Problèmes de Compilation

### Erreur : "Adafruit_SSD1306.h: No such file"

**Solution** : Installer la bibliothèque Adafruit SSD1306
1. Outils → Gérer les bibliothèques
2. Rechercher "Adafruit SSD1306"
3. Installer (+ dépendances)

### Erreur : "display was not declared in this scope"

**Solution** : Vérifier l'ordre des includes
- `display` est déclaré ligne 61 dans le .ino
- Inclus avant config.h et iconmenu.h

### Erreur : "K1 was not declared in this scope"

**Solution** : L'enum KeyIds doit être avant config.h
```cpp
// Vérifier l'ordre :
enum KeyIds { K1, K2, ... };  // Ligne ~84
// ...
#include "config.h"           // Ligne ~131
```

### Erreur : "multiple definition of..."

**Solution** : Éviter les définitions multiples
- Utilisez `extern` pour les variables partagées
- Utilisez `#pragma once` dans les .h

## 🐛 Debug avec le Moniteur Série

Ajoutez du code de debug dans setup() et loop() :

```cpp
void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("=== MACROPAD DEBUG ===");
  
  // Test écran OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println("ERREUR: Ecran OLED non trouve!");
  } else {
    Serial.println("OK: Ecran OLED initialise");
  }
  
  // Test touches
  Serial.println("Test touches (appuyez sur chaque touche):");
  for (uint8_t i = 0; i < NUM_KEYS; i++) {
    Serial.print("K"); Serial.print(i+1); 
    Serial.print(" = Pin D"); Serial.println(KEY_PINS[i]);
  }
}

void loop() {
  // Debug état touches
  static unsigned long lastDebug = 0;
  if (millis() - lastDebug > 1000) {
    Serial.print("Layer: "); Serial.print(currentLayer);
    Serial.print(" | State: "); Serial.print(currentState);
    Serial.print(" | Encoder: A="); Serial.print(digitalRead(ENC_A));
    Serial.print(" B="); Serial.print(digitalRead(ENC_B));
    Serial.print(" SW="); Serial.println(digitalRead(ENC_SW));
    lastDebug = millis();
  }
  
  // ... reste du code
}
```

## 📊 Codes d'Erreur Communs

| Code/Message | Signification | Solution |
|--------------|---------------|----------|
| `Erreur : Impossible de trouver l'écran OLED !` | Écran non détecté sur I2C | Vérifier câblage et adresse I2C |
| Display reste sur bootscreen | Loop ne s'exécute pas | Vérifier setup() et while(1) |
| Touches collent | Pas de break; | Ajouter break; après chaque case |
| Volume ne change pas | Consumer non initialisé | Vérifier Consumer.begin() |

## 🛠️ Outils de Diagnostic

### Scanner I2C

```cpp
#include <Wire.h>

void scanI2C() {
  Serial.println("Scan I2C...");
  for (byte addr = 1; addr < 127; addr++) {
    Wire.beginTransmission(addr);
    if (Wire.endTransmission() == 0) {
      Serial.print("Trouve: 0x");
      Serial.println(addr, HEX);
    }
  }
}
```

### Test Encodeur

```cpp
void testEncoder() {
  Serial.print("ENC_A: "); Serial.print(digitalRead(ENC_A));
  Serial.print(" | ENC_B: "); Serial.print(digitalRead(ENC_B));
  Serial.print(" | ENC_SW: "); Serial.println(digitalRead(ENC_SW));
  delay(100);
}
```

### Test Touches

```cpp
void testKeys() {
  for (uint8_t i = 0; i < NUM_KEYS; i++) {
    if (digitalRead(KEY_PINS[i]) == LOW) {
      Serial.print("Touche K"); Serial.print(i+1); Serial.println(" pressee");
    }
  }
}
```

## 📞 Support

Si vous rencontrez un problème non résolu :

1. **Vérifiez la documentation**
   - README.md
   - STRUCTURE.md
   - EXAMPLES.md
   - ARCHITECTURE.md

2. **Activez le debug série**
   - Ajoutez Serial.println() aux endroits critiques
   - Ouvrez le moniteur série (115200 baud)

3. **Testez étape par étape**
   - Commentez les sections du code
   - Isolez le problème

4. **Réinitialisez aux valeurs par défaut**
   - Re-téléchargez le firmware original
   - Vérifiez avec la config de base

## ⚙️ Configuration Avancée

### Changer la résolution de l'écran

Si vous avez un écran 128x64 :

```cpp
const int SCREEN_HEIGHT = 64;  // Au lieu de 32
```

### Changer le nombre de touches

```cpp
const uint8_t KEY_PINS[] = { D2, D3, D4, D5, D6 };  // 5 touches au lieu de 9
const uint8_t NUM_KEYS = 5;
enum KeyIds { K1, K2, K3, K4, K5 };  // Adapter l'enum
```

### Désactiver le screensaver

```cpp
// Dans loop(), commentez la section screensaver
/*
if (millis() - lastActionTime > SCREENSAVER_TIMEOUT && currentState != STATE_SCREENSAVER) {
  // ...
}
*/
```

## 🔄 Réinitialisation

Pour réinitialiser complètement :

1. Re-téléverser le firmware
2. Débrancher/rebrancher l'USB
3. Vérifier toutes les connexions
4. Tester avec le code minimal

## 📝 Checklist de Vérification

- [ ] Écran OLED allumé et affiche le bootscreen
- [ ] Toutes les touches répondent
- [ ] Encodeur tourne et change les valeurs
- [ ] Bouton encodeur détecte les clics
- [ ] Macros s'exécutent correctement
- [ ] Layers changent avec K9
- [ ] Screensaver s'active après inactivité
- [ ] Menu de configuration accessible
- [ ] USB HID reconnu par l'ordinateur

Si tous les éléments sont cochés, votre macropad fonctionne correctement ! 🎉
