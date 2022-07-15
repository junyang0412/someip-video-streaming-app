#include <iostream>

#include <opencv4/opencv2/imgcodecs.hpp>
#include <opencv4/opencv2/imgproc.hpp>
#include <opencv4/opencv2/highgui.hpp>

#include <vsomeip/vsomeip.hpp>

#include "conf.hpp"

std::shared_ptr<vsomeip::application> application;

/**
 * one thing to note is that on the same machine we do not have to
 * take care of the size of payload being sent but in case of multi
 * machine communication we do have to divide a packet into the 
*/

void on_message(const std::shared_ptr<vsomeip::message>& _message) {
    if(_message->get_message_type() == vsomeip::message_type_e::MT_REQUEST) {
        std::shared_ptr<vsomeip::message> response
            = vsomeip::runtime::get()->create_response(_message);
        std::shared_ptr<vsomeip::payload> payload
            = vsomeip::runtime::get()->create_payload();

        cv::VideoCapture capture_device(0);
            cv::Mat capture_img;
        while(true) {
            capture_device.read(capture_img);
            int rows = capture_img.rows;
            int cols = capture_img.cols;
            uchar *img_data = capture_img.data;
            const uchar* img_start = capture_img.datastart;
            const uchar* img_end = capture_img.dataend;
            const uchar* img_limit = capture_img.datalimit;

            cv::imshow("SEND_IMG", capture_img);
            cv::waitKey(1);

            size_t total_data_packets = rows*cols*3/1400;
            if((rows*cols*3)%1400 != 0) total_data_packets++;

            for(size_t i{};i<total_data_packets;i++) {
                size_t len{0};
                std::vector<vsomeip::byte_t> payload_bytestream;
                while((img_data != img_end) && (len < 1400)) {
                    //std::cout << *img_data << " ";
                    payload_bytestream.push_back(*img_data++);
                    len++;
                }
                payload->set_data(payload_bytestream);
                response->set_payload(payload);
                response->set_method(actions::stream_rcv);
                application->send(response);
            }

            // std::cout << "TYPE" << capture_img.type() << std::endl;
            // std::cout << "data len: " << len << std::endl;
            //std::cout << "ROWS: " << rows << ", COLS: " << cols << std::endl;
            
        }
    }
}

int main(int argc, char const *argv[]) {
    application = vsomeip::runtime::get()->create_application("StreamSource");
    application->init();
    application->offer_service(video_streaming_service,instance_id);
    application->register_message_handler(
        video_streaming_service,
        instance_id,
        actions::start_capture,
        on_message
    );
    application->start();
    return 0;
}
