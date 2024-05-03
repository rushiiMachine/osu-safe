#include "driver.h"
#include "filter.c"

NTSTATUS DriverEntry(
        _In_ PDRIVER_OBJECT DriverObject,
        _In_ PUNICODE_STRING RegistryPath
) {
    UNREFERENCED_PARAMETER(DriverEntry);
    UNREFERENCED_PARAMETER(RegistryPath);

    NTSTATUS status;

    // Try to register our filter
    status = FltRegisterFilter(DriverObject,
                               &FilterRegistration,
                               &GlobalFilterData.FilterHandle);

    FLT_ASSERTMSG("Failed to FltRegisterFilter on osu-safe", NT_SUCCESS(status));

    if (NT_SUCCESS(status)) {
        // Start our registered filter
        status = FltStartFiltering(GlobalFilterData.FilterHandle);

        // If failed to start, then unregister and return error
        if (!NT_SUCCESS(status)) {
            FltUnregisterFilter(GlobalFilterData.FilterHandle);
            GlobalFilterData.FilterHandle = NULL;
        }
    }

    return status;
}