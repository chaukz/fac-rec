#include "Capture.h"

bool Capture::open(int deviceIndex) {
    cap_.open(deviceIndex);
    return cap_.isOpened();
}

bool Capture::readFrame(cv::Mat& out) {
    cap_.read(out);
    return !out.empty();
}
