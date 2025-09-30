/**
 * @file Translations.cpp
 * @author rob040@users.github.com
 * @brief Language translation support
 * @date 2025-09-20
 * @copyright Copyright (c) 2025 rob040@users.github.com
 * This code is licensed under MIT license (see LICENSE.txt for details)
 */

#pragma once
#include <Arduino.h>
#include "pgmspace.h"
//#include "TranslationStrings.h"

/* enum of supported languages */
#include "TranslationStrings.h"
#define X(lang, code, name) lang,
typedef enum lang_e {
  LANGUAGELIST
  LANG_COUNT
} lang_t;
#undef X

/* enum of message IDs */
#define TR(msg, ...) msg,
typedef enum msgid_e {
  #include "TranslationStrings.h"
  NUM_MESSAGES
} msgid_t;
#undef TR

// Number of supported languages
#define NUM_LANGUAGES LANG_COUNT

/* Function prototypes */
void setLanguage(lang_t lang);
lang_t getLanguage();
lang_t getLanguageFromCode(const char* code);
const char* getLanguageCode(lang_t lang);
const char* getLanguageName(lang_t lang);
const char* getTranslation(int msg_id);
const char* getTranslation(int msg_id, lang_t lang);
String getTranslationStr(int msg_id);
String getTranslationStr(int msg_id, lang_t lang);
// EOF
