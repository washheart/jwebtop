// Copyright (c) 2012 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.
//
// ---------------------------------------------------------------------------
//
// This file was generated by the CEF translator tool. If making changes by
// hand only do so within the body of existing method and function
// implementations. See the translator.README.txt file in the tools directory
// for more information.
//

#include "include/cef_app.h"
#include "include/capi/cef_app_capi.h"
#include "include/cef_origin_whitelist.h"
#include "include/capi/cef_origin_whitelist_capi.h"
#include "include/cef_scheme.h"
#include "include/capi/cef_scheme_capi.h"
#include "include/cef_storage.h"
#include "include/capi/cef_storage_capi.h"
#include "include/cef_task.h"
#include "include/capi/cef_task_capi.h"
#include "include/cef_url.h"
#include "include/capi/cef_url_capi.h"
#include "include/cef_v8.h"
#include "include/capi/cef_v8_capi.h"
#include "include/cef_web_plugin.h"
#include "include/capi/cef_web_plugin_capi.h"
#include "libcef_dll/cpptoc/browser_cpptoc.h"
#include "libcef_dll/cpptoc/cookie_manager_cpptoc.h"
#include "libcef_dll/cpptoc/domdocument_cpptoc.h"
#include "libcef_dll/cpptoc/domevent_cpptoc.h"
#include "libcef_dll/cpptoc/domnode_cpptoc.h"
#include "libcef_dll/cpptoc/drag_data_cpptoc.h"
#include "libcef_dll/cpptoc/frame_cpptoc.h"
#include "libcef_dll/cpptoc/post_data_cpptoc.h"
#include "libcef_dll/cpptoc/post_data_element_cpptoc.h"
#include "libcef_dll/cpptoc/request_cpptoc.h"
#include "libcef_dll/cpptoc/response_cpptoc.h"
#include "libcef_dll/cpptoc/scheme_handler_callback_cpptoc.h"
#include "libcef_dll/cpptoc/stream_reader_cpptoc.h"
#include "libcef_dll/cpptoc/stream_writer_cpptoc.h"
#include "libcef_dll/cpptoc/v8context_cpptoc.h"
#include "libcef_dll/cpptoc/v8exception_cpptoc.h"
#include "libcef_dll/cpptoc/v8value_cpptoc.h"
#include "libcef_dll/cpptoc/web_plugin_info_cpptoc.h"
#include "libcef_dll/cpptoc/web_urlrequest_cpptoc.h"
#include "libcef_dll/cpptoc/xml_reader_cpptoc.h"
#include "libcef_dll/cpptoc/zip_reader_cpptoc.h"
#include "libcef_dll/ctocpp/app_ctocpp.h"
#include "libcef_dll/ctocpp/content_filter_ctocpp.h"
#include "libcef_dll/ctocpp/cookie_visitor_ctocpp.h"
#include "libcef_dll/ctocpp/domevent_listener_ctocpp.h"
#include "libcef_dll/ctocpp/domvisitor_ctocpp.h"
#include "libcef_dll/ctocpp/display_handler_ctocpp.h"
#include "libcef_dll/ctocpp/download_handler_ctocpp.h"
#include "libcef_dll/ctocpp/drag_handler_ctocpp.h"
#include "libcef_dll/ctocpp/find_handler_ctocpp.h"
#include "libcef_dll/ctocpp/focus_handler_ctocpp.h"
#include "libcef_dll/ctocpp/jsdialog_handler_ctocpp.h"
#include "libcef_dll/ctocpp/keyboard_handler_ctocpp.h"
#include "libcef_dll/ctocpp/life_span_handler_ctocpp.h"
#include "libcef_dll/ctocpp/load_handler_ctocpp.h"
#include "libcef_dll/ctocpp/menu_handler_ctocpp.h"
#include "libcef_dll/ctocpp/permission_handler_ctocpp.h"
#include "libcef_dll/ctocpp/print_handler_ctocpp.h"
#include "libcef_dll/ctocpp/proxy_handler_ctocpp.h"
#include "libcef_dll/ctocpp/read_handler_ctocpp.h"
#include "libcef_dll/ctocpp/render_handler_ctocpp.h"
#include "libcef_dll/ctocpp/request_handler_ctocpp.h"
#include "libcef_dll/ctocpp/resource_bundle_handler_ctocpp.h"
#include "libcef_dll/ctocpp/scheme_handler_ctocpp.h"
#include "libcef_dll/ctocpp/scheme_handler_factory_ctocpp.h"
#include "libcef_dll/ctocpp/storage_visitor_ctocpp.h"
#include "libcef_dll/ctocpp/task_ctocpp.h"
#include "libcef_dll/ctocpp/v8accessor_ctocpp.h"
#include "libcef_dll/ctocpp/v8context_handler_ctocpp.h"
#include "libcef_dll/ctocpp/v8handler_ctocpp.h"
#include "libcef_dll/ctocpp/web_urlrequest_client_ctocpp.h"
#include "libcef_dll/ctocpp/write_handler_ctocpp.h"


// GLOBAL FUNCTIONS - Body may be edited by hand.

CEF_EXPORT int cef_initialize(const struct _cef_settings_t* settings,
    struct _cef_app_t* application) {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: settings; type: struct_byref_const
  DCHECK(settings);
  if (!settings)
    return 0;
  // Unverified params: application

  // Translate param: settings; type: struct_byref_const
  CefSettings settingsObj;
  if (settings)
    settingsObj.Set(*settings, false);

  // Execute
  bool _retval = CefInitialize(
      settingsObj,
      CefAppCToCpp::Wrap(application));

  // Return type: bool
  return _retval;
}

CEF_EXPORT void cef_shutdown() {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  CefShutdown();

#ifndef NDEBUG
  // Check that all wrapper objects have been destroyed
  ASSERTEQ(CefBrowserCppToC::DebugObjCt, 0);
  ASSERTEQ(CefContentFilterCToCpp::DebugObjCt, 0);
  ASSERTEQ(CefCookieManagerCppToC::DebugObjCt, 0);
  ASSERTEQ(CefCookieVisitorCToCpp::DebugObjCt, 0);
  ASSERTEQ(CefDOMDocumentCppToC::DebugObjCt, 0);
  ASSERTEQ(CefDOMEventCppToC::DebugObjCt, 0);
  ASSERTEQ(CefDOMEventListenerCToCpp::DebugObjCt, 0);
  ASSERTEQ(CefDOMNodeCppToC::DebugObjCt, 0);
  ASSERTEQ(CefDOMVisitorCToCpp::DebugObjCt, 0);
  ASSERTEQ(CefDisplayHandlerCToCpp::DebugObjCt, 0);
  ASSERTEQ(CefDownloadHandlerCToCpp::DebugObjCt, 0);
  ASSERTEQ(CefDragDataCppToC::DebugObjCt, 0);
  ASSERTEQ(CefDragHandlerCToCpp::DebugObjCt, 0);
  ASSERTEQ(CefFindHandlerCToCpp::DebugObjCt, 0);
  ASSERTEQ(CefFocusHandlerCToCpp::DebugObjCt, 0);
  ASSERTEQ(CefFrameCppToC::DebugObjCt, 0);
  ASSERTEQ(CefJSDialogHandlerCToCpp::DebugObjCt, 0);
  ASSERTEQ(CefKeyboardHandlerCToCpp::DebugObjCt, 0);
  ASSERTEQ(CefLifeSpanHandlerCToCpp::DebugObjCt, 0);
  ASSERTEQ(CefLoadHandlerCToCpp::DebugObjCt, 0);
  ASSERTEQ(CefMenuHandlerCToCpp::DebugObjCt, 0);
  ASSERTEQ(CefPermissionHandlerCToCpp::DebugObjCt, 0);
  ASSERTEQ(CefPostDataCppToC::DebugObjCt, 0);
  ASSERTEQ(CefPostDataElementCppToC::DebugObjCt, 0);
  ASSERTEQ(CefPrintHandlerCToCpp::DebugObjCt, 0);
  ASSERTEQ(CefProxyHandlerCToCpp::DebugObjCt, 0);
  ASSERTEQ(CefReadHandlerCToCpp::DebugObjCt, 0);
  ASSERTEQ(CefRenderHandlerCToCpp::DebugObjCt, 0);
  ASSERTEQ(CefRequestCppToC::DebugObjCt, 0);
  ASSERTEQ(CefRequestHandlerCToCpp::DebugObjCt, 0);
  ASSERTEQ(CefResourceBundleHandlerCToCpp::DebugObjCt, 0);
  ASSERTEQ(CefResponseCppToC::DebugObjCt, 0);
  ASSERTEQ(CefSchemeHandlerCToCpp::DebugObjCt, 0);
  ASSERTEQ(CefSchemeHandlerCallbackCppToC::DebugObjCt, 0);
  ASSERTEQ(CefSchemeHandlerFactoryCToCpp::DebugObjCt, 0);
  ASSERTEQ(CefStorageVisitorCToCpp::DebugObjCt, 0);
  ASSERTEQ(CefStreamReaderCppToC::DebugObjCt, 0);
  ASSERTEQ(CefStreamWriterCppToC::DebugObjCt, 0);
  ASSERTEQ(CefTaskCToCpp::DebugObjCt, 0);
  ASSERTEQ(CefV8AccessorCToCpp::DebugObjCt, 0);
  ASSERTEQ(CefV8ContextCppToC::DebugObjCt, 0);
  ASSERTEQ(CefV8ContextHandlerCToCpp::DebugObjCt, 0);
  ASSERTEQ(CefV8ExceptionCppToC::DebugObjCt, 0);
  ASSERTEQ(CefV8HandlerCToCpp::DebugObjCt, 0);
  ASSERTEQ(CefV8ValueCppToC::DebugObjCt, 0);
  ASSERTEQ(CefWebPluginInfoCppToC::DebugObjCt, 0);
  ASSERTEQ(CefWebURLRequestClientCToCpp::DebugObjCt, 0);
  ASSERTEQ(CefWebURLRequestCppToC::DebugObjCt, 0);
  ASSERTEQ(CefWriteHandlerCToCpp::DebugObjCt, 0);
  ASSERTEQ(CefXmlReaderCppToC::DebugObjCt, 0);
  ASSERTEQ(CefZipReaderCppToC::DebugObjCt, 0);
#endif  // !NDEBUG
}

CEF_EXPORT void cef_do_message_loop_work() {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  CefDoMessageLoopWork();
}

CEF_EXPORT void cef_do_message_loop_work_with_dispatcher(
    const struct _cef_message_dispatcher_t* messageDispatcher) {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: messageDispatcher; type: struct_byref_const
  DCHECK(messageDispatcher);
  if (!messageDispatcher)
    return;

  // Translate param: messageDispatcher; type: struct_byref_const
  CefMessageDispatcher messageDispatcherObj;
  if (messageDispatcher)
    messageDispatcherObj.Set(*messageDispatcher, false);

  // Execute
  CefDoMessageLoopWorkWithDispatcher(
      messageDispatcherObj);
}

CEF_EXPORT void cef_start_inner_message_pump() {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  CefStartInnerMessagePump();
}

CEF_EXPORT void cef_end_inner_message_pump() {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  CefEndInnerMessagePump();
}

CEF_EXPORT void cef_run_message_loop() {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  CefRunMessageLoop();
}

CEF_EXPORT void cef_quit_message_loop() {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  CefQuitMessageLoop();
}

CEF_EXPORT void cef_cache_doom_all(enum cef_cache_type_t cacheType) {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  CefCacheDoomAll(
      cacheType);
}

CEF_EXPORT void cef_refresh_plugins() {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  CefRefreshPlugins();
}

CEF_EXPORT int cef_get_max_sockets_per_group() {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  int _retval = CefGetMaxSocketsPerGroup();

  // Return type: simple
  return _retval;
}

CEF_EXPORT void cef_set_max_sockets_per_group(int count) {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  CefSetMaxSocketsPerGroup(
      count);
}

CEF_EXPORT cef_string_userfree_t cef_get_user_agent() {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  CefString _retval = CefGetUserAgent();

  // Return type: string
  return _retval.DetachToUserFree();
}

CEF_EXPORT void cef_set_user_agent(const cef_string_t* newUserAgent) {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: newUserAgent; type: string_byref_const
  DCHECK(newUserAgent);
  if (!newUserAgent)
    return;

  // Execute
  CefSetUserAgent(
      CefString(newUserAgent));
}

CEF_EXPORT int cef_add_cross_origin_whitelist_entry(
    const cef_string_t* source_origin, const cef_string_t* target_protocol,
    const cef_string_t* target_domain, int allow_target_subdomains) {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: source_origin; type: string_byref_const
  DCHECK(source_origin);
  if (!source_origin)
    return 0;
  // Verify param: target_protocol; type: string_byref_const
  DCHECK(target_protocol);
  if (!target_protocol)
    return 0;
  // Unverified params: target_domain

  // Execute
  bool _retval = CefAddCrossOriginWhitelistEntry(
      CefString(source_origin),
      CefString(target_protocol),
      CefString(target_domain),
      allow_target_subdomains?true:false);

  // Return type: bool
  return _retval;
}

CEF_EXPORT int cef_remove_cross_origin_whitelist_entry(
    const cef_string_t* source_origin, const cef_string_t* target_protocol,
    const cef_string_t* target_domain, int allow_target_subdomains) {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: source_origin; type: string_byref_const
  DCHECK(source_origin);
  if (!source_origin)
    return 0;
  // Verify param: target_protocol; type: string_byref_const
  DCHECK(target_protocol);
  if (!target_protocol)
    return 0;
  // Unverified params: target_domain

  // Execute
  bool _retval = CefRemoveCrossOriginWhitelistEntry(
      CefString(source_origin),
      CefString(target_protocol),
      CefString(target_domain),
      allow_target_subdomains?true:false);

  // Return type: bool
  return _retval;
}

CEF_EXPORT int cef_clear_cross_origin_whitelist() {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  bool _retval = CefClearCrossOriginWhitelist();

  // Return type: bool
  return _retval;
}

CEF_EXPORT int cef_register_custom_scheme(const cef_string_t* scheme_name,
    int is_standard, int is_local, int is_display_isolated) {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: scheme_name; type: string_byref_const
  DCHECK(scheme_name);
  if (!scheme_name)
    return 0;

  // Execute
  bool _retval = CefRegisterCustomScheme(
      CefString(scheme_name),
      is_standard?true:false,
      is_local?true:false,
      is_display_isolated?true:false);

  // Return type: bool
  return _retval;
}

CEF_EXPORT int cef_register_client_scheme(const cef_string_t* scheme_name,
    const cef_string_t* root_folder) {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: scheme_name; type: string_byref_const
  DCHECK(scheme_name);
  if (!scheme_name)
    return 0;
  // Verify param: root_folder; type: string_byref_const
  DCHECK(root_folder);
  if (!root_folder)
    return 0;

  // Execute
  bool _retval = CefRegisterClientScheme(
      CefString(scheme_name),
      CefString(root_folder));

  // Return type: bool
  return _retval;
}

CEF_EXPORT int cef_register_scheme_handler_factory(
    const cef_string_t* scheme_name, const cef_string_t* domain_name,
    struct _cef_scheme_handler_factory_t* factory) {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: scheme_name; type: string_byref_const
  DCHECK(scheme_name);
  if (!scheme_name)
    return 0;
  // Unverified params: domain_name, factory

  // Execute
  bool _retval = CefRegisterSchemeHandlerFactory(
      CefString(scheme_name),
      CefString(domain_name),
      CefSchemeHandlerFactoryCToCpp::Wrap(factory));

  // Return type: bool
  return _retval;
}

CEF_EXPORT int cef_clear_scheme_handler_factories() {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  bool _retval = CefClearSchemeHandlerFactories();

  // Return type: bool
  return _retval;
}

CEF_EXPORT int cef_visit_storage(enum cef_storage_type_t type,
    const cef_string_t* origin, const cef_string_t* key,
    struct _cef_storage_visitor_t* visitor) {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: visitor; type: refptr_diff
  DCHECK(visitor);
  if (!visitor)
    return 0;
  // Unverified params: origin, key

  // Execute
  bool _retval = CefVisitStorage(
      type,
      CefString(origin),
      CefString(key),
      CefStorageVisitorCToCpp::Wrap(visitor));

  // Return type: bool
  return _retval;
}

CEF_EXPORT int cef_set_storage(enum cef_storage_type_t type,
    const cef_string_t* origin, const cef_string_t* key,
    const cef_string_t* value) {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: origin; type: string_byref_const
  DCHECK(origin);
  if (!origin)
    return 0;
  // Verify param: key; type: string_byref_const
  DCHECK(key);
  if (!key)
    return 0;
  // Verify param: value; type: string_byref_const
  DCHECK(value);
  if (!value)
    return 0;

  // Execute
  bool _retval = CefSetStorage(
      type,
      CefString(origin),
      CefString(key),
      CefString(value));

  // Return type: bool
  return _retval;
}

CEF_EXPORT int cef_delete_storage(enum cef_storage_type_t type,
    const cef_string_t* origin, const cef_string_t* key) {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Unverified params: origin, key

  // Execute
  bool _retval = CefDeleteStorage(
      type,
      CefString(origin),
      CefString(key));

  // Return type: bool
  return _retval;
}

CEF_EXPORT int cef_set_storage_path(enum cef_storage_type_t type,
    const cef_string_t* path) {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Unverified params: path

  // Execute
  bool _retval = CefSetStoragePath(
      type,
      CefString(path));

  // Return type: bool
  return _retval;
}

CEF_EXPORT int cef_currently_on(cef_thread_id_t threadId) {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  bool _retval = CefCurrentlyOn(
      threadId);

  // Return type: bool
  return _retval;
}

CEF_EXPORT int cef_post_task(cef_thread_id_t threadId,
    struct _cef_task_t* task) {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: task; type: refptr_diff
  DCHECK(task);
  if (!task)
    return 0;

  // Execute
  bool _retval = CefPostTask(
      threadId,
      CefTaskCToCpp::Wrap(task));

  // Return type: bool
  return _retval;
}

CEF_EXPORT int cef_post_delayed_task(cef_thread_id_t threadId,
    struct _cef_task_t* task, int64 delay_ms) {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: task; type: refptr_diff
  DCHECK(task);
  if (!task)
    return 0;

  // Execute
  bool _retval = CefPostDelayedTask(
      threadId,
      CefTaskCToCpp::Wrap(task),
      delay_ms);

  // Return type: bool
  return _retval;
}

CEF_EXPORT int cef_parse_url(const cef_string_t* url,
    struct _cef_urlparts_t* parts) {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: url; type: string_byref_const
  DCHECK(url);
  if (!url)
    return 0;
  // Verify param: parts; type: struct_byref
  DCHECK(parts);
  if (!parts)
    return 0;

  // Translate param: parts; type: struct_byref
  CefURLParts partsObj;
  if (parts)
    partsObj.AttachTo(*parts);

  // Execute
  bool _retval = CefParseURL(
      CefString(url),
      partsObj);

  // Restore param: parts; type: struct_byref
  if (parts)
    partsObj.DetachTo(*parts);

  // Return type: bool
  return _retval;
}

CEF_EXPORT int cef_create_url(const struct _cef_urlparts_t* parts,
    cef_string_t* url) {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: parts; type: struct_byref_const
  DCHECK(parts);
  if (!parts)
    return 0;
  // Verify param: url; type: string_byref
  DCHECK(url);
  if (!url)
    return 0;

  // Translate param: parts; type: struct_byref_const
  CefURLParts partsObj;
  if (parts)
    partsObj.Set(*parts, false);
  // Translate param: url; type: string_byref
  CefString urlStr(url);

  // Execute
  bool _retval = CefCreateURL(
      partsObj,
      urlStr);

  // Return type: bool
  return _retval;
}

CEF_EXPORT int cef_register_extension(const cef_string_t* extension_name,
    const cef_string_t* javascript_code, struct _cef_v8handler_t* handler) {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: extension_name; type: string_byref_const
  DCHECK(extension_name);
  if (!extension_name)
    return 0;
  // Verify param: javascript_code; type: string_byref_const
  DCHECK(javascript_code);
  if (!javascript_code)
    return 0;
  // Unverified params: handler

  // Execute
  bool _retval = CefRegisterExtension(
      CefString(extension_name),
      CefString(javascript_code),
      CefV8HandlerCToCpp::Wrap(handler));

  // Return type: bool
  return _retval;
}

CEF_EXPORT size_t cef_get_web_plugin_count() {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  size_t _retval = CefGetWebPluginCount();

  // Return type: simple
  return _retval;
}

CEF_EXPORT struct _cef_web_plugin_info_t* cef_get_web_plugin_info(int index) {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  CefRefPtr<CefWebPluginInfo> _retval = CefGetWebPluginInfo(
      index);

  // Return type: refptr_same
  return CefWebPluginInfoCppToC::Wrap(_retval);
}

CEF_EXPORT struct _cef_web_plugin_info_t* cef_get_web_plugin_info_byname(
    const cef_string_t* name) {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: name; type: string_byref_const
  DCHECK(name);
  if (!name)
    return NULL;

  // Execute
  CefRefPtr<CefWebPluginInfo> _retval = CefGetWebPluginInfo(
      CefString(name));

  // Return type: refptr_same
  return CefWebPluginInfoCppToC::Wrap(_retval);
}
