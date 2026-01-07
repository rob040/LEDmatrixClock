/**
 * @file Translations.cpp
 * @brief Language translation support
 * @author rob040@users.github.com,  https://github.com/rob040
 * @date 2025-09-20
 * @copyright Copyright (c) 2025 rob040@users.github.com
 * @license This code is licensed under MIT license (see LICENSE.txt for details)
 */

#pragma once
#include <Arduino.h>
//#include "pgmspace.h"
//#include "TranslationStrings.h"

/* enum of supported languages */
#include "TranslationStrings.h"
#define X(lang, code, name) LANG_##lang,
typedef enum lang_e {
  LANGUAGELIST
  LANG_COUNT
} lang_t;
#undef X

// Dummy definition to satisfy VCcode Intellisense
#define TR(msg, ...)

/* enum of message IDs */
typedef enum msgid_e {
  #undef TR
  #define TR(msg, ...) msg,
  #include "TranslationStrings.h"
  NUM_MESSAGES
} msgid_t;
#undef TR

// Number of supported languages
#define NUM_LANGUAGES LANG_COUNT

/* Function prototypes */
void setCurrentLanguageId(lang_t lang);
lang_t getCurrentLanguageId();
lang_t getLanguageIdFromCode(const char* code);
const char* getLanguageCode(lang_t lang);
const char* getLanguageName(lang_t lang);
const char* getTranslation(int msg_id);
const char* getTranslation(int msg_id, lang_t lang);
String getTranslationStr(int msg_id);
String getTranslationStr(int msg_id, lang_t lang);
String getLocaleLongDateStr(time_t t, lang_t lang, bool addweekday, bool addyear, bool imperial_US);

// EOF
