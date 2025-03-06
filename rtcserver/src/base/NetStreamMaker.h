#pragma once
#include <stdint.h>

class  CNetStreamMaker
{
public:
    CNetStreamMaker();
    virtual ~CNetStreamMaker();

    char* get(){return m_pData;}
    uint32_t size(){return m_nCurrent;}
    void clear(){m_nCurrent = 0;}

    /**
     * 向数据流中添加内容
     * @param data[in] 需要添加的数据
     * @param size[in] 数据大小
     * @return 成功true，失败false
     */
    bool append_data(char* data, uint32_t size );
    bool sdsMakeRoomFor(uint32_t addlen); // 确保有足够空间添加数据
    void sdsIncrLen(int incr); // 增加当前长度
    void consume(uint32_t nbytes); // 处理并移除前nbytes字节


private:
    bool ensureCapacity(uint32_t capacity); // 内部使用，确保容量足够

private:
    char*    m_pData;      //内存地址
    uint32_t m_nCurrent;   //当前写入数据的位置
    uint32_t m_nMax;       //内存大小
};
