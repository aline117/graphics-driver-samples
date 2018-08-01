////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Utility implementation
//
// Copyright (C) Microsoft Corporation
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "CosKmd.h"

#include "CosKmdLogging.h"
#include "CosKmdUtil.tmh"

#include "CosKmdAdapter.h"
#include "CosKmdContext.h"
#include "CosKmdAllocation.h"
#include "CosGpuCommand.h"
#include "CosKmdGlobal.h"
#include "CosKmdUtil.h"

D3DDDIFORMAT
TranslateDxgiFormat(
    DXGI_FORMAT dxgiFormat)
{
    switch (dxgiFormat)
    {
    case DXGI_FORMAT_R8G8B8A8_UNORM:
    case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
        return D3DDDIFMT_A8B8G8R8;
    case DXGI_FORMAT_R10G10B10A2_UNORM:
        return D3DDDIFMT_A2B10G10R10;
    case DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM:
        return D3DDDIFMT_A2B10G10R10_XR_BIAS;
    case DXGI_FORMAT_R16G16B16A16_FLOAT:
        return D3DDDIFMT_A16B16G16R16F;
    case DXGI_FORMAT_B8G8R8A8_UNORM:
    case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
        return D3DDDIFMT_A8R8G8B8;
    case DXGI_FORMAT_B8G8R8X8_UNORM:
    case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
        return D3DDDIFMT_X8R8G8B8;
    case DXGI_FORMAT_B5G6R5_UNORM:
        return D3DDDIFMT_R5G6B5;
    case DXGI_FORMAT_B5G5R5A1_UNORM:
        return D3DDDIFMT_A1R5G5B5;
    case DXGI_FORMAT_B4G4R4A4_UNORM:
        return D3DDDIFMT_A4R4G4B4;
    case DXGI_FORMAT_A8_UNORM:
        return D3DDDIFMT_A8;
    case DXGI_FORMAT_R16G16_UNORM:
        return D3DDDIFMT_G16R16;
    case DXGI_FORMAT_R16G16B16A16_UNORM:
        return D3DDDIFMT_A16B16G16R16;
    case DXGI_FORMAT_R16_FLOAT:
        return D3DDDIFMT_R16F;
    case DXGI_FORMAT_R16G16_FLOAT:
        return D3DDDIFMT_G16R16F;
    case DXGI_FORMAT_R32_FLOAT:
        return D3DDDIFMT_R32F;
    case DXGI_FORMAT_R32G32_FLOAT:
        return D3DDDIFMT_G32R32F;
    case DXGI_FORMAT_R32G32B32A32_FLOAT:
        return D3DDDIFMT_A32B32G32R32F;
    case DXGI_FORMAT_YUY2:
        return D3DDDIFMT_YUY2;
    case DXGI_FORMAT_D16_UNORM:
        return D3DDDIFMT_D16_LOCKABLE;
    case DXGI_FORMAT_D32_FLOAT:
        return D3DDDIFMT_D32F_LOCKABLE;
    case DXGI_FORMAT_R8G8_SNORM:
        return D3DDDIFMT_V8U8;
    case DXGI_FORMAT_R8_UNORM:
        return D3DDDIFMT_R8;
    case DXGI_FORMAT_R8G8_UNORM:
        return D3DDDIFMT_G8R8;
    case DXGI_FORMAT_BC1_UNORM:
    case DXGI_FORMAT_BC1_UNORM_SRGB:
        return D3DDDIFMT_DXT1;
    case DXGI_FORMAT_BC2_UNORM:
    case DXGI_FORMAT_BC2_UNORM_SRGB:
        return D3DDDIFMT_DXT2;
    case DXGI_FORMAT_BC3_UNORM:
    case DXGI_FORMAT_BC3_UNORM_SRGB:
        return D3DDDIFMT_DXT3;
    case DXGI_FORMAT_R8G8B8A8_SNORM:
        return D3DDDIFMT_Q8W8V8U8;
    case DXGI_FORMAT_R16G16_SNORM:
        return D3DDDIFMT_V16U16;
    case DXGI_FORMAT_UNKNOWN:
        return D3DDDIFMT_UNKNOWN;
    default:
        NT_ASSERT(FALSE);
        return D3DDDIFMT_UNKNOWN;
    }
}

#if USE_SIMPENROX

extern "C"
{

void * __cdecl malloc(
    _In_ size_t _Size)
{
    UNREFERENCED_PARAMETER(_Size);

    return NULL;
}

//
// TODO[indyz]: Add support for print out from SimPenrose
//

int __cdecl printf(
    _In_z_ _Printf_format_string_ char const* const _Format,
    ...)
{
    UNREFERENCED_PARAMETER(_Format);

    return 0;
}

int __cdecl fprintf(
    _Inout_                       FILE*       const _Stream,
    _In_z_ _Printf_format_string_ char const* const _Format,
    ...)
{
    UNREFERENCED_PARAMETER(_Stream);
    UNREFERENCED_PARAMETER(_Format);

    return 0;
}

FILE * __cdecl __acrt_iob_func(unsigned)
{
    return NULL;
}

}

#endif // USE_SIMPENROX

COS_NONPAGED_SEGMENT_BEGIN; //================================================

void* __cdecl operator new (
    size_t Size,
    POOL_TYPE PoolType,
    COS_ALLOC_TAG Tag
    ) throw ()
{
    if (!Size) Size = 1;
    return ExAllocatePoolWithTag(PoolType, Size, ULONG(Tag));
} // operator new ( size_t, POOL_TYPE, COS_ALLOC_TAG )

void __cdecl operator delete ( void* Ptr ) throw ()
{
    if (Ptr) ExFreePool(Ptr);
} // operator delete ( void* )

void __cdecl operator delete (void* Ptr, size_t) throw ()
{
    if (Ptr) ExFreePool(Ptr);
} // operator delete (void*, size_t)

void* __cdecl operator new[] (
    size_t Size,
    POOL_TYPE PoolType,
    COS_ALLOC_TAG Tag
    ) throw ()
{
    if (!Size) Size = 1;
    return ExAllocatePoolWithTag(PoolType, Size, ULONG(Tag));
} // operator new[] ( size_t, POOL_TYPE, COS_ALLOC_TAG )

void __cdecl operator delete[] ( void* Ptr ) throw ()
{
    if (Ptr) ExFreePool(Ptr);
} // operator delete[] ( void* )

void* __cdecl operator new ( size_t, void* Ptr ) throw ()
{
    return Ptr;
} // operator new ( size_t, void* )

void __cdecl operator delete ( void*, void* ) throw ()
{} // void operator delete ( void*, void* )

void* __cdecl operator new[] ( size_t, void* Ptr ) throw ()
{
    return Ptr;
} // operator new[] ( size_t, void* )

void __cdecl operator delete[] ( void*, void* ) throw ()
{} // void operator delete[] ( void*, void* )

COS_NONPAGED_SEGMENT_END; //==================================================
COS_PAGED_SEGMENT_BEGIN; //===================================================

_Use_decl_annotations_
DXGI_FORMAT DxgiFormatFromD3dDdiFormat (D3DDDIFORMAT Format)
{
    PAGED_CODE();

    switch (Format)
    {
    case D3DDDIFMT_A8:
        return DXGI_FORMAT_A8_UNORM;
    case D3DDDIFMT_L8:
        // Map D3D9 L8 format to D3D11 R8 - see shader converter EmitSamplerSwizzle()
        return DXGI_FORMAT_R8_UNORM;
    case D3DDDIFMT_L16:
        // Map D3D9 L16 format to D3D11 R16 - see shader converter EmitSamplerSwizzle()
        return DXGI_FORMAT_R16_UNORM;
    case D3DDDIFMT_R5G6B5:
        return DXGI_FORMAT_B5G6R5_UNORM;
    case D3DDDIFMT_A1R5G5B5:
        return DXGI_FORMAT_B5G5R5A1_UNORM;
    case D3DDDIFMT_A8R8G8B8:
        return DXGI_FORMAT_B8G8R8A8_UNORM;
    case D3DDDIFMT_X8R8G8B8:
        COS_LOG_WARNING("Forcing D3DDDIFMT_X8R8G8B8 to DXGI_FORMAT_B8G8R8A8_UNORM");
        return DXGI_FORMAT_B8G8R8A8_UNORM;//DXGI_FORMAT_R8G8B8A8_UNORM;
    case D3DDDIFMT_A8B8G8R8:
        return DXGI_FORMAT_R8G8B8A8_UNORM;
    case D3DDDIFMT_YUY2:
        return DXGI_FORMAT_YUY2;
    case D3DDDIFMT_DXT1:
        return DXGI_FORMAT_BC1_UNORM;
    case D3DDDIFMT_DXT2:
    case D3DDDIFMT_DXT3:
        return DXGI_FORMAT_BC2_UNORM;
    case D3DDDIFMT_DXT4:
    case D3DDDIFMT_DXT5:
        return DXGI_FORMAT_BC3_UNORM;
    case D3DDDIFMT_V8U8:
        return DXGI_FORMAT_R8G8_SNORM;
    case D3DDDIFMT_D16:
    case D3DDDIFMT_D16_LOCKABLE:
        return DXGI_FORMAT_D16_UNORM;
    case D3DDDIFMT_D32F_LOCKABLE:
        return DXGI_FORMAT_D32_FLOAT;
    case D3DDDIFMT_S8D24:
    case D3DDDIFMT_X8D24:
    case D3DDDIFMT_D24S8:
    case D3DDDIFMT_D24X8:
        return DXGI_FORMAT_D24_UNORM_S8_UINT;
    case D3DDDIFMT_A16B16G16R16:
        return DXGI_FORMAT_R16G16B16A16_UNORM;
    case D3DDDIFMT_R16F:
        return DXGI_FORMAT_R16_FLOAT;
    case D3DDDIFMT_G16R16F:
        return DXGI_FORMAT_R16G16_FLOAT;
    case D3DDDIFMT_A16B16G16R16F:
        return DXGI_FORMAT_R16G16B16A16_FLOAT;
    case D3DDDIFMT_R32F:
        return DXGI_FORMAT_R32_FLOAT;
    case D3DDDIFMT_G32R32F:
        return DXGI_FORMAT_R32G32_FLOAT;
    case D3DDDIFMT_A32B32G32R32F:
        return DXGI_FORMAT_R32G32B32A32_FLOAT;
    case D3DDDIFMT_A2B10G10R10:
        return DXGI_FORMAT_R10G10B10A2_UNORM;
    case D3DDDIFMT_G16R16:
        return DXGI_FORMAT_R16G16_UNORM;
    case D3DDDIFMT_V16U16:
        return DXGI_FORMAT_R16G16_SNORM;
    case D3DDDIFMT_Q8W8V8U8:
        return DXGI_FORMAT_R8G8B8A8_SNORM;
    case D3DDDIFMT_Q16W16V16U16:
        return DXGI_FORMAT_R16G16B16A16_SNORM;
    case D3DDDIFMT_A4R4G4B4:
        return DXGI_FORMAT_B4G4R4A4_UNORM;
    case D3DDDIFMT_P8:
        return DXGI_FORMAT_P8;
    case D3DDDIFMT_A8P8:
        return DXGI_FORMAT_A8P8;
    case D3DDDIFMT_VERTEXDATA:
    case D3DDDIFMT_INDEX16:
    case D3DDDIFMT_INDEX32:
    case D3DDDIFMT_UNKNOWN:
        return DXGI_FORMAT_UNKNOWN;
    default:
        NT_ASSERT(!"Unknown format");
        return DXGI_FORMAT_UNKNOWN;
    }
}

_Use_decl_annotations_
NTSTATUS CosOpenDevice (
    UNICODE_STRING* FileNamePtr,
    ACCESS_MASK DesiredAccess,
    ULONG ShareAccess,
    FILE_OBJECT** FileObjectPPtr,
    COS_ALLOC_TAG AllocTag
    )
{
    PAGED_CODE();
    COS_ASSERT_MAX_IRQL(PASSIVE_LEVEL);

    HANDLE fileHandle;
    OBJECT_ATTRIBUTES attributes;
    InitializeObjectAttributes(
        &attributes,
        FileNamePtr,
        OBJ_KERNEL_HANDLE,
        NULL,       // RootDirectory
        NULL);      // SecurityDescriptor

    IO_STATUS_BLOCK iosb;
    NTSTATUS status = ZwCreateFile(
            &fileHandle,
            DesiredAccess,
            &attributes,
            &iosb,
            nullptr,                    // AllocationSize
            FILE_ATTRIBUTE_NORMAL,
            ShareAccess,
            FILE_OPEN,
            FILE_NON_DIRECTORY_FILE,    // CreateOptions
            nullptr,                    // EaBuffer
            0);                         // EaLength
    if (!NT_SUCCESS(status)) {
        COS_LOG_ERROR(
            "ZwCreateFile failed. (status=%!STATUS!, FileNamePtr=%wZ, DesiredAccess=0x%x, ShareAccess=0x%x)",
            status,
            FileNamePtr,
            DesiredAccess,
            ShareAccess);
        return status;
    } // if
    auto closeHandle = COS_FINALLY::Do([&] {
        PAGED_CODE();
        NTSTATUS closeStatus = ZwClose(fileHandle);
        UNREFERENCED_PARAMETER(closeStatus);
        NT_ASSERT(NT_SUCCESS(closeStatus));
    });

    status = ObReferenceObjectByHandleWithTag(
            fileHandle,
            DesiredAccess,
            *IoFileObjectType,
            KernelMode,
            AllocTag,
            reinterpret_cast<PVOID*>(FileObjectPPtr),
            nullptr);   // HandleInformation
    if (!NT_SUCCESS(status)) {
        COS_LOG_ERROR(
            "ObReferenceObjectByHandleWithTag(...) failed. (status=%!STATUS!, fileHandle=%p)",
            status,
            fileHandle);
        return status;
    } // if

    NT_ASSERT(*FileObjectPPtr);
    NT_ASSERT(status == STATUS_SUCCESS);
    return status;
}

_Use_decl_annotations_
NTSTATUS CosSendWriteSynchronously (
    FILE_OBJECT* FileObjectPtr,
    void* InputBufferPtr,
    ULONG InputBufferLength,
    ULONG_PTR* InformationPtr
    )
{
    PAGED_CODE();
    COS_ASSERT_MAX_IRQL(PASSIVE_LEVEL);

    KEVENT event;
    KeInitializeEvent(&event, NotificationEvent, FALSE);

    DEVICE_OBJECT* deviceObjectPtr = IoGetRelatedDeviceObject(FileObjectPtr);
    auto iosb = IO_STATUS_BLOCK();
    IRP* irpPtr = IoBuildSynchronousFsdRequest(
            IRP_MJ_WRITE,
            deviceObjectPtr,
            InputBufferPtr,
            InputBufferLength,
            nullptr,            // StartingOffset
            &event,
            &iosb);
    if (!irpPtr) {
        COS_LOG_LOW_MEMORY(
            "IoBuildSynchronousFsdRequest(...) failed. (deviceObjectPtr=%p, FileObjectPtr=%p, InputBufferPtr=%p, InputBufferLength=%d)",
            deviceObjectPtr,
            FileObjectPtr,
            InputBufferPtr,
            InputBufferLength);
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    IO_STACK_LOCATION* irpSp = IoGetNextIrpStackLocation(irpPtr);
    irpSp->FileObject = FileObjectPtr;

    iosb.Status = STATUS_NOT_SUPPORTED;
    NTSTATUS status = IoCallDriver(deviceObjectPtr, irpPtr);
    if (status == STATUS_PENDING) {
        KeWaitForSingleObject(
            &event,
            Executive,
            KernelMode,
            FALSE,          // Alertable
            nullptr);       // Timeout

        status = iosb.Status;
    }

    *InformationPtr = iosb.Information;
    return status;
}

_Use_decl_annotations_
NTSTATUS CosSendIoctlSynchronously (
    FILE_OBJECT* FileObjectPtr,
    ULONG IoControlCode,
    void* InputBufferPtr,
    ULONG InputBufferLength,
    void* OutputBufferPtr,
    ULONG OutputBufferLength,
    BOOLEAN InternalDeviceIoControl,
    ULONG_PTR* InformationPtr
    )
{
    PAGED_CODE();
    COS_ASSERT_MAX_IRQL(PASSIVE_LEVEL);

    KEVENT event;
    KeInitializeEvent(&event, NotificationEvent, FALSE);

    DEVICE_OBJECT* deviceObjectPtr = IoGetRelatedDeviceObject(FileObjectPtr);
    auto iosb = IO_STATUS_BLOCK();
    IRP* irpPtr = IoBuildDeviceIoControlRequest(
            IoControlCode,
            deviceObjectPtr,
            InputBufferPtr,
            InputBufferLength,
            OutputBufferPtr,                    // OutputBuffer
            OutputBufferLength,                 // OutputBufferLength
            InternalDeviceIoControl,
            &event,
            &iosb);
    if (!irpPtr) {
        COS_LOG_LOW_MEMORY(
            "IoBuildDeviceIoControlRequest(...) failed. (IoControlCode=0x%x, deviceObjectPtr=%p, FileObjectPtr=%p)",
            IoControlCode,
            deviceObjectPtr,
            FileObjectPtr);
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    IO_STACK_LOCATION* irpSp = IoGetNextIrpStackLocation(irpPtr);
    irpSp->FileObject = FileObjectPtr;

    iosb.Status = STATUS_NOT_SUPPORTED;
    NTSTATUS status = IoCallDriver(deviceObjectPtr, irpPtr);
    if (status == STATUS_PENDING) {
        KeWaitForSingleObject(
            &event,
            Executive,
            KernelMode,
            FALSE,          // Alertable
            nullptr);       // Timeout

        status = iosb.Status;
    }

    *InformationPtr = iosb.Information;
    return status;
}

COS_PAGED_SEGMENT_END; //=====================================================

