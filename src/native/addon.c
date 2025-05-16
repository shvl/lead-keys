#include <node_api.h>
#include "seq_lead_keys.h"

// Forward declarations
static napi_value AddCommand(napi_env env, napi_callback_info info);
static napi_value Start(napi_env env, napi_callback_info info);
static napi_value Stop(napi_env env, napi_callback_info info);

static napi_value Init(napi_env env, napi_value exports) {
    napi_value fn;
    napi_status status;

    // Initialize the library
    if (!seq_lead_keys_init()) {
        napi_throw_error(env, NULL, "Failed to initialize key listener");
        return NULL;
    }

    // Add command function
    status = napi_create_function(env, NULL, 0, AddCommand, NULL, &fn);
    if (status != napi_ok) {
        napi_throw_error(env, NULL, "Failed to create addCommand function");
        return NULL;
    }
    status = napi_set_named_property(env, exports, "addCommand", fn);
    if (status != napi_ok) {
        napi_throw_error(env, NULL, "Failed to set addCommand property");
        return NULL;
    }

    // Start function
    status = napi_create_function(env, NULL, 0, Start, NULL, &fn);
    if (status != napi_ok) {
        napi_throw_error(env, NULL, "Failed to create start function");
        return NULL;
    }
    status = napi_set_named_property(env, exports, "start", fn);
    if (status != napi_ok) {
        napi_throw_error(env, NULL, "Failed to set start property");
        return NULL;
    }

    // Stop function
    status = napi_create_function(env, NULL, 0, Stop, NULL, &fn);
    if (status != napi_ok) {
        napi_throw_error(env, NULL, "Failed to create stop function");
        return NULL;
    }
    status = napi_set_named_property(env, exports, "stop", fn);
    if (status != napi_ok) {
        napi_throw_error(env, NULL, "Failed to set stop property");
        return NULL;
    }

    return exports;
}

static napi_value AddCommand(napi_env env, napi_callback_info info) {
    napi_status status;
    size_t argc = 1;
    napi_value args[1];
    char sequence[256];
    size_t len;

    status = napi_get_cb_info(env, info, &argc, args, NULL, NULL);
    if (status != napi_ok) {
        napi_throw_error(env, NULL, "Failed to parse arguments");
        return NULL;
    }

    status = napi_get_value_string_utf8(env, args[0], sequence, sizeof(sequence), &len);
    if (status != napi_ok) {
        napi_throw_error(env, NULL, "Failed to get sequence string");
        return NULL;
    }

    bool success = seq_lead_keys_add_command(sequence);
    napi_value result;
    status = napi_get_boolean(env, success, &result);
    if (status != napi_ok) {
        napi_throw_error(env, NULL, "Failed to create result");
        return NULL;
    }

    return result;
}

static napi_value Start(napi_env env, napi_callback_info info) {
    napi_status status;
    bool success = seq_lead_keys_start();
    napi_value result;
    status = napi_get_boolean(env, success, &result);
    if (status != napi_ok) {
        napi_throw_error(env, NULL, "Failed to create result");
        return NULL;
    }
    return result;
}

static napi_value Stop(napi_env env, napi_callback_info info) {
    seq_lead_keys_stop();
    return NULL;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, Init) 