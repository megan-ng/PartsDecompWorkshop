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
#ifndef MEDIALTOOLS_UTILS_HXX
#define MEDIALTOOLS_UTILS_HXX

#include <itkImageFileWriter.h>
#include <itkImageFileReader.h>

template<typename TImage>
void writeImage(const std::string & filePath, const typename TImage::Pointer &image,unsigned divisions) {
    using FileWriterType = itk::ImageFileWriter<TImage>;
    typename FileWriterType::Pointer writer = FileWriterType::New();
    writer->SetFileName(filePath );
    writer->SetInput( image );
    writer->SetNumberOfStreamDivisions(divisions);
    try
    {
        writer->Update();
        std::cout << "wrote file " << writer->GetFileName() << std::endl;
    }catch (itk::ExceptionObject &e){
        std::cout << e.what() << std::endl;
    }
}

template<typename TImage>
typename TImage::Pointer readImage(const std::string & filePath) {
    using FileReaderType = itk::ImageFileReader<TImage>;
    typename FileReaderType::Pointer reader = FileReaderType::New();
    reader->SetFileName(filePath );
    try
    {
        reader->Update();
        std::cout << "read file " << reader->GetFileName() << std::endl;
    }catch (itk::ExceptionObject &e){
        std::cout << e.what() << std::endl;
    }
    return reader->GetOutput();
}

template<typename TData>
typename itk::Array2D<TData> readCSV(const std::string & datapath) {
        using CSVReaderType = itk::CSVArray2DFileReader<TData>;
        typename CSVReaderType::Pointer csvReader = CSVReaderType::New();

        csvReader->SetFileName(datapath);
        csvReader->SetFieldDelimiterCharacter(',');
        csvReader->HasColumnHeadersOff();
        csvReader->HasRowHeadersOff();
        csvReader->Parse();

        typename CSVReaderType::Array2DDataObjectPointer dataObject = csvReader->GetOutput();
        return dataObject->GetMatrix();
}

#endif //MEDIALTOOLS_UTILS_HXX
