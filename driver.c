#include "driver.h"
#include "filter.h"
#include "util.h"

NTSTATUS DriverEntry(
    _In_ CONST PDRIVER_OBJECT DriverObject,
    _In_ CONST PUNICODE_STRING RegistryPath
) {
    UNREFERENCED_PARAMETER((void*) DriverEntry);
    UNREFERENCED_PARAMETER(RegistryPath);

    NTSTATUS status;

    // Try to register our filter
    status = FltRegisterFilter(DriverObject,
                               &FilterRegistration,
                               &gFilterData.FilterHandle);

    FLT_ASSERTMSG("Failed to FltRegisterFilter on osu-safe", NT_SUCCESS(status));

    if (NT_SUCCESS(status)) {
        // Start our registered filter
        status = FltStartFiltering(gFilterData.FilterHandle);

        // If failed to start, then unregister and return error
        if (!NT_SUCCESS(status)) {
            FltUnregisterFilter(gFilterData.FilterHandle);
            gFilterData.FilterHandle = NULL;
        }
    }

    return status;
}
