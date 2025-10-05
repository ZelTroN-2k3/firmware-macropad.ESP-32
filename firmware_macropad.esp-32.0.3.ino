/**
 * ===================================================================================
 * FIRMWARE POUR MACROPAD GENERIQUE : OLED + ENCODEUR + 9 TOUCHES
 * ===================================================================================
 * Auteur     : [ANCHER.P]
 * Date       : 04/10/2025
 * * Description: 
 * Firmware pour un macropad basé sur une carte Arduino Nano ESP32.
 * Gère 9 touches programmables, un encodeur rotatif pour le volume et un écran OLED.
  * un menu de configuration, un screensaver et un menu de démarrage à icônes.
 * * 👉 Fichier principal de configuration : config.h
 * * Composants :
 * - 1x Arduino Nano ESP32
 * - 9x Switches mécaniques (Cherry MX ou similaire)
 * - 1x Encodeur rotatif EC11
 * - 1x Écran OLED I2C 0.91" (128x32, driver SSD1306)
 * * 👉 Comment personnaliser ?
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
  int i = 0;
  String _en = " =qwertyuiopasdfghjkl;zxcvbnQWERTYUIOPASDFGHJKL:ZXCVBNm,./M<>?1234567890!@#$%^&*()",
         _fr = " =azertyuiopqsdfghjklmwxcvbnAZERTYUIOPQSDFGHJKLMWXCVBN,;:!?./ & \"'(- _  1234567890",
         str = "";
  while (text[i] != 0){
    str = str + (String)_en[_fr.indexOf((String)text[i++])];
  }
  return str;
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
int8_t transTable[16] = { 0, -1, +1, 0, +1, 0, 0, -1, -1, 0, 0, +1, 0, +1, -1, 0 };
uint8_t lastAB = 0;
int8_t accum = 0;
const uint8_t STEPS_PER_DETENT = 4;
unsigned long lastStepMs = 0, lastClickMs = 0;
const unsigned long STEP_COOLDOWN_MS = 2, CLICK_DEBOUNCE_MS = 200;

// Variables pour la mise en veille de l'OLED
unsigned long lastActionTime = 0;
const unsigned long SLEEP_DELAY = 30000;  // Délai en ms (ici 30 s)
bool isSleeping = false;

// Variables pour la simulation du volume (affichage OLED)
int currentVol = 50;
bool muted = false;

// Variables pour les modes de l'encodeur
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
const unsigned long SCREENSAVER_DELAY = SLEEP_DELAY - 15000; // Se lance 15s avant la veille
bool isScreensaverActive = false;
int8_t iconX = 0, iconY = 0;   // Position de l'icône
int8_t iconDX = 1, iconDY = 1;  // Direction de l'icône (vitesse)
// Tableau contenant les 4 frames de l'animation de rotation
const unsigned char* screensaverFrames[] = { // Tableau contenant les 4 frames de l'animation de rotation
  icon_screensaver_0_16x16,
  icon_screensaver_90_16x16,
  icon_screensaver_180_16x16,
  icon_screensaver_270_16x16
};
uint8_t currentFrame = 0; // Pour suivre l'image actuelle de l'animation

// --- Variables pour le menu d'icônes ---
enum ProgramState { STATE_ICON_MENU, STATE_NORMAL };
ProgramState currentState = STATE_ICON_MENU;


/* ---------------------------------------------------------- */
/* ---------------- PROTOTYPES DES FONCTIONS ---------------- */
/* ---------------------------------------------------------- */
void showMessage(const char* msg);
void showVolume();
void wakeUp();
void drawMenu();
void setBrightness(uint8_t brightness);
void fireMacro(uint8_t id);
void handleSerialCommands();
void drawIconMenu();
void drawScreensaver();
String fr2en(String text);
void returnToIconMenu();
void handleIconMenu();
void handleNormalMode();

#include "debug.h"  // Dépend des fonctions du fichier principal (qui seront déclarées juste après)
#include "config.h" // Dépend des fonctions et variables du fichier principal
#include "iconmenu.h"

/* =============================================== */
/* ==================== SETUP ==================== */
/* =============================================== */
void setup() {
  // Initialisation de la communication Série en premier pour le débogage
  #if defined(DEBUG_MODE_ENABLED)
    Serial.begin(115200);
    // Attente de la connexion du port série
    //while (!Serial);  // <-- "On commente //" "ou supprime" cette ligne pour un démarrage autonome
  #endif

  // Initialisation des broches des touches
  for (uint8_t i = 0; i < NUM_KEYS; i++) {
    pinMode(KEY_PINS[i], INPUT_PULLUP);
    lastKeyState[i] = digitalRead(KEY_PINS[i]);
  }

  // Initialisation des broches de l'encodeur
  pinMode(ENC_A, INPUT_PULLUP);
  pinMode(ENC_B, INPUT_PULLUP);
  pinMode(ENC_SW, INPUT_PULLUP);
  lastAB = (digitalRead(ENC_A) << 1) | digitalRead(ENC_B);

  // Initialisation de l'I2C
  //Wire.begin(A4 /*SDA*/, A5 /*SCL*/);

  // Initialisation de l'I2C en utilisant les broches par défaut (A4/SDA, A5/SCL)
  Wire.begin();
  
  // Initialisation de l'écran OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    #if defined(DEBUG_MODE_ENABLED)
      Serial.println(F("ERREUR : Initialisation de l'ecran OLED impossible !"));
    #endif
    for (;;);
  }

  // Initialisation du bus USB
  Keyboard.begin();
  Mouse.begin();
  Consumer.begin();

  // Écran de démarrage
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


/* ================================================================== */
/* ==================== LOOP (BOUCLE PRINCIPALE) ==================== */
/* ================================================================== */

void loop() {
  // Le gestionnaire de commandes série est toujours actif
  #if defined(DEBUG_MODE_ENABLED)
    handleSerialCommands();
  #endif

  // Le "chef d'orchestre" : appelle la bonne fonction selon l'état
  switch (currentState) {
    case STATE_ICON_MENU:
      handleIconMenu();
      break;
    case STATE_NORMAL:
      handleNormalMode();
      break;
  }

  // La gestion de l'inactivité est toujours active
  unsigned long idleTime = millis() - lastActionTime;
  if (isSleeping) {
    // Ne rien faire
  } else if (idleTime > SLEEP_DELAY) {
    display.ssd1306_command(SSD1306_DISPLAYOFF);
    isSleeping = true;
    isScreensaverActive = false;
  } else if (idleTime > SCREENSAVER_DELAY) {
    if (!isScreensaverActive) {
      isScreensaverActive = true;
      iconX = 0; iconY = 0;
      iconDX = 1; iconDY = 1;
    }
    drawScreensaver();
  }
}

/* ================================================================== */
/* ==================== DEFINITION DES FONCTIONS ==================== */
/* ================================================================== */

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
                if (accum >= STEPS_PER_DETENT || accum <= -STEPS_PER_DETENT) {
                  int8_t direction = (accum > 0) ? 1 : -1;
                  setBrightness(max(0, min(255, oledBrightness + (15 * direction))));
                  display.clearDisplay(); display.setCursor(0, 8);
                  display.print("Luminosite: "); display.print(oledBrightness);
                  display.display();
                  accum = 0;
                }
              }
              lastAB = nowAB;
            }
          }
          while (digitalRead(ENC_SW) == LOW) { delay(10); }
          drawMenu();
          break;
        case 1: // Option "Menu Principal"
          returnToIconMenu();
          break;
        case 2: // Option "Sortir"
          isInMenu = false;
          showVolume();
          break;
      }
    }
  } else {
    // --- MODE DE FONCTIONNEMENT HORS-MENU ---
    for (uint8_t i = 0; i < NUM_KEYS; i++) {
      bool pressed = (digitalRead(KEY_PINS[i]) == LOW);
      if (pressed && !lastKeyState[i]) { wakeUp(); fireMacro(i); }
      lastKeyState[i] = pressed;
    }

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
            case MODE_UNDO_REDO: if (direction > 0) sendCombo_Ctrl('y'); else sendCombo_Ctrl('z'); break;
          }
          showVolume();
          accum = 0;
        }
      }
      lastAB = nowAB;
    }

    bool buttonState = (digitalRead(ENC_SW) == LOW);
    static bool lastButtonState = false;
    static unsigned long buttonPressStartTime = 0;
    static bool actionTakenOnPress = false;
    if (buttonState && !lastButtonState) { buttonPressStartTime = millis(); actionTakenOnPress = false; }
    if (buttonState && !actionTakenOnPress && (millis() - buttonPressStartTime > VERY_LONG_PRESS_DURATION)) {
      isInMenu = !isInMenu;
      if (isInMenu) { wakeUp(); selectedMenuItem = 0; blockMenuClickUntilRelease = true; drawMenu(); } 
      else { showVolume(); }
      actionTakenOnPress = true;
    }
    if (!buttonState && lastButtonState) {
      if (!actionTakenOnPress) {
        wakeUp();
        unsigned long pressDuration = millis() - buttonPressStartTime;
        if (pressDuration > LONG_PRESS_DURATION) {
          if (currentEncoderMode == MODE_VOLUME) currentEncoderMode = MODE_SCROLL;
          else if (currentEncoderMode == MODE_SCROLL) currentEncoderMode = MODE_UNDO_REDO;
          else if (currentEncoderMode == MODE_UNDO_REDO) currentEncoderMode = MODE_VOLUME;
        } else {
          switch (currentEncoderMode) {
            case MODE_VOLUME: muted = !muted; Consumer.press(HID_USAGE_CONSUMER_MUTE); Consumer.release(); break;
            case MODE_SCROLL: Mouse.click(MOUSE_MIDDLE); break;
            case MODE_UNDO_REDO: break;
          }
        }
        showVolume();
      }
    }
    lastButtonState = buttonState;
  }
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

// Affiche la barre de volume sur l'écran OLED.
void showVolume() {
  wakeUp();
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);

  // Affiche le titre en fonction du mode de l'encodeur
  switch (currentEncoderMode) {
    case MODE_VOLUME:
      display.print(F("Mode: Volume"));
      break;
    case MODE_SCROLL:
      display.print(F("Mode: Scroll"));
      // On affiche l'icône de scroll au lieu du texte
      display.drawBitmap(
        (SCREEN_WIDTH - 16) / 2, // Centré en X
        20,                       // En haut de l'écran en Y
        icon_scroll_16x16,       // Données de l'icône
        16, 16,                  // Largeur et hauteur
        SSD1306_WHITE);
      break;
    case MODE_UNDO_REDO:
      display.print(F("Mode: Undo/Redo"));
      // On affiche l'icône de undo_redo et le texte
      display.drawBitmap(
        (SCREEN_WIDTH - 16) / 2, // Centré en X
        20,                       // En haut de l'écran en Y
        icon_undo_redo_16x16,       // Données de l'icône
        16, 16,                  // Largeur et hauteur
        SSD1306_WHITE);      
      break;
  }

  // Affiche la barre de volume ou l'état Mute, UNIQUEMENT en mode Volume
  if (currentEncoderMode == MODE_VOLUME) {
    if (muted) {
      // 1. On affiche d'abord le texte en haut de l'écran
      display.setCursor(0, 10); // Position X (environ centré), Position Y (en haut)
      display.println(F("ETAT: MUTE"));

      // 2. Ensuite, on dessine l'icône en dessous du texte
      display.drawBitmap(
        (SCREEN_WIDTH - 16) / 2,  // Position X (centrée)
        20,                       // Position Y (plus bas pour ne pas chevaucher le texte)
        icon_mute_16x16,          // Le tableau de l'icône
        16, 16,                   // Largeur et hauteur
        SSD1306_WHITE);           // Couleur
        
    } else {
      // Sinon, on affiche l'état normal et la barre de volume
      display.setCursor(0, 10);
      display.print(F("ETAT: "));
      // On affiche l'icône de sound et le texte
      display.drawBitmap(
        (SCREEN_WIDTH - 16) / 2 + 40, // Centré en X
        1,                       // En haut de l'écran en Y
        icon_sound_16x16,       // Données de l'icône
        16, 16,                  // Largeur et hauteur
        SSD1306_WHITE);            
      display.print(currentVol);
      display.println('%');
      int barW = map(currentVol, 0, 100, 0, SCREEN_WIDTH - 10);
      display.drawRect(5, 22, SCREEN_WIDTH - 10, 8, SSD1306_WHITE);
      if (barW > 0) display.fillRect(5, 22, barW, 8, SSD1306_WHITE);
    }
  }
  
  display.display();
}

// Sort l'écran du mode veille et réinitialise le minuteur.
void wakeUp() { 
  if (isSleeping) {
    display.ssd1306_command(SSD1306_DISPLAYON);
    isSleeping = false;
  }
  isScreensaverActive = false; // screensaver
  lastActionTime = millis();
}

/**
 * @brief Applique une nouvelle valeur de contraste (luminosité) à l'écran.
 * @param brightness La valeur de luminosité (0-255).
 */
void setBrightness(uint8_t brightness) {
  oledBrightness = brightness;
  display.ssd1306_command(SSD1306_SETCONTRAST);
  display.ssd1306_command(oledBrightness);
}

/**
 * @brief Dessine et anime l'icône de l'économiseur d'écran,
 * avec un effet de rotation.
 */
void drawScreensaver() {
  // Met à jour la position de l'icône
  iconX += iconDX;
  iconY += iconDY;

  // Gère les rebonds sur les bords
  if (iconX <= 0 || iconX >= SCREEN_WIDTH - 16) {
    iconDX = -iconDX; // Inverse la direction horizontale
  }
  if (iconY <= 0 || iconY >= SCREEN_HEIGHT - 16) {
    iconDY = -iconDY; // Inverse la direction verticale
  }

  // Affiche l'icône
  display.clearDisplay();
  
  // Affiche la frame actuelle de l'animation de rotation
  display.drawBitmap(iconX, iconY, screensaverFrames[currentFrame], 16, 16, SSD1306_WHITE);
  
  display.display();
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

/* ------------------------------ Fin du code -------------------------------- */

