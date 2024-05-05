#pragma once

#include <ntifs.h>
#include <wdm.h>

/**
 * Missing header export
 * @see https://learn.microsoft.com/en-us/windows/win32/procthread/zwqueryinformationprocess
 */
extern NTSTATUS ZwQueryInformationProcess(
        _In_      HANDLE ProcessHandle,
        _In_      PROCESSINFOCLASS ProcessInformationClass,
        _Out_     PVOID ProcessInformation,
        _In_      ULONG ProcessInformationLength,
        _Out_opt_ PULONG ReturnLength
);

/**
 * A kernel mode variant of GetProcessImageFileNameW from psapi.h
 * @param ProcessId
 * @param ImageName A pointer to an uninitialized UNICODE_STRING,
 *                  whose buffer should be freed with ExFreePool when done with.
 */
NTSTATUS ZwGetProcessImageFileNameW(
        _In_ HANDLE ProcessId,
        _Out_ PUNICODE_STRING ImageName
);

/**
 * Finds the last path section and creates a new UNICODE_STRING with the buffer pointing the ending section.
 * This does not allocate a new buffer.
 */
UNICODE_STRING GetFileNameW(_In_ UNICODE_STRING FilePath);

/**
 * Same thing as GetFileNameW but it gets everything up until the last directory separator
 */
UNICODE_STRING GetParentNameW(_In_ UNICODE_STRING FilePath);

/**
 * Advances the buffer pointer a specific amount of bytes.
 * This will cause UB if the amount of bytes isn't a multiple of 2.
 */
BOOLEAN SkipBytesW(_Inout_ PUNICODE_STRING String, USHORT bytes);
