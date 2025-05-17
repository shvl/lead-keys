// src/native/addon.c

#include <node_api.h>
#include "seq_lead_keys.h"

// Global N-API state & callback references
static napi_env  g_env;
static napi_ref  g_lead_callback_ref   = NULL;
static napi_ref  g_partial_callback_ref = NULL;
static napi_ref  g_complete_callback_ref = NULL;

// Helper: console.log from C
#ifdef DEBUG
static napi_value call_console_log(napi_env env, const char* str) {
  napi_value global, console, log_fn, msg, result;
  if (napi_get_global(env, &global) != napi_ok) return NULL;
  if (napi_get_named_property(env, global, "console", &console) != napi_ok) return NULL;
  if (napi_get_named_property(env, console, "log", &log_fn) != napi_ok) return NULL;
  if (napi_create_string_utf8(env, str, NAPI_AUTO_LENGTH, &msg) != napi_ok) return NULL;
  if (napi_call_function(env, console, log_fn, 1, &msg, &result) != napi_ok) return NULL;
  return result;
}
#endif



// Trampoline: C → JS
static void on_lead_sequence(void) {
  if (!g_lead_callback_ref) return;

  napi_handle_scope scope;
  napi_open_handle_scope(g_env, &scope);

  napi_value js_cb, global;
  napi_get_reference_value(g_env, g_lead_callback_ref, &js_cb);
  napi_get_global(g_env, &global);
  napi_call_function(g_env, global, js_cb, 0, NULL, NULL);

  napi_close_handle_scope(g_env, scope);
}

// Trampoline: C → JS
static void on_partial_sequence(const char* sequence) {
  if (!g_partial_callback_ref) return;

  napi_handle_scope scope;
  napi_open_handle_scope(g_env, &scope);

  napi_value js_cb, global, arg;
  napi_get_reference_value(g_env, g_partial_callback_ref, &js_cb);
  napi_get_global(g_env, &global);
  // Create a JS string from the C sequence
  napi_create_string_utf8(g_env, sequence, NAPI_AUTO_LENGTH, &arg);
  napi_call_function(g_env, global, js_cb, 1, &arg, NULL);

  napi_close_handle_scope(g_env, scope);
}

// Trampoline: C → JS
static void on_complete_sequence(const char* sequence) {
  if (!g_complete_callback_ref) return;

  napi_handle_scope scope;
  napi_open_handle_scope(g_env, &scope);

  napi_value js_cb, global, arg;
  napi_get_reference_value(g_env, g_complete_callback_ref, &js_cb);
  napi_get_global(g_env, &global);
  // Create a JS string from the C sequence
  napi_create_string_utf8(g_env, sequence, NAPI_AUTO_LENGTH, &arg);
  napi_call_function(g_env, global, js_cb, 1, &arg, NULL);

  napi_close_handle_scope(g_env, scope);
}

// Forward declarations
static napi_value AddCommand(napi_env env, napi_callback_info info);
static napi_value Start(napi_env env, napi_callback_info info);
static napi_value Stop(napi_env env, napi_callback_info info);
static napi_value SetLeadCallback(napi_env env, napi_callback_info info);
static napi_value SetPartialCallback(napi_env env, napi_callback_info info);
static napi_value SetCompleteCallback(napi_env env, napi_callback_info info);

// Module init
NAPI_MODULE_INIT() {
  napi_value fn;

  // Initialize C core
  if (!seq_lead_keys_init()) {
    napi_throw_error(env, NULL, "Failed to initialize key listener");
    return NULL;
  }

  // Bind methods
  napi_create_function(env, "addCommand", NAPI_AUTO_LENGTH, AddCommand, NULL, &fn);
  napi_set_named_property(env, exports, "addCommand", fn);

  napi_create_function(env, "start", NAPI_AUTO_LENGTH, Start, NULL, &fn);
  napi_set_named_property(env, exports, "start", fn);

  napi_create_function(env, "stop", NAPI_AUTO_LENGTH, Stop, NULL, &fn);
  napi_set_named_property(env, exports, "stop", fn);

  napi_create_function(env, "setLeadCallback", NAPI_AUTO_LENGTH, SetLeadCallback, NULL, &fn);
  napi_set_named_property(env, exports, "setLeadCallback", fn);

  napi_create_function(env, "setPartialCallback", NAPI_AUTO_LENGTH, SetPartialCallback, NULL, &fn);
  napi_set_named_property(env, exports, "setPartialCallback", fn);

  napi_create_function(env, "setCompleteCallback", NAPI_AUTO_LENGTH, SetCompleteCallback, NULL, &fn);
  napi_set_named_property(env, exports, "setCompleteCallback", fn);

  return exports;
}

// addCommand(sequence): boolean
static napi_value AddCommand(napi_env env, napi_callback_info info) {
  size_t argc = 1;
  napi_value args[1], result;
  char sequence[256];
  size_t len;

  napi_get_cb_info(env, info, &argc, args, NULL, NULL);
  napi_get_value_string_utf8(env, args[0], sequence, sizeof(sequence), &len);

  bool ok = seq_lead_keys_add_command(sequence);
  napi_get_boolean(env, ok, &result);
  return result;
}

// start(): boolean
static napi_value Start(napi_env env, napi_callback_info info) {
  bool ok = seq_lead_keys_start();
  napi_value result;
  napi_get_boolean(env, ok, &result);
  return result;
}

// stop(): void
static napi_value Stop(napi_env env, napi_callback_info info) {
  seq_lead_keys_stop();
  return NULL;
}

// setLeadCallback(cb): void
static napi_value SetLeadCallback(napi_env env, napi_callback_info info) {
  size_t argc = 1;
  napi_value args[1], undefined;
  napi_valuetype type;

  napi_get_cb_info(env, info, &argc, args, NULL, NULL);
  napi_typeof(env, args[0], &type);
  if (type != napi_function) {
    napi_throw_type_error(env, NULL, "Expected a function");
    return NULL;
  }
  if (g_lead_callback_ref) napi_delete_reference(env, g_lead_callback_ref);
  napi_create_reference(env, args[0], 1, &g_lead_callback_ref);

  g_env = env;
  seq_lead_keys_set_lead_callback(on_lead_sequence);

  napi_get_undefined(env, &undefined);
  return undefined;
}

// setPartialCallback(cb): void
static napi_value SetPartialCallback(napi_env env, napi_callback_info info) {
  size_t argc = 1;
  napi_value args[1], undefined;
  napi_valuetype type;

  napi_get_cb_info(env, info, &argc, args, NULL, NULL);
  napi_typeof(env, args[0], &type);
  if (type != napi_function) {
    napi_throw_type_error(env, NULL, "Expected a function");
    return NULL;
  }
  if (g_partial_callback_ref) napi_delete_reference(env, g_partial_callback_ref);
  napi_create_reference(env, args[0], 1, &g_partial_callback_ref);

  g_env = env;
  seq_lead_keys_set_partial_callback(on_partial_sequence);

  napi_get_undefined(env, &undefined);
  return undefined;
}

// setCompleteCallback(cb): void
static napi_value SetCompleteCallback(napi_env env, napi_callback_info info) {
  size_t argc = 1;
  napi_value args[1], undefined;
  napi_valuetype type;

  napi_get_cb_info(env, info, &argc, args, NULL, NULL);
  napi_typeof(env, args[0], &type);
  if (type != napi_function) {
    napi_throw_type_error(env, NULL, "Expected a function");
    return NULL;
  }
  if (g_complete_callback_ref) napi_delete_reference(env, g_complete_callback_ref);
  napi_create_reference(env, args[0], 1, &g_complete_callback_ref);

  g_env = env;
  seq_lead_keys_set_complete_callback(on_complete_sequence);

  napi_get_undefined(env, &undefined);
  return undefined;
}
