#pragma once

#include <fltKernel.h>

#pragma prefast(disable:__WARNING_ENCODE_MEMBER_FUNCTION_POINTER, "Not valid for kernel mode drivers")

/**
 * Entry routine for this KMDF driver.
 * @param DriverObject This driver's WDM object
 * @param RegistryPath This driver's Parameters key in the registry.
 * @return A status code as defined in ntdef.h
 * @see https://learn.microsoft.com/en-us/windows-hardware/drivers/wdf/driverentry-for-kmdf-drivers
 */
NTSTATUS DriverEntry(
        _In_ PDRIVER_OBJECT DriverObject,
        _In_ PUNICODE_STRING RegistryPath
);
