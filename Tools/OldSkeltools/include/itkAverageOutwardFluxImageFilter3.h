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
// Created by tabish on 2019-02-05.
//

#ifndef _itkAverageOutwardFluxImageFilter3_h
#define _itkAverageOutwardFluxImageFilter3_h

#include <time.h>

#include <itkImage.h>
#include <itkNeighborhoodIterator.h>
#include <itkImageRegionConstIterator.h>
#include <vector>
#include <random>
#include <cmath>

//using namespace std;

namespace itk
{

/// \brief This class computes the average outward flux associated to an object From its
/// Gradient Field. Without using distance transform as a mask. so aof is calculated both
/// inside and outside the object.

/// TODO:
/// 1. manual instantation
template< class TInputImage, //::itk::Image<::itk::CovariantVector<TOutputPixelType,Dimension>, Dimension>
	      class TOutputPixelType = float>
    class ITK_TEMPLATE_EXPORT  AverageOutwardFluxImageFilter3:
	public ImageToImageFilter<TInputImage, ::itk::Image<TOutputPixelType,TInputImage::ImageDimension> >
{
	public:
        using OutputImageType = itk::Image< TOutputPixelType, TInputImage::ImageDimension >;

        /** Standard class typedefs. */
        using Self = AverageOutwardFluxImageFilter3;
        using Superclass = ImageToImageFilter<TInputImage, OutputImageType>;
        using Pointer = SmartPointer<Self>;
        using ConstPointer = SmartPointer<const Self>;

        static const SizeValueType SphereRadius = 1;

		/** Method for creation through the object factory */
		itkNewMacro(Self);

		/** Run-time type information (and related methods). */
		itkTypeMacro( AverageOutwardFluxImageFilter3, ImageToImageFilter );

		using InputConstPointerType = typename TInputImage::ConstPointer;
        using InputPointerType = typename TInputImage::Pointer;
        using OutputPointerType = typename OutputImageType::Pointer;

        using InputPixelType = typename TInputImage::PixelType;

		using InputConstIndexIteratorType = itk::ImageRegionConstIteratorWithIndex< TInputImage >;
		using InputConstNeighborhoodIteratorType = itk::ConstNeighborhoodIterator< TInputImage >;
		using OutputIteratorType = itk::ImageRegionIterator< OutputImageType >;

        using OutputImageRegionType = typename Superclass::OutputImageRegionType;

        using NormalType = itk::Vector<double, TInputImage::ImageDimension>;

	protected:
        AverageOutwardFluxImageFilter3();
        ~AverageOutwardFluxImageFilter3() = default;

        /// \brief Compute the AOF.
        void DynamicThreadedGenerateData(const OutputImageRegionType & outputRegionForThread) override;

        void GenerateOutputInformation() override;

        void GenerateInputRequestedRegion() override;

        void PrintSelf(std::ostream& os, Indent indent) const;

private:
        void NormalsToASphere();
        std::vector< itk::Vector<double, TInputImage::ImageDimension> > m_Points;
};
} // end namespace itk
#ifndef ITK_MANUAL_INSTANTIATION
#include "itkAverageOutwardFluxImageFilter3.hxx"
#endif

#endif // _itkAverageOutwardFluxImageFilter2_h
