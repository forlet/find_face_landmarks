// std
#include <iostream>
#include <exception>

// Boost
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/format.hpp>

// sfl
#include <sfl/sequence_face_landmarks.h>
#include <sfl/utilities.h>

// OpenCV
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

using std::cout;
using std::endl;
using std::cerr;
using std::string;
using std::runtime_error;
using namespace boost::program_options;
using namespace boost::filesystem;

int main(int argc, char* argv[])
{
	// Parse command line arguments
	string inputPath, outputPath, landmarksModelPath;
	try {
		options_description desc("Allowed options");
		desc.add_options()
			("help", "display the help message")
			("input,i", value<string>(&inputPath)->required(), "path to video sequence")
			("output,o", value<string>(&outputPath)->required(), "output path")
			("landmarks,l", value<string>(&landmarksModelPath)->required(), "path to landmarks model file")
			;
		variables_map vm;
		store(command_line_parser(argc, argv).options(desc).
			positional(positional_options_description().add("input", -1)).run(), vm);
		if (vm.count("help")) {
			cout << "Usage: find_face_landmarks [options]" << endl;
			cout << desc << endl;
			exit(0);
		}
		notify(vm);
		if (!is_regular_file(landmarksModelPath)) throw error("landmarks must be a path to a file!");
	}
	catch (const error& e) {
		cout << "Error while parsing command-line arguments: " << e.what() << endl;
		cout << "Use --help to display a list of options." << endl;
		exit(1);
	}

	try
	{
		// Initialize Sequence Face Landmarks
        std::shared_ptr<sfl::SequenceFaceLandmarks> sfl;
        sfl = sfl::SequenceFaceLandmarks::create(landmarksModelPath);
        sfl->clear();

        // Read input frame
        cv::Mat input_frame = cv::imread(inputPath);

        // Find landmarks
        const sfl::Frame& lmsFrame = sfl->addFrame(input_frame);
        if (lmsFrame.faces.empty()) {
            std::cerr << "ERROR: unable to detect face in: " << inputPath << std::endl;
            return EXIT_FAILURE;
        }

        const sfl::Face* face = lmsFrame.getFace(sfl::getMainFaceID(sfl->getSequence()));
        const std::vector<cv::Point>& landmarks = face->landmarks;

        // Dump landmarks
        std::ofstream out_file(outputPath);
        if (!out_file.is_open())
            throw std::runtime_error("unable to open: " + outputPath + " for writing.");
        out_file << "x,y" << std::endl;
        for (const cv::Point& p : landmarks)
        {
            out_file << std::to_string(p.x) << "," << std::to_string(p.y) << std::endl;
        }
	}
	catch (std::exception& e)
	{
		cerr << e.what() << endl;
		return 1;
	}

	return 0;
}

