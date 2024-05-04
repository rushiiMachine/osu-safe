#include "util.h"
#include <limits.h>

NTSTATUS ZwGetProcessImageFileNameW(
        _In_ HANDLE ProcessId,
        _Out_ PUNICODE_STRING ImageName
) {
    NTSTATUS status;
    PEPROCESS eProcess = NULL;
    HANDLE hProcess = NULL;
    ULONG length;

    // Fetch the EPROCESS from a pid if it exists
    status = PsLookupProcessByProcessId(ProcessId, &eProcess);
    if (!NT_SUCCESS(status)) {
        DbgPrint("!!! osu-safe.sys --- PsLookupProcessByProcessId failed: 0x%X\n", status);
        goto cleanup;
    }

    // Fetch the proc handle
    status = ObOpenObjectByPointer(eProcess, 0, NULL, 0, 0, KernelMode, &hProcess);
    if (!NT_SUCCESS(status)) {
        DbgPrint("!!! osu-safe.sys --- ObOpenObjectByPointer failed: 0x%X\n", status);
        goto cleanup;
    }

    // Get the buffer length we need to allocate for the path
    status = ZwQueryInformationProcess(ProcessId, ProcessImageFileName, NULL, 0, &length);
    if (!NT_SUCCESS(status) && status != STATUS_INFO_LENGTH_MISMATCH) {
        DbgPrint("!!! osu-safe.sys --- ZwQueryInformationProcess length query failed: 0x%X\n", status);
        goto cleanup;
    }

            NT_ASSERTMSG("!!! osu-safe.sys --- ZwQueryInformationProcess too long\n", length <= USHRT_MAX);

    // Allocate string buffer
    ImageName->Length = 0;
    ImageName->MaximumLength = (USHORT) length;
    ImageName->Buffer = ExAllocatePoolZero(NonPagedPool, length, 'jnf8');

    if (!ImageName->Buffer) {
        DbgPrint("!!! osu-safe.sys --- Insufficient resources to allocate pool");
        status = STATUS_INSUFFICIENT_RESOURCES;
        goto cleanup;
    }

    // Fill in the string path buffer
    status = ZwQueryInformationProcess(ProcessId,
                                       ProcessImageFileName,
                                       ImageName->Buffer,
                                       length,
                                       &length);
    if (!NT_SUCCESS(status)) {
        DbgPrint("!!! osu-safe.sys --- ZwQueryInformationProcess failed: 0x%X\n", status);
        goto cleanup;
    }

    cleanup:
    if (eProcess) { ObDereferenceObject(eProcess); }
    if (hProcess) { ZwClose(hProcess); }

    return status;
}

UNICODE_STRING GetFileNameW(_In_ UNICODE_STRING FilePath) {
    PWCHAR bufPtr = (PWCHAR) ((PCHAR) FilePath.Buffer + FilePath.Length);
    while (bufPtr >= FilePath.Buffer) {
        if (*bufPtr == L'\\') {
            ++bufPtr;
            break;
        }
        --bufPtr;
    }

    UNICODE_STRING fileName = {
            /* Length= */ (USHORT) ((PCHAR) FilePath.Buffer - (PCHAR) bufPtr),
            /* MaximumLength= */ 0,
            /* Buffer= */ bufPtr,
    };

    return fileName;
}

//NTSTATUS CacheZwQueryInformationProcess() {
//    if (!ZwQueryInformationProcess) {
//        UNICODE_STRING routineName = RTL_CONSTANT_STRING(L"ZwQueryInformationProcess");
//        ZwQueryInformationProcess = (QUERY_INFO_PROCESS) MmGetSystemRoutineAddress(&routineName);
//
//        if (!ZwQueryInformationProcess) {
//            DbgPrint("!!! osu-safe.sys --- Cannot resolve ZwQueryInformationProcess\n");
//            return STATUS_LINK_FAILED;
//        }
//    }
//
//    return STATUS_SUCCESS;
//}
