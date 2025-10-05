#pragma once

// =============================================================================
//     MODULE DU MENU D'ICÔNES
// =============================================================================

// --- Déclarations des fonctions externes ---
void showMessage(const char* msg);
void openViaRun(const char* cmd);
void wakeUp();
void drawMenu();
void returnToIconMenu();
void fireMacro(uint8_t id);
void showVolume();

// --- Déclaration des variables GLOBALES utilisées par ce module ---
extern Adafruit_SSD1306 display;
extern ProgramState currentState;
extern uint8_t currentLayer;
extern EncoderMode currentEncoderMode;
extern bool isInMenu;
extern int8_t transTable[16];
extern uint8_t lastAB;
extern int8_t accum;
extern unsigned long lastClickMs;
extern bool lastKeyState[];
extern const uint8_t NUM_KEYS;
extern const uint8_t KEY_PINS[];
extern const unsigned long CLICK_DEBOUNCE_MS;
extern const uint8_t STEPS_PER_DETENT;
extern const uint8_t ENC_A;
extern const uint8_t ENC_B;
extern const uint8_t ENC_SW;
extern const int SCREEN_WIDTH;
extern const int SCREEN_HEIGHT;


// --- Variables propres à ce module ---
int8_t selectedIconIndex = 0;
const unsigned char* iconMenu[] = {
  icon_menu_pc_16x16, icon_menu_souris_16x16, icon_menu_clavier_16x16, icon_menu_audio_16x16,
  icon_menu_wifi_16x16, icon_menu_bluetooth_16x16, icon_menu_macros_16x16, icon_menu_parametres_16x16
};
const uint8_t NUM_ICONS = sizeof(iconMenu) / sizeof(iconMenu[0]);

const unsigned char* layerIcons[] = {
  icon_layer_0_16x16,
  icon_layer_1_16x16,
  icon_layer_2_16x16
};


// --- Fonction d'aide pour l'affichage personnalisé ---
/**
 * @brief Affiche un écran personnalisé avec une icône et un titre.
 * @param label Le texte à afficher en haut de l'écran.
 * @param icon Le bitmap de l'icône 16x16 à afficher.
 * @param cursorX La position X du début du texte.
 * @param cursorY La position Y du début du texte.
 */
void displayCustomScreen(const char* label, const unsigned char* icon, int cursorX = 0, int cursorY = 0) {
  display.clearDisplay();
  display.drawBitmap((SCREEN_WIDTH - 16) / 2, 8, icon, 16, 16, SSD1306_WHITE);
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(cursorX, cursorY);
  display.print(label);
  display.display();
}

/**
 * @brief Dessine le menu d'icônes principal avec l'icône du layer actuel
 */
void drawIconMenu() {
  display.clearDisplay();
  
  // Afficher l'icône du layer actuel en haut à gauche
  display.drawBitmap(0, 0, layerIcons[currentLayer], 16, 16, SSD1306_WHITE);
  
  // Afficher les icônes du menu
  for (uint8_t i = 0; i < NUM_ICONS; i++) {
    display.drawBitmap(i * 16, 16, iconMenu[i], 16, 16, SSD1306_WHITE);
  }
  display.fillRect(selectedIconIndex * 16, 16, 16, 16, SSD1306_INVERSE);
  display.display();
}

void handleIconMenu() {
  // ... (La lecture des touches et la gestion de la rotation de l'encodeur ne changent pas) ...
  for (uint8_t i = 0; i < NUM_KEYS; i++) {
    bool pressed = (digitalRead(KEY_PINS[i]) == LOW);
    if (pressed && !lastKeyState[i]) {
      wakeUp();
      fireMacro(i);
      delay(2000);
      returnToIconMenu();
      return;
    }
    lastKeyState[i] = pressed;
  }
  
  uint8_t nowAB = (digitalRead(ENC_A) << 1) | digitalRead(ENC_B);
  if (nowAB != lastAB) {
    int8_t dir = transTable[(lastAB << 2) | nowAB];
    if (dir != 0) {
      accum += dir;
      if (accum >= STEPS_PER_DETENT) {
        selectedIconIndex = (selectedIconIndex + 1) % NUM_ICONS;
        accum = 0;
        drawIconMenu();
      } else if (accum <= -STEPS_PER_DETENT) {
        selectedIconIndex = (selectedIconIndex - 1 + NUM_ICONS) % NUM_ICONS;
        accum = 0;
        drawIconMenu();
      }
    }
    lastAB = nowAB;
  }

  // Clic sur l'encodeur dans le menu d'icônes
  if (digitalRead(ENC_SW) == LOW && (millis() - lastClickMs > CLICK_DEBOUNCE_MS)) {
    lastClickMs = millis();
    wakeUp();
    
    // Action en fonction de l'icône sélectionnée
    switch (selectedIconIndex) {
      case 0: showMessage("PC"); break;
      case 1: showMessage("Souris"); break;
      case 2: showMessage("Clavier"); break;
      case 3: showMessage("Audio"); break;
      case 4: showMessage("WiFi"); break;
      case 5: showMessage("Bluetooth"); break;
      case 6: showMessage("Macros"); break;
      case 7: showMessage("Parametres"); break;
    }
    
    delay(1000);
    returnToIconMenu();
  }
}
