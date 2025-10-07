/**
 * ===================================================================================
 * FIRMWARE POUR MACROPAD AVANCÉ AVEC INTERFACE WEB
 * ===================================================================================
 * Auteur      : [ANCHER.Patrick]
 * Pseudo      : [ZelTroN2k3]
 * Date        : 07/10/2025
 * Version     : 1.0.4 (avec interface web)
 *
 * * Description:
 * Firmware pour un macropad basé sur une carte Arduino Nano ESP32, entièrement
 * configurable via une interface web intuitive. Gère 9 touches sur 3 couches,
 * un encodeur rotatif, un écran OLED avec menus et statuts, et un screensaver.
 *
 * * 👉 Fichiers de configuration :
 * - La configuration des macros se fait via l'INTERFACE WEB (enregistrée dans config.json).
 * - Les identifiants Wi-Fi et la version sont dans la section "Informations sur le Firmware" de ce fichier.
 *
 * * Composants :
 * - 1x Arduino Nano ESP32
 * - 9x Switches mécaniques (Cherry MX ou similaire)
 * - 1x Encodeur rotatif EC11
 * - 1x Écran OLED I2C 0.91" (128x32, driver SSD1306)
 *
 * * 👉 Comment personnaliser ?
 * 1. Connectez-vous à l'adresse IP du macropad depuis un navigateur web.
 * 2. Utilisez les onglets pour naviguer entre les couches (Layers).
 * 3. Remplissez les champs "Commande", "Libellé" et "Icône" pour chaque touche.
 * 4. Cliquez sur "Sauvegarder". Les changements sont appliqués instantanément.
 */

#include "FS.h"
#include "SPIFFS.h"
#include <ArduinoJson.h>
#include <WiFi.h>              
#include <ESPAsyncWebServer.h> 
#include "webpage.h"           
#include "time.h"
#include "base64.h"

// --- Informations sur le Firmware ---
#define FIRMWARE_VERSION "v1.0.4"
const char* AUTHOR = "ANCHER.P";

// --- Identifiants Wi-Fi ---
const char* ssid = "Freebox-ZelTroN2k3";  // VOTRE_SSID_WIFI
const char* password = "7AC429D2DEZ2"; // VOTRE_MOT_DE_PASSE_WIFI

// --- Configuration du temps (NTP) ---
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 3600;   // Décalage GMT en secondes (3600 = +1h pour Paris)
const int   daylightOffset_sec = 3600; // Décalage pour l'heure d'été

// --- Serveur Web ---
AsyncWebServer server(80); // Adresse IP: DE VOTRE ESP32: EXEMPLE: 192.168.1.8

// On crée les objets pour chaque périphérique USB
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
String fr2en(String text) {
  int i = 0;
  String _en = " =qwertyuiopasdfghjkl;zxcvbnQWERTYUIOPASDFGHJKL:ZXCVBNm,./M<>?1234567890!@#$%^&*()",
         _fr = " =azertyuiopqsdfghjklmwxcvbnAZERTYUIOPQSDFGHJKLMWXCVBN,;:!?./ & \"'(- _  1234567890",
         str = "";
  while (text[i] != 0) {
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
// Structure pour stocker les détails d'une macro
struct Macro { String cmd; String label; String iconName; };

// Identifiants symboliques pour les touches
enum KeyIds { K1, K2, K3, K4, K5, K6, K7, K8, K9 };
bool lastKeyState[NUM_KEYS];  // Tableau pour l'anti-rebond des touches

// Variables pour la gestion des couches (layers)
const uint8_t NUM_LAYERS = 3;  // Nombre total de couches (0, 1, 2)
uint8_t currentLayer = 0;      // Couche actuellement active

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
const unsigned long LONG_PRESS_DURATION = 500;  // 500 ms pour un appui long

// --- Variables pour le menu de configuration ---
bool isInMenu = false;
const char* menuItems[] = { "Luminosite", "Menu Principal", "Sortir" };
const uint8_t NUM_MENU_ITEMS = sizeof(menuItems) / sizeof(menuItems[0]);
int8_t selectedMenuItem = 0;
uint8_t oledBrightness = 255;                         // 255 = max, 0 = min
const unsigned long VERY_LONG_PRESS_DURATION = 1500;  // 1.5s pour entrer dans le menu
bool blockMenuClickUntilRelease = false;              // Sécurité pour éviter le clic instantané

// --- Variables pour le screensaver "Falling Stars" ---
const unsigned long SCREENSAVER_DELAY = SLEEP_DELAY - 15000; // Se lance 15s avant la veille
bool isScreensaverActive = false;
// Structure pour représenter une seule étoile
struct Star { int x; int y;};
const int NUM_STARS = 40; // Nombre d'étoiles à afficher. Ajustez selon vos goûts !
Star stars[NUM_STARS];    // Le tableau qui contiendra toutes nos étoiles

// --- Variables pour le menu d'icônes ---
enum ProgramState { STATE_ICON_MENU, STATE_NORMAL };
ProgramState currentState = STATE_ICON_MENU;

// --- Variables pour la configuration web ---
Macro macros[NUM_LAYERS][NUM_KEYS]; // Tableau pour stocker les macros chargées du fichier

/**
 * @brief Fait le lien entre un nom d'icône (String) et ses données bitmap.
 * @param iconName Le nom de l'icône tel que stocké dans le JSON.
 * @return Un pointeur vers les données de l'icône, ou une icône par défaut.
 */
const unsigned char* getIconByName(String iconName) {
  if (iconName.equalsIgnoreCase("icon_notpad_16x16")) return icon_notpad_16x16;
  if (iconName.equalsIgnoreCase("icon_menu_parametres_16x16")) return icon_menu_parametres_16x16;
  if (iconName.equalsIgnoreCase("icon_menu_clavier_16x16")) return icon_menu_clavier_16x16;
  if (iconName.equalsIgnoreCase("icon_menu_audio_16x16")) return icon_menu_audio_16x16;
  if (iconName.equalsIgnoreCase("icon_menu_parametres_16x16")) return icon_menu_parametres_16x16;
  if (iconName.equalsIgnoreCase("icon_menu_wifi_16x16")) return icon_menu_wifi_16x16;
  if (iconName.equalsIgnoreCase("icon_menu_bluetooth_16x16")) return icon_menu_bluetooth_16x16;
  // Ajoutez d'autres 'if' ici pour toutes vos icônes...
  
  // Si aucune icône n'est trouvée, on retourne une icône par défaut
  return icon_menu_macros_16x16;
}

// --- NOUVEAU : Structure et liste des icônes pour l'aide ---
struct Icon {
  const char* name;
  const unsigned char* data;
};
// Remplace l'ancien tableau iconNames[]
const Icon iconList[] = {
  {"icon_menu_pc_16x16", icon_menu_pc_16x16},
  {"icon_menu_souris_16x16", icon_menu_souris_16x16},
  {"icon_menu_clavier_16x16", icon_menu_clavier_16x16},
  {"icon_menu_audio_16x16", icon_menu_audio_16x16},
  {"icon_menu_wifi_16x16", icon_menu_wifi_16x16},
  {"icon_menu_bluetooth_16x16", icon_menu_bluetooth_16x16},
  {"icon_menu_macros_16x16", icon_menu_macros_16x16},
  {"icon_menu_parametres_16x16", icon_menu_parametres_16x16},
  {"icon_layer_0_16x16", icon_layer_0_16x16},
  {"icon_layer_1_16x16", icon_layer_1_16x16},
  {"icon_layer_2_16x16", icon_layer_2_16x16},
  {"icon_layer_3_16x16", icon_layer_3_16x16},
  {"icon_mute_16x16", icon_mute_16x16},
  {"icon_sound_16x16", icon_sound_16x16},
  {"icon_scroll_16x16", icon_scroll_16x16},
  {"icon_undo_redo_16x16", icon_undo_redo_16x16},
  {"icon_notpad_16x16", icon_notpad_16x16},
  {"icon_wifi_on_16x16", icon_wifi_on_16x16},
  {"icon_wifi_off_16x16", icon_wifi_off_16x16},
  {"icon_json_on_16x16", icon_json_on_16x16},
  {"icon_json_off_16x16", icon_json_off_16x16}
  // Si vous ajoutez une icône, ajoutez son nom ici
};


/* ---------------------------------------------------------- */
/* ---------------- PROTOTYPES DES FONCTIONS ---------------- */
/* ---------------------------------------------------------- */
// Fonctions existantes
void showMessage(const char* msg);
void showVolume();
void wakeUp();
void drawMenu();
void setBrightness(uint8_t brightness);
void fireMacro(uint8_t id);
void handleSerialCommands();
void drawIconMenu();
void returnToIconMenu();
void handleIconMenu();
void handleNormalMode();
String fr2en(String text);

// NOUVEAUX prototypes à ajouter
void displayInfoScreen();
void createDefaultConfig();
void loadConfig();
const unsigned char* getIconByName(String iconName);
void displayCustomAction(const char* label, const unsigned char* icon);
void initStars();
void drawScreensaver();

#include "debug.h"   // Dépend des fonctions du fichier principal (qui seront déclarées juste après)
#include "iconmenu.h"

/* =============================================== */
/* ==================== SETUP ==================== */
/* =============================================== */
void setup() {
/*
* Note: 
* Pour que ce changement prenne effet, vous devrez soit supprimer le fichier config.json 
* de votre macropad (en ajoutant une ligne SPIFFS.remove("/config.json"); temporairement dans setup), 
* soit mettre à jour les valeurs manuellement via l'interface web.
*/

// Initialisation de la communication Série en premier pour le débogage
#if defined(DEBUG_MODE_ENABLED)
  Serial.begin(115200);
  // Attente de la connexion du port série
  //while (!Serial);  // <-- "On commente //" "ou supprime" cette ligne pour un démarrage autonome
#endif
  randomSeed(analogRead(A3)); // <-- AJOUTER CETTE LIGNE (Utilisez une broche non connectée si possible)

  // --- Initialisation de SPIFFS et chargement de la configuration ---
  if(!SPIFFS.begin(true)){
    #if defined(DEBUG_MODE_ENABLED)
      Serial.println("ERREUR : Impossible de monter le systeme de fichiers SPIFFS !");
    #endif
    return;
  }
  createDefaultConfig(); // Crée le fichier de config par défaut si besoin
  loadConfig();          // Charge les macros depuis le fichier
  // --- Fin du bloc ---

  // --- Connexion Wi-Fi et configuration du serveur ---
  #if defined(DEBUG_MODE_ENABLED)
    Serial.print("Connexion a ");
    Serial.println(ssid);
  #endif
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    #if defined(DEBUG_MODE_ENABLED)
      Serial.print(".");
    #endif
  }
  #if defined(DEBUG_MODE_ENABLED)
    Serial.println("\nWiFi connecte !");
    Serial.print("Adresse IP: ");
    Serial.println(WiFi.localIP());
  #endif

  // --- Initialisation du temps ---
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  // ======================================== 
  // --- Définition des routes du serveur ---
  // ========================================

  // Route principale (/) : envoie la page web
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    // 1. On charge la page depuis la PROGMEM dans une String en RAM
    String page = FPSTR(index_html);

    // 2. On effectue les remplacements manuellement
    page.replace("%FIRMWARE_VERSION%", FIRMWARE_VERSION);
    page.replace("%AUTHOR%", AUTHOR);
    page.replace("%IP_ADDRESS%", WiFi.localIP().toString());

    // 3. On envoie la page modifiée
    request->send(200, "text/html", page);
  });

  // Route (/config) : envoie le fichier de configuration au format JSON
  server.on("/config", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/config.json", "application/json");
  });

  // Route (/save) : reçoit les nouvelles données et sauvegarde
  server.on("/save", HTTP_POST, [](AsyncWebServerRequest *request){}, NULL, 
    [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total){
      
      File configFile = SPIFFS.open("/config.json", "w");
      if (configFile) {
        configFile.write(data, len);
        configFile.close();
        loadConfig(); // On recharge la config en direct !
        request->send(200, "text/plain", "OK");
      } else {
        request->send(500, "text/plain", "Erreur sauvegarde fichier");
      }
  });

  // VERSION de la route /getIconList
  server.on("/getIconList", HTTP_GET, [](AsyncWebServerRequest *request){
    JsonDocument doc;
    JsonArray array = doc.to<JsonArray>();

    for (const Icon& icon : iconList) {
      // Crée une image BMP 1-bit 16x16 en mémoire
      // Taille: 62 octets d'en-tête + (16 lignes * 4 octets/ligne) = 126 octets
      uint8_t bmp[126];
      memset(bmp, 0, sizeof(bmp));

      // En-tête de fichier BMP
      bmp[0] = 'B'; bmp[1] = 'M';
      *((uint32_t*)(bmp + 2)) = sizeof(bmp); // Taille totale
      *((uint32_t*)(bmp + 10)) = 62;

      // En-tête DIB (inchangé)
      *((uint32_t*)(bmp + 14)) = 40;
      *((int32_t*)(bmp + 18)) = 16;
      *((int32_t*)(bmp + 22)) = 16;
      *((uint16_t*)(bmp + 26)) = 1;
      *((uint16_t*)(bmp + 28)) = 1;
      *((uint32_t*)(bmp + 46)) = 2;

      // Palette de couleurs (noir, blanc) (inchangée)
      *((uint32_t*)(bmp + 54)) = 0x00000000;
      *((uint32_t*)(bmp + 58)) = 0x00FFFFFF;
      
      // CORRECTION : Copie les données de l'icône avec 2 octets de padding par ligne
      for(int i = 0; i < 16; i++) {
        // Pointeur vers le début de la ligne de destination dans le buffer BMP
        uint8_t* dest_row = bmp + 62 + (i * 4);
        // Pointeur vers le début de la ligne source dans les données PROGMEM (en inversant l'ordre des lignes)
        const unsigned char* src_row = icon.data + ((15 - i) * 2);
        
        // Copie les 2 octets de pixels
        dest_row[0] = src_row[0];
        dest_row[1] = src_row[1];
        // Les 2 octets de padding (dest_row[2] et dest_row[3]) sont déjà à 0 grâce à memset
      }

      // Encode le BMP en Base64
      std::string base64_str = base64_encode(bmp, sizeof(bmp));
      String data_url = "data:image/bmp;base64," + String(base64_str.c_str());

      // Ajoute l'objet à notre tableau JSON
      JsonObject obj = array.add<JsonObject>();
      obj["name"] = icon.name;
      obj["data"] = data_url;
    }

    String response;
    serializeJson(doc, response);
    request->send(200, "application/json", response);
  });
  
  // --- Route pour servir le favicon.ico ---
  server.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "image/x-icon", favicon_ico_16x16, sizeof(favicon_ico_16x16));
  });

  // Gère toutes les AUTRES requêtes non trouvées
  server.onNotFound([](AsyncWebServerRequest *request){
    request->send(404, "text/plain", "Not found");
  });

  // Démarrage du serveur
  server.begin();
  // ===========================
  // --- Fin du bloc serveur ---
  // ===========================


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
        Serial.println(F("ERREUR: Initialisation de l'ecran OLED impossible !"));
    #endif
    for (;;)
      ;
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

  // --- Affichage de l'écran d'informations ---
  displayInfoScreen();
  delay(5000); // Pause de 5 secondes

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
      initStars(); // On initialise les étoiles
    }
    drawScreensaver();
  }
}

/* ================================================================== */
/* ==================== DEFINITION DES FONCTIONS ==================== */
/* ================================================================== */

/**
 * @brief Affiche l'écran d'informations au démarrage (IP, Date, Heure).
 */
void displayInfoScreen() {
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    #if defined(DEBUG_MODE_ENABLED)
      Serial.println("Impossible de récupérer l'heure");
    #endif
    showMessage("Erreur Heure");
    delay(2000);
    return;
  }
  
  char timeString[9]; // HH:MM:SS
  strftime(timeString, sizeof(timeString), "%H:%M:%S", &timeinfo);
  
  char dateString[11]; // DD/MM/YYYY
  strftime(dateString, sizeof(dateString), "%d/%m/%Y", &timeinfo);

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  
  display.setCursor(0, 0);
  display.print("IP: " + WiFi.localIP().toString());
  
  display.setCursor(0, 12);
  display.print("Date: ");
  display.print(dateString);
  
  display.setCursor(0, 24);
  display.print("Heure: ");
  display.print(timeString);
  
  display.display();
}

/**
 * @brief Crée un fichier de configuration par défaut s'il n'existe pas,
 * en utilisant la nouvelle structure d'objets (commande, libellé, icône).
 */
void createDefaultConfig() {
  // Cette fonction ne s'exécute que si le fichier n'existe pas déjà
  if (!SPIFFS.exists("/config.json")) {
    #if defined(DEBUG_MODE_ENABLED)
      Serial.println("Fichier config.json non trouve, creation du fichier par defaut...");
    #endif
    
    File configFile = SPIFFS.open("/config.json", "w");
    if (!configFile) {
      #if defined(DEBUG_MODE_ENABLED)
        Serial.println("Erreur durant la création du fichier de configuration.");
      #endif
      return;
    }

    // On utilise ArduinoJson pour créer la structure JSON complète
    JsonDocument doc;

    // --- Layer 0 ---
    JsonObject L0K1 = doc["L0K1"].to<JsonObject>();
    L0K1["cmd"] = "run:notepad3.exe";
    L0K1["label"] = "NOTEPAD.3";
    L0K1["icon"] = "icon_notpad_16x16";

    JsonObject L0K2 = doc["L0K2"].to<JsonObject>();
    L0K2["cmd"] = "run:cmd";
    L0K2["label"] = "(CMD) Commandes";
    L0K2["icon"] = "icon_menu_parametres_16x16";

    // --- Layer 1 (Edition) ---
    JsonObject L1K1 = doc["L1K1"].to<JsonObject>();
    L1K1["cmd"] = "key:c";
    L1K1["label"] = "Copier";
    L1K1["icon"] = "icon_menu_clavier_16x16";

    JsonObject L1K2 = doc["L1K2"].to<JsonObject>();
    L1K2["cmd"] = "key:v";
    L1K2["label"] = "Coller";
    L1K2["icon"] = "icon_menu_clavier_16x16";

    // --- Layer 2 (Media) ---
    JsonObject L2K1 = doc["L2K1"].to<JsonObject>();
    L2K1["cmd"] = "media:play";
    L2K1["label"] = "Play/Pause";
    L2K1["icon"] = "icon_menu_audio_16x16";
    
    // Écrit le JSON joliment formaté dans le fichier
    if (serializeJsonPretty(doc, configFile) == 0) {
      #if defined(DEBUG_MODE_ENABLED)
        Serial.println("Erreur durant l'écriture dans le fichier de configuration.");
      #endif
    }
    
    configFile.close();
  }
}

/**
 * @brief Lit le fichier config.json et charge les macros dans le tableau global.
 */
void loadConfig() {
  File configFile = SPIFFS.open("/config.json", "r");
  if (!configFile) {
    #if defined(DEBUG_MODE_ENABLED)
      Serial.println("Impossible d'ouvrir config.json pour la lecture.");
    #endif
    return;
  }

  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, configFile);
  if (error) {
    #if defined(DEBUG_MODE_ENABLED)
      Serial.print("Erreur de parsing JSON : ");
      Serial.println(error.c_str());
    #endif
    configFile.close();
    return;
  }

  // Boucle pour remplir le tableau `macros` depuis le document JSON
  for (int l = 0; l < NUM_LAYERS; l++) {
    for (int k = 0; k < NUM_KEYS; k++) {
      String key = "L" + String(l) + "K" + String(k + 1);
      
      if (!doc[key].isNull() && doc[key].is<JsonObject>()) {
        JsonObject obj = doc[key];
        macros[l][k].cmd = obj["cmd"].as<String>();
        macros[l][k].label = obj["label"].as<String>();
        macros[l][k].iconName = obj["icon"].as<String>();
      } else {
        // Si la macro n'est pas définie ou n'est pas un objet, on la vide
        macros[l][k].cmd = "";
        macros[l][k].label = "";
        macros[l][k].iconName = "";
      }
    }
  }
  
  configFile.close();
  #if defined(DEBUG_MODE_ENABLED)
    Serial.println("Configuration des macros chargee avec succes depuis config.json.");
  #endif
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
        if (accum >= STEPS_PER_DETENT) {
          selectedMenuItem = (selectedMenuItem + 1) % NUM_MENU_ITEMS;
          accum = 0;
        } else if (accum <= -STEPS_PER_DETENT) {
          selectedMenuItem = (selectedMenuItem - 1 + NUM_MENU_ITEMS) % NUM_MENU_ITEMS;
          accum = 0;
        }
        drawMenu();
      }
      lastAB = nowAB;
    }

    if (digitalRead(ENC_SW) == HIGH) { blockMenuClickUntilRelease = false; }

    if (!blockMenuClickUntilRelease && digitalRead(ENC_SW) == LOW && (millis() - lastClickMs > CLICK_DEBOUNCE_MS)) {
      lastClickMs = millis();
      switch (selectedMenuItem) {
        case 0:  // Option "Luminosite"
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
                  display.clearDisplay();
                  display.setCursor(0, 8);
                  display.print("Luminosite: ");
                  display.print(oledBrightness);
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
        case 1:  // Option "Menu Principal"
          returnToIconMenu();
          break;
        case 2:  // Option "Sortir"
          isInMenu = false;
          showVolume();
          break;
      }
    }
  } else {
    // --- MODE DE FONCTIONNEMENT HORS-MENU ---
    for (uint8_t i = 0; i < NUM_KEYS; i++) {
      bool pressed = (digitalRead(KEY_PINS[i]) == LOW);
      if (pressed && !lastKeyState[i]) {
        wakeUp();
        fireMacro(i);
      }
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
            case MODE_UNDO_REDO:
              if (direction > 0) sendCombo_Ctrl('y');
              else sendCombo_Ctrl('z');
              break;
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
    if (buttonState && !lastButtonState) {
      buttonPressStartTime = millis();
      actionTakenOnPress = false;
    }
    if (buttonState && !actionTakenOnPress && (millis() - buttonPressStartTime > VERY_LONG_PRESS_DURATION)) {
      isInMenu = !isInMenu;
      if (isInMenu) {
        wakeUp();
        selectedMenuItem = 0;
        blockMenuClickUntilRelease = true;
        drawMenu();
      } else {
        showVolume();
      }
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
            case MODE_VOLUME:
              muted = !muted;
              Consumer.press(HID_USAGE_CONSUMER_MUTE);
              Consumer.release();
              break;
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
 * @brief Affiche un écran personnalisé avec une icône et un libellé.
 * @param label Le texte à afficher.
 * @param icon Un pointeur vers les données bitmap de l'icône 16x16.
 */
void displayCustomAction(const char* label, const unsigned char* icon) {
  display.clearDisplay();
  // On dessine l'icône
  display.drawBitmap((SCREEN_WIDTH - 16) / 2, 14, icon, 16, 16, SSD1306_WHITE);
  // On prépare et dessine le texte
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 2); // Position X, Y
  // Centre le texte approximativement
  int16_t x1, y1;
  uint16_t w, h;
  display.getTextBounds(label, 0, 0, &x1, &y1, &w, &h);
  display.setCursor((SCREEN_WIDTH - w) / 2, 2);
  display.print(label);
  // On affiche le tout
  display.display();
}

/**
 * @brief Point central pour déclencher les macros.
 * @param id L'identifiant de la touche pressée (de 0 à 8).
 * @note Les actions sont lues et interprétées depuis le tableau 'macros' chargé de config.json.
 */
void fireMacro(uint8_t id) {
  // --- GESTION DU CHANGEMENT DE COUCHE (inchangé) ---
  if (id == K9) {
    currentLayer = (currentLayer + 1) % NUM_LAYERS;
    char layerMsg[10];
    sprintf(layerMsg, "Layer %d", currentLayer);
    showMessage(layerMsg);
    return;
  }

  // --- NOUVELLE LOGIQUE : Lit et interprète la macro depuis le tableau en mémoire ---
  
  // 1. Récupère la structure complète de la macro pour la touche et la couche actuelles
  Macro macro = macros[currentLayer][id];
  String command = macro.cmd;
  command.trim(); // Nettoie les espaces avant/après

  // 2. Vérifie si une commande est assignée
  if (command.length() == 0) {
    showMessage("Touche non assignee");
    return;
  }

  // 3. Sépare la commande en préfixe et en valeur (ex: "run" et "notepad.exe")
  int separatorIndex = command.indexOf(':');
  String prefix = "";
  String value = "";

  if (separatorIndex != -1) {
    prefix = command.substring(0, separatorIndex);
    value = command.substring(separatorIndex + 1);
  } else {
    // S'il n'y a pas de préfixe, on considère par défaut que c'est une commande "run"
    prefix = "run";
    value = command;
  }

  // 4. Prépare le libellé et l'icône pour l'affichage
  // S'il y a un libellé personnalisé, on l'utilise. Sinon, on affiche la valeur de la commande.
  String labelToShow = macro.label.length() > 0 ? macro.label : value;
  // On récupère le pointeur vers les données de l'icône grâce à son nom
  const unsigned char* iconToShow = getIconByName(macro.iconName);


  // 5. Exécute l'action correspondante et affiche le retour visuel personnalisé
  if (prefix.equalsIgnoreCase("run")) {
    openViaRun(value.c_str());
    displayCustomAction(labelToShow.c_str(), iconToShow);
  } else if (prefix.equalsIgnoreCase("key")) {
    sendCombo_Ctrl(value.charAt(0));
    displayCustomAction(labelToShow.c_str(), iconToShow);
  } else if (prefix.equalsIgnoreCase("keyshift")) {
    sendCombo_CtrlShift(value.charAt(0));
    displayCustomAction(labelToShow.c_str(), iconToShow);
  } else if (prefix.equalsIgnoreCase("media")) {
    if (value.equalsIgnoreCase("play")) Consumer.press(HID_USAGE_CONSUMER_PLAY_PAUSE);
    else if (value.equalsIgnoreCase("next")) Consumer.press(HID_USAGE_CONSUMER_SCAN_NEXT);
    else if (value.equalsIgnoreCase("prev")) Consumer.press(HID_USAGE_CONSUMER_SCAN_PREVIOUS);
    Consumer.release();
    displayCustomAction(labelToShow.c_str(), iconToShow);
  } else if (prefix.equalsIgnoreCase("text")) {
    Keyboard.print(value);
    displayCustomAction(labelToShow.c_str(), iconToShow);
  } else {
    showMessage("Prefixe inconnu");
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
        (SCREEN_WIDTH - 16) / 2,  // Centré en X
        20,                       // En haut de l'écran en Y
        icon_scroll_16x16,        // Données de l'icône
        16, 16,                   // Largeur et hauteur
        SSD1306_WHITE);
      break;
    case MODE_UNDO_REDO:
      display.print(F("Mode: Undo/Redo"));
      // On affiche l'icône de undo_redo et le texte
      display.drawBitmap(
        (SCREEN_WIDTH - 16) / 2,  // Centré en X
        20,                       // En haut de l'écran en Y
        icon_undo_redo_16x16,     // Données de l'icône
        16, 16,                   // Largeur et hauteur
        SSD1306_WHITE);
      break;
  }

  // Affiche la barre de volume ou l'état Mute, UNIQUEMENT en mode Volume
  if (currentEncoderMode == MODE_VOLUME) {
    if (muted) {
      // 1. On affiche d'abord le texte en haut de l'écran
      display.setCursor(0, 10);  // Position X (environ centré), Position Y (en haut)
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
        (SCREEN_WIDTH - 16) / 2 + 40,  // Centré en X
        1,                             // En haut de l'écran en Y
        icon_sound_16x16,              // Données de l'icône
        16, 16,                        // Largeur et hauteur
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
  isScreensaverActive = false;  // screensaver
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
 * @brief Initialise les étoiles à des positions aléatoires sur l'écran.
 * Appelée une seule fois au début de l'économiseur d'écran.
 */
void initStars() {
  for (int i = 0; i < NUM_STARS; i++) {
    stars[i].x = random(0, SCREEN_WIDTH);
    stars[i].y = random(0, SCREEN_HEIGHT);
  }
}

/**
 * @brief Dessine et anime l'effet de chute d'étoiles.
 */
void drawScreensaver() {
  display.clearDisplay();

  // Pour chaque étoile dans notre tableau...
  for (int i = 0; i < NUM_STARS; i++) {
    // 1. On la fait tomber d'un pixel
    stars[i].y++;

    // 2. Si elle sort de l'écran par le bas...
    if (stars[i].y >= SCREEN_HEIGHT) {
      // ... on la replace en haut, à une position X aléatoire.
      stars[i].y = 0;
      stars[i].x = random(0, SCREEN_WIDTH);
    }
    
    // 3. On dessine le pixel de l'étoile à sa nouvelle position
    display.drawPixel(stars[i].x, stars[i].y, SSD1306_WHITE);
  }
  
  display.display();
  delay(10); // Ralentit un peu l'animation pour un effet plus fluide
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
  isInMenu = false;                // S'assure qu'on quitte le menu de configuration
  currentState = STATE_ICON_MENU;  // Règle l'état du programme sur le menu d'icônes
  wakeUp();                        // Réveille l'écran et réinitialise le minuteur d'inactivité
  drawIconMenu();                  // Dessine le menu d'icônes
}

/* ------------------------------ Fin du code -------------------------------- */
