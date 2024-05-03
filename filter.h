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

#ifdef ALLOC_PRAGMA
#pragma alloc_text(PAGE, OsuFilterUnload)
#pragma alloc_text(PAGE, OsuFilterTeardownQuery)
#endif

/**
 * Global static instance of the registration data we use to call FltRegisterFilter
 * @see https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/fltkernel/ns-fltkernel-_flt_registration
 */
const FLT_REGISTRATION FilterRegistration = {
        /* Size= */                             sizeof(FLT_REGISTRATION),
        /* Version= */                          FLT_REGISTRATION_VERSION,
        /* Flags= */                            0,
        /* ContextRegistration= */              NULL,
        /* OperationRegistration= */            NULL,
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
OSU_FILTER_DATA GlobalFilterData = {
        /* FilterHandle= */ NULL,
};