#include "vpxt_test_declarations.h"

int test_force_key_frame(int argc,
                         const char** argv,
                         const std::string &working_dir,
                         const std::string sub_folder_str,
                         int test_type,
                         int delete_ivf,
                         int artifact_detection)
{
    char *comp_out_str = "Key Frame Frequency";
    char *test_dir = "test_force_key_frame";
    int input_ver = vpxt_check_arg_input(argv[1], argc);

    if (input_ver < 0)
        return vpxt_test_help(argv[1], 0);

    std::string input = argv[2];
    int Mode = atoi(argv[3]);
    int bitrate = atoi(argv[4]);
    int ForceKeyFrameInt = atoi(argv[5]);
    std::string enc_format = argv[6];

    int speed = 0;

    //////////// Formatting Test Specific directory ////////////
    std::string cur_test_dir_str;
    std::string file_index_str;
    char main_test_dir_char[255] = "";
    char file_index_output_char[255] = "";

    if (initialize_test_directory(argc, argv, test_type, working_dir, test_dir,
        cur_test_dir_str, file_index_str, main_test_dir_char,
        file_index_output_char, sub_folder_str) == 11)
        return kTestErrFileMismatch;

    std::string ForceKeyFrame = cur_test_dir_str + slashCharStr() + test_dir +
        "_compression";
    vpxt_enc_format_append(ForceKeyFrame, enc_format);

    std::string KeyFrameoutputfile = cur_test_dir_str + slashCharStr() + test_dir +
        "_compression_key_frames.txt";

    ///////////// OutPutfile ////////////
    std::string text_file_str = cur_test_dir_str + slashCharStr() + test_dir;
    FILE *fp;

    vpxt_open_output_file(test_type, text_file_str, fp);

    if (test_type == kFullTest)
        print_header_full_test(argc, argv, main_test_dir_char);

    if (test_type == kCompOnly)
        print_header_compression_only(argc, argv, main_test_dir_char);

    if (test_type == kTestOnly)
        print_header_test_only(argc, argv, cur_test_dir_str);

    vpxt_cap_string_print(PRINT_BTH, "%s", test_dir);

    VP8_CONFIG opt;
    vpxt_default_parameters(opt);

    /////////////////// Use Custom Settings ///////////////////
    if (input_ver == 2)
    {
        if (!vpxt_file_exists_check(argv[argc-1]))
        {
            tprintf(PRINT_BTH, "\nInput Settings file %s does not exist\n",
                argv[argc-1]);

            fclose(fp);
            record_test_complete(file_index_str, file_index_output_char, test_type);
            return kTestIndeterminate;
        }

        opt = vpxt_input_settings(argv[argc-1]);
        bitrate = opt.target_bandwidth;
    }

    /////////////////////////////////////////////////////////

    opt.target_bandwidth = bitrate;
    opt.auto_key = 0;//1;

    // Run Test only (Runs Test, Sets up test to be run, or skips compresion of
    // files)
    if (test_type == kTestOnly)
    {
        // This test requires no preperation before a Test Only Run
    }
    else
    {
        opt.Mode = Mode;
        opt.key_freq = 0;//ForceKeyFrameInt;

        if (vpxt_compress_force_key_frame(input.c_str(), ForceKeyFrame.c_str(),
            speed, bitrate, opt, comp_out_str, ForceKeyFrameInt, 0,
            ForceKeyFrameInt, enc_format) == -1)
        {
            fclose(fp);
            record_test_complete(file_index_str, file_index_output_char, test_type);
            return kTestIndeterminate;
        }
    }

    // Create Compression only stop test short.
    if (test_type == kCompOnly)
    {
        fclose(fp);
        record_test_complete(file_index_str, file_index_output_char, test_type);
        return kTestEncCreated;
    }

    vpxt_display_key_frames(ForceKeyFrame.c_str(), 1);

    int f_key_frames = vpxt_check_force_key_frames(KeyFrameoutputfile.c_str(),
        ForceKeyFrameInt, ForceKeyFrame.c_str());

    int test_state = kTestFailed;

    if(f_key_frames == 0)
        test_state = kTestPassed;

    if(test_state == kTestPassed)
    {
        vpxt_formated_print(RESPRT, "Key Frames occur only when Force Key "
            "Frame dictates: %i - Passed", ForceKeyFrameInt);
        tprintf(PRINT_BTH, "\n");
    }

    if (test_state == kTestFailed)
        tprintf(PRINT_BTH, "\nFailed\n");
    if (test_state == kTestPassed)
        tprintf(PRINT_BTH, "\nPassed\n");

    if (delete_ivf)
        vpxt_delete_files(2, ForceKeyFrame.c_str(), KeyFrameoutputfile.c_str());

    fclose(fp);
    record_test_complete(file_index_str, file_index_output_char, test_type);
    return test_state;

}