/**
 * @file Translations.cpp
 * @brief Provides language translation support for messages in the application.
 * @author rob040@users.github.com,  https://github.com/rob040
 *
 * This module manages translation strings for multiple languages, allowing
 * retrieval of translated messages by message ID and language. It stores
 * translation strings and language metadata in PROGMEM for memory efficiency
 * on embedded platforms. The current language can be set and queried, and
 * translation strings can be accessed as C-strings or Arduino String objects.
 *
 * Features:
 * - Stores translation strings for multiple languages in PROGMEM.
 * - Provides functions to get language codes and names from language enums.
 * - Allows retrieval of translated messages by message ID and language.
 * - Supports setting and getting the current application language.
 * - Includes temporary buffer management for reading strings from PROGMEM.
 *
 * Usage:
 * - Use setCurrentLanguageId() to change the current language.
 * - Use getTranslation() or getTranslationStr() to retrieve translated messages.
 * - Use getLanguageCode() and getLanguageName() for language metadata.
 *
 * @date 2025-09-20
 * @copyright Copyright (c) 2025 rob040@users.github.com
 * @license MIT License (see LICENSE.txt for details)
 */

#include <Arduino.h>
#include <pgmspace.h>
#include <TimeLib.h> // --> https://github.com/PaulStoffregen/Time

#include "Translations.h"
#include "Timestr.h"

/* Global variables */
static lang_t currentLanguage = LANG_EN; // default language is English

// Translation strings in PROGMEM
#define SP(lang, msg, str) const char tr_msg_##msg##_##lang[] PROGMEM = str;
#define TR(msg, l1, l2, l3, l4, l5, l6, l7, l8, l9, l10, l11, l12, l13, l14) \
  SP(EN, msg, l1) \
  SP(NL, msg, l2) \
  SP(DE, msg, l3) \
  SP(FR, msg, l4) \
  SP(IT, msg, l5) \
  SP(ES, msg, l6) \
  SP(PT, msg, l7) \
  SP(DK, msg, l8) \
  SP(NO, msg, l9) \
  SP(SE, msg, l10) \
  SP(PL, msg, l11) \
  SP(CZ, msg, l12) \
  SP(SK, msg, l13) \
  SP(MIN, msg, l14) // minimal (English) language
#include "TranslationStrings.h"
#undef TR
#undef SP

// Translation table of pointers to translation strings in PROGMEM
const char* const translationTable[NUM_MESSAGES][LANG_COUNT] PROGMEM =  {
#define TR(msg, ...) { tr_msg_##msg##_##EN, tr_msg_##msg##_##NL, tr_msg_##msg##_##DE, tr_msg_##msg##_##FR, tr_msg_##msg##_##IT, tr_msg_##msg##_##ES, tr_msg_##msg##_##PT, tr_msg_##msg##_##DK, tr_msg_##msg##_##NO, tr_msg_##msg##_##SE, tr_msg_##msg##_##PL, tr_msg_##msg##_##CZ, tr_msg_##msg##_##SK, tr_msg_##msg##_##MIN },
#include "TranslationStrings.h"
#undef TR
}; // translationTable

// Number of messages to translate
#define NUM_MESSAGES_TRANSLATE (sizeof(translationTable) / sizeof(translationTable[0]))
_Static_assert(NUM_MESSAGES_TRANSLATE == NUM_MESSAGES, "Translation table size does not match number of messages");

// language code comma separated string in PROGMEM
static const char languageCodeTable[]  = {
#define X(lang, code, name) code ","
  LANGUAGELIST
#undef X
};

// language name comma separated string in PROGMEM
static const char languageNameTable[] PROGMEM = {
#define X(lang, code, name) name ","
  LANGUAGELIST
#undef X
};

// Temporary buffer for reading strings from PROGMEM
#define MAX_TRANSLATION_LEN  128
static char translationBuffer[MAX_TRANSLATION_LEN];

// Get two-letter language code string from lang_t enum
const char* getLanguageCode(lang_t lang) {
  static char code[4];
  strncpy(code, findWordInCommaList(languageCodeTable, lang, NUM_LANGUAGES).c_str(), sizeof(code));
  code[sizeof(code)-1] = 0; // ensure null termination
  Serial.printf_P(PSTR("getLanguageCode lang=%d: %s\n"), lang, code);
  return (code[0] != 0) ? code : "en"; // default to "en" if not found
}

// Get lang_t enum from two-letter language code string
lang_t getLanguageIdFromCode(const char* code) {
  if (code != nullptr) {
    String codes = FPSTR(languageCodeTable);
    for (int i = 0; i < NUM_LANGUAGES; i++) {
      String langcode = codes.substring(i*3, i*3+2);
      if (langcode.equalsIgnoreCase(String(code))) {
        return static_cast<lang_t>(i);
      }
    }
  }
  return LANG_EN; // default to English if not found
}

// Get language name string from lang_t enum
const char* getLanguageName(lang_t lang) {
  static char name[16];
  String namestr = FPSTR(languageNameTable);
  namestr = findWordInCommaList(namestr, lang, NUM_LANGUAGES);
  Serial.printf_P(PSTR("getLanguageName lang=%d: %s\n"), lang, namestr.c_str());
  strncpy(name, namestr.c_str(), sizeof(name));
  name[sizeof(name)-1] = 0; // ensure null termination
  return name;
}

// Get translation string from message ID and current language
const char* getTranslation(int msg_id) {
  return getTranslation(msg_id, currentLanguage);
}

// Get translation string from message ID and specified language
const char* getTranslation(int msg_id, lang_t lang) {
  translationBuffer[0] = 0; // clear buffer
  //Serial.printf_P(PSTR("getTranslation msg_id=%d lang=%d\n"), msg_id, lang);
  if ((msg_id >= 0) && (msg_id < NUM_MESSAGES) && (lang >= 0) && (lang < NUM_LANGUAGES)) {
    memcpy_P(translationBuffer, translationTable[msg_id][lang], MAX_TRANSLATION_LEN);
    //Serial.println(translationBuffer);
    return translationBuffer;
  }
  return "unknown";
}

/**
 * @brief Retrieves the translated string for a given message ID in the current language.
 *
 * This function calls getTranslation with the specified message ID and the current language,
 * then converts the result to a String object.
 *
 * @param msg_id The identifier of the message to be translated.
 * @return String The translated message as a String object.
 */
String getTranslationStr(int msg_id) {
  return String(getTranslation(msg_id, currentLanguage));
}

/**
 * @brief Retrieves the translated string for a given message ID and language.
 *
 * This function calls getTranslation with the specified message ID and language,
 * and returns the result as a String object.
 *
 * @param msg_id The identifier of the message to translate.
 * @param lang The language in which to retrieve the translation.
 * @return String containing the translated message.
 */
String getTranslationStr(int msg_id, lang_t lang) {
  return String(getTranslation(msg_id, lang));
}

/**
 * @brief Sets the current language for the application.
 *
 * This function updates the global language setting if the provided language
 * identifier is within the valid range of supported languages.
 *
 * @param lang The language identifier to set (must be between 0 and NUM_LANGUAGES - 1).
 */
void setCurrentLanguageId(lang_t lang) {
  if (lang >= 0 && lang < NUM_LANGUAGES) {
    currentLanguage = lang;
  }
}

/**
 * @brief Retrieves the currently selected language.
 *
 * @return lang_t The current language setting.
 */
lang_t getCurrentLanguageId() {
  return currentLanguage;
}

String getLocaleLongDateStr(time_t t, lang_t lang, bool addweekday, bool addyear, bool imperial_US) {
  String dateStr;
  if (addweekday) {
    dateStr = getDayName(weekday(t)) + ", ";
  }
  if (imperial_US && ((lang == LANG_EN)||(lang == LANG_MIN))) {
    dateStr += getMonthName(month(t)) + " " + String(day(t), DEC) + ",";
  } else {
    dateStr += String(day(t), DEC);
    if ((LANG_DE == lang)||(LANG_DK == lang)||(LANG_NO == lang)) {
      dateStr += ".";
    }
    if ((LANG_ES == lang)||(LANG_PT == lang)) {
      dateStr += " de";
    }
     dateStr += " " + getMonthName(month(t)) ;
  }
  if (addyear) {
    if ((LANG_ES == lang)||(LANG_PT == lang)) {
      dateStr += " de";
    }
    dateStr += " " + String(year(t), DEC);
  }
  return dateStr;
}


// EOF



