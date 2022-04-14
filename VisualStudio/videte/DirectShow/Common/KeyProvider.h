#ifndef CKEYSERVICEPROVIDER_H_DEFINED
#define CKEYSERVICEPROVIDER_H_DEFINED

class AFX_EXT_CLASS CKeyProvider : public IServiceProvider 
{
public:
    // IUnknown interface
    STDMETHODIMP QueryInterface(REFIID riid, void ** ppv);
    STDMETHODIMP_(ULONG) AddRef();
    STDMETHODIMP_(ULONG) Release();

    CKeyProvider();

    // IServiceProvider
    STDMETHODIMP QueryService(REFIID siid, REFIID riid, void **ppv);
    
private:
    LONG m_cRef;
};

#endif // CKEYSERVICEPROVIDER_H_DEFINED