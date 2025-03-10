
#ifndef  __SESSION_DESCRIPTION_H_
#define  __SESSION_DESCRIPTION_H_

#include <string>
#include <vector>
#include <memory>

namespace xrtc {

enum class SdpType {
    k_offer = 0,//推流来的是offer
    k_answer = 1, //返回的是answer
};

enum class MediaType {
    MEDIA_TYPE_AUDIO,
    MEDIA_TYPE_VIDEO
};
class MediaContentDescription {
public:
    virtual ~MediaContentDescription() {}
    virtual MediaType type() = 0;
    virtual std::string mid() = 0;
};
class AudioContentDescription : public MediaContentDescription {
public:
    MediaType type() override { return MediaType::MEDIA_TYPE_AUDIO; }
    std::string mid() override { return "audio"; }
};

class VideoContentDescription : public MediaContentDescription {
public:
    MediaType type() override { return MediaType::MEDIA_TYPE_VIDEO; }
    std::string mid() override { return "video"; }
};
//内容分组
class ContentGroup {
public:
    ContentGroup(const std::string& semantics) : _semantics(semantics) {}
    ~ContentGroup() {}

    std::string semantics() const { return _semantics; }
    const std::vector<std::string>& content_names() const { return _content_names; }
    bool has_content_name(const std::string& content_name);
    void add_content_name(const std::string& content_name);

private:
    std::string _semantics; //其实就是组的名字，要起个有意义一点的，它的语义
    std::vector<std::string> _content_names; //存了哪些名字
};
class SessionDescription {
public:
    SessionDescription(SdpType type);
    ~SessionDescription();
    
    void add_content(std::shared_ptr<MediaContentDescription> content);
    //获取当前的所有的m行 
    const std::vector<std::shared_ptr<MediaContentDescription>>& contents() const {
        return _contents;
    }
    void add_group(const ContentGroup& group);
    std::vector<const ContentGroup*> get_group_by_name(const std::string& name) const;

    std::string to_string();

private:
    SdpType _sdp_type;
    std::vector<std::shared_ptr<MediaContentDescription>> _contents;
	//存了哪些组
    std::vector<ContentGroup> _content_groups;
};

} // namespace xrtc

#endif  //__SESSION_DESCRIPTION_H_


