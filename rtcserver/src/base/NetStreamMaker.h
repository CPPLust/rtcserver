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
     * �����������������
     * @param data[in] ��Ҫ��ӵ�����
     * @param size[in] ���ݴ�С
     * @return �ɹ�true��ʧ��false
     */
    bool append_data(char* data, uint32_t size );
    bool sdsMakeRoomFor(uint32_t addlen); // ȷ�����㹻�ռ��������
    void sdsIncrLen(int incr); // ���ӵ�ǰ����
    void consume(uint32_t nbytes); // �����Ƴ�ǰnbytes�ֽ�


private:
    bool ensureCapacity(uint32_t capacity); // �ڲ�ʹ�ã�ȷ�������㹻

private:
    char*    m_pData;      //�ڴ��ַ
    uint32_t m_nCurrent;   //��ǰд�����ݵ�λ��
    uint32_t m_nMax;       //�ڴ��С
};
