#include "vpxt_test_declarations.h"

int test_one_pass_vs_two_pass(int argc, char *argv[], string WorkingDir, string FilesAr[], int TestType)
{
    char *CompressString = "Allow Drop Frames";
    char *MyDir = "test_one_pass_vs_two_pass";

    if (!(argc == 4 || argc == 5))
    {
        vpxt_cap_string_print(PRINT_STD, "  %s", MyDir);
        printf(
            "\n\n"
            "    <Input File>\n"
            "    <Target Bit Rate>\n"
            "\n"
        );
        return 0;
    }

    char *input = argv[2];
    int BitRate = atoi(argv[3]);

    int speed = 0;

    ////////////Formatting Test Specific Directory////////////
    string CurTestDirStr = "";
    char MainTestDirChar[255] = "";
    string FileIndexStr = "";
    char FileIndexOutputChar[255] = "";

    if (initialize_test_directory(argc, argv, TestType, WorkingDir, MyDir, CurTestDirStr, FileIndexStr, MainTestDirChar, FileIndexOutputChar, FilesAr) == 11)
        return 11;

    string OnePassOutFile1 = CurTestDirStr;
    OnePassOutFile1.append(slashCharStr());
    OnePassOutFile1.append(MyDir);
    OnePassOutFile1.append("_compression_one_pass_1.ivf");

    string OnePassOutFile2 = CurTestDirStr;
    OnePassOutFile2.append(slashCharStr());
    OnePassOutFile2.append(MyDir);
    OnePassOutFile2.append("_compression_one_pass_2.ivf");

    string OnePassOutFile3 = CurTestDirStr;
    OnePassOutFile3.append(slashCharStr());
    OnePassOutFile3.append(MyDir);
    OnePassOutFile3.append("_compression_one_pass_3.ivf");

    string TwoPassOutFile1 = CurTestDirStr;
    TwoPassOutFile1.append(slashCharStr());
    TwoPassOutFile1.append(MyDir);
    TwoPassOutFile1.append("_compression_two_pass_1.ivf");

    string TwoPassOutFile2 = CurTestDirStr;
    TwoPassOutFile2.append(slashCharStr());
    TwoPassOutFile2.append(MyDir);
    TwoPassOutFile2.append("_compression_two_pass_2.ivf");

    string TwoPassOutFile3 = CurTestDirStr;
    TwoPassOutFile3.append(slashCharStr());
    TwoPassOutFile3.append(MyDir);
    TwoPassOutFile3.append("_compression_two_pass_3.ivf");

    /////////////OutPutfile////////////
    string TextfileString = CurTestDirStr;
    TextfileString.append(slashCharStr());
    TextfileString.append(MyDir);

    if (TestType == COMP_ONLY || TestType == TEST_AND_COMP)
        TextfileString.append(".txt");
    else
        TextfileString.append("_TestOnly.txt");

    FILE *fp;

    if ((fp = freopen(TextfileString.c_str(), "w", stderr)) == NULL)
    {
        printf("Cannot open out put file: %s\n", TextfileString.c_str());
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

    vpxt_cap_string_print(PRINT_BOTH, "%s", MyDir);

    VP8_CONFIG opt;
    vpxt_default_parameters(opt);

    ///////////////////Use Custom Settings///////////////////
    if (argc == 5)
    {
        if (!vpxt_file_exists_check(argv[argc-1]))
        {
            tprintf("\nInput Settings file %s does not exist\n", argv[argc-1]);

            fclose(fp);
            record_test_complete(FileIndexStr, FileIndexOutputChar, TestType);
            return 2;
        }

        opt = vpxt_input_settings(argv[argc-1]);
        BitRate = opt.target_bandwidth;
    }

    /////////////////////////////////////////////////////////
    int CompressInt = opt.allow_df;

    int BitRate1 = BitRate - (BitRate * 0.3);
    int BitRate2 = BitRate;
    int BitRate3 = BitRate + (BitRate * 0.3);


    //Run Test only (Runs Test, Sets up test to be run, or skips compresion of files)
    if (TestType == TEST_ONLY)
    {
        //This test requires no preperation before a Test Only Run
    }
    else
    {
        opt.Mode = 5;
        opt.target_bandwidth = BitRate1;

        if (vpxt_compress_ivf_to_ivf(input, TwoPassOutFile1.c_str(), speed, BitRate1, opt, CompressString, CompressInt, 0) == -1)
        {
            fclose(fp);
            record_test_complete(FileIndexStr, FileIndexOutputChar, TestType);
            return 2;
        }

        opt.target_bandwidth = BitRate2;

        if (vpxt_compress_ivf_to_ivf(input, TwoPassOutFile2.c_str(), speed, BitRate2, opt, CompressString, CompressInt, 0) == -1)
        {
            fclose(fp);
            record_test_complete(FileIndexStr, FileIndexOutputChar, TestType);
            return 2;
        }

        opt.target_bandwidth = BitRate3;

        if (vpxt_compress_ivf_to_ivf(input, TwoPassOutFile3.c_str(), speed, BitRate3, opt, CompressString, CompressInt, 0) == -1)
        {
            fclose(fp);
            record_test_complete(FileIndexStr, FileIndexOutputChar, TestType);
            return 2;
        }

        opt.Mode = 2;
        opt.target_bandwidth = BitRate1;

        if (vpxt_compress_ivf_to_ivf(input, OnePassOutFile1.c_str(), speed, BitRate1, opt, CompressString, CompressInt, 0) == -1)
        {
            fclose(fp);
            record_test_complete(FileIndexStr, FileIndexOutputChar, TestType);
            return 2;
        }

        opt.target_bandwidth = BitRate2;

        if (vpxt_compress_ivf_to_ivf(input, OnePassOutFile2.c_str(), speed, BitRate2, opt, CompressString, CompressInt, 0) == -1)
        {
            fclose(fp);
            record_test_complete(FileIndexStr, FileIndexOutputChar, TestType);
            return 2;
        }

        opt.target_bandwidth = BitRate3;

        if (vpxt_compress_ivf_to_ivf(input, OnePassOutFile3.c_str(), speed, BitRate3, opt, CompressString, CompressInt, 0) == -1)
        {
            fclose(fp);
            record_test_complete(FileIndexStr, FileIndexOutputChar, TestType);
            return 2;
        }
    }

    //Create Compression only stop test short.
    if (TestType == COMP_ONLY)
    {
        //Compression only run
        fclose(fp);
        record_test_complete(FileIndexStr, FileIndexOutputChar, TestType);
        return 10;
    }

    printf("\n");

    float SizeTwoPass1 = vpxt_ivf_data_rate(TwoPassOutFile1.c_str(), 1);
    float SizeOnePass1 = vpxt_ivf_data_rate(OnePassOutFile1.c_str(), 1);
    float SizeTwoPass2 = vpxt_ivf_data_rate(TwoPassOutFile2.c_str(), 1);
    float SizeOnePass2 = vpxt_ivf_data_rate(OnePassOutFile2.c_str(), 1);
    float SizeTwoPass3 = vpxt_ivf_data_rate(TwoPassOutFile3.c_str(), 1);
    float SizeOnePass3 = vpxt_ivf_data_rate(OnePassOutFile3.c_str(), 1);

    double PSNRTwoPass1;
    double PSNRTwoPass2;
    double PSNRTwoPass3;

    double PSNROnePass1;
    double PSNROnePass2;
    double PSNROnePass3;

    PSNRTwoPass1 = vpxt_ivf_psnr(input, TwoPassOutFile1.c_str(), 1, 0, 1, NULL);
    PSNRTwoPass2 = vpxt_ivf_psnr(input, TwoPassOutFile2.c_str(), 1, 0, 1, NULL);
    PSNRTwoPass3 = vpxt_ivf_psnr(input, TwoPassOutFile3.c_str(), 1, 0, 1, NULL);
    PSNROnePass1 = vpxt_ivf_psnr(input, OnePassOutFile1.c_str(), 1, 0, 1, NULL);
    PSNROnePass2 = vpxt_ivf_psnr(input, OnePassOutFile2.c_str(), 1, 0, 1, NULL);
    PSNROnePass3 = vpxt_ivf_psnr(input, OnePassOutFile3.c_str(), 1, 0, 1, NULL);

    //    double PSRNPerc = vpxt_abs_double(((PSNR2 - PSNR1) / PSNR1) * 100.00);
    //    double BRPerc = vpxt_abs_double(((Size2 - Size1) / Size1) * 100.00);
    int Pass = 0;

    float TwoPassA = 0;
    float TwoPassB = 0;
    float TwoPassC = 0;

    float OnePassA = 0;
    float OnePassB = 0;
    float OnePassC = 0;

    float minCommon = 0;
    float maxCommon = 0;

    if (SizeTwoPass1 > SizeOnePass1) //take area over same range we have decent data for.
    {
        minCommon = SizeTwoPass1;
    }
    else
    {
        minCommon = SizeOnePass1;
    }

    if (SizeTwoPass3 > SizeOnePass3)
    {
        maxCommon = SizeOnePass3;
    }
    else
    {
        maxCommon = SizeTwoPass3;
    }

    vpxt_solve_quadradic(SizeTwoPass1, SizeTwoPass2, SizeTwoPass3, PSNRTwoPass1, PSNRTwoPass2, PSNRTwoPass3, TwoPassA, TwoPassB, TwoPassC);
    float TwoPassAreaVal = vpxt_area_under_quadradic(TwoPassA, TwoPassB, TwoPassC, minCommon, maxCommon);

    vpxt_solve_quadradic(SizeOnePass1, SizeOnePass2, SizeOnePass3, PSNROnePass1, PSNROnePass2, PSNROnePass3, OnePassA, OnePassB, OnePassC);
    float OnePassAreaVal = vpxt_area_under_quadradic(OnePassA, OnePassB, OnePassC, minCommon, maxCommon);

    tprintf("\n\n"
            "Data Points:\n"
            "\n"
            " Two Pass\n"
            "\n"
            "(%.2f,%2.2f)\n"
            "(%.2f,%2.2f)\n"
            "(%.2f,%2.2f)\n"
            "\n"
            " One Pass\n"
            "\n"
            "(%.2f,%2.2f)\n"
            "(%.2f,%2.2f)\n"
            "(%.2f,%2.2f)\n"
            "\n"
            "\n"
            , SizeTwoPass1, PSNRTwoPass1
            , SizeTwoPass2, PSNRTwoPass2
            , SizeTwoPass3, PSNRTwoPass3
            , SizeOnePass1, PSNROnePass1
            , SizeOnePass2, PSNROnePass2
            , SizeOnePass3, PSNROnePass3
           );

    tprintf("Two Pass Curve: y = %fx^2 + %fx + %f\n", TwoPassA, TwoPassB, TwoPassC);
    tprintf("One Pass Curve: y = %fx^2 + %fx + %f\n", OnePassA, OnePassB, OnePassC);
    tprintf("\nGood Quality area under curve for interval %.2f - %.2f = %.2f\n", minCommon, maxCommon, TwoPassAreaVal);
    tprintf("Best Quality area under curve for interval %.2f - %.2f = %.2f\n", minCommon, maxCommon, OnePassAreaVal);

    tprintf("\n\nResults:\n\n");

    if (TwoPassAreaVal == OnePassAreaVal)
    {
        vpxt_formated_print(RESPRT, "Two Pass area under curve: %.2f == One Pass area under curve: %.2f - Failed", TwoPassAreaVal, OnePassAreaVal);
        tprintf("\n");
    }

    if (OnePassAreaVal < TwoPassAreaVal)
    {
        vpxt_formated_print(RESPRT, "Two Pass area under curve: %.2f > One Pass area under curve: %.2f - Passed", TwoPassAreaVal, OnePassAreaVal);
        tprintf("\n");
        Pass = 1;
    }

    if (OnePassAreaVal > TwoPassAreaVal)
    {
        vpxt_formated_print(RESPRT, "Two Pass  area under curve: %.2f < One Pass area under curve: %.2f - Failed", TwoPassAreaVal, OnePassAreaVal);
        tprintf("\n");
    }

    if (Pass == 1)
    {
        tprintf("\nPassed\n");

        fclose(fp);
        record_test_complete(FileIndexStr, FileIndexOutputChar, TestType);
        return 1;
    }
    else
    {
        tprintf("\nFailed\n");

        fclose(fp);
        record_test_complete(FileIndexStr, FileIndexOutputChar, TestType);
        return 0;
    }

    fclose(fp);
    record_test_complete(FileIndexStr, FileIndexOutputChar, TestType);
    return 6;
}