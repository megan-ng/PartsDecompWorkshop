//**********************************************************
//Copyright 2021 Tabish Syed
//
//Licensed under the Apache License, Version 2.0 (the "License");
//you may not use this file except in compliance with the License.
//You may obtain a copy of the License at
//
//http://www.apache.org/licenses/LICENSE-2.0
//
//Unless required by applicable law or agreed to in writing, software
//distributed under the License is distributed on an "AS IS" BASIS,
//WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//See the License for the specific language governing permissions and
//limitations under the License.
//**********************************************************
//
// Created by tabish on 2021-07-26.
//

#include <itkImage.h>
#include <cstdlib>
#include <string>
#include <iostream>
#include <itkListSample.h>
#include <experimental/filesystem>
#include <itkLogger.h>
#include <itkStdStreamLogOutput.h>

#include "itkCommandLineArgumentParser.h"
#include "utils.h"
#include "medial.h"

namespace fs = std::experimental::filesystem;

// #include <itkMesh.h>
// #include <itkThresholdImageFilter.h>
// #include <itkBinaryMask3DMeshSource.h>
// #include <itkMeshFileWriter.h>
// #include "itkConnectedComponentImageFilter.h"
// #include "itkLabelShapeKeepNObjectsImageFilter.h"
// #include "itkRescaleIntensityImageFilter.h"

// using ImageType = itk::Image<float, 3U>;
// using MeshType = itk::Mesh<float, 3U>;
// // template<typename ImageType, typename MeshType>
// typename MeshType::Pointer generateMesh(const typename ImageType::Pointer volume, bool testMode = false)
// {
//     using ConnectedComponentFilter = itk::ConnectedComponentImageFilter<ImageType, ImageType>;
//     auto connectedComponentsFilter = ConnectedComponentFilter::New();
//     connectedComponentsFilter->SetInput(volume);

//     if(testMode)
//     {
//         connectedComponentsFilter->Update();
//         std::cout << "Number of objects" << connectedComponentsFilter->GetObjectCount() << std::endl; 
//     }

//     using ComponentExtractionFilter = itk::LabelShapeKeepNObjectsImageFilter<ImageType>;
//     auto componentExtractor = ComponentExtractionFilter::New();
//     componentExtractor->SetInput(connectedComponentsFilter->GetOutput());
//     componentExtractor->SetBackgroundValue(0);
//     componentExtractor->SetNumberOfObjects(1);
//     componentExtractor->SetAttribute(ComponentExtractionFilter::LabelObjectType::NUMBER_OF_PIXELS);

//     using RescaleIntensityFilterType = itk::RescaleIntensityImageFilter<ImageType, ImageType>;
//     auto intensityRescaler = RescaleIntensityFilterType::New();
//     intensityRescaler->SetOutputMinimum(0);
//     intensityRescaler->SetOutputMaximum(itk::NumericTraits<typename ImageType::PixelType>::max());
//     intensityRescaler->SetInput(componentExtractor->GetOutput());

//     using GaussianFilterType = itk::DiscreteGaussianImageFilter<ImageType, ImageType>;
//     auto smoother = GaussianFilterType::New();
//     smoother->SetInput(intensityRescaler->GetOutput());
//     double variance[3] = {1,1,1};
//     smoother->SetVariance(variance);
//     smoother->SetUseImageSpacingOff();


//     using BinaryThresholdFilterType = itk::BinaryThresholdImageFilter<ImageType, ImageType>;
//     auto binaryThresholder = BinaryThresholdFilterType::New();
//     binaryThresholder->SetInput(smoother->GetOutput());
//     binaryThresholder->SetLowerThreshold(itk::NumericTraits<typename ImageType::PixelType>::max() / 2);
//     binaryThresholder->SetOutsideValue(0);

//     using BinaryImageToMeshFilter = itk::BinaryMask3DMeshSource<ImageType, MeshType>;
//     auto imageToMeshFilter = BinaryImageToMeshFilter::New();
//     imageToMeshFilter->SetInput(binaryThresholder->GetOutput());
//     imageToMeshFilter->SetObjectValue(255);

//     imageToMeshFilter->Update();
//     MeshType::Pointer mesh = imageToMeshFilter->GetOutput();

//     if(testMode)
//     {
//         std::cout << "Saving mesh" << std::endl;

//         using WriterType = itk::MeshFileWriter<MeshType>;
//         auto writer = WriterType::New();
//         writer->SetFileName("generatedMesh.obj");
//         writer->SetInput(mesh);
//         writer ->Update();
//     }

//     return mesh;
// }

template<typename DistanceImageType, typename FluxImageType, typename OutputImageType, typename MedialStructureComputeFunction>
int computeMedialStructure(const itk::CommandLineArgumentParser::Pointer &parser,
                           const itk::Logger::Pointer &logger,
                           const MedialStructureComputeFunction &compute){

    logger->Info("Starting Medial Surface computation\n");
    std::string inputFilename;
    fs::path inputFilePath, rootPath, outputFolderPath;
    bool hasInputFilePath =
            parser->GetCommandLineArgument("-input", inputFilename);

    // Read the input image.
    if (!hasInputFilePath) {
        logger->Error("Input file not specified\n");
        return EXIT_FAILURE;
    }else{
        inputFilePath = inputFilename;
        if(parser->ArgumentExists("-outputFolder")){
            std::string outputFoldername;
            parser->GetCommandLineArgument("-outputFolder", outputFoldername);
            outputFolderPath = outputFoldername;
        }else {
            outputFolderPath = inputFilePath.parent_path() / inputFilePath.stem();
            std::vector<std::string> newargs;
            newargs.emplace_back("-outputFolder");
            newargs.push_back(outputFolderPath);
            parser->AddCommandLineArguments(newargs);
            logger->Info("-outputFolder missing using default: " + outputFolderPath.string() + "\n");
        }
        if(fs::exists(outputFolderPath)){
            if(fs::is_directory(outputFolderPath)) {
                logger->Warning("Directory " + outputFolderPath.string() + " already Exists\n");
                logger->Warning("Old files in " + outputFolderPath.string() + "may be overwritten\n");
            }else{
                logger->Critical("File named " + outputFolderPath.string() + " already Exists\n");
                return EXIT_FAILURE;
            }
        }else{
            fs::create_directory(outputFolderPath);
            logger->Info("Created directory " + outputFolderPath.string() + "\n");
        }
    }
    logger->Info("output data Folder : " + outputFolderPath.string() + "\n");
    if (!fs::exists(inputFilePath)) {
        logger->Critical("File " + inputFilename + " not found\n");
        return EXIT_FAILURE;
    }
    bool lowMemory = parser->ArgumentExists("-lowMemory");
    if(lowMemory) logger->Info("Using The low memory option!\n");

    fs::path distanceMapFilePath = outputFolderPath / "signedDistanceMap.tif";
    fs::path spokeFilePath = outputFolderPath / "CPT.tif";
    typename DistanceImageType::Pointer distanceMap;

    using DistanceValueType = typename DistanceImageType::PixelType;
    using DisplacementImageType = typename itk::Image<itk::Vector<DistanceValueType, DistanceImageType::ImageDimension>,
        DistanceImageType::ImageDimension>;

    typename DisplacementImageType::Pointer spokeField;
    if (!fs::exists(distanceMapFilePath) || (!lowMemory && !fs::exists(spokeFilePath)) ) {
        if (lowMemory){
            logger->Info("Computing distance map without spokes\n");
            distanceMap = computeAstrocyteSignedDistanceMapWithoutSpokes<DistanceImageType>(parser,logger);
            //writeImage<DistanceImageType>(distanceMapFileName, distanceMap);
        }
        else {
            logger->Info("Computing distance map\n");
            auto distClosesPointPair = computeAstrocyteSignedDistanceMap<DistanceImageType>(parser, logger);
            distanceMap = distClosesPointPair.first;
            writeImage<DistanceImageType>(distanceMapFilePath, distanceMap);
            spokeField = distClosesPointPair.second;
            writeImage<DisplacementImageType>(spokeFilePath, spokeField);
        }
    }else{
        if(lowMemory){
            logger->Info("reading already computed distance map..\n");
            distanceMap = readImage<DistanceImageType>(distanceMapFilePath);
        }
        else{
            logger->Info("reading already computed distance map + spokeFile..\n");
            distanceMap = readImage<DistanceImageType>(distanceMapFilePath);
            spokeField = readImage<DisplacementImageType>(spokeFilePath);
        }
    }
    fs::path aofFilePath = outputFolderPath / "aof.tif";
    typename FluxImageType::Pointer aof, aof2;

    if (!fs::exists(aofFilePath) ){
        if(lowMemory){
            aof = computeAOF<DistanceImageType>(parser,logger,lowMemory);
            //writeImage<FluxImageType>(aofFileName, aof);
        }
        else {
            aof = computeAOFFromSpokes<DistanceValueType, DistanceImageType::ImageDimension, DistanceValueType>(
                    spokeField);
            writeImage<FluxImageType>(aofFilePath, aof);
        }
    }else{
        logger->Info("reading already computed m_AOF map..\n");
        aof = readImage<FluxImageType >(aofFilePath);
    }
    compute(parser, distanceMap, aof, logger);

    return EXIT_SUCCESS;
}


std::string helpstring() {
    std::ostringstream ss;
    ss << "Medial Tools\n";
    ss << "$ medial -medialSurface -input <path_to_astrocyte_file>\n\n";
    ss << "Options:: \n";
    ss << "===========================================\n";

    ss << "\t -input\n";
    ss << "\t\t path to input file\n";

    ss << "\t -outputFolder\n";
    ss << "\t\t path of folder to write output images to\n";

    ss << "\t -endpoints\n";
    ss << "\t\t path to file containing endpoints to be fixed for medial surface\n";

    ss << "\t -h, --help\n";
    ss << "\t\t display this help\n";

    ss << "\t -medialSurface       ::(medial Surface computation)\n";
    ss << "\t -medialCurve         ::(medial curve computation)\n";
    ss << "\t -lowMemory           ::(use when system memory is limited)\n";
    //------------------------------------------------------------------------
    ss << "\n\n";
    ss << "Examples:\n";
    ss << "=========\n";
    ss << "Medial Surface example:\n";
    ss << "$./medial -medialSurface -input /home/leonardo/tabish/storage/astroData/astro1/astrocyte.tif\n";
    ss << "------------------------------------------";
    ss << "\n\n";

    std::string helpString(ss.str());
    return helpString;
}

int main(int argc, char* argv[]){
    itk::CommandLineArgumentParser::Pointer parser = itk::CommandLineArgumentParser::New();
    itk::Logger::Pointer logger = itk::Logger::New();
    itk::StdStreamLogOutput::Pointer itkcout = itk::StdStreamLogOutput::New();
    itk::StdStreamLogOutput::Pointer itkfout = itk::StdStreamLogOutput::New();
    itkcout->SetStream(std::cout);
    logger->SetLevelForFlushing(itk::LoggerBaseEnums::PriorityLevel::DEBUG);

#ifdef DEBUG_MODE
    logger->SetPriorityLevel(itk::LoggerBaseEnums::PriorityLevel::DEBUG);
#endif

#ifdef RELEASE_MODE
    logger->SetPriorityLevel(itk::LoggerBaseEnums::PriorityLevel::CRITICAL);
#endif
    logger->AddLogOutput(itkcout);
    std::string humanReadableFormat = "[%b-%d-%Y, %H:%M:%S]";
    logger->SetHumanReadableFormat(humanReadableFormat);
    logger->SetTimeStampFormat(itk::LoggerBaseEnums::TimeStampFormat::HUMANREADABLE);

    // map to list module code, module name.
    std::map<int, std::string> modesMap;
    modesMap[0] = "Medial Surface\n";
    modesMap[1] = "Medial Curve\n";
    modesMap[93] = "Undocumented/Experimental\n";

    std::vector<std::string> modules;
    modules.emplace_back("-medialSurface");
    modules.emplace_back("-experiment");
    modules.emplace_back("-medialCurve");

    parser->SetCommandLineArguments(argc, argv);
    parser->SetProgramHelpText(helpstring());

    if(parser->CheckForRequiredArguments() == itk::CommandLineArgumentParser::HELPREQUESTED){
        return EXIT_SUCCESS;
    }
    if(parser->CheckForRequiredArguments() == itk::CommandLineArgumentParser::FAILED ||
       !parser->ExactlyOneExists(modules)){
        std::cout << parser->GetProgramHelpText() << std::endl;
        return EXIT_FAILURE;
    }
    std::string logFileName="/tmp/medialTools.log";

    if(parser->GetCommandLineArgument("-logfile", logFileName)){
        std::ofstream fileStream(logFileName);
        itkfout->SetStream(fileStream);
        logger->AddLogOutput(itkfout);
    }
    logger->Info( "Starting Medial Tools\n");

    int module = 0;
    if(parser->ArgumentExists("-medialSurface")){
        module = 0;
        logger->Info("Selected : " + modesMap[module] + "\n");
    } else if(parser->ArgumentExists("-medialCurve")) {
        module = 1;
        logger->Info("Selected: " + modesMap[module] + "\n");
    } else if (parser->ArgumentExists("-experiment")) {
        module = 93;
        logger->Info("Selected : " + modesMap[module] + "\n");
    } else {
        logger->Info("Using default : " + modesMap[module] + "\n");
    }

    using DistanceValueType = float;
    using DistanceImageType = itk::Image<DistanceValueType, 3>;
    using FluxImageType = DistanceImageType ;
    using AstrocyteImageType = itk::Image<unsigned char, 3>;
    
    switch(module){
        case 0: // medial Surface Computation
            computeMedialStructure<DistanceImageType, FluxImageType, AstrocyteImageType>(parser, logger, computeMedialSurface<DistanceImageType, FluxImageType, AstrocyteImageType>);
            break;
        case 1: // medial Curve Computation
            computeMedialStructure<DistanceImageType, FluxImageType, AstrocyteImageType>(parser, logger, computeMedialCurve<DistanceImageType, FluxImageType, AstrocyteImageType>);
            break;
        case 93:
            // experiment(parser, logger);
            break;
        default:
            std::cout<<"Unknown Module"<<std::endl;
    }
    return EXIT_SUCCESS;
}

