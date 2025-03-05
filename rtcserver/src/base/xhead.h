
 #ifndef  __BASE_XHEAD_H_
#define  __BASE_XHEAD_H_

#include <stdint.h>

namespace xrtc {

const int XHEAD_SIZE = 36;
const uint32_t XHEAD_MAGIC_NUM = 0xfb202202;

struct xhead_t {
    //id
    uint16_t id;  // ���������ݰ����б��
    //�汾
    uint16_t version; // Э��汾
    //��־id
    uint32_t log_id; // logid���������������Σ����㶨λ����
    char provider[16]; // �������������Դ
    uint32_t magic_num;  // ħ�����֣������԰�����У��
    uint32_t reserved;  // �����ֶΣ�������չ
    uint32_t body_len;  // ��ʾ������ĳ���
};


} // namespace xrtc

#endif  //__BASE_XHEAD_H_


