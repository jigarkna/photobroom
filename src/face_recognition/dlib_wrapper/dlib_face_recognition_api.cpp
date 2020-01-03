
#include "dlib_face_recognition_api.hpp"

#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/matrix.h>
#include <dlib/dnn.h>
#include <dlib/image_transforms.h>
#include <QRgb>

// based on:
// https://github.com/davisking/dlib/blob/6b581d91f6b9b847a8163420630ef947e7cc88db/tools/python/src/cnn_face_detector.cpp

namespace dlib_api
{
    namespace
    {

        class cnn_face_detection_model_v1
        {

        public:

            cnn_face_detection_model_v1(const std::string& model_filename)
            {
                dlib::deserialize(model_filename) >> net;
            }

            std::vector<dlib::mmod_rect> detect (
                const QImage& pyimage,
                const int upsample_num_times
            )
            {
                dlib::pyramid_down<2> pyr;
                std::vector<dlib::mmod_rect> rects;

                // Copy the data into dlib based objects
                const QRect size = pyimage.rect();
                dlib::matrix<dlib::rgb_pixel> image;
                image.set_size(size.height(), size.width());

                for(int r = 0; r < size.height(); r++)
                    for(int c = 0; c < size.width(); c++)
                    {
                        const QRgb rgb = pyimage.pixel(c, r);
                        image(r, c) = dlib::rgb_pixel(qRed(rgb), qGreen(rgb), qBlue(rgb));
                    }

                // Upsampling the image will allow us to detect smaller faces but will cause the
                // program to use more RAM and run longer.
                unsigned int levels = upsample_num_times;
                while (levels > 0)
                {
                    levels--;
                    pyramid_up(image, pyr);
                }

                auto dets = net(image);

                // Scale the detection locations back to the original image size
                // if the image was upscaled.
                for (auto&& d : dets) {
                    d.rect = pyr.rect_down(d.rect, upsample_num_times);
                    rects.push_back(d);
                }

                return rects;
            }

        private:

            template <long num_filters, typename SUBNET> using con5d = dlib::con<num_filters,5,5,2,2,SUBNET>;
            template <long num_filters, typename SUBNET> using con5  = dlib::con<num_filters,5,5,1,1,SUBNET>;

            template <typename SUBNET> using downsampler  = dlib::relu<dlib::affine<con5d<32, dlib::relu<dlib::affine<con5d<32, dlib::relu<dlib::affine<con5d<16,SUBNET>>>>>>>>>;
            template <typename SUBNET> using rcon5  = dlib::relu<dlib::affine<con5<45,SUBNET>>>;

            using net_type = dlib::loss_mmod<dlib::con<1,9,9,1,1,rcon5<rcon5<rcon5<downsampler<dlib::input_rgb_image_pyramid<dlib::pyramid_down<6>>>>>>>>;

            net_type net;
        };


        std::vector<QRect> raw_face_locations(const QImage& img, int number_of_times_to_upsample, const std::string& model)
        {
            if (model == "cnn")
            {
                cnn_face_detection_model_v1 cnn_face_detector("");
                cnn_face_detector.detect(img, number_of_times_to_upsample);

                return {};
            }
            else
            {
                //auto face_detector = dlib::get_frontal_face_detector();
                //face_detector(img, number_of_times_to_upsample);

                return {};
            }
        }
    }

    std::vector<QRect> face_locations(const QImage& img, int number_of_times_to_upsample, const std::string& model)
    {
        if (model == "cnn")
            return {}; //[_trim_css_to_bounds(_rect_to_css(face.rect), img.shape) for face in _raw_face_locations(img, number_of_times_to_upsample, "cnn")];
        else
            return {}; //[_trim_css_to_bounds(_rect_to_css(face), img.shape) for face in _raw_face_locations(img, number_of_times_to_upsample, model)];
    }
}
