#pragma once

#include "filter.h"
#include "util.h"
#include <wdm.h>
#include <minwindef.h>
#include <fltKernel.h>

NTSTATUS OsuFilterUnload(
        _In_ FLT_FILTER_UNLOAD_FLAGS Flags
) {
    UNREFERENCED_PARAMETER(Flags);

    PAGED_CODE();

    FltUnregisterFilter(gFilterData.FilterHandle);

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

FLT_PREOP_CALLBACK_STATUS OsuPreStreamHandleCreate(
        _Inout_ PFLT_CALLBACK_DATA Data,
        _In_ PCFLT_RELATED_OBJECTS FltObjects,
        _Out_ PVOID* CompletionContext
) {
    UNREFERENCED_PARAMETER(FltObjects);
    UNREFERENCED_PARAMETER(CompletionContext);

    PAGED_CODE();

    NTSTATUS status;
    HANDLE invokingProcId;
    UNICODE_STRING invokingProcPath;
    PFLT_FILE_NAME_INFORMATION filenameInfo = NULL;

    // Get the process that triggered this event
    invokingProcId = PsGetThreadProcessId(Data->Thread);

    // Get the process's path
    status = ZwGetProcessImageFileNameW(invokingProcId, &invokingProcPath);
    if (!NT_SUCCESS(status)) {
        DbgPrint("!!! osu-safe.sys --- ZwGetProcessImageFileNameW failed; status 0x%X\n", status);
        goto cleanup;
    }

    // Check that the proc path ends with osu!.exe
    UNICODE_STRING invokingProcName = GetFileNameW(invokingProcPath);
    if (RtlEqualUnicodeString(&invokingProcName, &OSU_NAME, FALSE)) {
        goto cleanup;
    }

    // Try to get the file name if it's available or safe to get, otherwise skip if unavailable
    status = FltGetFileNameInformation(Data,
                                       FLT_FILE_NAME_NORMALIZED | FLT_FILE_NAME_QUERY_DEFAULT,
                                       &filenameInfo);

    if (!NT_SUCCESS(status)) {
        DbgPrint("!!! osu-safe.sys --- FltGetFileNameInformation failed; status 0x%X\n", status);
        goto cleanup;
    }

    // Process result of FltGetFileNameInformation to get extension
    UNICODE_STRING extension;
    status = FltParseFileName(&filenameInfo->Name, &extension, NULL, NULL);
    if (!NT_SUCCESS(status)) {
        DbgPrint("!!! osu-safe.sys --- FltParseFileName failed; status 0x%X\n", status);
        goto cleanup;
    }

    // Check if the filename extension is either jpg or png, which osu! supports
    if (!RtlEqualUnicodeString(&extension, &JPG, TRUE) &&
        !RtlEqualUnicodeString(&extension, &PNG, TRUE)) {
        // Not a matching extension, skip
        goto cleanup;
    }

    UNICODE_STRING invokingProcDir = GetParentNameW(invokingProcName);
    UNICODE_STRING targetFilePath = filenameInfo->Name;

    // Check if this is in the osu!.exe directory
    if (!RtlPrefixUnicodeString(&invokingProcDir, &targetFilePath, FALSE))
        goto cleanup;

    NT_ASSERT(SkipBytesW(&targetFilePath, invokingProcDir.Length));

    // Check if this is in the songs folder
    if (!RtlPrefixUnicodeString(&SONGS, &targetFilePath, FALSE))
        goto cleanup;

    Data->IoStatus.Status = STATUS_ACCESS_DENIED;
    Data->IoStatus.Information = 0;

    cleanup:
    if (invokingProcPath.Buffer) { ExFreePool(invokingProcPath.Buffer); }
    if (filenameInfo) { FltReleaseFileNameInformation(filenameInfo); }

    return FLT_PREOP_SUCCESS_NO_CALLBACK;
}
