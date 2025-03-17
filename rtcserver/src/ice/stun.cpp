#include <rtc_base/byte_order.h>
#include <rtc_base/crc32.h>
#include "rtc_base/logging.h"

#include "ice/stun.h"

namespace xrtc {

const char EMPTY_TRANSACION_ID[] = "000000000000";
const size_t STUN_FINGERPRINT_XOR_VALUE = 0x5354554e;

StunMessage::StunMessage() :
    _type(0),
    _length(0),
    _transaction_id(EMPTY_TRANSACION_ID)
{
}

StunMessage::~StunMessage() = default;

/*
* 属性FINGERPRINT
        FINGERPRINT可以出现在所有的STUN消息当中，WebRTC会强制校验FINGERPRINT的属性
        可以用于解复用
    Type: 0x8028
    Value的类型：UInt32
 计算方法：
    首先用STUN的头部+所有属性的内容（不包含FINGERPRINT自身），计算crc32的值
    FINGERPRINT value = crc32值 ^ 0x5354554e
*/
bool StunMessage::validate_fingerprint(const char* data, size_t len) {

    size_t fingerprint_attr_size = k_stun_attribute_header_size +StunUInt32Attribute::SIZE;
    if (len % 4 != 0 || len < k_stun_header_size + fingerprint_attr_size) {
        return false;
    }
    

    const char* magic_cookie = data + k_stun_transaction_id_offset -k_stun_magic_cookie_length;
    if (rtc::GetBE32(magic_cookie) != k_stun_magic_cookie) {
        return false;
    }
    

    const char* fingerprint_attr_data = data + len - fingerprint_attr_size;
    if (rtc::GetBE16(fingerprint_attr_data) != STUN_ATTR_FINGERPRINT ||
            rtc::GetBE16(fingerprint_attr_data + sizeof(uint16_t)) !=
            StunUInt32Attribute::SIZE) 
    {
        return false;
    }
    

    uint32_t fingerprint = rtc::GetBE32(fingerprint_attr_data + k_stun_attribute_header_size);

    return (fingerprint ^ STUN_FINGERPRINT_XOR_VALUE) ==rtc::ComputeCrc32(data, len - fingerprint_attr_size);
}

bool StunMessage::read(rtc::ByteBufferReader* buf) {
    const char* psrc = buf->Data();
    char a1 = *psrc;
    char a2 = *(psrc + 1);
    if (!buf) {
        return false;
    }
    /*
	          0                   1                   2                   3
		 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
		+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		|0 0| STUN Message Type (14bit) |    Message Length (16bit)     |
		+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		|                         Magic Cookie (32bit)                  |
		+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		|                                                               |
		|                     Transaction ID (96 bits)                  |
		|                                                               |
		+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

    */
    if (!buf->ReadUInt16(&_type)) {
        return false;
    }
    printf("++++++++++++++++++++++++++++++++++++++\n");
    for (int i = 7; i >= 0; --i) {
        printf("%d ", ((a1 >> i) & 1));
    }
    printf("\n");

    RTC_LOG(LS_INFO) << "+++++++++++++++++++ " << _type;
    // rtp/rtcp 10(2)
    if (_type & 0x0800) {
        return false;
    }
    
    if (!buf->ReadUInt16(&_length)) {
        return false;
    }
    
    std::string magic_cookie;
    if (!buf->ReadString(&magic_cookie, k_stun_magic_cookie_length)) {
        return false;
    }
    
    std::string transaction_id;
    if (!buf->ReadString(&transaction_id, k_stun_transaction_id_length)) {
        return false;
    }
    
    uint32_t magic_cookie_int;
    memcpy(&magic_cookie_int, magic_cookie.data(), sizeof(magic_cookie_int));
    if (rtc::NetworkToHost32(magic_cookie_int) != k_stun_magic_cookie) {
        transaction_id.insert(0, magic_cookie);
    }
    
    _transaction_id = transaction_id;
    
    if (buf->Length() != _length) {
        return false;
    }
    
    _attrs.resize(0);

    while (buf->Length() > 0) {
        uint16_t attr_type;
        uint16_t attr_length;
        if (!buf->ReadUInt16(&attr_type)) {
            return false;
        }

        if (!buf->ReadUInt16(&attr_length)) {
            return false;
        }
        
        std::unique_ptr<StunAttribute> attr = 
            _create_attribute(attr_type, attr_length);
        if (!attr) {
            if (attr_length % 4 != 0) {
                attr_length += (4 - (attr_length % 4));
            }

            if (!buf->Consume(attr_length)) {
                return false;
            }
        } else {
            if (!attr->read(buf)) {
                return false;
            }

            _attrs.push_back(std::move(attr));
        }
    }

    return true;
}

std::unique_ptr<StunAttribute> StunMessage::_create_attribute(uint16_t type, 
        uint16_t length) 
{
    return nullptr;
}
} // namespace xrtc


