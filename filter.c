#pragma once

#include "filter.h"
#include <fltKernel.h>

NTSTATUS OsuFilterUnload(
        _In_ FLT_FILTER_UNLOAD_FLAGS Flags
) {
    UNREFERENCED_PARAMETER(Flags);

    PAGED_CODE();

    FltUnregisterFilter(GlobalFilterData.FilterHandle);

    return STATUS_SUCCESS;
}

NTSTATUS OsuFilterTeardownQuery(
        _In_ PCFLT_RELATED_OBJECTS FltObjects,
        _In_ FLT_INSTANCE_QUERY_TEARDOWN_FLAGS Reason
) {
    UNREFERENCED_PARAMETER(FltObjects);
    UNREFERENCED_PARAMETER(Reason);

    PAGED_CODE();

    // Always allow teardowns
    return STATUS_SUCCESS;
}