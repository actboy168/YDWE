#include "StdAfx.h"

#ifndef TRACE
#define TRACE
#endif


namespace DuiLib {

///////////////////////////////////////////////////////////////////////////////////////
//
//
//

CMarkupNode::CMarkupNode() : m_pOwner(NULL)
{
}

CMarkupNode::CMarkupNode(CMarkup* pOwner, int iPos) : m_pOwner(pOwner), m_iPos(iPos), m_nAttributes(0)
{
}

CMarkupNode CMarkupNode::GetSibling()
{
    if( m_pOwner == NULL ) return CMarkupNode();
    ULONG iPos = m_pOwner->m_pElements[m_iPos].iNext;
    if( iPos == 0 ) return CMarkupNode();
    return CMarkupNode(m_pOwner, iPos);
}

bool CMarkupNode::HasSiblings() const
{
    if( m_pOwner == NULL ) return false;
    ULONG iPos = m_pOwner->m_pElements[m_iPos].iNext;
    return iPos > 0;
}

CMarkupNode CMarkupNode::GetChild()
{
    if( m_pOwner == NULL ) return CMarkupNode();
    ULONG iPos = m_pOwner->m_pElements[m_iPos].iChild;
    if( iPos == 0 ) return CMarkupNode();
    return CMarkupNode(m_pOwner, iPos);
}

CMarkupNode CMarkupNode::GetChild(const wchar_t* pstrName)
{
    if( m_pOwner == NULL ) return CMarkupNode();
    ULONG iPos = m_pOwner->m_pElements[m_iPos].iChild;
    while( iPos != 0 ) {
        if( wcscmp(m_pOwner->m_pstrXML + m_pOwner->m_pElements[iPos].iStart, pstrName) == 0 ) {
            return CMarkupNode(m_pOwner, iPos);
        }
        iPos = m_pOwner->m_pElements[iPos].iNext;
    }
    return CMarkupNode();
}

bool CMarkupNode::HasChildren() const
{
    if( m_pOwner == NULL ) return false;
    return m_pOwner->m_pElements[m_iPos].iChild != 0;
}

CMarkupNode CMarkupNode::GetParent()
{
    if( m_pOwner == NULL ) return CMarkupNode();
    ULONG iPos = m_pOwner->m_pElements[m_iPos].iParent;
    if( iPos == 0 ) return CMarkupNode();
    return CMarkupNode(m_pOwner, iPos);
}

bool CMarkupNode::IsValid() const
{
    return m_pOwner != NULL;
}

const wchar_t* CMarkupNode::GetName() const
{
    if( m_pOwner == NULL ) return NULL;
    return m_pOwner->m_pstrXML + m_pOwner->m_pElements[m_iPos].iStart;
}

const wchar_t* CMarkupNode::GetValue() const
{
    if( m_pOwner == NULL ) return NULL;
    return m_pOwner->m_pstrXML + m_pOwner->m_pElements[m_iPos].iData;
}

const wchar_t* CMarkupNode::GetAttributeName(int iIndex)
{
    if( m_pOwner == NULL ) return NULL;
    if( m_nAttributes == 0 ) _MapAttributes();
    if( iIndex < 0 || iIndex >= m_nAttributes ) return L"";
    return m_pOwner->m_pstrXML + m_aAttributes[iIndex].iName;
}

const wchar_t* CMarkupNode::GetAttributeValue(int iIndex)
{
    if( m_pOwner == NULL ) return NULL;
    if( m_nAttributes == 0 ) _MapAttributes();
    if( iIndex < 0 || iIndex >= m_nAttributes ) return L"";
    return m_pOwner->m_pstrXML + m_aAttributes[iIndex].iValue;
}

const wchar_t* CMarkupNode::GetAttributeValue(const wchar_t* pstrName)
{
    if( m_pOwner == NULL ) return NULL;
    if( m_nAttributes == 0 ) _MapAttributes();
    for( int i = 0; i < m_nAttributes; i++ ) {
        if( wcscmp(m_pOwner->m_pstrXML + m_aAttributes[i].iName, pstrName) == 0 ) return m_pOwner->m_pstrXML + m_aAttributes[i].iValue;
    }
    return L"";
}

bool CMarkupNode::GetAttributeValue(int iIndex, wchar_t* pstrValue, SIZE_T cchMax)
{
    if( m_pOwner == NULL ) return false;
    if( m_nAttributes == 0 ) _MapAttributes();
    if( iIndex < 0 || iIndex >= m_nAttributes ) return false;
    wcsncpy(pstrValue, m_pOwner->m_pstrXML + m_aAttributes[iIndex].iValue, cchMax);
    return true;
}

bool CMarkupNode::GetAttributeValue(const wchar_t* pstrName, wchar_t* pstrValue, SIZE_T cchMax)
{
    if( m_pOwner == NULL ) return false;
    if( m_nAttributes == 0 ) _MapAttributes();
    for( int i = 0; i < m_nAttributes; i++ ) {
        if( wcscmp(m_pOwner->m_pstrXML + m_aAttributes[i].iName, pstrName) == 0 ) {
            wcsncpy(pstrValue, m_pOwner->m_pstrXML + m_aAttributes[i].iValue, cchMax);
            return true;
        }
    }
    return false;
}

int CMarkupNode::GetAttributeCount()
{
    if( m_pOwner == NULL ) return 0;
    if( m_nAttributes == 0 ) _MapAttributes();
    return m_nAttributes;
}

bool CMarkupNode::HasAttributes()
{
    if( m_pOwner == NULL ) return false;
    if( m_nAttributes == 0 ) _MapAttributes();
    return m_nAttributes > 0;
}

bool CMarkupNode::HasAttribute(const wchar_t* pstrName)
{
    if( m_pOwner == NULL ) return false;
    if( m_nAttributes == 0 ) _MapAttributes();
    for( int i = 0; i < m_nAttributes; i++ ) {
        if( wcscmp(m_pOwner->m_pstrXML + m_aAttributes[i].iName, pstrName) == 0 ) return true;
    }
    return false;
}

void CMarkupNode::_MapAttributes()
{
    m_nAttributes = 0;
    const wchar_t* pstr = m_pOwner->m_pstrXML + m_pOwner->m_pElements[m_iPos].iStart;
    const wchar_t* pstrEnd = m_pOwner->m_pstrXML + m_pOwner->m_pElements[m_iPos].iData;
    pstr += wcslen(pstr) + 1;
    while( pstr < pstrEnd ) {
        m_pOwner->_SkipWhitespace(pstr);
        m_aAttributes[m_nAttributes].iName = pstr - m_pOwner->m_pstrXML;
        pstr += wcslen(pstr) + 1;
        m_pOwner->_SkipWhitespace(pstr);
        if( *pstr++ != _T('\"') ) return; // if( *pstr != _T('\"') ) { pstr = ::CharNextW(pstr); return; }
        
        m_aAttributes[m_nAttributes++].iValue = pstr - m_pOwner->m_pstrXML;
        if( m_nAttributes >= MAX_XML_ATTRIBUTES ) return;
        pstr += wcslen(pstr) + 1;
    }
}


///////////////////////////////////////////////////////////////////////////////////////
//
//
//

CMarkup::CMarkup(const wchar_t* pstrXML)
{
    m_pstrXML = NULL;
    m_pElements = NULL;
    m_nElements = 0;
    m_bPreserveWhitespace = true;
    if( pstrXML != NULL ) Load(pstrXML);
}

CMarkup::~CMarkup()
{
    Release();
}

bool CMarkup::IsValid() const
{
    return m_pElements != NULL;
}

void CMarkup::SetPreserveWhitespace(bool bPreserve)
{
    m_bPreserveWhitespace = bPreserve;
}

bool CMarkup::Load(const wchar_t* pstrXML)
{
    Release();
    SIZE_T cchLen = wcslen(pstrXML) + 1;
    m_pstrXML = static_cast<wchar_t*>(malloc(cchLen * sizeof(wchar_t)));
    ::CopyMemory(m_pstrXML, pstrXML, cchLen * sizeof(wchar_t));
    bool bRes = _Parse();
    if( !bRes ) Release();
    return bRes;
}

bool CMarkup::LoadFromMem(BYTE* pByte, DWORD dwSize, int encoding)
{
    if (encoding == XMLFILE_ENCODING_UTF8)
    {
        if( dwSize >= 3 && pByte[0] == 0xEF && pByte[1] == 0xBB && pByte[2] == 0xBF ) 
        {
            pByte += 3; dwSize -= 3;
        }
        DWORD nWide = ::MultiByteToWideChar( CP_UTF8, 0, (LPCSTR)pByte, dwSize, NULL, 0 );

        m_pstrXML = static_cast<wchar_t*>(malloc((nWide + 1)*sizeof(wchar_t)));
        ::MultiByteToWideChar( CP_UTF8, 0, (LPCSTR)pByte, dwSize, m_pstrXML, nWide );
        m_pstrXML[nWide] = _T('\0');
    }
    else if (encoding == XMLFILE_ENCODING_ASNI)
    {
        DWORD nWide = ::MultiByteToWideChar( CP_ACP, 0, (LPCSTR)pByte, dwSize, NULL, 0 );

        m_pstrXML = static_cast<wchar_t*>(malloc((nWide + 1)*sizeof(wchar_t)));
        ::MultiByteToWideChar( CP_ACP, 0, (LPCSTR)pByte, dwSize, m_pstrXML, nWide );
        m_pstrXML[nWide] = _T('\0');
    }
    else
    {
        if ( dwSize >= 2 && ( ( pByte[0] == 0xFE && pByte[1] == 0xFF ) || ( pByte[0] == 0xFF && pByte[1] == 0xFE ) ) )
        {
            dwSize = dwSize / 2 - 1;

            if ( pByte[0] == 0xFE && pByte[1] == 0xFF )
            {
                pByte += 2;

                for ( DWORD nSwap = 0 ; nSwap < dwSize ; nSwap ++ )
                {
                    register CHAR nTemp = pByte[ ( nSwap << 1 ) + 0 ];
                    pByte[ ( nSwap << 1 ) + 0 ] = pByte[ ( nSwap << 1 ) + 1 ];
                    pByte[ ( nSwap << 1 ) + 1 ] = nTemp;
                }
            }
            else
            {
                pByte += 2;
            }

            m_pstrXML = static_cast<wchar_t*>(malloc((dwSize + 1)*sizeof(wchar_t)));
            ::CopyMemory( m_pstrXML, pByte, dwSize * sizeof(wchar_t) );
            m_pstrXML[dwSize] = _T('\0');

            pByte -= 2;
        }
    }

    bool bRes = _Parse();
    if( !bRes ) Release();
    return bRes;
}

bool CMarkup::LoadFromFile(const wchar_t* pstrFilename, int encoding)
{
    Release();
	CUIBuffer buf;

	buf = CUIFile::LoadZip(pstrFilename);

	if (!buf)
	{
		return _Failed(L"Could not open file");
	}

	return LoadFromMem(buf.ptrData.get(), buf.nSize, encoding);
}

void CMarkup::Release()
{
    if( m_pstrXML != NULL ) free(m_pstrXML);
    if( m_pElements != NULL ) free(m_pElements);
    m_pstrXML = NULL;
    m_pElements = NULL;
    m_nElements;
}

void CMarkup::GetLastErrorMessage(wchar_t* pstrMessage, SIZE_T cchMax) const
{
    wcsncpy(pstrMessage, m_szErrorMsg, cchMax);
}

void CMarkup::GetLastErrorLocation(wchar_t* pstrSource, SIZE_T cchMax) const
{
    wcsncpy(pstrSource, m_szErrorXML, cchMax);
}

CMarkupNode CMarkup::GetRoot()
{
    if( m_nElements == 0 ) return CMarkupNode();
    return CMarkupNode(this, 1);
}

bool CMarkup::_Parse()
{
    _ReserveElement(); // Reserve index 0 for errors
    ::ZeroMemory(m_szErrorMsg, sizeof(m_szErrorMsg));
    ::ZeroMemory(m_szErrorXML, sizeof(m_szErrorXML));
    wchar_t* pstrXML = m_pstrXML;
    return _Parse(pstrXML, 0);
}

bool CMarkup::_Parse(wchar_t*& pstrText, ULONG iParent)
{
    _SkipWhitespace(pstrText);
    ULONG iPrevious = 0;
    for( ; ; ) 
    {
        if( *pstrText == _T('\0') && iParent <= 1 ) return true;
        _SkipWhitespace(pstrText);
        if( *pstrText != _T('<') ) return _Failed(L"Expected start tag", pstrText);
        if( pstrText[1] == _T('/') ) return true;
        *pstrText++ = _T('\0');
        _SkipWhitespace(pstrText);
        // Skip comment or processing directive
        if( *pstrText == _T('!') || *pstrText == _T('?') ) {
            wchar_t ch = *pstrText;
            if( *pstrText == _T('!') ) ch = _T('-');
            while( *pstrText != _T('\0') && !(*pstrText == ch && *(pstrText + 1) == _T('>')) ) pstrText = ::CharNextW(pstrText);
            if( *pstrText != _T('\0') ) pstrText += 2;
            _SkipWhitespace(pstrText);
            continue;
        }
        _SkipWhitespace(pstrText);
        // Fill out element structure
        XMLELEMENT* pEl = _ReserveElement();
        ULONG iPos = pEl - m_pElements;
        pEl->iStart = pstrText - m_pstrXML;
        pEl->iParent = iParent;
        pEl->iNext = pEl->iChild = 0;
        if( iPrevious != 0 ) m_pElements[iPrevious].iNext = iPos;
        else if( iParent > 0 ) m_pElements[iParent].iChild = iPos;
        iPrevious = iPos;
        // Parse name
        const wchar_t* pstrName = pstrText;
        _SkipIdentifier(pstrText);
        wchar_t* pstrNameEnd = pstrText;
        if( *pstrText == _T('\0') ) return _Failed(L"Error parsing element name", pstrText);
        // Parse attributes
        if( !_ParseAttributes(pstrText) ) return false;
        _SkipWhitespace(pstrText);
        if( pstrText[0] == _T('/') && pstrText[1] == _T('>') )
        {
            pEl->iData = pstrText - m_pstrXML;
            *pstrText = _T('\0');
            pstrText += 2;
        }
        else
        {
            if( *pstrText != _T('>') ) return _Failed(L"Expected start-tag closing", pstrText);
            // Parse node data
            pEl->iData = ++pstrText - m_pstrXML;
            wchar_t* pstrDest = pstrText;
            if( !_ParseData(pstrText, pstrDest, _T('<')) ) return false;
            // Determine type of next element
            if( *pstrText == _T('\0') && iParent <= 1 ) return true;
            if( *pstrText != _T('<') ) return _Failed(L"Expected end-tag start", pstrText);
            if( pstrText[0] == _T('<') && pstrText[1] != _T('/') ) 
            {
                if( !_Parse(pstrText, iPos) ) return false;
            }
            if( pstrText[0] == _T('<') && pstrText[1] == _T('/') ) 
            {
                *pstrDest = _T('\0');
                *pstrText = _T('\0');
                pstrText += 2;
                _SkipWhitespace(pstrText);
                SIZE_T cchName = pstrNameEnd - pstrName;
                if( wcsncmp(pstrText, pstrName, cchName) != 0 ) return _Failed(L"Unmatched closing tag", pstrText);
                pstrText += cchName;
                _SkipWhitespace(pstrText);
                if( *pstrText++ != _T('>') ) return _Failed(L"Unmatched closing tag", pstrText);
            }
        }
        *pstrNameEnd = _T('\0');
        _SkipWhitespace(pstrText);
    }
}

CMarkup::XMLELEMENT* CMarkup::_ReserveElement()
{
    if( m_nElements == 0 ) m_nReservedElements = 0;
    if( m_nElements >= m_nReservedElements ) {
        m_nReservedElements += (m_nReservedElements / 2) + 500;
        m_pElements = static_cast<XMLELEMENT*>(realloc(m_pElements, m_nReservedElements * sizeof(XMLELEMENT)));
    }
    return &m_pElements[m_nElements++];
}

void CMarkup::_SkipWhitespace(const wchar_t*& pstr) const
{
    while( *pstr > _T('\0') && *pstr <= _T(' ') ) pstr = ::CharNextW(pstr);
}

void CMarkup::_SkipWhitespace(wchar_t*& pstr) const
{
    while( *pstr > _T('\0') && *pstr <= _T(' ') ) pstr = ::CharNextW(pstr);
}

void CMarkup::_SkipIdentifier(const wchar_t*& pstr) const
{
    // 属性只能用英文，所以这样处理没有问题
    while( *pstr != _T('\0') && (*pstr == _T('_') || *pstr == _T(':') || _istalnum(*pstr)) ) pstr = ::CharNextW(pstr);
}

void CMarkup::_SkipIdentifier(wchar_t*& pstr) const
{
    // 属性只能用英文，所以这样处理没有问题
    while( *pstr != _T('\0') && (*pstr == _T('_') || *pstr == _T(':') || _istalnum(*pstr)) ) pstr = ::CharNextW(pstr);
}

bool CMarkup::_ParseAttributes(wchar_t*& pstrText)
{   
    if( *pstrText == _T('>') ) return true;
    *pstrText++ = _T('\0');
    _SkipWhitespace(pstrText);
    while( *pstrText != _T('\0') && *pstrText != _T('>') && *pstrText != _T('/') ) {
        _SkipIdentifier(pstrText);
        wchar_t* pstrIdentifierEnd = pstrText;
        _SkipWhitespace(pstrText);
        if( *pstrText != _T('=') ) return _Failed(L"Error while parsing attributes", pstrText);
        *pstrText++ = _T(' ');
        *pstrIdentifierEnd = _T('\0');
        _SkipWhitespace(pstrText);
        if( *pstrText++ != _T('\"') ) return _Failed(L"Expected attribute value", pstrText);
        wchar_t* pstrDest = pstrText;
        if( !_ParseData(pstrText, pstrDest, _T('\"')) ) return false;
        if( *pstrText == _T('\0') ) return _Failed(L"Error while parsing attribute string", pstrText);
        *pstrDest = _T('\0');
        if( pstrText != pstrDest ) *pstrText = _T(' ');
        pstrText++;
        _SkipWhitespace(pstrText);
    }
    return true;
}

bool CMarkup::_ParseData(wchar_t*& pstrText, wchar_t*& pstrDest, char cEnd)
{
    while( *pstrText != _T('\0') && *pstrText != cEnd ) {
		if( *pstrText == _T('&') ) {
			while( *pstrText == _T('&') ) {
				_ParseMetaChar(++pstrText, pstrDest);
			}
			if (*pstrText == cEnd)
				break;
		}

        if( *pstrText == _T(' ') ) {
            *pstrDest++ = *pstrText++;
            if( !m_bPreserveWhitespace ) _SkipWhitespace(pstrText);
        }
        else {
            wchar_t* pstrTemp = ::CharNextW(pstrText);
            while( pstrText < pstrTemp) {
                *pstrDest++ = *pstrText++;
            }
        }
    }
    // Make sure that MapAttributes() works correctly when it parses
    // over a value that has been transformed.
    wchar_t* pstrFill = pstrDest + 1;
    while( pstrFill < pstrText ) *pstrFill++ = _T(' ');
    return true;
}

void CMarkup::_ParseMetaChar(wchar_t*& pstrText, wchar_t*& pstrDest)
{
    if( pstrText[0] == _T('a') && pstrText[1] == _T('m') && pstrText[2] == _T('p') && pstrText[3] == _T(';') ) {
        *pstrDest++ = _T('&');
        pstrText += 4;
    }
    else if( pstrText[0] == _T('l') && pstrText[1] == _T('t') && pstrText[2] == _T(';') ) {
        *pstrDest++ = _T('<');
        pstrText += 3;
    }
    else if( pstrText[0] == _T('g') && pstrText[1] == _T('t') && pstrText[2] == _T(';') ) {
        *pstrDest++ = _T('>');
        pstrText += 3;
    }
    else if( pstrText[0] == _T('q') && pstrText[1] == _T('u') && pstrText[2] == _T('o') && pstrText[3] == _T('t') && pstrText[4] == _T(';') ) {
        *pstrDest++ = _T('\"');
        pstrText += 5;
    }
    else if( pstrText[0] == _T('a') && pstrText[1] == _T('p') && pstrText[2] == _T('o') && pstrText[3] == _T('s') && pstrText[4] == _T(';') ) {
        *pstrDest++ = _T('\'');
        pstrText += 5;
    }
    else {
        *pstrDest++ = _T('&');
    }
}

bool CMarkup::_Failed(const wchar_t* pstrError, const wchar_t* pstrLocation)
{
    // Register last error
    wcsncpy(m_szErrorMsg, pstrError, (sizeof(m_szErrorMsg) / sizeof(m_szErrorMsg[0])) - 1);
    wcsncpy(m_szErrorXML, pstrLocation != NULL ? pstrLocation : L"", lengthof(m_szErrorXML) - 1);
    return false; // Always return 'false'
}

} // namespace DuiLib
