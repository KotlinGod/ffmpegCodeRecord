//
// avio 文件目录操作
//
#include "iostream"

extern "C" {
#include "libavformat/avio.h"
#include "libavutil/log.h"


}

int main() {
    AVIODirContext *avioDirContext = nullptr;
    AVIODirEntry *avioDirEntry = nullptr;
    int ret = avio_open_dir(&avioDirContext, "../../../Google 云端硬盘/", nullptr);
    if (ret < 0) {
        av_log(nullptr, AV_LOG_ERROR, "打开目录失败\n");
        return -1;
    }
    while (true) {
        ret = avio_read_dir(avioDirContext, &avioDirEntry);
        if (ret < 0) {
            av_log(nullptr, AV_LOG_ERROR, "读取文件失败\n");
            return -1;
        }
        if (!avioDirEntry) {
            av_log(nullptr, AV_LOG_INFO, "目录读取完毕\n");
            break;
        }
        av_log(nullptr, AV_LOG_INFO, "%s:%lld:%lld\n", avioDirEntry->name, avioDirEntry->size,avioDirEntry->filemode);

        avio_free_directory_entry(&avioDirEntry);
    }

    avio_close_dir(&avioDirContext);
    return 0;
}
