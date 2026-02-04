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
// Created by tabish on 2021-07-27.
//

#include "medial.h"

template
itk::Image<float,3>::Pointer
computeAstrocyteSignedDistanceMapWithoutSpokes<itk::Image<float,3>>(const itk::CommandLineArgumentParser::Pointer &parser,
                                                                    const itk::Logger::Pointer &logger);

template itk::Image<float,3>::Pointer computeAOF< itk::Image<float,3> >(const itk::CommandLineArgumentParser::Pointer &parser,
                                                                        const itk::Logger::Pointer &logger,bool);

//template itk::Image<float,3>::Pointer computeAOF< itk::Image<float,3> >(itk::Image<float,3>::Pointer signedDistanceFunction,bool);

template itk::Image<float,3>::Pointer
computeAOFFromSpokes<float,3,float>(itk::Image<itk::Vector<float, 3>, 3>::Pointer spokeField);

template
std::pair<itk::Image<float,3>::Pointer, itk::Image<itk::Vector<float,3>, 3>::Pointer>
computeAstrocyteSignedDistanceMap<itk::Image<float,3>>(const itk::CommandLineArgumentParser::Pointer &parser,
                                                       const itk::Logger::Pointer &logger);

template
typename itk::Image<float,3>::Pointer
skeletonToBoundaryMap<itk::Image<float,3>, itk::Image<unsigned char,3>>( itk::Image<float,3>::Pointer weightedSkeleton,
                                                                         itk::Image<unsigned char,3>::Pointer boundary);


void mapWeightedSkeletonToBoundary(const std::string & weightedSkeletonFileName, const std::string & objectFileName){
    constexpr unsigned Dimension = 3;
    using SkeletonImageType = itk::Image<float, Dimension>;
    using BoundaryPixelType = unsigned char;
    using BoundaryImageType =itk::Image<BoundaryPixelType, Dimension>;
    BoundaryImageType::SpacingType spacing;
    spacing[0] = 4.1341146; spacing[1] = 4.1341146; spacing[2] = 8.0;

    typename SkeletonImageType::Pointer skel = readImage<SkeletonImageType>(weightedSkeletonFileName);
    using SkeletonInformationChangeFilterType = itk::ChangeInformationImageFilter< SkeletonImageType >;
    SkeletonInformationChangeFilterType::Pointer skeleton = SkeletonInformationChangeFilterType::New();
    skeleton->SetInput( skel );
    skeleton->SetOutputSpacing( spacing );
    skeleton->ChangeSpacingOn();
    skeleton->Update();

    typename BoundaryImageType::Pointer astro = readImage<BoundaryImageType>(objectFileName);
    using BoundaryInformationChangeFilterType = itk::ChangeInformationImageFilter< BoundaryImageType >;
    BoundaryInformationChangeFilterType::Pointer astrocyte = BoundaryInformationChangeFilterType::New();
    astrocyte->SetInput( astro );
    astrocyte->SetOutputSpacing( spacing );
    astrocyte->ChangeSpacingOn();

    using StructuringElementType = itk::BinaryBallStructuringElement< BoundaryPixelType, Dimension  >;
    using ErodeFilterType = itk::BinaryErodeImageFilter<BoundaryImageType , BoundaryImageType , StructuringElementType >;
    typename ErodeFilterType::Pointer  binaryErode = ErodeFilterType::New();
    StructuringElementType  structuringElement;
    structuringElement.SetRadius( 1 );  // 3x3 structuring element
    structuringElement.CreateStructuringElement();
    binaryErode->SetKernel(  structuringElement );
    binaryErode->SetInput( astrocyte->GetOutput() );
    binaryErode->SetErodeValue(1);
    //binaryErode->Update();

    using SubtractImageFilterType = itk::SubtractImageFilter<BoundaryImageType , BoundaryImageType , BoundaryImageType >;
    SubtractImageFilterType::Pointer boundary = SubtractImageFilterType::New();
    boundary->SetInput1(astrocyte->GetOutput() );
    boundary->SetInput2(binaryErode->GetOutput() );
    boundary->Update();

    typename SkeletonImageType::Pointer weightedBoundary = skeletonToBoundaryMap<SkeletonImageType, BoundaryImageType >(
            skeleton->GetOutput(), boundary->GetOutput());
    std::string outputFileName = fs::path(objectFileName).parent_path() / "weightedBoundary.mha";
    writeImage<SkeletonImageType>(outputFileName, weightedBoundary);
}
