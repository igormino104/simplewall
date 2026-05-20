// simplewall
// Copyright (c) 2016-2026 Henry++

#pragma once

#include "routine.h"

#include <winsock2.h>
#include <ws2ipdef.h>
#include <ws2tcpip.h>
#include <windns.h>
#include <mstcpip.h>
#include <iphlpapi.h>
#include <aclapi.h>
#include <dbt.h>
#include <fwpmu.h>
#include <mmsystem.h>
#include <netfw.h>
#include <shlguid.h>
#include <shobjidl.h>
#include <softpub.h>
#include <subauth.h>
#include <mscat.h>

#include "app.h"
#include "rapp.h"
#include "main.h"

#include "resource.h"

DECLSPEC_SELECTANY STATIC_DATA config = {0};
DECLSPEC_SELECTANY PROFILE_DATA profile_info = {0};

DECLSPEC_SELECTANY PR_HASHTABLE apps_table = NULL;
DECLSPEC_SELECTANY PR_LIST rules_list = NULL;
DECLSPEC_SELECTANY PR_HASHTABLE rules_config = NULL;
DECLSPEC_SELECTANY PR_HASHTABLE log_table = NULL;

DECLSPEC_SELECTANY PR_HASHTABLE cache_information = NULL;
DECLSPEC_SELECTANY PR_HASHTABLE cache_resolution = NULL;

DECLSPEC_SELECTANY PR_HASHTABLE colors_table = NULL;

DECLSPEC_SELECTANY PR_ARRAY filter_ids = NULL;

DECLSPEC_SELECTANY R_QUEUED_LOCK lock_apps = PR_QUEUED_LOCK_INIT;
DECLSPEC_SELECTANY R_QUEUED_LOCK lock_apply = PR_QUEUED_LOCK_INIT;
DECLSPEC_SELECTANY R_QUEUED_LOCK lock_rules = PR_QUEUED_LOCK_INIT;
DECLSPEC_SELECTANY R_QUEUED_LOCK lock_rules_config = PR_QUEUED_LOCK_INIT;
DECLSPEC_SELECTANY R_QUEUED_LOCK lock_loglist = PR_QUEUED_LOCK_INIT;
DECLSPEC_SELECTANY R_QUEUED_LOCK lock_notify = PR_QUEUED_LOCK_INIT;
DECLSPEC_SELECTANY R_QUEUED_LOCK lock_profile = PR_QUEUED_LOCK_INIT;
DECLSPEC_SELECTANY R_QUEUED_LOCK lock_transaction = PR_QUEUED_LOCK_INIT;

DECLSPEC_SELECTANY R_QUEUED_LOCK lock_cache_information = PR_QUEUED_LOCK_INIT;
DECLSPEC_SELECTANY R_QUEUED_LOCK lock_cache_resolution = PR_QUEUED_LOCK_INIT;

DECLSPEC_SELECTANY R_WORKQUEUE file_queue = {0};
DECLSPEC_SELECTANY R_WORKQUEUE log_queue = {0};
DECLSPEC_SELECTANY R_WORKQUEUE resolver_queue = {0};
DECLSPEC_SELECTANY R_WORKQUEUE resolve_notify_queue = {0};
DECLSPEC_SELECTANY R_WORKQUEUE wfp_queue = {0};

DECLSPEC_SELECTANY R_FREE_LIST context_free_list = {0};
DECLSPEC_SELECTANY R_FREE_LIST listview_free_list = {0};

// timers array
DECLSPEC_SELECTANY const LONG64 timer_array[] =
{
	2LL * 60LL, // 2 min
	5LL * 60LL, // 5 min
	10LL * 60LL, // 10 min
	30LL * 60LL, // 30 min
	1LL * 3600LL, // 1 hour
	2LL * 3600LL, // 2 hour
	4LL * 3600LL, // 4 hour
	6LL * 3600LL, // 6 hour
	12LL * 3600LL, // 12 hour
	24LL * 3600LL // 24 hour
};

// dropped events callback subscription (win7+)
#if !defined(FWP_DIRECTION_IN)
#define FWP_DIRECTION_IN 0x00003900L
#endif // !FWP_DIRECTION_IN

#if !defined(FWP_DIRECTION_OUT)
#define FWP_DIRECTION_OUT 0x00003901L
#endif // !FWP_DIRECTION_OUT

#if !defined(FWP_DIRECTION_FORWARD)
#define FWP_DIRECTION_FORWARD 0x00003902L
#endif // !FWP_DIRECTION_FORWARD

#if !defined(FWP_DIRECTION_FORWARD2)
#define FWP_DIRECTION_FORWARD2 0x00003903L
#endif // !FWP_DIRECTION_FORWARD2

#define WM_NOTIFICATION (WM_APP + 21)

// Compatibility for simplewall sources against the current routine API.
#if !defined(I_DEFAULT)
#define I_DEFAULT INT_ERROR
#endif // !I_DEFAULT

typedef BOOL (WINAPI *PAPP_CRYPTCATADMINCALCHASHFROMFILEHANDLE2) (
	_In_ HCATADMIN hcat_admin,
	_In_ HANDLE hfile,
	_Inout_ PULONG file_hash_length_ptr,
	_Out_writes_bytes_to_opt_ (*file_hash_length_ptr, *file_hash_length_ptr) PBYTE file_hash,
	_Reserved_ DWORD flags
);

typedef BOOL (WINAPI *PAPP_CRYPTCATADMINACQUIRECONTEXT2) (
	_Out_ HCATADMIN_PTR hcat_admin_ptr,
	_In_opt_ const GUID *subsystem_guid,
	_In_opt_ PCWSTR algorithm_id,
	_In_opt_ PCCERT_STRONG_SIGN_PARA strong_hash_policy,
	_Reserved_ DWORD flags
);

FORCEINLINE NTSTATUS _app_compat_createhardlink (
	_In_ PR_STRINGREF existing_path,
	_In_ PR_STRINGREF new_path
)
{
	if (CreateHardLinkW (new_path->buffer, existing_path->buffer, NULL))
		return STATUS_SUCCESS;

	return NTSTATUS_FROM_WIN32 (GetLastError ());
}

FORCEINLINE NTSTATUS _app_compat_fs_setpos (
	_In_ HANDLE hfile,
	_In_ LONG64 new_pos
)
{
	LARGE_INTEGER li;

	li.QuadPart = new_pos;

	return _r_fs_setpos (hfile, &li);
}

FORCEINLINE BOOLEAN _app_compat_rebar_getinfo (
	_In_ HWND hwnd,
	_In_opt_ INT ctrl_id,
	_In_ UINT band_id,
	_Inout_ LPREBARBANDINFOW band_info
)
{
	return (_r_wnd_sendmessage (hwnd, ctrl_id, RB_GETBANDINFO, band_id, (LPARAM)band_info) != FALSE);
}

FORCEINLINE BOOLEAN _app_compat_rebar_setinfo (
	_In_ HWND hwnd,
	_In_opt_ INT ctrl_id,
	_In_ UINT band_id,
	_In_ LPREBARBANDINFOW band_info
)
{
	return (_r_wnd_sendmessage (hwnd, ctrl_id, RB_SETBANDINFO, band_id, (LPARAM)band_info) != FALSE);
}

FORCEINLINE BOOLEAN _app_compat_toolbar_getidealsize (
	_In_ HWND hwnd,
	_In_opt_ INT ctrl_id,
	_In_ BOOLEAN is_large,
	_Out_ PSIZE size
)
{
	RtlZeroMemory (size, sizeof (SIZE));

	return (_r_wnd_sendmessage (hwnd, ctrl_id, TB_GETIDEALSIZE, is_large, (LPARAM)size) != FALSE);
}

FORCEINLINE PR_STRING _app_compat_querytaginformation (
	_In_ ULONG pid,
	_In_ LPCVOID tag
)
{
	PR_STRING string;
	HANDLE hprocess;
	NTSTATUS status;

	status = _r_sys_openprocess (ULongToHandle (pid), PROCESS_QUERY_LIMITED_INFORMATION, &hprocess);

	if (!NT_SUCCESS (status))
		return NULL;

	string = _r_sys_querytaginformation (hprocess, tag);

	NtClose (hprocess);

	return string;
}

FORCEINLINE BOOLEAN _app_compat_enumhashtable (
	_In_ PR_HASHTABLE hashtable,
	_Out_opt_ PVOID_PTR entry_ptr,
	_Out_opt_ PULONG hash_code_ptr,
	_Inout_ PULONG_PTR enum_key
)
{
	ULONG_PTR hash_code;
	BOOLEAN result;

	result = _r_obj_enumhashtable (hashtable, entry_ptr, hash_code_ptr ? &hash_code : NULL, enum_key);

	if (result && hash_code_ptr)
		*hash_code_ptr = (ULONG)hash_code;

	return result;
}

FORCEINLINE BOOLEAN _app_compat_enumhashtablepointer (
	_In_ PR_HASHTABLE hashtable,
	_Out_opt_ PVOID_PTR entry_ptr,
	_Out_opt_ PULONG hash_code_ptr,
	_Inout_ PULONG_PTR enum_key
)
{
	ULONG_PTR hash_code;
	BOOLEAN result;

	result = _r_obj_enumhashtablepointer (hashtable, entry_ptr, hash_code_ptr ? &hash_code : NULL, enum_key);

	if (result && hash_code_ptr)
		*hash_code_ptr = (ULONG)hash_code;

	return result;
}

FORCEINLINE NTSTATUS _app_compat_createprocess (
	_In_opt_ PR_STRINGREF file_name,
	_In_opt_ PR_STRINGREF command_line,
	_In_opt_ PR_STRINGREF directory,
	_In_ BOOLEAN is_wait
)
{
	return _r_sys_createprocess (
		file_name ? file_name->buffer : NULL,
		command_line ? command_line->buffer : NULL,
		directory ? directory->buffer : NULL,
		is_wait
	);
}

#define _r_calc_filetime2largeinteger(out_buffer, file_time) _r_calc_filetime2largeinteger (file_time, out_buffer)
#define _r_config_getboolean(key_name, def_value, section_name) _r_config_getboolean_ex (key_name, def_value, section_name)
#define _r_config_getlong(key_name, def_value, section_name) _r_config_getlong_ex (key_name, def_value, section_name)
#define _r_config_getlong64(key_name, def_value, section_name) _r_config_getlong64_ex (key_name, def_value, section_name)
#define _r_config_getulong(key_name, def_value, section_name) _r_config_getulong_ex (key_name, def_value, section_name)
#define _r_config_getfont(key_name, logfont, dpi_value, section_name) _r_config_getfont_ex (key_name, logfont, dpi_value, section_name)
#define _r_config_getstringexpand(key_name, def_value, section_name) _r_config_getstringexpand_ex (key_name, def_value, section_name)
#define _r_config_setboolean(key_name, value, section_name) _r_config_setboolean_ex (key_name, value, section_name)
#define _r_config_setlong(key_name, value, section_name) _r_config_setlong_ex (key_name, value, section_name)
#define _r_config_setlong64(key_name, value, section_name) _r_config_setlong64_ex (key_name, value, section_name)
#define _r_config_setulong(key_name, value, section_name) _r_config_setulong_ex (key_name, value, section_name)
#define _r_config_setfont(key_name, logfont, dpi_value, section_name) _r_config_setfont_ex (key_name, logfont, dpi_value, section_name)
#define _r_config_setstringexpand(key_name, value, section_name) _r_config_setstringexpand_ex (key_name, value, section_name)
#define _r_crypt_getfilehash(out_buffer, algorithm_id, path, hfile) _r_crypt_getfilehash (algorithm_id, path, hfile, out_buffer)
#define _r_ctrl_setselection(hwnd, ctrl_id, start_pos, end_pos) _r_ctrl_setselection (hwnd, ctrl_id, MAKELPARAM (start_pos, end_pos))
#define _r_filedialog_setpath(file_dialog, path) _r_filedialog_setpath (file_dialog, (path)->buffer)
#define _r_format_interval(seconds) _r_format_interval (seconds, FALSE)
#define _r_fs_createfile(out_buffer, path, create_disposition, desired_access, share_access, file_attributes, create_option, is_directory, allocation_size) _r_fs_createfile (path, create_disposition, desired_access, share_access, file_attributes, create_option, is_directory, allocation_size, out_buffer)
#define _r_fs_createhardlink(existing_path, new_path) _app_compat_createhardlink (existing_path, new_path)
#define _r_fs_getsize(path, hfile, out_buffer) _r_fs_getsize2 (path, hfile, out_buffer)
#define _r_fs_isexists(path) _r_fs_exists (path)
#define _r_fs_openfile(out_buffer, path, desired_access, share_access, open_options, is_directory) _r_fs_openfile (path, desired_access, share_access, open_options, is_directory, out_buffer)
#define _r_fs_setpos(hfile, new_pos) _app_compat_fs_setpos (hfile, new_pos)
#define _r_imagelist_create(out_buffer, width, height, flags, initial_count, grow_count) _r_imagelist_create (width, height, flags, initial_count, grow_count, out_buffer)
#define _r_imagelist_getsystem(out_buffer, icons_size) _r_imagelist_getsystem (icons_size, out_buffer)
#define _r_listview_scroll(hwnd, ctrl_id, scroll_pos) _r_wnd_sendmessage (hwnd, ctrl_id, LVM_SCROLL, 0, scroll_pos)
#define _r_menu_addseparator(hmenu) _r_menu_additem (hmenu, 0, NULL)
#define _r_obj_addlistitem(list_node, list_item, new_index_ptr) _r_obj_addlistitem_ex (list_node, list_item, new_index_ptr)
#define _r_obj_enumhashtable(hashtable, entry_ptr, hash_code_ptr, enum_key) _app_compat_enumhashtable (hashtable, entry_ptr, hash_code_ptr, enum_key)
#define _r_obj_enumhashtablepointer(hashtable, entry_ptr, hash_code_ptr, enum_key) _app_compat_enumhashtablepointer (hashtable, entry_ptr, hash_code_ptr, enum_key)
#define _r_path_geticon(path, hicon_ptr, icon_id_ptr) _r_path_geticon (path, icon_id_ptr, hicon_ptr)
#define _r_rebar_getinfo(hwnd, ctrl_id, band_id, band_info) _app_compat_rebar_getinfo (hwnd, ctrl_id, band_id, band_info)
#define _r_rebar_setinfo(hwnd, ctrl_id, band_id, band_info) _app_compat_rebar_setinfo (hwnd, ctrl_id, band_id, band_info)
#define _r_reg_openkey(out_buffer, hroot, path, flags, desired_access) _r_reg_openkey (hroot, path, flags, desired_access, out_buffer)
#define _r_res_loadimage(out_buffer, hinst, type, name, format, width, height) _r_res_loadimage (hinst, type, name, format, width, height, out_buffer)
#define _r_res_loadresource(out_buffer, hinst, type, name, lang_id) _r_res_loadresource (hinst, type, name, lang_id, out_buffer)
#define _r_str_fromguid(out_buffer, guid, is_uppercase) _r_str_fromguid ((LPGUID)(guid), is_uppercase, out_buffer)
#define _r_str_fromsid(out_buffer, sid) _r_str_fromsid (sid, out_buffer)
#define _r_str_gethash(string, is_ignorecase) _r_str_gethash2 (string, is_ignorecase)
#define _r_sys_compressbuffer(out_buffer, format, buffer) _r_sys_compressbuffer (format, buffer, out_buffer)
#define _r_sys_createprocess(file_name, command_line, directory, is_wait) _app_compat_createprocess (file_name, command_line, directory, is_wait)
#define _r_sys_createthread(thread_handle, hprocess, base_address, arglist, environment, thread_name) _r_sys_createthread (thread_handle, hprocess, (PUSER_THREAD_START_ROUTINE)(base_address), arglist, environment, thread_name)
#define _r_sys_decompressbuffer(out_buffer, format, buffer) _r_sys_decompressbuffer (format, buffer, out_buffer)
#define _r_sys_getprocessimagepathbyid(out_buffer, hprocess_id, is_ntpathtodos) _r_sys_getprocessimagepathbyid (ULongToHandle (hprocess_id), is_ntpathtodos, out_buffer)
#define _r_sys_getservicesid(out_buffer, name) _r_sys_getservicesid (name, out_buffer)
#define _r_sys_getusername(out_buffer, sid, is_withdomain) _r_sys_getusername (sid, is_withdomain, out_buffer)
#define _r_sys_loadicon(out_buffer, hinst, icon_name, icon_size) _r_sys_loadicon (hinst, icon_name, icon_size, out_buffer)
#define _r_sys_loadlibrary2(out_buffer, lib_name, lib_flags) _r_sys_loadlibrary2 (lib_name, lib_flags, out_buffer)
#define _r_sys_loadlibraryasresource(out_buffer, lib_name) _r_sys_loadlibraryasresource (lib_name, out_buffer)
#define _r_sys_openprocess(out_buffer, process_id, desired_access) _r_sys_openprocess (ULongToHandle (process_id), desired_access, out_buffer)
#define _r_sys_queryprocessstring(out_buffer, process_handle, info_class) _r_sys_queryprocessstring (process_handle, info_class, out_buffer)
#define _r_sys_querytaginformation(pid, tag) _app_compat_querytaginformation (pid, tag)
#define _r_sys_querytokeninformation(out_buffer, token_handle, token_class) _r_sys_querytokeninformation (token_handle, token_class, out_buffer)
#define _r_theme_initialize(hwnd) _r_theme_initialize (hwnd, _r_theme_isenabled ())
#define _r_toolbar_getidealsize(hwnd, ctrl_id, is_large, size) _app_compat_toolbar_getidealsize (hwnd, ctrl_id, is_large, size)
#define _r_unixtime_to_filetime(file_time, unixtime) _r_unixtime_to_filetime (unixtime, file_time)

#include "controls.h"
#include "db.h"
#include "editor.h"
#include "helper.h"
#include "icons.h"
#include "listview.h"
#include "log.h"
#include "messages.h"
#include "network.h"
#include "notifications.h"
#include "packages.h"
#include "profile.h"
#include "search.h"
#include "security.h"
#include "timer.h"
#include "uwp.h"
#include "wfp.h"
