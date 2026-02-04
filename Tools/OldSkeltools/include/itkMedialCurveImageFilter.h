//**********************************************************
//Copyright 2019 Tabish Syed
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
// Created by tabish on 2019-12-05.
// Based on implementation from Monica Hernandez && Xavier Mellado in ITK Journal

#ifndef SKELTOOLS_ITKMEDIALCURVEIMAGEFILTER_H
#define SKELTOOLS_ITKMEDIALCURVEIMAGEFILTER_H
#include <ctime>

//STL
#include <functional>
#include <queue>

#include <itkImage.h>
#include <itkNeighborhoodIterator.h>
#include <itkImageRegionIterator.h>
#include <itkConstantBoundaryCondition.h>

#include "itkTopologyPreservingThinningBase.h"

namespace itk
{

/// \brief Computation of the Medial surface of object object.
/// This class implements the Medial Curve extraction
///	algorithm 3 as described in : "S. Bouix, K. Siddiqi, and A. Tannenbaum. Flux driven automatic centerline extraction."

    template< typename TInputImage,
            typename TAverageOutwardFluxPixelType = float,
            typename TOutputPixelType = unsigned char>
    class ITK_TEMPLATE_EXPORT MedialCurveImageFilter:public TopologyPreservingThinningBase<TInputImage, TAverageOutwardFluxPixelType, TOutputPixelType>
    {
    public:
        ITK_DISALLOW_COPY_AND_ASSIGN(MedialCurveImageFilter);

        /** Standard class type aliases. */
        using Self =  MedialCurveImageFilter;
        using Superclass = TopologyPreservingThinningBase<TInputImage, TAverageOutwardFluxPixelType, TOutputPixelType>;
        using Pointer = SmartPointer<Self>;
        using ConstPointer = SmartPointer<const Self>;

        /** Method for creation through the object factory. */
        itkNewMacro(Self);

        /** Run-time type information (and related methods). */
        itkTypeMacro(MedialCurveImageFilter, TopologyPreservingThinningBase);

        using TAverageOutwardFluxFImage =  ::itk::Image< TAverageOutwardFluxPixelType, TInputImage::ImageDimension>;
        using TOutputImage =  ::itk::Image< TOutputPixelType, TInputImage::ImageDimension >;

        using InputIndexType = typename TInputImage::IndexType ;
        using InputPixelType = typename TInputImage::PixelType ;

        using OutputIndexType = typename TOutputImage::IndexType ;
        using OutputSizeType = typename TOutputImage::SizeType ;

        using InputConstIteratorType = ::itk::ImageRegionConstIterator< TInputImage > ;
        using OutputIteratorType = itk::ImageRegionIterator< TOutputImage > ;
        using InputConstNeighborhoodIteratorType = itk::ConstNeighborhoodIterator< TInputImage > ;
        using AOFConstNeighborhoodIteratorType = itk::ConstNeighborhoodIterator< TAverageOutwardFluxFImage > ;
        using OutputNeighborhoodIteratorType = itk::NeighborhoodIterator< TOutputImage, typename itk::ConstantBoundaryCondition<TOutputImage> >;

        using Pixel = typename Superclass::Pixel;
        using HeapType = typename Superclass::HeapType;
    protected:
        MedialCurveImageFilter() = default;
        ~MedialCurveImageFilter() = default;
        /** Compute the medial curve. */
        void GenerateData() override ;

        bool IsEnd(OutputIndexType p) override;


        void PrintSelf(std::ostream& os, Indent indent) const;

    };


}//end itk namespace

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkMedialCurveImageFilter.hxx"
#endif

#endif //SKELTOOLS_ITKMEDIALCURVEIMAGEFILTER_H
