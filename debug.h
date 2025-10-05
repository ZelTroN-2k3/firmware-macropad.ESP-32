#pragma once

// =============================================================================
//     MODULE DE DEBOGAGE SÉRIE
// =============================================================================

// --- INTERRUPTEUR ON/OFF ---
// Pour activer le mode débogage, laissez cette ligne active.
// Pour le désactiver, commentez-la (ajoutez // au début).

#define DEBUG_MODE_ENABLED

// -----------------------------------------------------------------------------
// Le reste du fichier ne sera compilé que si DEBUG_MODE_ENABLED est défini.
#if defined(DEBUG_MODE_ENABLED)
// -----------------------------------------------------------------------------

/**
 * @brief Analyse et exécute une commande reçue via le port Série.
 * @param cmd La commande textuelle à exécuter.
 */
void parseSerialCommand(String cmd) {

  cmd.toLowerCase(); // Met la commande en minuscules
  
  if (cmd.startsWith("help")) {
    Serial.println(F("--- Commandes disponibles ---"));
    Serial.println(F("help          : Affiche cette aide"));
    Serial.println(F("layer [0-2]   : Change la couche active. Ex: 'layer 1'"));
    Serial.println(F("test [1-9]    : Simule un appui sur la touche Kx. Ex: 'test 3'"));
    Serial.println(F("---------------------------"));
  } else if (cmd.startsWith("layer")) {
    int layerNum = cmd.substring(6).toInt();
    if (layerNum >= 0 && layerNum < NUM_LAYERS) {
      currentLayer = layerNum;
      Serial.print(F("Couche active -> "));
      Serial.println(currentLayer);
      char layerMsg[10];
      sprintf(layerMsg, "Layer %d", currentLayer);
      showMessage(layerMsg);
    } else {
      Serial.println(F("Erreur: Numero de couche invalide."));
    }
  } else if (cmd.startsWith("test")) {
    int keyNum = cmd.substring(5).toInt();
    if (keyNum >= 1 && keyNum <= 9) {
      Serial.print(F("Simulation appui -> K"));
      Serial.println(keyNum);
      fireMacro(keyNum - 1);
    } else {
      Serial.println(F("Erreur: Numero de touche invalide (1-9)."));
    }
  } else {
    Serial.println(F("Erreur: Commande inconnue. Tapez 'help'."));
  }
}

/**
 * @brief Gère la lecture des commandes sur le port série.
 * Cette fonction doit être appelée à chaque tour de la boucle loop().
 */
void handleSerialCommands() {
  static String serialBuffer = "";
  while (Serial.available() > 0) {
    char c = Serial.read();
    if (c == '\n') {
      if (serialBuffer.length() > 0) {
        parseSerialCommand(serialBuffer);
      }
      serialBuffer = "";
    } else if (c >= 32) {
      serialBuffer += c;
    }
  }
}

#endif // Fin du bloc #if defined(DEBUG_MODE_ENABLED)

/* ------------------------------ Fin du code -------------------------------- */