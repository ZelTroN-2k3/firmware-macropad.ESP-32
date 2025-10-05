#pragma once

// =============================================================================
//     MODULE DES RACCOURCIS CLAVIER
// =============================================================================
// Ce fichier contient les fonctions utilitaires pour envoyer des raccourcis
// clavier et ouvrir des applications via Win+R

extern USBHIDKeyboard Keyboard;

/**
 * @brief Ouvre une application via Win+R (Exécuter)
 * @param cmd La commande à exécuter
 */
void openViaRun(const char* cmd) {
  Keyboard.press(KEY_LEFT_GUI);
  delay(100);
  Keyboard.press('r');
  delay(100);
  Keyboard.releaseAll();
  delay(300);
  
  Keyboard.print(cmd);
  delay(100);
  Keyboard.press(KEY_RETURN);
  delay(100);
  Keyboard.releaseAll();
}

/**
 * @brief Envoie Alt+Tab
 */
void sendAltTab() {
  Keyboard.press(KEY_LEFT_ALT);
  delay(50);
  Keyboard.press(KEY_TAB);
  delay(50);
  Keyboard.releaseAll();
}

/**
 * @brief Envoie Win+D (afficher le bureau)
 */
void sendWinD() {
  Keyboard.press(KEY_LEFT_GUI);
  delay(50);
  Keyboard.press('d');
  delay(50);
  Keyboard.releaseAll();
}

/**
 * @brief Envoie Ctrl + touche
 * @param key La touche à combiner avec Ctrl
 */
void sendCombo_Ctrl(char key) {
  Keyboard.press(KEY_LEFT_CTRL);
  delay(50);
  Keyboard.press(key);
  delay(50);
  Keyboard.releaseAll();
}

/**
 * @brief Envoie Ctrl + Shift + touche
 * @param key La touche à combiner avec Ctrl+Shift
 */
void sendCombo_CtrlShift(char key) {
  Keyboard.press(KEY_LEFT_CTRL);
  delay(50);
  Keyboard.press(KEY_LEFT_SHIFT);
  delay(50);
  Keyboard.press(key);
  delay(50);
  Keyboard.releaseAll();
}
