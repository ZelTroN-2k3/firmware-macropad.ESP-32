#pragma once // Empêche le fichier d'être inclus plusieurs fois par erreur

// --- Déclaration des variables GLOBALES utilisées ---
extern Adafruit_SSD1306 display;
extern USBHIDConsumerControl Consumer;
extern const int SCREEN_WIDTH;
extern const int SCREEN_HEIGHT;
// ---

// Voici un exemple complet pour lancer le Bloc-notes sur la touche 1.
// Pour l'activer, retirez les '/*' et '*/' et supprimez la ligne du TODO en dessous.  
  /*
  case K1: showMessage("NOTEPAD"); openViaRun("notepad.exe"); 
  case K7: showMessage("Macro 7"); sendAltTab(); break;
  case K8: showMessage("Macro 8"); sendWinD(); break;
    break;
  */

// =============================================================================
//     FICHIER DE CONFIGURATION DES MACROS
// =============================================================================
// C'est ici que vous définissez les actions pour chaque touche et chaque couche.
// -----------------------------------------------------------------------------

// --- gérer la logique d'affichage personnalisé ---
void displayCustomAction(const char* label, const unsigned char* icon) {
  // Ensuite, on prépare l'affichage
  display.clearDisplay();
  // 2. On dessine l'icône
  display.drawBitmap((SCREEN_WIDTH - 16) / 2, 14, icon, 16, 16, SSD1306_WHITE);
  // 3. On prépare et dessine le texte
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(42, 2); // Vous pouvez ajuster cette position
  display.print(label);
  // 4. On affiche le tout en une seule fois
  display.display();
}

void executeMacroForLayer0(uint8_t id) {
  showMessage("Layer 0");
  switch (id) {
    case K1:
      // D'abord, on exécute la macro qui prend du temps
      openViaRun("notepad3.exe");
      displayCustomAction("NOTEPAD.3", icon_notpad_16x16);
      break;

    case K2:
      // D'abord, on exécute la macro qui prend du temps
      openViaRun("cmd");
      displayCustomAction("(CMD) Commandes", icon_menu_parametres_16x16);
      break;

    case K3: showMessage("Macro 3.0"); /* TODO */ break;
    case K4: showMessage("Macro 4.0"); /* TODO */ break;
    case K5: showMessage("Macro 5.0"); /* TODO */ break;
    case K6: showMessage("Macro 6.0"); /* TODO */ break;
    case K7: showMessage("Macro 7.0"); /* TODO */ break;
    case K8: sendAltTab(); break;
  }
}

void executeMacroForLayer1(uint8_t id) {
  showMessage("Layer 1");
  switch (id) {
    case K1: showMessage("Copier");  sendCombo_Ctrl('c'); break;
    case K2: showMessage("Coller");  sendCombo_Ctrl('v'); break;
    case K3: showMessage("Couper");  sendCombo_Ctrl('x'); break;
    case K4: showMessage("Annuler"); sendCombo_Ctrl('z'); break;

    case K5: 
      showMessage("Re-Open Tab"); // 1. Affiche un message sur l'écran
      sendCombo_CtrlShift('t');   // 2. Envoie le raccourci Ctrl+Shift+T
      break;                      // 3. Ne pas oublier le break;
      
    case K6: showMessage("Macro 6.1"); /* TODO */ break;
    case K7: showMessage("Macro 7.1"); /* TODO */ break;
    case K8: sendWinD(); break;
  }
}

void executeMacroForLayer2(uint8_t id) {
  showMessage("Layer 2");
  switch (id) {
    case K1: showMessage("Play/Pause"); Consumer.press(HID_USAGE_CONSUMER_PLAY_PAUSE); Consumer.release(); break;
    case K2: showMessage("Suivant");    Consumer.press(HID_USAGE_CONSUMER_SCAN_NEXT); Consumer.release(); break;
    case K3: showMessage("Precedent");  Consumer.press(HID_USAGE_CONSUMER_SCAN_PREVIOUS); Consumer.release(); break;
    case K4: showMessage("Mute");       Consumer.press(HID_USAGE_CONSUMER_MUTE); Consumer.release(); break;
    case K5: showMessage("Vol +");      Consumer.press(HID_USAGE_CONSUMER_VOLUME_INCREMENT); Consumer.release(); break;
    case K6: showMessage("Vol -");      Consumer.press(HID_USAGE_CONSUMER_VOLUME_DECREMENT); Consumer.release(); break;
    case K7: showMessage("Macro 7.2"); /* TODO */ break;
    case K8: showMessage("Macro 8.2"); /* TODO */ break;
  }
}
