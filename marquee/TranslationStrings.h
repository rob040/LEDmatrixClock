/**
 * @file languages.h
 * @author  rob040@user.github.com
 * @brief Language definitions X-macros
 * @date 2025-09-29
 * @copyright Copyright (c) 2025
 * Licensed under the MIT license. See LICENSE file in the project root for details.
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
  X(NO, "no", "Norsk") \
  X(SE, "se", "Svenska") \
  X(PL, "pl", "Polski") \
  X(MIN, "EN", "Minimal English")  // special entry for minimal language, no translation, only ASCII chars

  // Other languages, not yet supported in this project:
  // Eastern european languages: (unlikely to show well with CP437 on 8x8 matrix)
  //X(CZ, "cz", "Čeština")
  //X(SL, "sl", "Slovenščina")
  //X(HU, "hu", "Magyar")
  //X(TR, "tr", "Türkçe")

  // Add more languages here
#endif  // LANGUAGELIST


#ifdef TR
// Messages to translate:

// "hello" max 5 char to fit on 4-tile display
TR(TR_HELLO,
  "hello",
  "hallo",
  "hallo",
  "bjour",
  "ciao",
  "hola",
  "olá",
  "hei",
  "hej",
  "cześć",
  "hello" ) // minimal (English) language

TR(TR_WMSTARTED,
  "Wifi Manager Started... Please Connect to AP: %s password: %s",
  "Wifi Manager gestart... Maak verbinding met AP: %s wachtwoord: %s",
  "Wifi Manager gestartet... Bitte verbinden Sie sich mit AP: %s Passwort: %s",
  "Gestionnaire Wifi démarré... Veuillez vous connecter à AP: %s mot de passe: %s",
  "Gestore Wifi avviato... Collegati a AP: %s password: %s",
  "Administrador de Wifi iniciado... Por favor, conéctese a AP: %s contraseña: %s",
  "Gerenciador de Wifi iniciado... Por favor, conecte-se ao AP: %s senha: %s",
  "Wifi Manager startet... Vennligst koble til AP: %s passord: %s",
  "Wifi Manager startar... Vänligen anslut till AP: %s lösenord: %s",
  "Menedżer Wifi uruchomiony... Połącz się z AP: %s hasło: %s",
  "Connect to AP: %s password: %s" ) // minimal (English) language

TR(TR_WMCONFIGURED,
  "Wifi Manager Configured... Rebooting",
  "Wifi Manager geconfigureerd... Herstarten",
  "Wifi Manager konfiguriert... Neustart",
  "Gestionnaire Wifi configuré... Redémarrage",
  "Gestore Wifi configurato... Riavvio",
  "Administrador de Wifi configurado... Reiniciando",
  "Gerenciador de Wifi configurado... Reiniciando",
  "Wifi Manager konfigurert... Omstart",
  "Wifi Manager konfigurerat... Startar om",
  "Menedżer Wifi skonfigurowany... Uruchamianie ponowne",
  "Configured... Rebooting" ) // minimal (English) language


  TR(TR_TEMPERATURE,
  "Temperature",
  "Temperatuur",
  "Temperatur" ,
  "Température",
  "Temperatura",
  "Temperatura",
  "Temperatur",
  "Temperatur",
  "Temperatur",
  "Temperatura",
  "" ) // minimal (English) language

TR(TR_HIGHLOW,
  "High/Low",
  "Hoog/Laag",
  "Hoch/Niedrig",
  "Haut/Bas",
  "Alto/Basso",
  "Alto/Baixo",
  "Alta/Baixa",
  "Høy/Lav",
  "Hög/Låg",
  "Wysoki/Niski",
  "" ) // minimal (English) language

TR(TR_HUMIDITY,
  "Humidity",
  "Vochtigheid",
  "Luftfeuchtigkeit",
  "Humidité",
  "Umidità",
  "Humedad",
  "Umidade",
  "Luftfuktighet",
  "Luftfuktighet",
  "Wilgotność",
  "" ) // minimal (English) language

TR(TR_WIND,
  "Wind",
  "Wind",
  "Wind",
  "Vent",
  "Vento",
  "Viento",
  "Vento",
  "Vind",
  "Vind",
  "Wiatr",
  "" ) // minimal (English) language

TR(TR_PRESSURE,
  "Pressure",
  "Luchtdruk",
  "Druck",
  "Pression",
  "Pressione",
  "Presión",
  "Pressão",
  "Trykk",
  "Tryck",
  "Ciśnienie",
  "" ) // minimal (English) language

//n.u. TR(TR_DAYNAMESHORT,
//  "Sun,Mon,Tue,Wed,Thu,Fri,Sat",
//  "Zo,Ma,Di,Wo,Do,Vr,Za",
//  "So,Mo,Di,Mi,Do,Fr,Sa" )

TR(TR_DAYNAMEFULL,
  "Sunday,Monday,Tuesday,Wednesday,Thursday,Friday,Saturday",
  "Zondag,Maandag,Dinsdag,Woensdag,Donderdag,Vrijdag,Zaterdag",
  "Sonntag,Montag,Dienstag,Mittwoch,Donnerstag,Freitag,Samstag",
  "Dimanche,Lundi,Mardi,Mercredi,Jeudi,Vendredi,Samedi",
  "Domenica,Lunedì,Martedì,Mercoledì,Giovedì,Venerdì,Sabato",
  "Domingo,Lunes,Martes,Miércoles,Jueves,Viernes,Sábado",
  "Domingo,Segunda-feira,Terça-feira,Quarta-feira,Quinta-feira,Sexta-feira,Sábado",
  "Søndag,Mandag,Tirsdag,Onsdag,Torsdag,Fredag,Lørdag",
  "Söndag,Måndag,Tisdag,Onsdag,Torsdag,Fredag,Lördag",
  "Niedziela,Poniedziałek,Wtorek,Środa,Czwartek,Piątek,Sobota",
  "" ) // minimal (English) language

TR(TR_MONTHNAMESHORT,
  "Jan,Feb,Mar,Apr,May,Jun,Jul,Aug,Sep,Oct,Nov,Dec",
  "Jan,Feb,Mrt,Apr,Mei,Jun,Jul,Aug,Sep,Okt,Nov,Dec",
  "Jan,Feb,Mär,Apr,Mai,Jun,Jul,Aug,Sep,Okt,Nov,Dez",
  "Jan,Fév,Mars,Avr,Mai,Juin,Juil,Août,Sep,Oct,Nov,Déc",
  "Gen,Feb,Mar,Apr,Mag,Giu,Lug,Ago,Set,Ott,Nov,Dic",
  "Ene,Feb,Mar,Abr,May,Jun,Jul,Ago,Sep,Oct,Nov,Dic",
  "Jan,Fev,Mar,Abr,Mai,Jun,Jul,Ago,Set,Out,Nov,Dez",
  "Jan,Feb,Mar,Apr,Mai,Jun,Jul,Aug,Sep,Okt,Nov,Des",
  "Jan,Feb,Mar,Apr,Maj,Jun,Jul,Aug,Sep,Okt,Nov,Dec",
  "Sty,Lut,Mar,Kwi,Maj,Cze,Lip,Sie,Wrz,Paź,Lis,Gru",
  "Jan,Feb,Mar,Apr,May,Jun,Jul,Aug,Sep,Oct,Nov,Dec" ) // minimal (English) language

//n.u. TR(TR_MONTHNAMEFULL,
//  "January,February,March,April,May,June,July,August,September,October,November,December",
//  "Januari,Februari,Maart,April,Mei,Juni,Juli,Augustus,September,Oktober,November,December",
//  "Januar,Februar,März,April,Mai,Juni,Juli,August,September,Oktober,November,Dezember" )

TR(TR_WINDDIRECTIONS,
  "N,NNE,NE,ENE,E,ESE,SE,SSE,S,SSW,SW,WSW,W,WNW,NW,NNW",
  "N,NNE,NE,ENE,E,ESE,SE,SSE,S,SSW,SW,WSW,W,WNW,NW,NNW",
  "N,NNO,NO,ONO,O,OZO,ZO,ZZO,Z,ZZW,ZW,WZW,W,WNW,NW,NNW",
  "N,NNE,NE,ENE,E,ESE,SE,SSE,S,SSW,SW,WSW,W,WNW,NW,NNW",
  "N,NNE,NE,ENE,E,ESE,SE,SSE,S,SSW,SW,WSW,W,WNW,NW,NNW",
  "N,NNE,NE,ENE,E,ESE,SE,SSE,S,SSW,SW,WSW,W,WNW,NW,NNW",
  "N,NNE,NE,ENE,E,ESE,SE,SSE,S,SSW,SW,WSW,W,WNW,NW,NNW",
  "N,NNE,NE,ENE,E,ESE,SE,SSE,S,SSW,SW,WSW,W,WNW,NW,NNW",
  "N,NNE,NE,ENE,E,ESE,SE,SSE,S,SSW,SW,WSW,W,WNW,NW,NNW",
  "N,NNE,NE,ENE,E,ESE,SE,SSE,S,SSW,SW,WSW,W,WNW,NW,NNW",
  "N,NNE,NE,ENE,E,ESE,SE,SSE,S,SSW,SW,WSW,W,WNW,NW,NNW" ) // minimal (English) language

TR(TR_PLEASESETOWMKEY,
  "Please provide an API key for weather.",
  "Geef een API-sleutel op voor het weer.",
  "Bitte geben Sie einen API-Schlüssel für das Wetter an.",
  "Veuillez fournir une clé API pour la météo.",
  "Fornisci una chiave API per il meteo.",
  "Proporcione una clave API para el clima.",
  "Forneça uma chave API para o clima.",
  "Vennligst oppgi en API-nøkkel for været.",
  "Vänligen ange en API-nyckel för vädret.",
  "Proszę podać klucz API dla pogody.",
  "Set OWM API key." ) // minimal (English) language

TR(TR_PLEASESETLOCATION,
  "Please set location for weather.",
  "Stel de locatie in voor het weer.",
  "Bitte Standort für das Wetter festlegen.",
  "Veuillez définir l'emplacement pour la météo.",
  "Imposta la posizione per il meteo.",
  "Establezca la ubicación para el clima.",
  "Defina a localização para o clima.",
  "Vennligst angi plassering for været.",
  "Vänligen ange plats för vädret.",
  "Proszę ustawić lokalizację dla pogody.",
  "Set location for weather." ) // minimal (English) language

TR(TR_CONNECTIONFORWEATHERFAILED,
  "Connection for weather data failed",
  "Verbinding voor weergegevens mislukt",
  "Verbindung für Wetterdaten fehlgeschlagen",
  "La connexion aux données météo a échoué",
  "Connessione per i dati meteo non riuscita",
  "La conexión para los datos meteorológicos falló",
  "Conexão para dados meteorológicos falhou",
  "Tilkobling for værdata mislyktes",
  "Anslutning för väderdata misslyckades",
  "Połączenie z danymi pogodowymi nie powiodło się",
  "Weather data connection failed" ) // minimal (English) language

TR(TR_TIMEOUTONWEATHERCLIENTDATARECEIVE,
  "TIMEOUT on weatherClient data receive",
  "TIMEOUT bij ontvangst van weatherClient-gegevens",
  "TIMEOUT beim Empfang von weatherClient-Daten",
  "TIMEOUT lors de la réception des données weatherClient",
  "TIMEOUT durante la ricezione dei dati weatherClient",
  "TIMEOUT al recibir datos de weatherClient",
  "TIMEOUT ao receber dados do weatherClient",
  "TIMEOUT ved mottak av weatherClient-data",
  "TIMEOUT vid mottagning av weatherClient-data",
  "TIMEOUT podczas odbierania danych weatherClient",
  "Timeout on weather data receive" ) // minimal (English) language


#endif  // TR