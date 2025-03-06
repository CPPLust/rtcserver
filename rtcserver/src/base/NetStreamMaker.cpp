#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "NetStreamMaker.h"

CNetStreamMaker::CNetStreamMaker()
    : m_nMax(0)
    , m_nCurrent(0)
    , m_pData(NULL)
{

}

CNetStreamMaker::~CNetStreamMaker()
{
    if (nullptr != m_pData)
    {
        free(m_pData);
        m_pData = NULL;
    }
}

bool CNetStreamMaker::append_data(char* data, uint32_t size )
{
   
    if (!sdsMakeRoomFor(size)) return false;
   
    memcpy( m_pData + m_nCurrent, data, size );

    sdsIncrLen(size);

    return true;
}
bool CNetStreamMaker::sdsMakeRoomFor(uint32_t addlen) {
    if (m_nCurrent + addlen > m_nMax) {
        unsigned dn = 16;
        while (m_nCurrent + addlen > dn) {
            dn <<= 1;
        }
        return ensureCapacity(dn);
    }
    return true;
}

void CNetStreamMaker::sdsIncrLen(int incr) {
    m_nCurrent += incr;
}

bool CNetStreamMaker::ensureCapacity(uint32_t capacity) {
    if (capacity <= m_nMax) return true; // Already have enough space

    void* dp = realloc(m_pData, capacity);
    if (!dp) {
        // Log::error("realloc failed");
        return false;
    }

    m_pData = (char*)dp;
    m_nMax = capacity;
    return true;
}

void CNetStreamMaker::consume(uint32_t nbytes) {
    if (nbytes >= m_nCurrent) {
        // 如果要处理的字节数大于或等于当前数据量，则清空所有数据
        m_nCurrent = 0;
    }
    else {
        // 将剩余的数据前移到起始位置
        memmove(m_pData, m_pData + nbytes, m_nCurrent - nbytes);
        m_nCurrent -= nbytes;
    }
}

