/**
 * @file TranslationStrings.h
 * @brief Language definitions X-macros
 * @author rob040@user.github.com,  https://github.com/rob040
 * @date 2025-09-29
 * @copyright Copyright (c) 2025
 * @license Licensed under the MIT license. See LICENSE file in the project root for details.
 *
 * This file contains the language definitions for the supported languages.
 *
 */

 // Supported language list
// X(XX, "xx", "LanguageName")
// XX is the language code,
// "xx" is the language code string used in the Weather URL and in the language selection dropdown list
// LanguageName is the name of the language in that language (it is used in the language selection dropdown list)
#ifndef LANGUAGELIST
#define LANGUAGELIST \
  X(EN, "en", "English") \
  X(NL, "nl", "Nederlands") \
  X(DE, "de", "Deutsch") \
  X(FR, "fr", "Français") \
  X(IT, "it", "Italiano") \
  X(ES, "es", "Español") \
  X(PT, "pt", "Português") \
  X(DK, "dk", "Dansk") \
  X(NO, "no", "Norsk") \
  X(SE, "se", "Svenska") \
  X(PL, "pl", "Polski") \
  X(CZ, "cz", "Čeština") \
  X(SK, "sk", "Slovenčina") \
  X(MIN, "EN", "Minimal English")  // special entry for minimal language, no translation, only ASCII chars

  // Other languages, not yet supported in this project:
  // Eastern european languages: (unlikely to show well with CP437 on 8x8 matrix)
  //X(HU, "hu", "Magyar")
  //X(TR, "tr", "Türkçe")

  // Add more languages here
#endif  // LANGUAGELIST


#ifdef TR
// Messages to translate:

// "hello" max 5 char to fit on 4-tile display
TR(TR_HELLO,
  "hello",      // English
  "hallo",      // Dutch
  "hallo",      // German
  "salut",      // French
  "ciao",       // Italian
  "hola",       // Spanish
  "olá",        // Portuguese
  "hej",        // Danish
  "hei",        // Norwegian
  "hej",        // Swedish
  "cześć",      // Polish
  "ahoj",       // Czech
  "ahoj",       // Slovak
  "hello" )     // minimal (English) language

TR(TR_WMSTARTED,
  "Wifi Manager Started... Please Connect to AP: %s password: %s",                     // English
  "Wifi Manager gestart... Maak verbinding met AP: %s wachtwoord: %s",                 // Dutch
  "Wifi Manager gestartet... Bitte verbinden Sie sich mit AP: %s Passwort: %s",        // German
  "Gestionnaire Wifi démarré... Veuillez vous connecter à AP: %s mot de passe: %s",    // French
  "Gestore Wifi avviato... Collegati a AP: %s password: %s",                           // Italian
  "Administrador de Wifi iniciado... Por favor, conéctese a AP: %s contraseña: %s",    // Spanish
  "Gerenciador de Wifi iniciado... Por favor, conecte-se ao AP: %s senha: %s",         // Portuguese
  "Wifi Manager startet... Opret forbindelse til AP: %s adgangskode: %s",              // Danish
  "Wifi Manager startet... Vennligst koble til AP: %s passord: %s",                    // Norwegian
  "Wifi Manager startar... Vänligen anslut till AP: %s lösenord: %s",                  // Swedish
  "Menedżer Wifi uruchomiony... Połącz się z AP: %s hasło: %s",                        // Polish
  "Správce WiFi spuštěn... Připojte se prosím k AP: %s heslo: %s",                     // Czech
  "Správca WiFi spustený... Pripojte sa prosím k AP: %s heslo: %s",                    // Slovak
  "Connect to AP: %s password: %s" )                                                   // minimal (English) language

TR(TR_WMCONFIGURED,
  "Wifi Manager Configured... Rebooting",                     // English
  "Wifi Manager geconfigureerd... Herstarten",                // Dutch
  "Wifi Manager konfiguriert... Neustart",                    // German
  "Gestionnaire Wifi configuré... Redémarrage",               // French
  "Gestore Wifi configurato... Riavvio",                      // Italian
  "Administrador de Wifi configurado... Reiniciando",         // Spanish
  "Gerenciador de Wifi configurado... Reiniciando",           // Portuguese
  "Wifi Manager konfigureret... Genstarter",                  // Danish
  "Wifi Manager konfigurert... Omstart",                      // Norwegian
  "Wifi Manager konfigurerat... Startar om",                  // Swedish
  "Menedżer Wifi skonfigurowany... Uruchamianie ponowne",     // Polish
  "Správce WiFi nakonfigurován... Restartuji",                // Czech
  "Správca WiFi nakonfigurovaný... Reštartujem",              // Slovak
  "Configured... Rebooting" )                                 // minimal (English) language

//"Connected to WiFi AP %s  Connect to" + " http://CLOCK-XXXXXX.local IP: 192.168.0.1"
TR(TR_CONNECTEDWIFI,
  "Connected to WiFi AP %s  Connect to ",                     // English
  "Verbonden met WiFi AP %s  Verbinden met ",                 // Dutch
  "Verbunden mit WiFi AP %s  Verbinden mit ",                 // German
  "Connecté au WiFi AP %s  Connecter à ",                     // French
  "Connesso a WiFi AP %s  Collegati a ",                      // Italian
  "Conectado a WiFi AP %s  Conéctese a ",                     // Spanish
  "Conectado ao WiFi AP %s  Conecte-se a ",                   // Portuguese
  "Forbundet til WiFi AP %s  Opret forbindelse til ",         // Danish
  "Koblet til WiFi AP %s  Koble til ",                        // Norwegian
  "Ansluten till WiFi AP %s  Anslut till ",                   // Swedish
  "Połączono z WiFi AP %s  Połącz z ",                        // Polish
  "Připojeno k WiFi AP %s  Připojit k ",                      // Czech
  "Pripojené k WiFi AP %s  Pripojiť k ",                      // Slovak
  "Connected to WiFi AP %s  Connect to " )                    // minimal (English) language

  TR(TR_TEMPERATURE,
  "Temperature",            // English
  "Temperatuur",            // Dutch
  "Temperatur" ,            // German
  "Température",            // French
  "Temperatura",            // Italian
  "Temperatura",            // Spanish
  "Temperatur",             // Portuguese
  "Temperatur",             // Danish
  "Temperatur",             // Norwegian
  "Temperatur",             // Swedish
  "Temperatura",            // Polish
  "Teplota",                // Czech
  "Teplota",                // Slovak
  "" )                      // minimal (English) language

TR(TR_HIGHLOW,
  "High/Low",               // English
  "Hoog/Laag",              // Dutch
  "Hoch/Niedrig",           // German
  "Haut/Bas",               // French
  "Alto/Basso",             // Italian
  "Alto/Baixo",             // Spanish
  "Alta/Baixa",             // Portuguese
  "Hög/Låg",                // Danish
  "Høy/Lav",                // Norwegian
  "Hög/Låg",                // Swedish
  "Wysoki/Niski",           // Polish
  "Vysoká/Nízká",           // Czech
  "Vysoká/Nízka",           // Slovak
  "" )                      // minimal (English) language

TR(TR_HUMIDITY,
  "Humidity",               // English
  "Vochtigheid",            // Dutch
  "Luftfeuchtigkeit",       // German
  "Humidité",               // French
  "Umidità",                // Italian
  "Humedad",                // Spanish
  "Umidade",                // Portuguese
  "Luftfugtighed",          // Danish
  "Luftfuktighet",          // Norwegian
  "Luftfuktighet",          // Swedish
  "Wilgotność",             // Polish
  "Vlhkost",                // Czech
  "Vlhkosť",                // Slovak
  "" )                      // minimal (English) language

TR(TR_WIND,
  "Wind",                   // English
  "Wind",                   // Dutch
  "Wind",                   // German
  "Vent",                   // French
  "Vento",                  // Italian
  "Viento",                 // Spanish
  "Vento",                  // Portuguese
  "Vind",                   // Danish
  "Vind",                   // Norwegian
  "Vind",                   // Swedish
  "Wiatr",                  // Polish
  "Vítr",                   // Czech
  "Vietor",                 // Slovak
  "" )                      // minimal (English) language

TR(TR_PRESSURE,
  "Pressure",               // English
  "Luchtdruk",              // Dutch
  "Druck",                  // German
  "Pression",               // French
  "Pressione",              // Italian
  "Presión",                // Spanish
  "Pressão",                // Portuguese
  "Tryk",                   // Danish
  "Trykk",                  // Norwegian
  "Tryck",                  // Swedish
  "Ciśnienie",              // Polish
  "Tlak",                   // Czech
  "Tlak",                   // Slovak
  "" )                      // minimal (English) language

//n.u. TR(TR_DAYNAMESHORT,
//  "Sun,Mon,Tue,Wed,Thu,Fri,Sat",
//  "Zo,Ma,Di,Wo,Do,Vr,Za",
//  "So,Mo,Di,Mi,Do,Fr,Sa" )

TR(TR_DAYNAMEFULL,
  "Sunday,Monday,Tuesday,Wednesday,Thursday,Friday,Saturday",                         // English
  "Zondag,Maandag,Dinsdag,Woensdag,Donderdag,Vrijdag,Zaterdag",                       // Dutch
  "Sonntag,Montag,Dienstag,Mittwoch,Donnerstag,Freitag,Samstag",                      // German
  "Dimanche,Lundi,Mardi,Mercredi,Jeudi,Vendredi,Samedi",                              // French
  "Domenica,Lunedì,Martedì,Mercoledì,Giovedì,Venerdì,Sabato",                         // Italian
  "Domingo,Lunes,Martes,Miércoles,Jueves,Viernes,Sábado",                             // Spanish
  "Domingo,Segunda-feira,Terça-feira,Quarta-feira,Quinta-feira,Sexta-feira,Sábado",   // Portuguese
  "Søndag,Mandag,Tirsdag,Onsdag,Torsdag,Fredag,Lørdag",                               // Danish
  "Søndag,Mandag,Tirsdag,Onsdag,Torsdag,Fredag,Lørdag",                               // Norwegian
  "Söndag,Måndag,Tisdag,Onsdag,Torsdag,Fredag,Lördag",                                // Swedish
  "Niedziela,Poniedziałek,Wtorek,Środa,Czwartek,Piątek,Sobota",                       // Polish
  "Neděle,Pondělí,Úterý,Středa,Čtvrtek,Pátek,Sobota",                                 // Czech
  "Nedeľa,Pondelok,Utorok,Streda,Štvrtok,Piatok,Sobota",                              // Slovak
  "" )                                                                                // minimal (English) language

TR(TR_MONTHNAMESHORT,
  "Jan,Feb,Mar,Apr,May,Jun,Jul,Aug,Sep,Oct,Nov,Dec",       // English
  "Jan,Feb,Mrt,Apr,Mei,Jun,Jul,Aug,Sep,Okt,Nov,Dec",       // Dutch
  "Jan,Feb,Mär,Apr,Mai,Jun,Jul,Aug,Sep,Okt,Nov,Dez",       // German
  "Jan,Fév,Mars,Avr,Mai,Juin,Juil,Août,Sep,Oct,Nov,Déc",   // French
  "Gen,Feb,Mar,Apr,Mag,Giu,Lug,Ago,Set,Ott,Nov,Dic",       // Italian
  "Ene,Feb,Mar,Abr,May,Jun,Jul,Ago,Sep,Oct,Nov,Dic",       // Spanish
  "Jan,Fev,Mar,Abr,Mai,Jun,Jul,Ago,Set,Out,Nov,Dez",       // Portuguese
  "Jan,Feb,Mar,Apr,Maj,Jun,Jul,Aug,Sep,Okt,Nov,Dec",       // Danish
  "Jan,Feb,Mar,Apr,Mai,Jun,Jul,Aug,Sep,Okt,Nov,Des",       // Norwegian
  "Jan,Feb,Mar,Apr,Maj,Jun,Jul,Aug,Sep,Okt,Nov,Dec",       // Swedish
  "Sty,Lut,Mar,Kwi,Maj,Cze,Lip,Sie,Wrz,Paź,Lis,Gru",       // Polish
  "Led,Úno,Bře,Dub,Kvě,Čvn,Čvc,Srp,Zář,Říj,Lis,Pro",       // Czech
  "Jan,Feb,Mar,Apr,Máj,Jún,Júl,Aug,Sep,Okt,Nov,Dec",       // Slovak
  "Jan,Feb,Mar,Apr,May,Jun,Jul,Aug,Sep,Oct,Nov,Dec" )      // minimal (English) language

TR(TR_WINDDIRECTIONS,
  "N,NNE,NE,ENE,E,ESE,SE,SSE,S,SSW,SW,WSW,W,WNW,NW,NNW",   // English
  "N,NNO,NO,ONO,O,OZO,ZO,ZZO,Z,ZZW,ZW,WZW,W,WNW,NW,NNW",   // Dutch
  "N,NNO,NO,ONO,O,OSO,SO,SSO,S,SSW,SW,WSW,W,WNW,NW,NNW",   // German
  "N,NNE,NE,ENE,E,ESE,SE,SSE,S,SSO,SO,OSO,O,ONO,NO,NNO",   // French
  "N,NNE,NE,ENE,E,ESE,SE,SSE,S,SSO,SO,OSO,O,ONO,NO,NNO",   // Italian
  "N,NNE,NE,ENE,E,ESE,SE,SSE,S,SSO,SO,OSO,O,ONO,NO,NNO",   // Spanish
  "N,NNE,NE,ENE,E,ESE,SE,SSE,S,SSO,SO,OSO,O,ONO,NO,NNO",   // Portuguese
  "N,NNO,NO,ONO,O,OSO,SO,SSO,S,SSV,SV,VSV,V,VNV,NV,NNV",   // Danish
  "N,NNO,NO,ONO,O,OSO,SO,SSO,S,SSV,SV,VSV,V,VNV,NV,NNV",   // Norwegian
  "N,NNO,NO,ONO,O,OSO,SO,SSO,S,SSV,SV,VSV,V,VNV,NV,NNV",   // Swedish
  "N,NNE,NE,ENE,E,ESE,SE,SSE,S,SSW,SW,WSW,W,WNW,NW,NNW",   // Polish (use EN; Polish uses 'P' for North AND south: "Północ,Wschód,Południe,Zachód")
  "S,SSV,SV,VSV,V,VJV,JV,JJV,J,JZJ,JZ,ZJZ,Z,ZSZ,SZ,SSZ",   // Czech
  "S,SSV,SV,VSV,V,VJV,JV,JJV,J,JZJ,JZ,ZJZ,Z,ZSZ,SZ,SSZ",   // Slovak
  "N,NNE,NE,ENE,E,ESE,SE,SSE,S,SSW,SW,WSW,W,WNW,NW,NNW" )  // minimal (English) language

TR(TR_PLEASESETOWMKEY,
  "Please provide an API key for weather.",                 // English
  "Geef een API-sleutel op voor het weer.",                 // Dutch
  "Bitte geben Sie einen API-Schlüssel für das Wetter an.", // German
  "Veuillez fournir une clé API pour la météo.",            // French
  "Fornisci una chiave API per il meteo.",                  // Italian
  "Proporcione una clave API para el clima.",               // Spanish
  "Forneça uma chave API para o clima.",                    // Portuguese
  "Angi en API-nøkkel for været.",                          // Danish
  "Vennligst oppgi en API-nøkkel for været.",               // Norwegian
  "Vänligen ange en API-nyckel för vädret.",                // Swedish
  "Proszę podać klucz API dla pogody.",                     // Polish
  "Zadejte prosím API klíč pro počasí.",                    // Czech
  "Zadajte prosím API kľúč pre počasie.",                   // Slovak
  "Set OWM API key." )                                      // minimal (English) language

TR(TR_PLEASESETLOCATION,
  "Please set location for weather.",                       // English
  "Stel de locatie in voor het weer.",                      // Dutch
  "Bitte Standort für das Wetter festlegen.",               // German
  "Veuillez définir l'emplacement pour la météo.",          // French
  "Imposta la posizione per il meteo.",                     // Italian
  "Establezca la ubicación para el clima.",                 // Spanish
  "Defina a localização para o clima.",                     // Portuguese
  "Angiv venligst placering for vejret.",                   // Danish
  "Vennligst angi plassering for været.",                   // Norwegian
  "Vänligen ange plats för vädret.",                        // Swedish
  "Proszę ustawić lokalizację dla pogody.",                 // Polish
  "Nastavte prosím umístění pro počasí.",                   // Czech
  "Nastavte prosím umiestnenie pre počasie.",               // Slovak
  "Set location for weather." )                             // minimal (English) language

TR(TR_CONNECTIONFORWEATHERFAILED,
  "Connection for weather data failed",                     // English
  "Verbinding voor weergegevens mislukt",                   // Dutch
  "Verbindung für Wetterdaten fehlgeschlagen",              // German
  "La connexion aux données météo a échoué",                // French
  "Connessione per i dati meteo non riuscita",              // Italian
  "La conexión para los datos meteorológicos falló",        // Spanish
  "Conexão para dados meteorológicos falhou",               // Portuguese
  "Forbindelse til vejrdata mislykkedes",                   // Danish
  "Tilkobling for værdata mislyktes",                       // Norwegian
  "Anslutning för väderdata misslyckades",                  // Swedish
  "Połączenie z danymi pogodowymi nie powiodło się",        // Polish
  "Selhání připojení k datům o počasí",                     // Czech
  "Zlyhanie pripojenia k údajom o počasí",                  // Slovak  ️
  "Weather data connection failed" )                        // minimal (English) language

TR(TR_TIMEOUTONWEATHERCLIENTDATARECEIVE,
  "TIMEOUT on weather data receive",                  // English
  "TIMEOUT bij ontvangst van weergegevens",           // Dutch
  "TIMEOUT beim Empfang von Wetterdaten",             // German
  "TIMEOUT lors de la réception des données météo",   // French
  "TIMEOUT durante la ricezione dei dati meteo",      // Italian
  "TIMEOUT al recibir datos meteorológicos",          // Spanish
  "TIMEOUT ao receber dados meteorológicos",          // Portuguese
  "TIMEOUT ved modtagelse af vejrdata",               // Danish
  "TIMEOUT ved mottak av værdata",                    // Norwegian
  "TIMEOUT vid mottagning av väderdata",              // Swedish
  "TIMEOUT podczas odbierania danych pogodowych",     // Polish
  "ČASOVÝ LIMIT při přijímání dat o počasí",          // Czech
  "ČASOVÝ LIMIT pri prijímaní údajov o počasí",       // Slovak
  "Timeout on weather data receive" )                 // minimal (English) language


#endif  // TR