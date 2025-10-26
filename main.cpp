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

#include "main.hpp"

#include <QApplication>
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <filesystem>
#include <fstream>
#include <mutex>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <csignal>
#include <exception>

#include "gui/model/schema/visual_shader.pb.h"

#include "gui/controller/visual_shader_editor.hpp"
#include "gui/model/message_model.hpp"
#include "generator/visual_shader_generator.hpp"

// The leak reports are not enough, it is better to use UMDH
// https://learn.microsoft.com/en-us/windows-hardware/drivers/debugger/umdh
#if defined(SHADER_GEN_DEBUG) && defined(_MSC_VER)
#include <crtdbg.h>
#endif  // SHADER_GEN_DEBUG && _MSC_VER

using VisualShader = gui::model::schema::VisualShader;

static std::mutex g_log_mutex;
static std::filesystem::path g_log_dir;

// create a timestamp string "YYYYMMDD_HHMMSS"
static std::string timestamp_now() {
  using namespace std::chrono;
  auto now = system_clock::now();
  std::time_t t = system_clock::to_time_t(now);
  std::tm tm;
#if defined(_WIN32)
  localtime_s(&tm, &t);
#else
  localtime_r(&t, &tm);
#endif
  std::ostringstream oss;
  oss << std::put_time(&tm, "%Y%m%d_%H%M%S");
  return oss.str();
}

static std::filesystem::path make_log_path(const std::string& prefix = "error") {
  try {
    if (g_log_dir.empty()) {
      QString appDir = QCoreApplication::applicationDirPath();
      g_log_dir = std::filesystem::path(appDir.toStdString()) / "logs";
      std::filesystem::create_directories(g_log_dir);
    }
    std::string filename = prefix + "_" + timestamp_now() + ".txt";
    return g_log_dir / filename;
  } catch (...) {
    // fallback to current directory
    return std::filesystem::path(prefix + "_" + timestamp_now() + ".txt");
  }
}

static void write_error_to_file(const QString& qmessage) {
  std::lock_guard<std::mutex> lock(g_log_mutex);
  try {
    std::string message = qmessage.toStdString();

    // Write a timestamped file for full dump
    auto p = make_log_path("error");
    std::ofstream ofs(p, std::ios::out | std::ios::trunc);
    if (ofs) {
      ofs << "[" << timestamp_now() << "]\n";
      ofs << message << "\n";
      ofs.close();
    }

    // Append a short description to last_error.txt (overwrites previous)
    auto last = g_log_dir.empty() ? std::filesystem::path("last_error.txt") : g_log_dir / "last_error.txt";
    std::ofstream last_ofs(last, std::ios::out | std::ios::trunc);
    if (last_ofs) {
      last_ofs << "[" << timestamp_now() << "] " << message << "\n";
      last_ofs.close();
    }

    // Also print to stderr so crash-reporting tools and consoles see it
    std::fprintf(stderr, "ERROR WRITTEN TO: %s\n", p.string().c_str());
    std::fflush(stderr);
  } catch (...) {}
}

static void qtMessageHandler(QtMsgType type, const QMessageLogContext& ctx, const QString& msg) {
  if (type != QtFatalMsg) return;
  QString formatted = QString("[FATAL] %1 (%2:%3, %4)")
                        .arg(msg)
                        .arg(ctx.file ? ctx.file : "")
                        .arg(ctx.line)
                        .arg(ctx.function ? ctx.function : "");
  write_error_to_file(QString("QtFatal: ") + formatted);
  std::abort();
}

static void terminateHandler() {
  QString message("Unhandled exception (std::terminate called)");
  try {
    if (std::current_exception()) {
      try {
        std::rethrow_exception(std::current_exception());
      } catch (const std::exception& e) {
        message = QString("Unhandled exception: %1").arg(e.what());
      } catch (...) {
        message = QString("Unhandled non-std::exception (unknown type)");
      }
    }
  } catch (...) {
    message = QString("Unhandled exception (failed to obtain details)");
  }

  write_error_to_file(message);
  std::abort(); // Ensure default terminate behaviour (abort).
}

static void signalHandler(int sig) {
#if defined(_WIN32)
  // On Windows we try a minimal stderr message (best-effort).
  const char* s = "Fatal signal received - application will terminate\n";
  fwrite(s, 1, strlen(s), stderr); // best-effort
#else
  const char* s = "Fatal signal received - application will terminate\n";
  ::write(STDERR_FILENO, s, strlen(s)); // POSIX async-signal-safe write
#endif

  try {
    write_error_to_file(QString("Fatal signal received: %1").arg(sig));
  } catch (...) {}

  // Restore default and re-raise so OS can produce core dump / minidump if configured.
  std::signal(sig, SIG_DFL);
  std::raise(sig);
}

int main(int argc, char** argv) {
  DEBUG_PRINT(license_notices);

  // Verify that the version of the library that we linked against is
  // compatible with the version of the headers we compiled against.
  GOOGLE_PROTOBUF_VERIFY_VERSION;

#if defined(SHADER_GEN_DEBUG) && defined(_MSC_VER)
  // Enable run-time memory check for debug builds in MSVC
  _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif  // SHADER_GEN_DEBUG && _MSC_VER

#ifdef SHADER_GEN_DEBUG
  qputenv("QT_DEBUG_PLUGINS", "1");  // Enable plugin diagnostics
#endif  // SHADER_GEN_DEBUG

  QApplication shader_gen_app(argc, argv);

  qInstallMessageHandler(qtMessageHandler);

  std::set_terminate(terminateHandler); // Install terminate handler to log uncaught exceptions

  // Register basic signal handlers (SIGSEGV, SIGABRT, SIGILL, SIGFPE)
  std::signal(SIGABRT, signalHandler);
  std::signal(SIGSEGV, signalHandler);
#if defined(SIGILL)
  std::signal(SIGILL, signalHandler);
#endif
#if defined(SIGFPE)
  std::signal(SIGFPE, signalHandler);
#endif

  QCoreApplication::setOrganizationName(ENIGMA_ORG_NAME);
  QCoreApplication::setApplicationName(SHADER_GEN_PROJECT_NAME);
  QCoreApplication::setApplicationVersion(SHADER_GEN_PROJECT_VERSION);
  QCommandLineParser parser;
  parser.setApplicationDescription(QCoreApplication::applicationName());
  parser.addHelpOption();
  parser.addVersionOption();
  parser.process(shader_gen_app);

  VisualShader visual_shader;

  MessageModel* root_model = new MessageModel(&visual_shader);
  bool load_result = dynamic_cast<ProtoModel*>(root_model)->loadFromJson("model.json");
  if (!load_result) {
  	ERROR_PRINT("Failed to load model from JSON");
  }
  root_model->build_sub_models();

  VisualShaderEditor* w = new VisualShaderEditor(root_model);

  w->show();

  int result{shader_gen_app.exec()};

  delete w;
  delete root_model;

  return result;
}
