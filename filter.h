#pragma once

#include <fltKernel.h>

typedef struct OSU_FILTER_DATA {
    /** An active handle to our filter returned by FltRegisterFilter */
    PFLT_FILTER FilterHandle;
} OSU_FILTER_DATA, * POSU_FILTER_DATA;

/**
 * Callback routine when this filter is being unloaded.
 * @param Flags Bitmask describing this unload request.
 * @return STATUS_SUCCESS or an error code such as STATUS_FLT_DO_NOT_DETACH
 * @see https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/fltkernel/nc-fltkernel-pflt_filter_unload_callback
 */
NTSTATUS OsuFilterUnload(
        _In_ FLT_FILTER_UNLOAD_FLAGS Flags
);

/**
 * Callback routine to decide whether a teardown of this filter is allowed.
 * @param FltObjects Opaque pointer to the objects related to the current I/O operation.
 * @param Flags Flags explaining why the filter is trying to be torn down.
 * @return A status code as defined in ntdef.h
 * @see https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/fltkernel/nc-fltkernel-pflt_instance_query_teardown_callback
 */
NTSTATUS OsuFilterTeardownQuery(
        _In_ PCFLT_RELATED_OBJECTS FltObjects,
        _In_ FLT_INSTANCE_QUERY_TEARDOWN_FLAGS Flags
);

/**
 * Callback routine before a file handle is created.
 * @see https://learn.microsoft.com/en-us/windows-hardware/drivers/kernel/irp-mj-create
 * @see https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/fltkernel/nc-fltkernel-pflt_pre_operation_callback
 */
FLT_PREOP_CALLBACK_STATUS OsuPreStreamHandleCreate(
        _Inout_ PFLT_CALLBACK_DATA Data,
        _In_ PCFLT_RELATED_OBJECTS FltObjects,
        _Out_ PVOID* CompletionContext
);

#ifdef ALLOC_PRAGMA
#pragma alloc_text(PAGE, OsuFilterUnload)
#pragma alloc_text(PAGE, OsuFilterTeardownQuery)
#pragma alloc_text(PAGE, OsuPreStreamHandleCreate)
#endif

/**
 * Constant global instance of the callbacks data for FLT_REGISTRATION
 * @see https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/fltkernel/ns-fltkernel-_flt_operation_registration
 */
static const FLT_OPERATION_REGISTRATION FilterRegistrationCallbacks[] = {
        {
                /* MajorFunction= */ IRP_MJ_CREATE,
                /* Flags= */ 0,
                /* PreOperation= */ OsuPreStreamHandleCreate,
                /* PostOperation= */ NULL,
        },

        // Array terminator
        {                            IRP_MJ_OPERATION_END},
};

static const FLT_CONTEXT_REGISTRATION FilterRegistrationContexts[] = {
        {
                /* ContextType= */ FLT_STREAMHANDLE_CONTEXT,
                /* Flags= */ 0,
                /* ContextCleanupCallback= */ NULL,
        },

        // Array terminator
        {                          FLT_CONTEXT_END},
};

/**
 * Constant global instance of the registration data we use to call FltRegisterFilter
 * @see https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/fltkernel/ns-fltkernel-_flt_registration
 */
static const FLT_REGISTRATION FilterRegistration = {
        /* Size= */                             sizeof(FLT_REGISTRATION),
        /* Version= */                          FLT_REGISTRATION_VERSION,
        /* Flags= */                            0,
        /* ContextRegistration= */              FilterRegistrationContexts,
        /* OperationRegistration= */            FilterRegistrationCallbacks,
        /* FilterUnloadCallback= */             OsuFilterUnload,
        /* InstanceSetupCallback= */            NULL,
        /* InstanceQueryTeardownCallback= */    OsuFilterTeardownQuery,
        /* InstanceTeardownStartCallback= */    NULL,
        /* InstanceTeardownCompleteCallback= */ NULL,
        /* GenerateFileNameCallback= */         NULL,
        /* NormalizeNameComponentCallback= */   NULL,
        /* NormalizeContextCleanupCallback= */  NULL,
};

/**
 * Global static instance of the filter data, initialized by DriverEntry.
 */
static OSU_FILTER_DATA gFilterData;

// String constants
static const UNICODE_STRING OSU_NAME = RTL_CONSTANT_STRING(L"osu!.exe");
static const UNICODE_STRING JPG = RTL_CONSTANT_STRING(L"jpg");
static const UNICODE_STRING PNG = RTL_CONSTANT_STRING(L"png");
