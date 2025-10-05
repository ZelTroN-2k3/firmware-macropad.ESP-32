#pragma once
#include <USBHIDKeyboard.h>

// =============================================================================
//     MODULE DES RACCOURCIS CLAVIER
// =============================================================================
// Ce fichier contient toutes les fonctions qui simulent des frappes
// ou des raccourcis clavier complexes.
// -----------------------------------------------------------------------------

extern USBHIDKeyboard Keyboard;

// --- Déclarations des fonctions externes ---
// On déclare ici les fonctions qui existent dans le fichier .ino principal
// pour que ce fichier sache qu'elles existent.
void showMessage(const char* msg);
void wakeUp();
String fr2en(String text);


// --- Définition des fonctions de raccourcis ---

/**
 * @brief Ouvre une application via le raccourci clavier Exécuter (Win+R).
 * @param textToRun La commande à exécuter (ex: "notepad.exe", "invite de commandes").
 */
void openViaRun(const char* textToRun) {
  // 1. Ouvre la fenêtre "Exécuter"
  String converted = fr2en(String(textToRun));
  Keyboard.press(KEY_LEFT_GUI);
  Keyboard.press('r');
  delay(120);
  Keyboard.releaseAll();
  delay(300);
  // 2. Valide avec la touche Entrée
  Keyboard.print(converted); 
  Keyboard.write(KEY_RETURN);
}

// Simule le raccourci clavier Alt + Tab.
void sendAltTab() {
  wakeUp();
  showMessage("Alt + Tab");
  Keyboard.press(KEY_LEFT_ALT);
  Keyboard.press(KEY_TAB);
  delay(50);
  Keyboard.releaseAll();
}

// Simule le raccourci clavier Win + D (afficher le bureau).
void sendWinD() {
  wakeUp();
  showMessage("Win + D");
  Keyboard.press(KEY_LEFT_GUI); // 'GUI' est la touche Windows
  Keyboard.press('d');
  delay(50);
  Keyboard.releaseAll();
}

// Simule le raccourci clavier Ctrl + [touche].
void sendCombo_Ctrl(char k) {
  Keyboard.press(KEY_LEFT_CTRL);
  Keyboard.press(k);
  delay(40);
  Keyboard.release(k);
  Keyboard.release(KEY_LEFT_CTRL);
}

// Simule le raccourci clavier Ctrl + Shift + [touche].
void sendCombo_CtrlShift(char k) {
  Keyboard.press(KEY_LEFT_CTRL);
  Keyboard.press(KEY_LEFT_SHIFT);
  Keyboard.press(k);
  delay(40);
  Keyboard.releaseAll(); // Utiliser releaseAll() est plus sûr ici
}

/* ------------------------------ Fin du code -------------------------------- */