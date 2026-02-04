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

#ifndef MEDIALTOOLS_UTILS_H
#define MEDIALTOOLS_UTILS_H
#include <string>
#include <sys/time.h>
#include <itkArray2D.h>
#include <itkCSVArray2DFileReader.h>

template<typename TImage>
void writeImage(const std::string & filePath, const typename TImage::Pointer &image, unsigned divisions=1);

template<typename TImage>
typename TImage::Pointer readImage(const std::string & filePath);

template<typename TData>
typename itk::Array2D<TData> readCSV(const std::string & datapath);

#include "utils.hxx"
#endif //MEDIALTOOLS_UTILS_H
