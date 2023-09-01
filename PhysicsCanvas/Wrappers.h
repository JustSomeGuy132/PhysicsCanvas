#pragma once
#include <d3d11.h>
#include <d3d11_3.h>
#include <wrl.h>
#include "Common\DeviceResources.h"
#include "..\Common\DirectXHelper.h"

class D3D11DeviceWrapper : public ID3D11Device
{
private:
    Microsoft::WRL::ComPtr<ID3D11Device3> m_device3;

public:
    // Constructor that takes an ID3D11Device3
    D3D11DeviceWrapper(ID3D11Device3* device3) : m_device3(device3) {}

    // Implement the necessary ID3D11Device methods by forwarding calls to m_device3
    
    // IUnknown methods
    STDMETHOD_(ULONG, AddRef)() override { return m_device3->AddRef(); }
    STDMETHOD_(ULONG, Release)() override { return m_device3->Release(); }
    STDMETHOD(QueryInterface)(REFIID riid, void** ppv) override { return m_device3->QueryInterface(riid, ppv); }

    // ID3D11Device methods
    STDMETHOD(CreateBuffer)(const D3D11_BUFFER_DESC* pDesc, const D3D11_SUBRESOURCE_DATA* pInitialData, ID3D11Buffer** ppBuffer) override
    {
        return m_device3->CreateBuffer(pDesc, pInitialData, ppBuffer);
    }
    STDMETHOD(CreateVertexShader)(const void* pShaderBytecode, SIZE_T BytecodeLength, ID3D11ClassLinkage* pClassLinkage, ID3D11VertexShader** ppVertexShader) override
    {
        return m_device3->CreateVertexShader(pShaderBytecode, BytecodeLength, pClassLinkage, ppVertexShader);
    }
    STDMETHOD(CreateInputLayout)(const D3D11_INPUT_ELEMENT_DESC* pInputElementDescs, UINT NumElements,
        const void* pShaderBytecodeWithInputSignature, SIZE_T BytecodeLength,
        ID3D11InputLayout** ppInputLayout) override
    {
        return m_device3->CreateInputLayout(pInputElementDescs, NumElements, pShaderBytecodeWithInputSignature, BytecodeLength, ppInputLayout);
    }
    STDMETHOD(CreatePixelShader)(const void* pShaderBytecode, SIZE_T BytecodeLength, ID3D11ClassLinkage* pClassLinkage,
        ID3D11PixelShader** ppPixelShader) override
    {
        return m_device3->CreatePixelShader(pShaderBytecode, BytecodeLength, pClassLinkage, ppPixelShader);
    }
    STDMETHOD(CreateGeometryShader)(const void* pShaderBytecode, SIZE_T BytecodeLength, ID3D11ClassLinkage* pClassLinkage,
        ID3D11GeometryShader** ppGeometryShader) override
    {
        return m_device3->CreateGeometryShader(pShaderBytecode, BytecodeLength, pClassLinkage, ppGeometryShader);
    }
    STDMETHOD(CreateComputeShader)(const void* pShaderBytecode, SIZE_T BytecodeLength, ID3D11ClassLinkage* pClassLinkage,
        ID3D11ComputeShader** ppComputeShader) override
    {
        return m_device3->CreateComputeShader(pShaderBytecode, BytecodeLength, pClassLinkage, ppComputeShader);
    }
    STDMETHOD(CreateDomainShader)(const void* pShaderBytecode, SIZE_T BytecodeLength, ID3D11ClassLinkage* pClassLinkage,
        ID3D11DomainShader** ppDomainShader) override
    {
        return m_device3->CreateDomainShader(pShaderBytecode, BytecodeLength, pClassLinkage, ppDomainShader);
    }
    STDMETHOD(CreateHullShader)(const void* pShaderBytecode, SIZE_T BytecodeLength, ID3D11ClassLinkage* pClassLinkage,
        ID3D11HullShader** ppHullShader) override
    {
        return m_device3->CreateHullShader(pShaderBytecode, BytecodeLength, pClassLinkage, ppHullShader);
    }
    STDMETHOD(CreateRasterizerState)(const D3D11_RASTERIZER_DESC* pRasterizerDesc, ID3D11RasterizerState** ppRasterizerState) override
    {
        return m_device3->CreateRasterizerState(pRasterizerDesc, ppRasterizerState);
    }
    STDMETHOD(CreateDepthStencilState)(const D3D11_DEPTH_STENCIL_DESC* pDepthStencilDesc,
        ID3D11DepthStencilState** ppDepthStencilState) override
    {
        return m_device3->CreateDepthStencilState(pDepthStencilDesc, ppDepthStencilState);
    }
    STDMETHOD(CreateBlendState)(const D3D11_BLEND_DESC* pBlendStateDesc, ID3D11BlendState** ppBlendState) override
    {
        return m_device3->CreateBlendState(pBlendStateDesc, ppBlendState);
    }
    STDMETHOD(CreateSamplerState)(const D3D11_SAMPLER_DESC* pSamplerDesc, ID3D11SamplerState** ppSamplerState) override
    {
        return m_device3->CreateSamplerState(pSamplerDesc, ppSamplerState);
    }
    STDMETHOD(CreateQuery)(const D3D11_QUERY_DESC* pQueryDesc, ID3D11Query** ppQuery) override
    {
        return m_device3->CreateQuery(pQueryDesc, ppQuery);
    }
    STDMETHOD(CreateCounter)(const D3D11_COUNTER_DESC* pCounterDesc, ID3D11Counter** ppCounter) override
    {
        return m_device3->CreateCounter(pCounterDesc, ppCounter);
    }
    STDMETHOD(CreateDeferredContext)(UINT ContextFlags, ID3D11DeviceContext** ppDeferredContext) override
    {
        return m_device3->CreateDeferredContext(ContextFlags, ppDeferredContext);
    }
    STDMETHOD(CreateClassLinkage)(ID3D11ClassLinkage** ppLinkage) override
    {
        return m_device3->CreateClassLinkage(ppLinkage);
    }
    STDMETHOD(CreatePredicate)(const D3D11_QUERY_DESC* pPredicateDesc, ID3D11Predicate** ppPredicate) override
    {
        return m_device3->CreatePredicate(pPredicateDesc, ppPredicate);
    }
    STDMETHOD(CreateDeferredContext1)(UINT ContextFlags, ID3D11DeviceContext1** ppDeferredContext)
    {
        return m_device3->CreateDeferredContext1(ContextFlags, ppDeferredContext);
    }
    STDMETHOD(CreateDeferredContext2)(UINT ContextFlags, ID3D11DeviceContext2** ppDeferredContext) 
    {
        return m_device3->CreateDeferredContext2(ContextFlags, ppDeferredContext);
    }
    STDMETHOD(CreateDeferredContext3)(UINT ContextFlags, ID3D11DeviceContext3** ppDeferredContext) 
    {
        return m_device3->CreateDeferredContext3(ContextFlags, ppDeferredContext);
    }
    STDMETHOD(CreateBlendState1)(const D3D11_BLEND_DESC1* pBlendStateDesc, ID3D11BlendState1** ppBlendState) 
    {
        return m_device3->CreateBlendState1(pBlendStateDesc, ppBlendState);
    }
    STDMETHOD(CreateRasterizerState2)(const D3D11_RASTERIZER_DESC2* pRasterizerDesc, ID3D11RasterizerState2** ppRasterizerState) 
    {
        return m_device3->CreateRasterizerState2(pRasterizerDesc, ppRasterizerState);
    }
    STDMETHOD(CreateShaderResourceView1)(ID3D11Resource* pResource, const D3D11_SHADER_RESOURCE_VIEW_DESC1* pDesc1, ID3D11ShaderResourceView1** ppSRView1) 
    {
        return m_device3->CreateShaderResourceView1(pResource, pDesc1, ppSRView1);
    }
    STDMETHOD(CreateUnorderedAccessView1)(ID3D11Resource* pResource, const D3D11_UNORDERED_ACCESS_VIEW_DESC1* pDesc1, ID3D11UnorderedAccessView1** ppUAView1) 
    {
        return m_device3->CreateUnorderedAccessView1(pResource, pDesc1, ppUAView1);
    }
    STDMETHOD(CreateTexture1D)(const D3D11_TEXTURE1D_DESC* pDesc, const D3D11_SUBRESOURCE_DATA* pInitialData, ID3D11Texture1D** ppTexture1D) override
    {
        return m_device3->CreateTexture1D(pDesc, pInitialData, ppTexture1D);
    }
    STDMETHOD(CreateTexture2D)(const D3D11_TEXTURE2D_DESC* pDesc, const D3D11_SUBRESOURCE_DATA* pInitialData, ID3D11Texture2D** ppTexture2D) override
    {
        return m_device3->CreateTexture2D(pDesc, pInitialData, ppTexture2D);
    }
    STDMETHOD(CreateTexture3D)(const D3D11_TEXTURE3D_DESC* pDesc, const D3D11_SUBRESOURCE_DATA* pInitialData, ID3D11Texture3D** ppTexture3D) override
    {
        return m_device3->CreateTexture3D(pDesc, pInitialData, ppTexture3D);
    }
    STDMETHOD(CreateShaderResourceView)(ID3D11Resource* pResource, const D3D11_SHADER_RESOURCE_VIEW_DESC* pDesc,
        ID3D11ShaderResourceView** ppSRView) override
    {
        return m_device3->CreateShaderResourceView(pResource, pDesc, ppSRView);
    }
    STDMETHOD(CreateUnorderedAccessView)(ID3D11Resource* pResource, const D3D11_UNORDERED_ACCESS_VIEW_DESC* pDesc,
        ID3D11UnorderedAccessView** ppUAView) override
    {
        return m_device3->CreateUnorderedAccessView(pResource, pDesc, ppUAView);
    }
    STDMETHOD(CreateRenderTargetView)(ID3D11Resource* pResource, const D3D11_RENDER_TARGET_VIEW_DESC* pDesc,
        ID3D11RenderTargetView** ppRTView) override
    {
        return m_device3->CreateRenderTargetView(pResource, pDesc, ppRTView);
    }
    STDMETHOD(CreateDepthStencilView)(ID3D11Resource* pResource, const D3D11_DEPTH_STENCIL_VIEW_DESC* pDesc,
        ID3D11DepthStencilView** ppDepthStencilView) override
    {
        return m_device3->CreateDepthStencilView(pResource, pDesc, ppDepthStencilView);
    }
    STDMETHOD(CreateGeometryShaderWithStreamOutput)(const void* pShaderBytecode, SIZE_T BytecodeLength,
        const D3D11_SO_DECLARATION_ENTRY* pSODeclaration, UINT NumEntries,
        const UINT* pBufferStrides, UINT NumStrides, UINT RasterizedStream,
        ID3D11ClassLinkage* pClassLinkage, ID3D11GeometryShader** ppGeometryShader) override
    {
        return m_device3->CreateGeometryShaderWithStreamOutput(pShaderBytecode, BytecodeLength,
            pSODeclaration, NumEntries,
            pBufferStrides, NumStrides, RasterizedStream,
            pClassLinkage, ppGeometryShader);
    }
    STDMETHOD(OpenSharedResource)(HANDLE hResource, REFIID ReturnedInterface, void** ppResource) override
    {
        return m_device3->OpenSharedResource(hResource, ReturnedInterface, ppResource);
    }
    STDMETHOD(CheckFormatSupport)(DXGI_FORMAT Format, UINT* pFormatSupport) override
    {
        return m_device3->CheckFormatSupport(Format, pFormatSupport);
    }
    STDMETHOD(CheckMultisampleQualityLevels)(DXGI_FORMAT Format, UINT SampleCount, UINT* pNumQualityLevels) override
    {
        return m_device3->CheckMultisampleQualityLevels(Format, SampleCount, pNumQualityLevels);
    }
    STDMETHOD_(void, CheckCounterInfo)(D3D11_COUNTER_INFO* pCounterInfo) override
    {
        return m_device3->CheckCounterInfo(pCounterInfo);
    }
    STDMETHOD(CheckCounter)(const D3D11_COUNTER_DESC* pDesc, D3D11_COUNTER_TYPE* pType, UINT* pActiveCounters, LPSTR szName, UINT* pNameLength, LPSTR szUnits, UINT* pUnitsLength, LPSTR szDescription, UINT* pDescriptionLength) override
    {
        return m_device3->CheckCounter(pDesc, pType, pActiveCounters, szName, pNameLength, szUnits, pUnitsLength, szDescription, pDescriptionLength);
    }
    STDMETHOD(CheckFeatureSupport)(D3D11_FEATURE Feature, void* pFeatureSupportData, UINT FeatureSupportDataSize) override
    {
        return m_device3->CheckFeatureSupport(Feature, pFeatureSupportData, FeatureSupportDataSize);
    }
    STDMETHOD_(HRESULT, GetPrivateData)(REFGUID guid, UINT* pDataSize, void* pData) override
    {
        return m_device3->GetPrivateData(guid, pDataSize, pData);
    }
    STDMETHOD_(HRESULT, SetPrivateData)(REFGUID guid, UINT DataSize, const void* pData) override
    {
        return m_device3->SetPrivateData(guid, DataSize, pData);
    }
    STDMETHOD(SetPrivateDataInterface)(REFGUID guid, const IUnknown* pData) override
    {
        return m_device3->SetPrivateDataInterface(guid, pData);
    }
    STDMETHOD_(D3D_FEATURE_LEVEL, GetFeatureLevel)() override
    {
        return m_device3->GetFeatureLevel();
    }
    STDMETHOD_(UINT, GetCreationFlags)() override
    {
        return m_device3->GetCreationFlags();
    }
    STDMETHOD(GetDeviceRemovedReason)() override
    {
        return m_device3->GetDeviceRemovedReason();
    }
    STDMETHOD_(void, GetImmediateContext)(ID3D11DeviceContext** ppImmediateContext) override
    {
        Microsoft::WRL::ComPtr<ID3D11DeviceContext> immediateContext;
        m_device3->GetImmediateContext(immediateContext.GetAddressOf());
        *ppImmediateContext = immediateContext.Detach();
    }
    STDMETHOD(SetExceptionMode)(UINT RaiseFlags) override
    {
        return m_device3->SetExceptionMode(RaiseFlags);
    }

    STDMETHOD_(UINT, GetExceptionMode)() override
    {
        return m_device3->GetExceptionMode();
    }
};

class D3D11DeviceContextWrapper : public ID3D11DeviceContext
{
private:
    Microsoft::WRL::ComPtr<ID3D11DeviceContext3> m_context3;

public:
    // Constructor that takes an ID3D11DeviceContext3
    D3D11DeviceContextWrapper(ID3D11DeviceContext3* context3) : m_context3(context3) {}

    // Implement the necessary ID3D11DeviceContext methods by forwarding calls to m_context3
    
    STDMETHOD_(void, VSSetConstantBuffers)(UINT StartSlot, UINT NumBuffers, ID3D11Buffer* const* ppConstantBuffers) override
    {
        m_context3->VSSetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
    }

    STDMETHOD_(void, PSSetShaderResources)(UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView* const* ppShaderResourceViews) override
    {
        m_context3->PSSetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
    }

    STDMETHOD_(void, PSSetShader)(ID3D11PixelShader* pPixelShader, ID3D11ClassInstance* const* ppClassInstances, UINT NumClassInstances) override
    {
        m_context3->PSSetShader(pPixelShader, ppClassInstances, NumClassInstances);
    }
    STDMETHOD_(void, GSSetShader)(ID3D11GeometryShader* pShader, ID3D11ClassInstance* const* ppClassInstances, UINT NumClassInstances) override
    {
        m_context3->GSSetShader(pShader, ppClassInstances, NumClassInstances);
    }
    STDMETHOD_(void, IASetPrimitiveTopology)(D3D11_PRIMITIVE_TOPOLOGY Topology) override
    {
        m_context3->IASetPrimitiveTopology(Topology);
    }
    STDMETHOD_(void, VSSetSamplers)(UINT StartSlot, UINT NumSamplers, ID3D11SamplerState* const* ppSamplers) override
    {
        m_context3->VSSetSamplers(StartSlot, NumSamplers, ppSamplers);
    }
    STDMETHOD_(void, Begin)(ID3D11Asynchronous* pAsync) override
    {
        m_context3->Begin(pAsync);
    }
    STDMETHOD_(void, End)(ID3D11Asynchronous* pAsync) override
    {
        m_context3->End(pAsync);
    }
    STDMETHOD_(HRESULT, GetData)(ID3D11Asynchronous* pAsync, void* pData, UINT DataSize, UINT GetDataFlags) override
    {
        return m_context3->GetData(pAsync, pData, DataSize, GetDataFlags);
    }
    STDMETHOD_(void, SetPredication)(ID3D11Predicate* pPredicate, BOOL PredicateValue) override
    {
        m_context3->SetPredication(pPredicate, PredicateValue);
    }

    STDMETHOD_(void, GSSetShaderResources)(UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView* const* ppShaderResourceViews) override
    {
        m_context3->GSSetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
    }

    STDMETHOD_(void, GSSetSamplers)(UINT StartSlot, UINT NumSamplers, ID3D11SamplerState* const* ppSamplers) override
    {
        m_context3->GSSetSamplers(StartSlot, NumSamplers, ppSamplers);
    }

    STDMETHOD_(void, OMSetRenderTargets)(UINT NumViews, ID3D11RenderTargetView* const* ppRenderTargetViews, ID3D11DepthStencilView* pDepthStencilView) override
    {
        m_context3->OMSetRenderTargets(NumViews, ppRenderTargetViews, pDepthStencilView);
    }
    STDMETHOD_(void, RSSetState)(ID3D11RasterizerState* pRasterizerState) override
    {
        m_context3->RSSetState(pRasterizerState);
    }

    STDMETHOD_(void, RSSetViewports)(UINT NumViewports, const D3D11_VIEWPORT* pViewports) override
    {
        m_context3->RSSetViewports(NumViewports, pViewports);
    }

    STDMETHOD_(void, RSSetScissorRects)(UINT NumRects, const D3D11_RECT* pRects) override
    {
        m_context3->RSSetScissorRects(NumRects, pRects);
    }

    STDMETHOD_(void, CopySubresourceRegion)(ID3D11Resource* pDstResource, UINT DstSubresource, UINT DstX, UINT DstY, UINT DstZ,
        ID3D11Resource* pSrcResource, UINT SrcSubresource, const D3D11_BOX* pSrcBox) override
    {
        m_context3->CopySubresourceRegion(pDstResource, DstSubresource, DstX, DstY, DstZ, pSrcResource, SrcSubresource, pSrcBox);
    }
    STDMETHOD_(void, ClearRenderTargetView)(ID3D11RenderTargetView* pRenderTargetView, const FLOAT ColorRGBA[4]) override
    {
        m_context3->ClearRenderTargetView(pRenderTargetView, ColorRGBA);
    }

    STDMETHOD_(void, ClearUnorderedAccessViewUint)(ID3D11UnorderedAccessView* pUnorderedAccessView, const UINT Values[4]) override
    {
        m_context3->ClearUnorderedAccessViewUint(pUnorderedAccessView, Values);
    }

    STDMETHOD_(void, ClearUnorderedAccessViewFloat)(ID3D11UnorderedAccessView* pUnorderedAccessView, const FLOAT Values[4]) override
    {
        m_context3->ClearUnorderedAccessViewFloat(pUnorderedAccessView, Values);
    }

    STDMETHOD_(void, ClearDepthStencilView)(ID3D11DepthStencilView* pDepthStencilView, UINT ClearFlags, FLOAT Depth, UINT8 Stencil) override
    {
        m_context3->ClearDepthStencilView(pDepthStencilView, ClearFlags, Depth, Stencil);
    }

    STDMETHOD_(void, GenerateMips)(ID3D11ShaderResourceView* pShaderResourceView) override
    {
        m_context3->GenerateMips(pShaderResourceView);
    }

    STDMETHOD_(void, VSSetShader)(ID3D11VertexShader* pVertexShader, ID3D11ClassInstance* const* ppClassInstances, UINT NumClassInstances) override
    {
        m_context3->VSSetShader(pVertexShader, ppClassInstances, NumClassInstances);
    }

    STDMETHOD_(void, PSSetConstantBuffers)(UINT StartSlot, UINT NumBuffers, ID3D11Buffer* const* ppConstantBuffers) override
    {
        m_context3->PSSetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
    }
    STDMETHOD_(void, VSSetShaderResources)(UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView* const* ppShaderResourceViews) override
    {
        m_context3->VSSetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
    }
    STDMETHOD_(void, PSSetSamplers)(UINT StartSlot, UINT NumSamplers, ID3D11SamplerState* const* ppSamplers) override
    {
        m_context3->PSSetSamplers(StartSlot, NumSamplers, ppSamplers);
    }
    STDMETHOD_(void, GSSetConstantBuffers)(UINT StartSlot, UINT NumBuffers, ID3D11Buffer* const* ppConstantBuffers) override
    {
        m_context3->GSSetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
    }
    STDMETHOD_(void, DrawIndexed)(UINT IndexCount, UINT StartIndexLocation, INT BaseVertexLocation) override
    {
        m_context3->DrawIndexed(IndexCount, StartIndexLocation, BaseVertexLocation);
    }

    STDMETHOD_(void, Draw)(UINT VertexCount, UINT StartVertexLocation) override
    {
        m_context3->Draw(VertexCount, StartVertexLocation);
    }
    STDMETHOD_(void, HSSetShaderResources)(UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView* const* ppShaderResourceViews) override
    {
        m_context3->HSSetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
    }

    STDMETHOD_(void, HSSetShader)(ID3D11HullShader* pShader, ID3D11ClassInstance* const* ppClassInstances, UINT NumClassInstances) override
    {
        m_context3->HSSetShader(pShader, ppClassInstances, NumClassInstances);
    }

    STDMETHOD_(void, HSSetSamplers)(UINT StartSlot, UINT NumSamplers, ID3D11SamplerState* const* ppSamplers) override
    {
        m_context3->HSSetSamplers(StartSlot, NumSamplers, ppSamplers);
    }

    STDMETHOD_(void, HSGetShaderResources)(UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView** ppShaderResourceViews) override
    {
        m_context3->HSGetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
    }
    STDMETHOD_(void, HSGetShader)(ID3D11HullShader** ppHullShader, ID3D11ClassInstance** ppClassInstances, UINT* pNumClassInstances) override
    {
        m_context3->HSGetShader(ppHullShader, ppClassInstances, pNumClassInstances);
    }

    STDMETHOD_(void, HSGetSamplers)(UINT StartSlot, UINT NumSamplers, ID3D11SamplerState** ppSamplers) override
    {
        m_context3->HSGetSamplers(StartSlot, NumSamplers, ppSamplers);
    }

    STDMETHOD_(void, HSGetConstantBuffers)(UINT StartSlot, UINT NumBuffers, ID3D11Buffer** ppConstantBuffers) override
    {
        m_context3->HSGetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
    }

    STDMETHOD_(void, DSSetShaderResources)(UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView* const* ppShaderResourceViews) override
    {
        m_context3->DSSetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
    }

    STDMETHOD_(void, DSSetShader)(ID3D11DomainShader* pShader, ID3D11ClassInstance* const* ppClassInstances, UINT NumClassInstances) override
    {
        m_context3->DSSetShader(pShader, ppClassInstances, NumClassInstances);
    }

    STDMETHOD_(void, DSSetSamplers)(UINT StartSlot, UINT NumSamplers, ID3D11SamplerState* const* ppSamplers) override
    {
        m_context3->DSSetSamplers(StartSlot, NumSamplers, ppSamplers);
    }
    STDMETHOD_(void, DrawIndexedInstanced)(UINT IndexCountPerInstance, UINT InstanceCount, UINT StartIndexLocation, INT BaseVertexLocation, UINT StartInstanceLocation) override
    {
        m_context3->DrawIndexedInstanced(IndexCountPerInstance, InstanceCount, StartIndexLocation, BaseVertexLocation, StartInstanceLocation);
    }

    STDMETHOD_(void, DrawInstanced)(UINT VertexCountPerInstance, UINT InstanceCount, UINT StartVertexLocation, UINT StartInstanceLocation) override
    {
        m_context3->DrawInstanced(VertexCountPerInstance, InstanceCount, StartVertexLocation, StartInstanceLocation);
    }
    STDMETHOD_(void, DrawIndexedInstancedIndirect)(ID3D11Buffer* pBufferForArgs, UINT AlignedByteOffsetForArgs) override
    {
        m_context3->DrawIndexedInstancedIndirect(pBufferForArgs, AlignedByteOffsetForArgs);
    }

    STDMETHOD_(void, DrawInstancedIndirect)(ID3D11Buffer* pBufferForArgs, UINT AlignedByteOffsetForArgs) override
    {
        m_context3->DrawInstancedIndirect(pBufferForArgs, AlignedByteOffsetForArgs);
    }

    STDMETHOD_(void, Dispatch)(UINT ThreadGroupCountX, UINT ThreadGroupCountY, UINT ThreadGroupCountZ) override
    {
        m_context3->Dispatch(ThreadGroupCountX, ThreadGroupCountY, ThreadGroupCountZ);
    }

    STDMETHOD_(void, DispatchIndirect)(ID3D11Buffer* pBufferForArgs, UINT AlignedByteOffsetForArgs) override
    {
        m_context3->DispatchIndirect(pBufferForArgs, AlignedByteOffsetForArgs);
    }

    
    STDMETHOD_(void, HSSetConstantBuffers)(UINT StartSlot, UINT NumBuffers, ID3D11Buffer* const* ppConstantBuffers) override
    {
        m_context3->HSSetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
    }

    STDMETHOD_(void, DSSetConstantBuffers)(UINT StartSlot, UINT NumBuffers, ID3D11Buffer* const* ppConstantBuffers) override
    {
        m_context3->DSSetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
    }

    STDMETHOD_(void, IASetInputLayout)(ID3D11InputLayout* pInputLayout) override
    {
        m_context3->IASetInputLayout(pInputLayout);
    }

    STDMETHOD_(void, IASetVertexBuffers)(UINT StartSlot, UINT NumBuffers, ID3D11Buffer* const* ppVertexBuffers, const UINT* pStrides, const UINT* pOffsets) override
    {
        m_context3->IASetVertexBuffers(StartSlot, NumBuffers, ppVertexBuffers, pStrides, pOffsets);
    }

    STDMETHOD_(void, IASetIndexBuffer)(ID3D11Buffer* pIndexBuffer, DXGI_FORMAT Format, UINT Offset) override
    {
        m_context3->IASetIndexBuffer(pIndexBuffer, Format, Offset);
    }

    STDMETHOD_(HRESULT, Map)(ID3D11Resource* pResource, UINT Subresource, D3D11_MAP MapType, UINT MapFlags, D3D11_MAPPED_SUBRESOURCE* pMappedResource) override
    {
        return m_context3->Map(pResource, Subresource, MapType, MapFlags, pMappedResource);
    }

    STDMETHOD_(void, Unmap)(ID3D11Resource* pResource, UINT Subresource) override
    {
        m_context3->Unmap(pResource, Subresource);
    }
    STDMETHOD_(void, OMSetRenderTargetsAndUnorderedAccessViews)(UINT NumRTVs, ID3D11RenderTargetView* const* ppRenderTargetViews, ID3D11DepthStencilView* pDepthStencilView,
        UINT UAVStartSlot, UINT NumUAVs, ID3D11UnorderedAccessView* const* ppUnorderedAccessViews, const UINT* pUAVInitialCounts) override
    {
        m_context3->OMSetRenderTargetsAndUnorderedAccessViews(NumRTVs, ppRenderTargetViews, pDepthStencilView, UAVStartSlot, NumUAVs, ppUnorderedAccessViews, pUAVInitialCounts);
    }

    STDMETHOD_(void, SetResourceMinLOD)(ID3D11Resource* pResource, FLOAT MinLOD) override
    {
        m_context3->SetResourceMinLOD(pResource, MinLOD);
    }

    STDMETHOD_(FLOAT, GetResourceMinLOD)(ID3D11Resource* pResource) override
    {
        return m_context3->GetResourceMinLOD(pResource);
    }

    STDMETHOD_(void, ResolveSubresource)(ID3D11Resource* pDstResource, UINT DstSubresource, ID3D11Resource* pSrcResource, UINT SrcSubresource, DXGI_FORMAT Format) override
    {
        m_context3->ResolveSubresource(pDstResource, DstSubresource, pSrcResource, SrcSubresource, Format);
    }

    STDMETHOD_(void, OMSetBlendState)(ID3D11BlendState* pBlendState, const FLOAT BlendFactor[4], UINT SampleMask) override
    {
        m_context3->OMSetBlendState(pBlendState, BlendFactor, SampleMask);
    }

    STDMETHOD_(void, OMSetDepthStencilState)(ID3D11DepthStencilState* pDepthStencilState, UINT StencilRef) override
    {
        m_context3->OMSetDepthStencilState(pDepthStencilState, StencilRef);
    }

    STDMETHOD_(void, SOSetTargets)(UINT NumBuffers, ID3D11Buffer* const* ppSOTargets, const UINT* pOffsets) override
    {
        m_context3->SOSetTargets(NumBuffers, ppSOTargets, pOffsets);
    }

    STDMETHOD_(void, DrawAuto)() override
    {
        m_context3->DrawAuto();
    }

    STDMETHOD_(void, CopyResource)(ID3D11Resource* pDstResource, ID3D11Resource* pSrcResource) override
    {
        m_context3->CopyResource(pDstResource, pSrcResource);
    }

    STDMETHOD_(void, UpdateSubresource)(ID3D11Resource* pDstResource, UINT DstSubresource, const D3D11_BOX* pDstBox, const void* pSrcData, UINT SrcRowPitch, UINT SrcDepthPitch) override
    {
        m_context3->UpdateSubresource(pDstResource, DstSubresource, pDstBox, pSrcData, SrcRowPitch, SrcDepthPitch);
    }

    STDMETHOD_(void, CopyStructureCount)(ID3D11Buffer* pDstBuffer, UINT DstAlignedByteOffset, ID3D11UnorderedAccessView* pSrcView) override
    {
        m_context3->CopyStructureCount(pDstBuffer, DstAlignedByteOffset, pSrcView);
    }

    STDMETHOD_(void, ExecuteCommandList)(ID3D11CommandList* pCommandList, BOOL RestoreContextState) override
    {
        m_context3->ExecuteCommandList(pCommandList, RestoreContextState);
    }

    STDMETHOD_(void, CSSetShaderResources)(UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView* const* ppShaderResourceViews) override
    {
        m_context3->CSSetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
    }

    STDMETHOD_(void, CSSetUnorderedAccessViews)(UINT StartSlot, UINT NumUAVs, ID3D11UnorderedAccessView* const* ppUnorderedAccessViews, const UINT* pUAVInitialCounts) override
    {
        m_context3->CSSetUnorderedAccessViews(StartSlot, NumUAVs, ppUnorderedAccessViews, pUAVInitialCounts);
    }
    STDMETHOD_(void, CSSetShader)(ID3D11ComputeShader* pComputeShader, ID3D11ClassInstance* const* ppClassInstances, UINT NumClassInstances) override
    {
        m_context3->CSSetShader(pComputeShader, ppClassInstances, NumClassInstances);
    }

    STDMETHOD_(void, CSSetSamplers)(UINT StartSlot, UINT NumSamplers, ID3D11SamplerState* const* ppSamplers) override
    {
        m_context3->CSSetSamplers(StartSlot, NumSamplers, ppSamplers);
    }

    STDMETHOD_(void, CSSetConstantBuffers)(UINT StartSlot, UINT NumBuffers, ID3D11Buffer* const* ppConstantBuffers) override
    {
        m_context3->CSSetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
    }

    STDMETHOD_(void, VSGetConstantBuffers)(UINT StartSlot, UINT NumBuffers, ID3D11Buffer** ppConstantBuffers) override
    {
        m_context3->VSGetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
    }
    STDMETHOD_(void, PSGetShaderResources)(UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView** ppShaderResourceViews) override
    {
        m_context3->PSGetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
    }

    STDMETHOD_(void, PSGetShader)(ID3D11PixelShader** ppPixelShader, ID3D11ClassInstance** ppClassInstances, UINT* pNumClassInstances) override
    {
        m_context3->PSGetShader(ppPixelShader, ppClassInstances, pNumClassInstances);
    }

    STDMETHOD_(void, PSGetSamplers)(UINT StartSlot, UINT NumSamplers, ID3D11SamplerState** ppSamplers) override
    {
        m_context3->PSGetSamplers(StartSlot, NumSamplers, ppSamplers);
    }

    STDMETHOD_(void, VSGetShader)(ID3D11VertexShader** ppVertexShader, ID3D11ClassInstance** ppClassInstances, UINT* pNumClassInstances) override
    {
        m_context3->VSGetShader(ppVertexShader, ppClassInstances, pNumClassInstances);
    }

    STDMETHOD_(void, PSGetConstantBuffers)(UINT StartSlot, UINT NumBuffers, ID3D11Buffer** ppConstantBuffers) override
    {
        m_context3->PSGetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
    }
    STDMETHOD_(void, IAGetInputLayout)(ID3D11InputLayout** ppInputLayout) override
    {
        m_context3->IAGetInputLayout(ppInputLayout);
    }

    STDMETHOD_(void, IAGetVertexBuffers)(UINT StartSlot, UINT NumBuffers, ID3D11Buffer** ppVertexBuffers, UINT* pStrides, UINT* pOffsets) override
    {
        m_context3->IAGetVertexBuffers(StartSlot, NumBuffers, ppVertexBuffers, pStrides, pOffsets);
    }

    STDMETHOD_(void, IAGetIndexBuffer)(ID3D11Buffer** pIndexBuffer, DXGI_FORMAT* Format, UINT* Offset) override
    {
        m_context3->IAGetIndexBuffer(pIndexBuffer, Format, Offset);
    }

    STDMETHOD_(void, GSGetConstantBuffers)(UINT StartSlot, UINT NumBuffers, ID3D11Buffer** ppConstantBuffers) override
    {
        m_context3->GSGetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
    }

    STDMETHOD_(void, GSGetShader)(ID3D11GeometryShader** ppGeometryShader, ID3D11ClassInstance** ppClassInstances, UINT* pNumClassInstances) override
    {
        m_context3->GSGetShader(ppGeometryShader, ppClassInstances, pNumClassInstances);
    }
    STDMETHOD_(void, IAGetPrimitiveTopology)(D3D11_PRIMITIVE_TOPOLOGY* pTopology) override
    {
        m_context3->IAGetPrimitiveTopology(pTopology);
    }

    STDMETHOD_(void, VSGetShaderResources)(UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView** ppShaderResourceViews) override
    {
        m_context3->VSGetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
    }

    STDMETHOD_(void, VSGetSamplers)(UINT StartSlot, UINT NumSamplers, ID3D11SamplerState** ppSamplers) override
    {
        m_context3->VSGetSamplers(StartSlot, NumSamplers, ppSamplers);
    }

    STDMETHOD_(void, GetPredication)(ID3D11Predicate** ppPredicate, BOOL* pPredicateValue) override
    {
        m_context3->GetPredication(ppPredicate, pPredicateValue);
    }

    STDMETHOD_(void, GSGetShaderResources)(UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView** ppShaderResourceViews) override
    {
        m_context3->GSGetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
    }
    STDMETHOD_(void, GSGetSamplers)(UINT StartSlot, UINT NumSamplers, ID3D11SamplerState** ppSamplers) override
    {
        m_context3->GSGetSamplers(StartSlot, NumSamplers, ppSamplers);
    }

    STDMETHOD_(void, OMGetRenderTargets)(UINT NumViews, ID3D11RenderTargetView** ppRenderTargetViews, ID3D11DepthStencilView** ppDepthStencilView) override
    {
        m_context3->OMGetRenderTargets(NumViews, ppRenderTargetViews, ppDepthStencilView);
    }

    STDMETHOD_(void, OMGetRenderTargetsAndUnorderedAccessViews)(UINT NumRTVs, ID3D11RenderTargetView** ppRenderTargetViews, ID3D11DepthStencilView** ppDepthStencilView,
        UINT UAVStartSlot, UINT NumUAVs, ID3D11UnorderedAccessView** ppUnorderedAccessViews) override
    {
        m_context3->OMGetRenderTargetsAndUnorderedAccessViews(NumRTVs, ppRenderTargetViews, ppDepthStencilView, UAVStartSlot, NumUAVs, ppUnorderedAccessViews);
    }

    STDMETHOD_(void, OMGetBlendState)(ID3D11BlendState** ppBlendState, FLOAT BlendFactor[4], UINT* pSampleMask) override
    {
        m_context3->OMGetBlendState(ppBlendState, BlendFactor, pSampleMask);
    }
    STDMETHOD_(void, OMGetDepthStencilState)(ID3D11DepthStencilState** ppDepthStencilState, UINT* pStencilRef) override
    {
        m_context3->OMGetDepthStencilState(ppDepthStencilState, pStencilRef);
    }

    STDMETHOD_(void, SOGetTargets)(UINT NumBuffers, ID3D11Buffer** ppSOTargets) override
    {
        m_context3->SOGetTargets(NumBuffers, ppSOTargets);
    }

    STDMETHOD_(void, RSGetState)(ID3D11RasterizerState** ppRasterizerState) override
    {
        m_context3->RSGetState(ppRasterizerState);
    }

    STDMETHOD_(void, RSGetViewports)(UINT* pNumViewports, D3D11_VIEWPORT* pViewports) override
    {
        m_context3->RSGetViewports(pNumViewports, pViewports);
    }

    STDMETHOD_(void, RSGetScissorRects)(UINT* pNumRects, D3D11_RECT* pRects) override
    {
        m_context3->RSGetScissorRects(pNumRects, pRects);
    }

    STDMETHOD_(void, DSGetShaderResources)(UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView** ppShaderResourceViews) override
    {
        m_context3->DSGetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
    }
    STDMETHOD_(void, DSGetShader)(ID3D11DomainShader** ppDomainShader, ID3D11ClassInstance** ppClassInstances, UINT* pNumClassInstances) override
    {
        m_context3->DSGetShader(ppDomainShader, ppClassInstances, pNumClassInstances);
    }

    STDMETHOD_(void, DSGetSamplers)(UINT StartSlot, UINT NumSamplers, ID3D11SamplerState** ppSamplers) override
    {
        m_context3->DSGetSamplers(StartSlot, NumSamplers, ppSamplers);
    }

    STDMETHOD_(void, DSGetConstantBuffers)(UINT StartSlot, UINT NumBuffers, ID3D11Buffer** ppConstantBuffers) override
    {
        m_context3->DSGetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
    }

    STDMETHOD_(void, CSGetShaderResources)(UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView** ppShaderResourceViews) override
    {
        m_context3->CSGetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
    }

    STDMETHOD_(void, CSGetUnorderedAccessViews)(UINT StartSlot, UINT NumUAVs, ID3D11UnorderedAccessView** ppUnorderedAccessViews) override
    {
        m_context3->CSGetUnorderedAccessViews(StartSlot, NumUAVs, ppUnorderedAccessViews);
    }

    STDMETHOD_(void, CSGetShader)(ID3D11ComputeShader** ppComputeShader, ID3D11ClassInstance** ppClassInstances, UINT* pNumClassInstances) override
    {
        m_context3->CSGetShader(ppComputeShader, ppClassInstances, pNumClassInstances);
    }
    STDMETHOD_(void, CSGetSamplers)(UINT StartSlot, UINT NumSamplers, ID3D11SamplerState** ppSamplers) override
    {
        m_context3->CSGetSamplers(StartSlot, NumSamplers, ppSamplers);
    }

    STDMETHOD_(void, CSGetConstantBuffers)(UINT StartSlot, UINT NumBuffers, ID3D11Buffer** ppConstantBuffers) override
    {
        m_context3->CSGetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
    }

    STDMETHOD_(void, ClearState)() override
    {
        m_context3->ClearState();
    }

    STDMETHOD_(void, Flush)() override
    {
        m_context3->Flush();
    }

    STDMETHOD_(D3D11_DEVICE_CONTEXT_TYPE, GetType)() override
    {
        return m_context3->GetType();
    }

    STDMETHOD_(UINT, GetContextFlags)() override
    {
        return m_context3->GetContextFlags();
    }

    STDMETHOD_(HRESULT, FinishCommandList)(BOOL RestoreDeferredContextState, ID3D11CommandList** ppCommandList) override
    {
        return m_context3->FinishCommandList(RestoreDeferredContextState, ppCommandList);
    }

    // IUnknown methods
    STDMETHOD(QueryInterface)(REFIID riid, void** ppvObject) override {
        if (riid == IID_IUnknown || riid == IID_ID3D11DeviceContext || riid == IID_ID3D11DeviceContext1 || riid == IID_ID3D11DeviceContext2 || riid == IID_ID3D11DeviceContext3) {
            *ppvObject = this;
            AddRef();
            return S_OK;
        }
        *ppvObject = nullptr;
        return E_NOINTERFACE;
    }

    STDMETHOD_(ULONG, AddRef)() override {
        return m_context3->AddRef();
    }

    STDMETHOD_(ULONG, Release)() override {
        return m_context3->Release();
    }

    //ID3D11DeviceChild methods

    STDMETHOD_(void, GetDevice)(ID3D11Device** ppDevice) override {
        return m_context3->GetDevice(ppDevice);
    }

    STDMETHOD(GetPrivateData)(REFGUID guid, UINT* pDataSize, void* pData) override {
        return m_context3->GetPrivateData(guid, pDataSize, pData);
    }

    STDMETHOD(SetPrivateData)(REFGUID guid, UINT DataSize, const void* pData) override {
        return m_context3->SetPrivateData(guid, DataSize, pData);
    }

    STDMETHOD(SetPrivateDataInterface)(REFGUID guid, const IUnknown* pData) override {
        return m_context3->SetPrivateDataInterface(guid, pData);
    }
};