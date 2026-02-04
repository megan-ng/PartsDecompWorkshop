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

#ifndef SKELTOOLS_MEDIAL_H
#define SKELTOOLS_MEDIAL_H

#include <string>
#include <experimental/filesystem>

#include <itkImage.h>
#include <itkVector.h>
#include <itkChangeInformationImageFilter.h>
#include <itkImageFileReader.h>
#include <itkSignedMaurerDistanceMapImageFilter.h>
#include <itkDiscreteGaussianImageFilter.h>
#include <itkGradientImageFilter.h>
#include <itkAverageOutwardFluxImageFilter.h>
#include <itkAverageOutwardFluxImageFilter2.h>
#include <itkAverageOutwardFluxImageFilter3.h>
#include <itkMedialCurveImageFilter.h>
#include <itkAnchoredMedialCurveImageFilter.h>
#include <itkImage.h>
#include <itkSignedDanielssonDistanceMapImageFilter.h>
#include <itkMultiplyImageFilter.h>
#include <itkListSample.h>
#include <itkKdTreeGenerator.h>
#include <itkBinaryErodeImageFilter.h>
#include <itkPostProcessSkeleton.h>
#include <itkMinimumMaximumImageCalculator.h>
#include <itkWeightedCentroidKdTreeGenerator.h>
#include <itkKdTreeBasedKmeansEstimator.h>
#include <itkMinimumDecisionRule.h>
#include <itkSampleClassifierFilter.h>
#include <itkLogger.h>
#include <itkCastImageFilter.h>

#include "itkCommandLineArgumentParser.h"
#include "utils.h"

namespace fs = std::experimental::filesystem;


//int clusterObjectPoints(const std::string & objectFileName);

template<typename DistanceImageType, typename FluxImageType, typename OutputImageType>
typename OutputImageType::Pointer
computeMedialCurve(const itk::CommandLineArgumentParser::Pointer &parser,
                   const typename DistanceImageType::Pointer &distanceMap, 
                   const typename FluxImageType::Pointer &aof,
                   const itk::Logger::Pointer &logger);

template<typename DistanceImageType, typename FluxImageType, typename OutputImageType>
typename OutputImageType::Pointer
computeMedialSurface(const itk::CommandLineArgumentParser::Pointer &parser,
                     const typename DistanceImageType::Pointer &distanceMap, 
                     const typename FluxImageType::Pointer &aof,
                     const itk::Logger::Pointer &logger);

void mapWeightedSkeletonToBoundary(const std::string & weightedSkeletonFileName, const std::string & objectFileName);

template<typename SkeletonImageType, typename BoundaryImageType>
typename SkeletonImageType::Pointer
skeletonToBoundaryMap(typename SkeletonImageType::Pointer weightedSkeleton, typename BoundaryImageType::Pointer boundary);

template<class TDistanceImage>
std::pair< typename TDistanceImage::Pointer,
        typename itk::Image<itk::Vector<float,TDistanceImage::ImageDimension>,TDistanceImage::ImageDimension>::Pointer>
computeAstrocyteSignedDistanceMap(const itk::CommandLineArgumentParser::Pointer &parser,
                                  const itk::Logger::Pointer &logger);

template<class TDistanceImage>
typename TDistanceImage::Pointer
computeAstrocyteSignedDistanceMapWithoutSpokes(const itk::CommandLineArgumentParser::Pointer &parser,
                                               const itk::Logger::Pointer &logger);

template<class TDistanceImage>
typename TDistanceImage::Pointer computeAOF(typename TDistanceImage::Pointer signedDistanceFunction, bool lowMemory=false);

template<class TDistanceImage>
typename TDistanceImage::Pointer computeAOF(const itk::CommandLineArgumentParser::Pointer &parser,
                                            const itk::Logger::Pointer &logger, bool lowMemory);

template<typename TInputValueType,
        unsigned int VImageDimension,
        typename TOutputValueType> //use std::conditional here..
typename itk::Image<TOutputValueType,VImageDimension>::Pointer
computeAOFFromSpokes(typename itk::Image<itk::Vector<TInputValueType, VImageDimension>, VImageDimension >::Pointer spokeField);

#include "medial.hxx"
#endif //SKELTOOLS_MEDIAL_H
