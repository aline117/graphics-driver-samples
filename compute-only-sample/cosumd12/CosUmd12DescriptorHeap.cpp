////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Descriptor Heap implementation
//
// Copyright (C) Microsoft Corporation
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "CosUmd12.h"

#if !(GPUVA || RS_2LEVEL)

int CosUmd12DescriptorHeap::CalculateSize(const D3D12DDIARG_CREATE_DESCRIPTOR_HEAP_0001 * pDesc)
{
    return sizeof(CosUmd12DescriptorHeap) + pDesc->NumDescriptors*sizeof(CosUmd12Descriptor);
}

HRESULT CosUmd12DescriptorHeap::Create(
    CosUmd12Device *    pDevice,
    _In_ const D3D12DDIARG_CREATE_DESCRIPTOR_HEAP_0001 *    pDesc,
    D3D12DDI_HDESCRIPTORHEAP    DescriptorHeap)
{
    CosUmd12DescriptorHeap * pDescriptorHeap = new(DescriptorHeap.pDrvPrivate) CosUmd12DescriptorHeap(pDevice, pDesc);

    return S_OK;
}

#endif  // !RS_2LEVEL

