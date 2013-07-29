#pragma once

#pragma pack(push, 1)
struct PACKET_HEADER
{
	UINT32 m_Magic;				// 幻数，'YDWE'
	UINT32 m_Length;			// 本消息全长
	UINT32 m_Checksum;			// CRC32校验码,为0则无校验
	UINT32 m_ProtocalVersion;	// 协议版本，目前是1
	UINT32 m_TypeId;			// 消息操作码，不同消息有不同的定义
};
struct PACKET_KEEPALIVE
{
	PACKET_HEADER header;
	UINT32 m_YDWEVersion;		// YDWE的版本号
};

class CPacketHeader
{
public:
	CPacketHeader()
		: m_Magic('YDWE'), m_Length(sizeof(CPacketHeader)), m_Checksum(0), m_ProtocalVersion(1), m_TypeId(0)
	{
	}

	PBYTE GetData()
	{
		return (PBYTE)this;
	}

	size_t GetSize()
	{
		return sizeof(CPacketHeader);
	}

protected:
	UINT32 m_Magic; // 'YDWE'
	UINT32 m_Length;
	UINT32 m_Checksum; // 为0则不检查
	UINT32 m_ProtocalVersion; // 协议版本，目前是1
	UINT32 m_TypeId;
};

class CPacketKeepAlive : public CPacketHeader
{
public:
	CPacketKeepAlive(UINT32 nVersion)
		: CPacketHeader(), m_YDWEVersion(nVersion)
	{
		m_TypeId   = 0x01;
		m_Length = GetSize();
	}

	size_t GetSize()
	{
		return sizeof(CPacketKeepAlive);
	}

protected:
	UINT32 m_YDWEVersion;
};

#pragma pack(pop)
