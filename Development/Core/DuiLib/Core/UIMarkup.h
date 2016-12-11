#ifndef __UIMARKUP_H__
#define __UIMARKUP_H__

#pragma once

namespace DuiLib {

enum
{
    XMLFILE_ENCODING_UTF8 = 0,
    XMLFILE_ENCODING_UNICODE = 1,
    XMLFILE_ENCODING_ASNI = 2,
};

class CMarkup;
class CMarkupNode;


class UILIB_API CMarkup
{
    friend class CMarkupNode;
public:
    CMarkup(const wchar_t* pstrXML = NULL);
    ~CMarkup();

    bool Load(const wchar_t* pstrXML);
    bool LoadFromMem(BYTE* pByte, DWORD dwSize, int encoding = XMLFILE_ENCODING_UTF8);
    bool LoadFromFile(const wchar_t* pstrFilename, int encoding = XMLFILE_ENCODING_UTF8);
    void Release();
    bool IsValid() const;

    void SetPreserveWhitespace(bool bPreserve = true);
    void GetLastErrorMessage(wchar_t* pstrMessage, SIZE_T cchMax) const;
    void GetLastErrorLocation(wchar_t* pstrSource, SIZE_T cchMax) const;

    CMarkupNode GetRoot();

private:
    typedef struct tagXMLELEMENT
    {
        ULONG iStart;
        ULONG iChild;
        ULONG iNext;
        ULONG iParent;
        ULONG iData;
    } XMLELEMENT;

    wchar_t* m_pstrXML;
    XMLELEMENT* m_pElements;
    ULONG m_nElements;
    ULONG m_nReservedElements;
    wchar_t m_szErrorMsg[100];
    wchar_t m_szErrorXML[50];
    bool m_bPreserveWhitespace;

private:
    bool _Parse();
    bool _Parse(wchar_t*& pstrText, ULONG iParent);
    XMLELEMENT* _ReserveElement();
    inline void _SkipWhitespace(wchar_t*& pstr) const;
    inline void _SkipWhitespace(const wchar_t*& pstr) const;
    inline void _SkipIdentifier(wchar_t*& pstr) const;
    inline void _SkipIdentifier(const wchar_t*& pstr) const;
    bool _ParseData(wchar_t*& pstrText, wchar_t*& pstrData, char cEnd);
    void _ParseMetaChar(wchar_t*& pstrText, wchar_t*& pstrDest);
    bool _ParseAttributes(wchar_t*& pstrText);
    bool _Failed(const wchar_t* pstrError, const wchar_t* pstrLocation = NULL);
};


class UILIB_API CMarkupNode
{
    friend class CMarkup;
private:
    CMarkupNode();
    CMarkupNode(CMarkup* pOwner, int iPos);

public:
    bool IsValid() const;

    CMarkupNode GetParent();
    CMarkupNode GetSibling();
    CMarkupNode GetChild();
    CMarkupNode GetChild(const wchar_t* pstrName);

    bool HasSiblings() const;
    bool HasChildren() const;
    const wchar_t* GetName() const;
    const wchar_t* GetValue() const;

    bool HasAttributes();
    bool HasAttribute(const wchar_t* pstrName);
    int GetAttributeCount();
    const wchar_t* GetAttributeName(int iIndex);
    const wchar_t* GetAttributeValue(int iIndex);
    const wchar_t* GetAttributeValue(const wchar_t* pstrName);
    bool GetAttributeValue(int iIndex, wchar_t* pstrValue, SIZE_T cchMax);
    bool GetAttributeValue(const wchar_t* pstrName, wchar_t* pstrValue, SIZE_T cchMax);

private:
    void _MapAttributes();

    enum { MAX_XML_ATTRIBUTES = 64 };

    typedef struct
    {
        ULONG iName;
        ULONG iValue;
    } XMLATTRIBUTE;

    int m_iPos;
    int m_nAttributes;
    XMLATTRIBUTE m_aAttributes[MAX_XML_ATTRIBUTES];
    CMarkup* m_pOwner;
};

} // namespace DuiLib

#endif // __UIMARKUP_H__
