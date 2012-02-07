#include "vpxt_test_declarations.h"

int test_lag_in_frames(int argc, const char *const *argv, const std::string &WorkingDir, std::string FilesAr[], int TestType, int DeleteIVF)
{
    char *CompressString = "Lag In Frames";
    char *MyDir = "test_lag_in_frames";
    int inputCheck = vpxt_check_arg_input(argv[1], argc);

    if (inputCheck < 0)
        return vpxt_test_help(argv[1], 0);

    std::string input = argv[2];
    int Mode = atoi(argv[3]);
    int BitRate = atoi(argv[4]);;
    int LagInFrames1Val = atoi(argv[5]);
    int LagInFrames2Val = atoi(argv[6]);
    std::string EncForm = argv[7];

    int speed = 0;

    ////////////Formatting Test Specific Directory////////////

    std::string CurTestDirStr = ""; // <- All Options need to set a value for this
    std::string FileIndexStr = "";
    char MainTestDirChar[255] = "";
    char FileIndexOutputChar[255] = "";

    if (initialize_test_directory(argc, argv, TestType, WorkingDir, MyDir, CurTestDirStr, FileIndexStr, MainTestDirChar, FileIndexOutputChar, FilesAr) == 11)
        return 11;

    char laginframesbuff[255];

    std::string LagInFrames0 = CurTestDirStr;
    LagInFrames0.append(slashCharStr());
    LagInFrames0.append("test_lag_in_frames_compression_0");
    vpxt_enc_format_append(LagInFrames0, EncForm);

    std::string LagInFrames1 = CurTestDirStr;
    LagInFrames1.append(slashCharStr());
    LagInFrames1.append(MyDir);
    LagInFrames1.append("_compression_");
    LagInFrames1.append(vpxt_itoa_custom(LagInFrames1Val, laginframesbuff, 10));
    vpxt_enc_format_append(LagInFrames1, EncForm);

    std::string LagInFrames2 = CurTestDirStr;
    LagInFrames2.append(slashCharStr());
    LagInFrames2.append(MyDir);
    LagInFrames2.append("_compression_");
    LagInFrames2.append(vpxt_itoa_custom(LagInFrames2Val, laginframesbuff, 10));
    vpxt_enc_format_append(LagInFrames2, EncForm);

    /////////////OutPutfile////////////
    std::string TextfileString = CurTestDirStr;
    TextfileString.append(slashCharStr());
    TextfileString.append(MyDir);

    if (TestType == COMP_ONLY || TestType == TEST_AND_COMP)
        TextfileString.append(".txt");
    else
        TextfileString.append("_TestOnly.txt");


    FILE *fp;

    if ((fp = freopen(TextfileString.c_str(), "w", stderr)) == NULL)
    {
        tprintf(PRINT_STD, "Cannot open out put file: %s\n", TextfileString.c_str());
        exit(1);
    }

    ////////////////////////////////
    //////////////////////////////////////////////////////////

    if (TestType == TEST_AND_COMP)
        print_header_full_test(argc, argv, MainTestDirChar);

    if (TestType == COMP_ONLY)
        print_header_compression_only(argc, argv, MainTestDirChar);

    if (TestType == TEST_ONLY)
        print_header_test_only(argc, argv, CurTestDirStr);

    vpxt_cap_string_print(PRINT_BTH, "%s", MyDir);

    VP8_CONFIG opt;
    vpxt_default_parameters(opt);

    ///////////////////Use Custom Settings///////////////////
    if (inputCheck == 2)
    {
        if (!vpxt_file_exists_check(argv[argc-1]))
        {
            tprintf(PRINT_BTH, "\nInput Settings file %s does not exist\n", argv[argc-1]);

            fclose(fp);
            record_test_complete(FileIndexStr, FileIndexOutputChar, TestType);
            return 2;
        }

        opt = vpxt_input_settings(argv[argc-1]);
        BitRate = opt.target_bandwidth;
    }

    /////////////////////////////////////////////////////////

    opt.target_bandwidth = BitRate;

    if (LagInFrames1Val > 25 || LagInFrames2Val > 25 || LagInFrames2Val < 0 || LagInFrames1Val < 0)
    {
        tprintf(PRINT_BTH, "\nLag in Frames settings must be between 0 and 25.\n");

        fclose(fp);
        record_test_complete(FileIndexStr, FileIndexOutputChar, TestType);
        return 2;
    }

    //Run Test only (Runs Test, Sets up test to be run, or skips compresion of files)
    if (TestType == TEST_ONLY)
    {
        //This test requires no preperation before a Test Only Run
    }
    else
    {
        opt.Mode = Mode;

        opt.allow_lag = 0;
        opt.lag_in_frames = LagInFrames1Val;

        if (vpxt_compress(input.c_str(), LagInFrames0.c_str(), speed, BitRate, opt, CompressString, 0, 1, EncForm) == -1)
        {
            fclose(fp);
            record_test_complete(FileIndexStr, FileIndexOutputChar, TestType);
            return 2;
        }

        opt.allow_lag = 1;
        opt.lag_in_frames = LagInFrames1Val;

        if (vpxt_compress(input.c_str(), LagInFrames1.c_str(), speed, BitRate, opt, CompressString, LagInFrames2Val, 1, EncForm) == -1)
        {
            fclose(fp);
            record_test_complete(FileIndexStr, FileIndexOutputChar, TestType);
            return 2;
        }

        opt.allow_lag = 1;
        opt.lag_in_frames = LagInFrames2Val;

        if (vpxt_compress(input.c_str(), LagInFrames2.c_str(), speed, BitRate, opt, CompressString, LagInFrames2Val, 1, EncForm) == -1)
        {
            fclose(fp);
            record_test_complete(FileIndexStr, FileIndexOutputChar, TestType);
            return 2;
        }
    }

    if (TestType == COMP_ONLY)
    {
        fclose(fp);
        record_test_complete(FileIndexStr, FileIndexOutputChar, TestType);
        return 10;
    }

    double LagInFrames0PSNR = vpxt_psnr(input.c_str(), LagInFrames0.c_str(), 0, PRINT_BTH, 1, NULL);
    double LagInFrames1PSNR = vpxt_psnr(input.c_str(), LagInFrames1.c_str(), 0, PRINT_BTH, 1, NULL);
    double LagInFrames2PSNR = vpxt_psnr(input.c_str(), LagInFrames2.c_str(), 0, PRINT_BTH, 1, NULL);

    double TenPer0 = LagInFrames0PSNR / 10;
    double TenPer1 = LagInFrames1PSNR / 10;
    double TenPer2 = LagInFrames2PSNR / 10;

    int lngRC1 = vpxt_compare_enc(LagInFrames0.c_str(), LagInFrames1.c_str(), 0);
    int lngRC2 = vpxt_compare_enc(LagInFrames1.c_str(), LagInFrames2.c_str(), 0);

    std::string QuantInStr0;
    vpxt_remove_file_extension(LagInFrames0.c_str(), QuantInStr0);
    //std::string QuantInStr0 = LagInFrames0;
    //QuantInStr0.erase(QuantInStr0.length() - 4, 4);
    QuantInStr0.append("quantizers.txt");
    int LagInFramesFound0 = vpxt_lag_in_frames_check(QuantInStr0.c_str());

    std::string QuantInStr1;
    vpxt_remove_file_extension(LagInFrames1.c_str(), QuantInStr1);
    //std::string QuantInStr1 = LagInFrames1;
    //QuantInStr1.erase(QuantInStr1.length() - 4, 4);
    QuantInStr1.append("quantizers.txt");
    int LagInFramesFound1 = vpxt_lag_in_frames_check(QuantInStr1.c_str());

    std::string QuantInStr2;
    vpxt_remove_file_extension(LagInFrames2.c_str(), QuantInStr2);
    //std::string QuantInStr2 = LagInFrames2;
    //QuantInStr2.erase(QuantInStr2.length() - 4, 4);
    QuantInStr2.append("quantizers.txt");
    int LagInFramesFound2 = vpxt_lag_in_frames_check(QuantInStr2.c_str());

    int PSNRTally = 0;
    int fail = 0;

    char LagInFrames0FileName[255] = "";
    char LagInFrames1FileName[255] = "";
    char LagInFrames2FileName[255] = "";

    vpxt_file_name(LagInFrames0.c_str(), LagInFrames0FileName, 0);
    vpxt_file_name(LagInFrames1.c_str(), LagInFrames1FileName, 0);
    vpxt_file_name(LagInFrames2.c_str(), LagInFrames2FileName, 0);

    tprintf(PRINT_BTH, "\n\nResults:\n\n");

    if (LagInFramesFound0 == 0)
    {
        vpxt_formated_print(RESPRT, "%s properly lagged %i frames - Passed", LagInFrames0FileName, LagInFramesFound0);
        tprintf(PRINT_BTH, "\n");
    }
    else
    {
        vpxt_formated_print(RESPRT, "%s improperly lagged frames %i - Failed", LagInFrames0FileName, LagInFramesFound0);
        tprintf(PRINT_BTH, "\n");
    }

    if (LagInFramesFound1 == LagInFrames1Val)
    {
        vpxt_formated_print(RESPRT, "%s properly lagged %i frames - Passed", LagInFrames1FileName, LagInFramesFound1);
        tprintf(PRINT_BTH, "\n");
    }
    else
    {
        vpxt_formated_print(RESPRT, "%s improperly lagged %i frames - Failed", LagInFrames1FileName, LagInFramesFound1);
        tprintf(PRINT_BTH, "\n");
    }

    if (LagInFramesFound2 == LagInFrames2Val)
    {
        vpxt_formated_print(RESPRT, "%s properly lagged %i frames - Passed", LagInFrames2FileName, LagInFramesFound2);
        tprintf(PRINT_BTH, "\n");
    }
    else
    {
        vpxt_formated_print(RESPRT, "%s improperly lagged %i frames - Failed", LagInFrames2FileName, LagInFramesFound2);
        tprintf(PRINT_BTH, "\n");
    }

    if (lngRC1 == -1)
    {
        vpxt_formated_print(RESPRT, "%s identical to %s - Failed", LagInFrames0FileName, LagInFrames1FileName);
        tprintf(PRINT_BTH, "\n");
        fail = 1;
    }
    else
    {
        vpxt_formated_print(RESPRT, "%s not identical to %s - Passed", LagInFrames0FileName, LagInFrames1FileName);
        tprintf(PRINT_BTH, "\n");
    }

    if (lngRC2 == -1)
    {
        vpxt_formated_print(RESPRT, "%s identical to %s - Failed", LagInFrames1FileName, LagInFrames2FileName);
        tprintf(PRINT_BTH, "\n");
        fail = 1;
    }
    else
    {
        vpxt_formated_print(RESPRT, "%s not identical to %s - Passed", LagInFrames1FileName, LagInFrames2FileName);
        tprintf(PRINT_BTH, "\n");
    }

    if (LagInFrames1PSNR <= (LagInFrames0PSNR + TenPer0) && LagInFrames1PSNR >= (LagInFrames0PSNR - TenPer0))
    {
        vpxt_formated_print(RESPRT, "PSNR for %s is within 10%% of PSNR for %s - %.2f < %.2f < %.2f - Passed", LagInFrames0FileName, LagInFrames1FileName, (LagInFrames0PSNR - TenPer0), LagInFrames1PSNR, (LagInFrames0PSNR + TenPer0));
        tprintf(PRINT_BTH, "\n");
    }
    else
    {
        if (!(LagInFrames1PSNR <= (LagInFrames0PSNR + TenPer0)))
        {
            vpxt_formated_print(RESPRT, "PSNR for %s is not within 10%% of PSNR for %s - %.2f < %.2f - Failed", LagInFrames0FileName, LagInFrames1FileName, (LagInFrames0PSNR + TenPer0), LagInFrames1PSNR);
            tprintf(PRINT_BTH, "\n");
            fail = 1;
        }
        else
        {
            vpxt_formated_print(RESPRT, "PSNR for %s is not within 10%% of PSNR for %s - %.2f < %.2f - Failed", LagInFrames0FileName, LagInFrames1FileName, LagInFrames1PSNR, (LagInFrames0PSNR - TenPer0));
            tprintf(PRINT_BTH, "\n");
            fail = 1;
        }
    }

    if (LagInFrames2PSNR <= (LagInFrames1PSNR + TenPer1) && LagInFrames2PSNR >= (LagInFrames1PSNR - TenPer1))
    {
        vpxt_formated_print(RESPRT, "PSNR for %s is within 10%% of PSNR for %s - %.2f < %.2f < %.2f - Passed", LagInFrames1FileName, LagInFrames2FileName, (LagInFrames1PSNR - TenPer1), LagInFrames2PSNR, (LagInFrames1PSNR + TenPer1));
        tprintf(PRINT_BTH, "\n");
    }
    else
    {
        if (!(LagInFrames1PSNR <= (LagInFrames0PSNR + TenPer0)))
        {
            vpxt_formated_print(RESPRT, "PSNR for %s is not within 10%% of PSNR for %s - %.2f < %.2f - Failed", LagInFrames1FileName, LagInFrames2FileName, (LagInFrames1PSNR + TenPer1), LagInFrames2PSNR);
            tprintf(PRINT_BTH, "\n");
            fail = 1;
        }
        else
        {
            vpxt_formated_print(RESPRT, "PSNR for %s is not within 10%% of PSNR for %s - %.2f < %.2f - Failed", LagInFrames1FileName, LagInFrames2FileName, LagInFrames2PSNR, (LagInFrames1PSNR - TenPer1));
            tprintf(PRINT_BTH, "\n");
            fail = 1;
        }
    }

    if (fail == 0)
    {
        tprintf(PRINT_BTH, "\nPassed\n");

        if (DeleteIVF)
            vpxt_delete_files(3, LagInFrames0.c_str(), LagInFrames1.c_str(), LagInFrames2.c_str());

        fclose(fp);
        record_test_complete(FileIndexStr, FileIndexOutputChar, TestType);
        return 1;
    }
    else
    {
        tprintf(PRINT_BTH, "\nFailed\n");

        if (DeleteIVF)
            vpxt_delete_files(3, LagInFrames0.c_str(), LagInFrames1.c_str(), LagInFrames2.c_str());

        fclose(fp);
        record_test_complete(FileIndexStr, FileIndexOutputChar, TestType);
        return 0;
    }

    fclose(fp);
    record_test_complete(FileIndexStr, FileIndexOutputChar, TestType);
    return 6;
}