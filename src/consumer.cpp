#include <iostream>

#include <opencv4/opencv2/imgcodecs.hpp>
#include <opencv4/opencv2/imgproc.hpp>
#include <opencv4/opencv2/highgui.hpp>

#include <vsomeip/vsomeip.hpp>

#include "conf.hpp"

std::shared_ptr<vsomeip::application> application;

enum status { unavailable, available };

void availability_handler(
    vsomeip::service_t s_id,
    vsomeip::instance_t i_id,
    bool status
) {
    if(status == status::available) {
        std::cout << "streaming is online\n";
        std::shared_ptr<vsomeip::message> request = vsomeip::runtime::get()->create_request();
        request->set_service(video_streaming_service);
        request->set_instance(instance_id);
        request->set_method(actions::start_capture);

        application->send(request);
    } else if(status == status::unavailable) {

    }
}

void on_message(const std::shared_ptr<vsomeip::message>& _message) {
    switch(_message->get_message_type()) {
        case vsomeip::message_type_e::MT_RESPONSE: {
            std::vector<vsomeip::byte_t> rec_data;
            std::shared_ptr<vsomeip::payload> payload = _message->get_payload();
            auto data = payload->get_data();
            auto size = payload->get_length();
            std::cout << "LENGTH: " << size << std::endl;
            cv::Mat rec_img(480,640,16,data);
            cv::imshow("RCV_IMG",rec_img);
            cv::waitKey(1);
            //exit(0);
        } break;
    }
}

int main(int argc, char const *argv[]) {
    application = vsomeip::runtime::get()->create_application("StreamSource");
    application->init();
    application->register_availability_handler(
        video_streaming_service, instance_id,
        availability_handler
    );
    application->register_message_handler(
        video_streaming_service,
        instance_id,
        actions::stream_rcv,
        on_message
    );
    application->request_service(video_streaming_service,instance_id);
    application->start();
    return 0;
}
