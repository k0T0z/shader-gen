/*********************************************************************************/
/*                                                                               */
/*  Copyright (C) 2024 Seif Kandil (k0T0z)                                       */
/*                                                                               */
/*  This file is a part of the ENIGMA Development Environment.                   */
/*                                                                               */
/*                                                                               */
/*  ENIGMA is free software: you can redistribute it and/or modify it under the  */
/*  terms of the GNU General Public License as published by the Free Software    */
/*  Foundation, version 3 of the license or any later version.                   */
/*                                                                               */
/*  This application and its source code is distributed AS-IS, WITHOUT ANY       */
/*  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS    */
/*  FOR A PARTICULAR PURPOSE. See the GNU General Public License for more        */
/*  details.                                                                     */
/*                                                                               */
/*  You should have recieved a copy of the GNU General Public License along      */
/*  with this code. If not, see <http://www.gnu.org/licenses/>                   */
/*                                                                               */
/*  ENIGMA is an environment designed to create games and other programs with a  */
/*  high-level, fully compilable language. Developers of ENIGMA or anything      */
/*  associated with ENIGMA are in no way responsible for its users or            */
/*  applications created by its users, or damages caused by the environment      */
/*  or programs made in the environment.                                         */
/*                                                                               */
/*********************************************************************************/

// This is a general purpose error handling macros header file so feel free to use it
// in your projects.

#ifndef ERROR_MACROS_HPP
#define ERROR_MACROS_HPP

#ifdef SHADER_GEN_DEBUG
#include <iostream>
#endif  // SHADER_GEN_DEBUG

#include <cstdint>
#include <string>

namespace shadergen_logger {
inline static void print_error(const char* function, const char* file, const int& line, const char* error,
                               const std::string& message) noexcept {
#ifdef SHADER_GEN_DEBUG
  std::cerr << "ERROR: " << error << "\n";
  std::cerr << "   at: " << function << " (" << file << ":" << line << ")" << "\n";
  std::cerr << "   " << message << "\n";
#endif  // SHADER_GEN_DEBUG
}

inline static void print_index_error(const char* function, const char* file, const int& line, const int64_t& index,
                                     const int64_t& size, const std::string& message,
                                     const bool& fatal = false) noexcept {
#ifdef SHADER_GEN_DEBUG
  std::cerr << (fatal ? "FATAL: " : "ERROR: ") << "Index " << index << " is out of bounds (" << size << ")." << "\n";
  std::cerr << "   at: " << function << " (" << file << ":" << line << ")" << "\n";
  std::cerr << "   " << message << "\n";
#endif  // SHADER_GEN_DEBUG
}

inline static void print_warning(const char* function, const char* file, const int& line, const char* warning,
                                 const std::string& message) noexcept {
#ifdef SHADER_GEN_DEBUG
  std::cerr << "WARNING: " << warning << "\n";
  std::cerr << "   at: " << function << " (" << file << ":" << line << ")" << "\n";
  std::cerr << "   " << message << "\n";
#endif  // SHADER_GEN_DEBUG
}

inline static void print_debug(const char* function, const char* file, const int& line,
                               const std::string& message) noexcept {
#ifdef SHADER_GEN_DEBUG
  std::cerr << "DEBUG at: " << function << " (" << file << ":" << line << ")" << "\n";
  std::cerr << "   " << message << "\n";
#endif  // SHADER_GEN_DEBUG
}

inline static void flush_stdout() noexcept {
#ifdef SHADER_GEN_DEBUG
  fflush(stdout);
#endif  // SHADER_GEN_DEBUG
}
}  // namespace shadergen_logger

// https://gcc.gnu.org/onlinedocs/cpp/Stringizing.html
#ifndef STRNGIZE
#define STRNGIZE(S) #S
#define XSTRNGIZE(S) STRNGIZE(S)
#endif  // STRNGIZE

// Don't use GENERATE_TRAP() directly, should only be used be the macros below.
#ifdef _MSC_VER
#define GENERATE_TRAP() __debugbreak()
#else
#define GENERATE_TRAP() __builtin_trap()
#endif

/**
 * Error Handling Macros
 * 
 * @brief These macros prioritize application stability over strict error enforcement,
 *        operating opposite to assert(). While assert() halts execution on failure,
 *        these macros attempt to handle errors gracefully.
 * 
 * @c WHY? To maintain program stability and avoid crashes, handle non-fatal errors
 *         and invalid data safely, return fallback values to allow continued
 * 		   execution, and provide non-silent variants for detailed debugging info.
 *
 * @note Uses @c ((void)0) to enforce semicolon usage after macro calls
 * @note Wraps conditions in if statements to prevent expansion issues
 *       Example: if (<CONDITION>) SILENT_FAIL_AND_RETURN(); // Works correctly without 
 * 		 braces
 *
 * @note @c SILENT_ macros are for non-critical errors and invalid data
 * @note You can add a message to any macro with no @c SILENT_ prefix
 * @note You can add a return value to any macro with @c NON_VOID suffix
 * @note @c CRITICAL_ macros will halt execution with a trap on failure. A critical
 *       error is a fatal error that should never occur in a stable application.
 */

#define SILENT_VALIDATE_INDEX(I, SIZE) \
  if ((I < 0) || (I >= SIZE)) {        \
    return;                            \
  } else                               \
    ((void)0)

#define SILENT_VALIDATE_INDEX_NON_VOID(I, SIZE, RETURN_VAL) \
  if ((I < 0) || (I >= SIZE)) {                             \
    return RETURN_VAL;                                      \
  } else                                                    \
    ((void)0)

#define VALIDATE_INDEX(I, SIZE, DETAILS)                                                     \
  if ((I < 0) || (I >= SIZE)) {                                                              \
    shadergen_logger::print_index_error(__FUNCTION__, __FILE__, __LINE__, I, SIZE, DETAILS); \
    return;                                                                                  \
  } else                                                                                     \
    ((void)0)

#define VALIDATE_INDEX_NON_VOID(I, SIZE, RETURN_VAL, DETAILS)                                \
  if ((I < 0) || (I >= SIZE)) {                                                              \
    shadergen_logger::print_index_error(__FUNCTION__, __FILE__, __LINE__, I, SIZE, DETAILS); \
    return RETURN_VAL;                                                                       \
  } else                                                                                     \
    ((void)0)

#define VALIDATE_CRITICAL_INDEX(I, SIZE, DETAILS)                                                  \
  if ((I < 0) || (I >= SIZE)) {                                                                    \
    shadergen_logger::print_index_error(__FUNCTION__, __FILE__, __LINE__, I, SIZE, DETAILS, true); \
    shadergen_logger::flush_stdout();                                                              \
    GENERATE_TRAP();                                                                               \
  } else                                                                                           \
    ((void)0)

//////////////////////////////////
//////////////////////////////////
//////////////////////////////////

#define SILENT_VALIDATE_UNSIGNED_INDEX(I, SIZE) \
  if (I >= SIZE) {                              \
    return;                                     \
  } else                                        \
    ((void)0)

#define SILENT_VALIDATE_UNSIGNED_INDEX_NON_VOID(I, SIZE, RETURN_VAL) \
  if (I >= SIZE) {                                                   \
    return RETURN_VAL;                                               \
  } else                                                             \
    ((void)0)

#define VALIDATE_UNSIGNED_INDEX(I, SIZE, DETAILS)                                            \
  if (I >= SIZE) {                                                                           \
    shadergen_logger::print_index_error(__FUNCTION__, __FILE__, __LINE__, I, SIZE, DETAILS); \
    return;                                                                                  \
  } else                                                                                     \
    ((void)0)

#define VALIDATE_UNSIGNED_INDEX_NON_VOID(I, SIZE, RETURN_VAL, DETAILS)                       \
  if (I >= SIZE) {                                                                           \
    shadergen_logger::print_index_error(__FUNCTION__, __FILE__, __LINE__, I, SIZE, DETAILS); \
    return RETURN_VAL;                                                                       \
  } else                                                                                     \
    ((void)0)

#define VALIDATE_CRITICAL_UNSIGNED_INDEX(I, SIZE, DETAILS)                                         \
  if (I >= SIZE) {                                                                                 \
    shadergen_logger::print_index_error(__FUNCTION__, __FILE__, __LINE__, I, SIZE, DETAILS, true); \
    shadergen_logger::flush_stdout();                                                              \
    GENERATE_TRAP();                                                                               \
  } else                                                                                           \
    ((void)0)

//////////////////////////////////
//////////////////////////////////
//////////////////////////////////

#define SILENT_CHECK_PARAM_NULLPTR(P) \
  if (P == nullptr) {                 \
    return;                           \
  } else                              \
    ((void)0)

#define SILENT_CHECK_PARAM_NULLPTR_NON_VOID(P, RETURN_VAL) \
  if (P == nullptr) {                                      \
    return RETURN_VAL;                                     \
  } else                                                   \
    ((void)0)

#define CHECK_PARAM_NULLPTR(P, DETAILS)                                                                          \
  if (P == nullptr) {                                                                                            \
    shadergen_logger::print_error(__FUNCTION__, __FILE__, __LINE__, "Parameter \"" STRNGIZE(P) "\" is nullptr.", \
                                  DETAILS);                                                                      \
    return;                                                                                                      \
  } else                                                                                                         \
    ((void)0)

#define CHECK_PARAM_NULLPTR_NON_VOID(P, RETURN_VAL, DETAILS)                                                     \
  if (P == nullptr) {                                                                                            \
    shadergen_logger::print_error(__FUNCTION__, __FILE__, __LINE__, "Parameter \"" STRNGIZE(P) "\" is nullptr.", \
                                  DETAILS);                                                                      \
    return RETURN_VAL;                                                                                           \
  } else                                                                                                         \
    ((void)0)

//////////////////////////////////
//////////////////////////////////
//////////////////////////////////

#define SILENT_CHECK_CONDITION_TRUE(C) \
  if (C) {                             \
    return;                            \
  } else                               \
    ((void)0)

#define SILENT_CHECK_CONDITION_TRUE_NON_VOID(C, RETURN_VAL) \
  if (C) {                                                  \
    return RETURN_VAL;                                      \
  } else                                                    \
    ((void)0)

#define CHECK_CONDITION_TRUE(C, DETAILS)                                                                      \
  if (C) {                                                                                                    \
    shadergen_logger::print_error(__FUNCTION__, __FILE__, __LINE__, "Condition \"" STRNGIZE(C) "\" is true.", \
                                  DETAILS);                                                                   \
    return;                                                                                                   \
  } else                                                                                                      \
    ((void)0)

#define CHECK_CONDITION_TRUE_NON_VOID(C, RETURN_VAL, DETAILS)                                                          \
  if (C) {                                                                                                             \
    shadergen_logger::print_error(__FUNCTION__, __FILE__, __LINE__,                                                    \
                                  "Condition \"" STRNGIZE(C) "\" is true. Returning: " STRNGIZE(RETURN_VAL), DETAILS); \
    return RETURN_VAL;                                                                                                 \
  } else                                                                                                               \
    ((void)0)

#define CHECK_CRITICAL_CONDITION_TRUE(C, DETAILS)                                                                    \
  if (C) {                                                                                                           \
    shadergen_logger::print_error(__FUNCTION__, __FILE__, __LINE__, "FATAL: Condition \"" STRNGIZE(C) "\" is true.", \
                                  DETAILS);                                                                          \
    shadergen_logger::flush_stdout();                                                                                \
    GENERATE_TRAP();                                                                                                 \
  } else                                                                                                             \
    ((void)0)

//////////////////////////////////
//////////////////////////////////
//////////////////////////////////

#define SILENT_CONTINUE_IF_TRUE(C) \
  if (C) {                         \
    continue;                      \
  } else                           \
    ((void)0)

#define CONTINUE_IF_TRUE(C, DETAILS)                                                              \
  if (C) {                                                                                        \
    shadergen_logger::print_error(__FUNCTION__, __FILE__, __LINE__,                               \
                                  "Condition \"" STRNGIZE(C) "\" is true. Continuing.", DETAILS); \
    continue;                                                                                     \
  } else                                                                                          \
    ((void)0)

#define SILENT_BREAK_IF_TRUE(C) \
  if (C) {                      \
    break;                      \
  } else                        \
    ((void)0)

#define BREAK_IF_TRUE(C, DETAILS)                                                               \
  if (C) {                                                                                      \
    shadergen_logger::print_error(__FUNCTION__, __FILE__, __LINE__,                             \
                                  "Condition \"" STRNGIZE(C) "\" is true. Breaking.", DETAILS); \
    break;                                                                                      \
  } else                                                                                        \
    ((void)0)

//////////////////////////////////
//
// Generic error macros.
//
//////////////////////////////////

#define SILENT_FAIL_AND_RETURN() \
  if (true) {                    \
    return;                      \
  } else                         \
    ((void)0)

#define SILENT_FAIL_AND_RETURN_NON_VOID(RETURN_VAL) \
  if (true) {                                       \
    return RETURN_VAL;                              \
  } else                                            \
    ((void)0)

#define FAIL_AND_RETURN(DETAILS)                                                                         \
  if (true) {                                                                                            \
    shadergen_logger::print_error(__FUNCTION__, __FILE__, __LINE__, "Method/function failed.", DETAILS); \
    return;                                                                                              \
  } else                                                                                                 \
    ((void)0)

#define FAIL_AND_RETURN_NON_VOID(RETURN_VAL, DETAILS)                                                   \
  if (true) {                                                                                           \
    shadergen_logger::print_error(__FUNCTION__, __FILE__, __LINE__,                                     \
                                  "Method/function failed. Returning: " STRNGIZE(RETURN_VAL), DETAILS); \
    return RETURN_VAL;                                                                                  \
  } else                                                                                                \
    ((void)0)

#define ERROR_PRINT(DETAILS)                                                      \
  if (true) {                                                                     \
    shadergen_logger::print_error(__FUNCTION__, __FILE__, __LINE__, "", DETAILS); \
  } else                                                                          \
    ((void)0)

#define WARN_PRINT(DETAILS)                                                         \
  if (true) {                                                                       \
    shadergen_logger::print_warning(__FUNCTION__, __FILE__, __LINE__, "", DETAILS); \
  } else                                                                            \
    ((void)0)

#define DEBUG_PRINT(DETAILS)                                                  \
  if (true) {                                                                 \
    shadergen_logger::print_debug(__FUNCTION__, __FILE__, __LINE__, DETAILS); \
  } else                                                                      \
    ((void)0)

#define CRASH_NOW(DETAILS)                                                                                      \
  if (true) {                                                                                                   \
    shadergen_logger::print_error(__FUNCTION__, __FILE__, __LINE__, "FATAL: Method/function failed.", DETAILS); \
    shadergen_logger::flush_stdout();                                                                           \
    GENERATE_TRAP();                                                                                            \
  } else                                                                                                        \
    ((void)0)

#endif  // ERROR_MACROS_HPP
