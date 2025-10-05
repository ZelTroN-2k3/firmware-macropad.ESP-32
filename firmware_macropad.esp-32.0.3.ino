/**
 * ===================================================================================
 * FIRMWARE POUR MACROPAD GENERIQUE : OLED + ENCODEUR + 9 TOUCHES
 * ===================================================================================
 * Auteur     : [ANCHER.P]
 * Date       : 04/10/2025
 * Description: 
 * Firmware pour un macropad basé sur une carte Arduino Nano ESP32.
 * Gère 9 touches programmables, un encodeur rotatif pour le volume et un écran OLED.
 * un menu de configuration, un screensaver et un menu de démarrage à icônes.
 * 
 * 👉 Fichier principal de configuration : config.h
 * 
 * Composants :
 * - 1x Arduino Nano ESP32
 * - 9x Switches mécaniques (Cherry MX ou similaire)
 * - 1x Encodeur rotatif EC11
 * - 1x Écran OLED I2C 0.91" (128x32, driver SSD1306)
 * 
 * 👉 Comment personnaliser ?
 * - La configuration des broches (pins) se trouve dans la section "PINS".
 * - Les actions des touches se personnalisent dans la fonction "fireMacro()".
 */
 
#include "USB.h"
#include "USBHIDMouse.h"
#include "USBHIDKeyboard.h"
#include <USBHIDConsumerControl.h>

// On crée les objets pour chaque périphérique USB
USBHIDMouse Mouse;
USBHIDKeyboard Keyboard;
USBHIDConsumerControl Consumer;

#include <Arduino.h>
#include <Wire.h> 
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "icondata.h" 
#include "key-shortcut.h"

/* ---------------- Conversion FR → EN pour Win+R ---------------- */
String fr2en(String text){
  text.replace("à", "a"); text.replace("â", "a"); text.replace("é", "e");
  text.replace("è", "e"); text.replace("ê", "e"); text.replace("ë", "e");
  text.replace("î", "i"); text.replace("ï", "i"); text.replace("ô", "o");
  text.replace("ù", "u"); text.replace("û", "u"); text.replace("ü", "u");
  text.replace("ç", "c"); text.replace("À", "A"); text.replace("Â", "A");
  text.replace("É", "E"); text.replace("È", "E"); text.replace("Ê", "E");
  text.replace("Ë", "E"); text.replace("Î", "I"); text.replace("Ï", "I");
  text.replace("Ô", "O"); text.replace("Ù", "U"); text.replace("Û", "U");
  text.replace("Ü", "U"); text.replace("Ç", "C");
  return text;
}

/* ---------------------------------------------- */
/* -------------------- OLED -------------------- */
/* ---------------------------------------------- */
const int SCREEN_WIDTH = 128;  // Largeur de l'écran OLED, en pixels
const int SCREEN_HEIGHT = 32;  // Hauteur de l'écran OLED, en pixels
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
const uint8_t OLED_ADDR = 0x3C;  // Adresse I2C (souvent 0x3C ou 0x3D)


/* ---------------------------------------------- */
/* -------------------- PINS -------------------- */
/* ---------------------------------------------- */
// Encodeur rotatif
const uint8_t ENC_A = A1;   // Signal A
const uint8_t ENC_B = A0;   // Signal B
const uint8_t ENC_SW = A2;  // Bouton de l'encodeur

// 9 touches programmables. 
// Avec le LAYERS il y à 3 couches (0, 1, 2) = 24 touches programmables.
// la touche dédiée (K9) est pressée, on change de couche.
const uint8_t KEY_PINS[] = { D2, D3, D4, D5, D6, D7, D8, D9, D10 };
const uint8_t NUM_KEYS = sizeof(KEY_PINS) / sizeof(KEY_PINS[0]);


/* ---------------------------------------------------- */
/* ---------------- VARIABLES GLOBALES ---------------- */
/* ---------------------------------------------------- */
// Identifiants symboliques pour les touches
enum KeyIds { K1, K2, K3, K4, K5, K6, K7, K8, K9 };
bool lastKeyState[NUM_KEYS]; // Tableau pour l'anti-rebond des touches

// Variables pour la gestion des couches (layers)
const uint8_t NUM_LAYERS = 3; // Nombre total de couches (0, 1, 2)
uint8_t currentLayer = 0;     // Couche actuellement active

// Variables pour l'encodeur
int8_t transTable[16] = {0, -1, 1, 0, 1, 0, 0, -1, -1, 0, 0, 1, 0, 1, -1, 0};
uint8_t lastAB = 0;
int8_t accum = 0;
const uint8_t STEPS_PER_DETENT = 4;

// Machine d'état pour gérer les différents modes
enum ProgramState { STATE_NORMAL, STATE_ICON_MENU, STATE_SCREENSAVER };
ProgramState currentState = STATE_ICON_MENU;

// Variables pour le volume et les modes de l'encodeur
int8_t currentVol = 50;
bool muted = false;
enum EncoderMode { MODE_VOLUME, MODE_SCROLL, MODE_UNDO_REDO };
EncoderMode currentEncoderMode = MODE_VOLUME;
unsigned long buttonPressStartTime = 0;
const unsigned long LONG_PRESS_DURATION = 500; // 500 ms pour un appui long

// --- Variables pour le menu de configuration ---
bool isInMenu = false;
const char* menuItems[] = { "Luminosite", "Menu Principal", "Sortir" };
const uint8_t NUM_MENU_ITEMS = sizeof(menuItems) / sizeof(menuItems[0]);
int8_t selectedMenuItem = 0;
uint8_t oledBrightness = 255; // 255 = max, 0 = min
const unsigned long VERY_LONG_PRESS_DURATION = 1500; // 1.5s pour entrer dans le menu
bool blockMenuClickUntilRelease = false; // Sécurité pour éviter le clic instantané

// --- Variables pour le screensaver configuration ---
unsigned long lastActionTime = 0;
const unsigned long SCREENSAVER_TIMEOUT = 30000; // 30 secondes d'inactivité
int iconX = 56, iconY = 8;
int iconDX = 1, iconDY = 1;
uint8_t currentFrame = 0;

// --- Anti-rebond pour les clics de l'encodeur ---
unsigned long lastClickMs = 0;
const unsigned long CLICK_DEBOUNCE_MS = 300;
bool lastButtonState = HIGH;

// Déclarations anticipées des fonctions
void showMessage(const char* msg);
void wakeUp();
void drawIconMenu();

// Inclure le module du menu d'icônes après la déclaration des variables
#include "config.h"
#include "iconmenu.h"

/* ================================================= */
/* ================== FONCTIONS ==================== */
/* ================================================= */

/**
 * @brief Réveille l'écran et réinitialise le minuteur d'inactivité.
 */
void wakeUp() {
  if (currentState == STATE_SCREENSAVER) {
    currentState = STATE_ICON_MENU;
    drawIconMenu();
  }
  lastActionTime = millis();
}

// Affiche un message temporaire sur l'écran OLED.
void showMessage(const char* msg) {
  wakeUp();
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 8);
  display.println(msg);
  display.display();
}

/**
 * @brief Affiche le niveau de volume actuel sur l'écran.
 */
void showVolume() {
  wakeUp();
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  
  if (muted) {
    display.println(F("Volume: MUTE"));
  } else {
    display.print(F("Volume: "));
    display.print(currentVol);
    display.println(F("%"));
  }
  
  // Barre de progression
  int barWidth = (currentVol * 100) / 100;
  display.fillRect(10, 20, barWidth, 8, SSD1306_WHITE);
  display.drawRect(9, 19, 102, 10, SSD1306_WHITE);
  
  display.display();
}

/**
 * @brief Point central pour déclencher les macros.
 * @param id L'identifiant de la touche pressée (de 0 à 8).
 * @note C'EST ICI QU'IL FAUT PERSONNALISER LES ACTIONS DE CHAQUE TOUCHE.
 */
void fireMacro(uint8_t id) {
  // --- GESTION DU CHANGEMENT DE COUCHE ---
  // Si la touche dédiée (K9) est pressée, on change de couche et on s'arrête là.
  if (id == K9) {
    currentLayer = (currentLayer + 1) % NUM_LAYERS;
    char layerMsg[10];
    sprintf(layerMsg, "Layer %d", currentLayer);
    showMessage(layerMsg);
    return;
  }

  // Appelle la fonction de macro correspondante à la couche active
  switch (currentLayer) {
    case 0:
      executeMacroForLayer0(id);
      break;
    case 1:
      executeMacroForLayer1(id);
      break;
    case 2:
      executeMacroForLayer2(id);
      break;
  }
}

/**
 * @brief Applique une nouvelle valeur de contraste (luminosité) à l'écran.
 * @param brightness La valeur de luminosité (0-255).
 */
void setBrightness(uint8_t brightness) {
  display.ssd1306_command(0x81);
  display.ssd1306_command(brightness);
}

/**
 * @brief Dessine et anime l'icône de l'économiseur d'écran,
 * avec un effet de rotation.
 */
void drawScreensaver() {
  display.clearDisplay();
  
  const unsigned char* frames[] = {
    screensaver_frame_0,
    screensaver_frame_1,
    screensaver_frame_2,
    screensaver_frame_3
  };
  
  display.drawBitmap(iconX, iconY, frames[currentFrame], 16, 16, SSD1306_WHITE);
  display.display();
  
  // Déplace l'icône
  iconX += iconDX;
  iconY += iconDY;
  
  // Rebondir sur les bords
  if (iconX <= 0 || iconX >= SCREEN_WIDTH - 16) iconDX = -iconDX;
  if (iconY <= 0 || iconY >= SCREEN_HEIGHT - 16) iconDY = -iconDY;
  
  delay(10); // Ralentit un peu l'animation

  // Passe à la frame suivante pour le prochain appel
  currentFrame = (currentFrame + 1) % 4; // Boucle de 0 à 3
}

/**
 * @brief Dessine le menu de configuration sur l'écran OLED.
 */
void drawMenu() {
  display.clearDisplay();
  display.setTextSize(1);
  
  for (uint8_t i = 0; i < NUM_MENU_ITEMS; i++) {
    display.setCursor(10, (i * 12) + 1);
    if (i == selectedMenuItem) {
      // Affiche l'élément sélectionné en vidéo inverse (texte noir sur fond blanc)
      display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
      display.print(F("> "));
      display.print(menuItems[i]);
    } else {
      display.setTextColor(SSD1306_WHITE);
      display.print(F("  "));
      display.print(menuItems[i]);
    }
  }
  display.display();
}

/**
 * @brief Gère le retour à l'écran du menu principal d'icônes.
 */
void returnToIconMenu() {
  isInMenu = false;                   // S'assure qu'on quitte le menu de configuration
  currentState = STATE_ICON_MENU;     // Règle l'état du programme sur le menu d'icônes
  wakeUp();                           // Réveille l'écran et réinitialise le minuteur d'inactivité
  drawIconMenu();                     // Dessine le menu d'icônes
}

/**
 * @brief Gère la logique du mode de fonctionnement normal.
 * Appelée à chaque tour de boucle lorsque currentState == STATE_NORMAL.
 */
void handleNormalMode() {
  if (isInMenu) {
    // --- GESTION DU MENU DE CONFIGURATION ---
    uint8_t nowAB = (digitalRead(ENC_A) << 1) | digitalRead(ENC_B);
    if (nowAB != lastAB) {
      int8_t dir = transTable[(lastAB << 2) | nowAB];
      if (dir != 0) {
        accum += dir;
        if (accum >= STEPS_PER_DETENT) { selectedMenuItem = (selectedMenuItem + 1) % NUM_MENU_ITEMS; accum = 0; } 
        else if (accum <= -STEPS_PER_DETENT) { selectedMenuItem = (selectedMenuItem - 1 + NUM_MENU_ITEMS) % NUM_MENU_ITEMS; accum = 0; }
        drawMenu();
      }
      lastAB = nowAB;
    }

    if (digitalRead(ENC_SW) == HIGH) { blockMenuClickUntilRelease = false; }

    if (!blockMenuClickUntilRelease && digitalRead(ENC_SW) == LOW && (millis() - lastClickMs > CLICK_DEBOUNCE_MS)) {
      lastClickMs = millis();
      switch (selectedMenuItem) {
        case 0: // Option "Luminosite"
          showMessage("Tournez pour regler");
          while (digitalRead(ENC_SW) == LOW) { delay(10); }
          while (digitalRead(ENC_SW) == HIGH) {
            nowAB = (digitalRead(ENC_A) << 1) | digitalRead(ENC_B);
            if (nowAB != lastAB) {
              int8_t dir = transTable[(lastAB << 2) | nowAB];
              if (dir != 0) {
                accum += dir;
                if (accum >= STEPS_PER_DETENT) {
                  oledBrightness = min(255, oledBrightness + 15);
                  accum = 0;
                  setBrightness(oledBrightness);
                } else if (accum <= -STEPS_PER_DETENT) {
                  oledBrightness = max(0, oledBrightness - 15);
                  accum = 0;
                  setBrightness(oledBrightness);
                }
                display.clearDisplay();
                display.setCursor(0, 0);
                display.print(F("Luminosite: "));
                display.print(oledBrightness);
                display.display();
              }
              lastAB = nowAB;
            }
          }
          drawMenu();
          blockMenuClickUntilRelease = true;
          break;

        case 1: // Option "Menu Principal"
          returnToIconMenu();
          break;

        case 2: // Option "Sortir"
          isInMenu = false;
          wakeUp();
          showMessage("Retour normal");
          delay(500);
          drawIconMenu();
          break;
      }
    }
  } else {
    // --- MODE NORMAL (pas dans le menu) ---
    // Lecture des touches
    for (uint8_t i = 0; i < NUM_KEYS; i++) {
      bool pressed = (digitalRead(KEY_PINS[i]) == LOW);
      if (pressed && !lastKeyState[i]) {
        wakeUp();
        fireMacro(i);
      }
      lastKeyState[i] = pressed;
    }

    // Gestion de l'encodeur rotatif
    uint8_t nowAB = (digitalRead(ENC_A) << 1) | digitalRead(ENC_B);
    if (nowAB != lastAB) {
      int8_t dir = transTable[(lastAB << 2) | nowAB];
      if (dir != 0) {
        accum += dir;
        if (accum >= STEPS_PER_DETENT || accum <= -STEPS_PER_DETENT) {
          wakeUp();
          int8_t direction = (accum > 0) ? 1 : -1;
          switch (currentEncoderMode) {
            case MODE_VOLUME:
              if (direction > 0) Consumer.press(HID_USAGE_CONSUMER_VOLUME_INCREMENT);
              else Consumer.press(HID_USAGE_CONSUMER_VOLUME_DECREMENT);
              Consumer.release();
              currentVol = max(0, min(100, currentVol + (2 * direction)));
              break;
            case MODE_SCROLL: Mouse.move(0, 0, direction); break;
            case MODE_UNDO_REDO:
              if (direction > 0) sendCombo_Ctrl('y');
              else sendCombo_Ctrl('z');
              break;
          }
          accum = 0;
          showVolume();
        }
      }
      lastAB = nowAB;
    }

    // Gestion du bouton de l'encodeur (court = mute, long = changement de mode, très long = menu)
    bool buttonState = digitalRead(ENC_SW);
    if (buttonState == LOW && lastButtonState == HIGH) {
      buttonPressStartTime = millis();
    }
    
    if (buttonState == LOW && lastButtonState == LOW) {
      unsigned long pressDuration = millis() - buttonPressStartTime;
      
      // Appui très long : entrer dans le menu
      if (pressDuration >= VERY_LONG_PRESS_DURATION && !isInMenu) {
        isInMenu = true;
        selectedMenuItem = 0;
        drawMenu();
        blockMenuClickUntilRelease = true;
        while (digitalRead(ENC_SW) == LOW) { delay(10); }
      }
    }
    
    if (buttonState == HIGH && lastButtonState == LOW) {
      unsigned long pressDuration = millis() - buttonPressStartTime;
      
      // Appui court : action selon le mode
      if (pressDuration < LONG_PRESS_DURATION && (millis() - lastClickMs > CLICK_DEBOUNCE_MS)) {
        lastClickMs = millis();
        wakeUp();
        switch (currentEncoderMode) {
          case MODE_VOLUME: muted = !muted; Consumer.press(HID_USAGE_CONSUMER_MUTE); Consumer.release(); break;
          case MODE_SCROLL: Mouse.click(MOUSE_MIDDLE); break;
          case MODE_UNDO_REDO: break;
        }
        showVolume();
      }
      // Appui long : changement de mode
      else if (pressDuration >= LONG_PRESS_DURATION && pressDuration < VERY_LONG_PRESS_DURATION) {
        currentEncoderMode = (EncoderMode)((currentEncoderMode + 1) % 3);
        switch (currentEncoderMode) {
          case MODE_VOLUME: showMessage("Mode: Volume"); break;
          case MODE_SCROLL: showMessage("Mode: Scroll"); break;
          case MODE_UNDO_REDO: showMessage("Mode: Undo/Redo"); break;
        }
      }
    }
    lastButtonState = buttonState;
  }
}

/* =============================================== */
/* ==================== SETUP ==================== */
/* =============================================== */
void setup() {
  Serial.begin(115200);
  
  // Initialisation des périphériques USB
  Mouse.begin();
  Keyboard.begin();
  Consumer.begin();
  USB.begin();
  
  // Configuration des pins des touches en INPUT_PULLUP
  for (uint8_t i = 0; i < NUM_KEYS; i++) {
    pinMode(KEY_PINS[i], INPUT_PULLUP);
    lastKeyState[i] = HIGH;
  }
  
  // Configuration des pins de l'encodeur
  pinMode(ENC_A, INPUT_PULLUP);
  pinMode(ENC_B, INPUT_PULLUP);
  pinMode(ENC_SW, INPUT_PULLUP);
  lastAB = (digitalRead(ENC_A) << 1) | digitalRead(ENC_B);
  
  // Initialisation de l'écran OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println(F("Erreur : Impossible de trouver l'écran OLED !"));
    while (1);
  }
  
  // Affichage du bootscreen
  display.clearDisplay();
  display.drawBitmap(0, 0, bootscreen_128x32, 128, 32, SSD1306_WHITE);
  display.display();
  delay(2000);

  // On applique la luminosité par défaut
  setBrightness(oledBrightness);
  display.setTextSize(1);
  lastActionTime = millis();

  // --- On active le profil par défaut ---
  currentLayer = 0;
  currentEncoderMode = MODE_VOLUME;
  
  // Affichage du menu d'icônes initial
  drawIconMenu();
}

/* =============================================== */
/* ==================== LOOP ===================== */
/* =============================================== */
void loop() {
  // Gestion de l'économiseur d'écran
  if (millis() - lastActionTime > SCREENSAVER_TIMEOUT && currentState != STATE_SCREENSAVER) {
    currentState = STATE_SCREENSAVER;
    iconX = 56; iconY = 8;
    iconDX = 1; iconDY = 1;
  }

  // Machine d'état principale
  switch (currentState) {
    case STATE_NORMAL:
      handleNormalMode();
      break;
      
    case STATE_ICON_MENU:
      handleIconMenu();
      break;
      
    case STATE_SCREENSAVER:
      // Détection d'activité pour sortir du screensaver
      for (uint8_t i = 0; i < NUM_KEYS; i++) {
        if (digitalRead(KEY_PINS[i]) == LOW) {
          wakeUp();
          break;
        }
      }
      
      uint8_t nowAB = (digitalRead(ENC_A) << 1) | digitalRead(ENC_B);
      if (nowAB != lastAB || digitalRead(ENC_SW) == LOW) {
        lastAB = nowAB;
        wakeUp();
      }
      
      if (currentState == STATE_SCREENSAVER) {
        drawScreensaver();
      }
      break;
  }

  // Vérification pour entrer en screensaver depuis le menu d'icônes
  if (currentState == STATE_ICON_MENU && millis() - lastActionTime > SCREENSAVER_TIMEOUT) {
    currentState = STATE_SCREENSAVER;
    iconX = 56; iconY = 8;
    iconDX = 1; iconDY = 1;
  }
}

/* ------------------------------ Fin du code -------------------------------- */
