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
void displayCustomScreen(const char* label, const unsigned char* icon, int cursorX, int cursorY) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(cursorX, cursorY);
  display.print(F(label));
  // On dessine l'icône, toujours centrée horizontalement
  display.drawBitmap((SCREEN_WIDTH - 16) / 2, 14, icon, 16, 16, SSD1306_WHITE);
  display.display();
}


// --- Fonctions du menu d'icônes ---
void drawIconMenu() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  display.setCursor(0, 4);
  display.print(F("Layer:"));
  display.drawBitmap(32, 0, layerIcons[currentLayer], 16, 16, SSD1306_WHITE);

  display.setCursor(88, 4);
  display.print(F("MENU"));

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
      wakeUp();
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


  if (digitalRead(ENC_SW) == LOW && (millis() - lastClickMs > CLICK_DEBOUNCE_MS)) {
    lastClickMs = millis();
    while(digitalRead(ENC_SW) == LOW) { delay(10); }

    switch (selectedIconIndex) {
      case 0: showMessage("Profil: General"); currentLayer = 0; currentEncoderMode = MODE_VOLUME; break;
      case 1: showMessage("Profil: Navigation"); currentEncoderMode = MODE_SCROLL; break;
      case 2: showMessage("Profil: Edition"); currentLayer = 1; currentEncoderMode = MODE_UNDO_REDO; break;
      case 3: showMessage("Profil: Media"); currentLayer = 2; currentEncoderMode = MODE_VOLUME; break;

      case 4: // Icône WiFi
        openViaRun("ms-settings:network-wifi");
        displayCustomScreen("PARAMETRES WIFI", icon_menu_wifi_16x16, 18, 2);
        delay(2000);
        returnToIconMenu();
        return;

      case 5: // Icône Bluetooth
        openViaRun("ms-settings:bluetooth");
        displayCustomScreen("PARAMETRES BLUETOOTH", icon_menu_bluetooth_16x16, 5, 2);
        delay(2000);
        returnToIconMenu();
        return;
      
      case 6: showMessage("Profil: Macros"); currentLayer = 0; currentEncoderMode = MODE_VOLUME; break;
      case 7: isInMenu = true; currentState = STATE_NORMAL; wakeUp(); drawMenu(); return;
    }

    currentState = STATE_NORMAL;
    wakeUp();
    drawIconMenu();
    delay(1000);
    showVolume();
  }
}