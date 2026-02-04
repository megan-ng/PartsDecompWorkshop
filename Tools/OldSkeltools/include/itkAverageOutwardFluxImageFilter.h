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
//

#ifndef _itkAverageOutwardFluxImageFilter_h
#define _itkAverageOutwardFluxImageFilter_h

#include <time.h>

#include <itkImage.h>
#include <itkNeighborhoodIterator.h>
#include <itkImageRegionIterator.h>

#include <itkImageToImageFilter.h>
#include <vector>
#include <random>


//using namespace std;

namespace itk
{

/// \brief This class computes the average outward flux associated to an object implicitly 
/// represented as a m_Distance transform. This class implements Average outward flux
///  from "S. Bouix, K. Siddiqi, and A. Tannenbaum. Flux driven automatic centerline extraction.

/// TODO:
/// 1. manual instantation
template< class TInputImage,
	  class TOutputPixelType = float,
	  class TInputVectorPixelType = ::itk::CovariantVector<TOutputPixelType,TInputImage::ImageDimension> >
    class ITK_TEMPLATE_EXPORT  AverageOutwardFluxImageFilter:
	public ImageToImageFilter<TInputImage, ::itk::Image<TOutputPixelType,TInputImage::ImageDimension> >
{
	public:
        /** Standard class typedefs. */
        using Self = AverageOutwardFluxImageFilter;
        using TOutputImage = itk::Image< TOutputPixelType, TInputImage::ImageDimension >;
        using Superclass = ImageToImageFilter<TInputImage, TOutputImage>;
        using Pointer = SmartPointer<Self>;
        using ConstPointer = SmartPointer<const Self>;


		using TInputVectorImage =  itk::Image< TInputVectorPixelType, TInputImage::ImageDimension >;



		/** Method for creation through the object factory */
		itkNewMacro(Self);

		/** Run-time type information (and related methods). */
		itkTypeMacro( AverageOutwardFluxImageFilter, ImageToImageFilter );

		using InputConstPointerType = typename TInputImage::ConstPointer;
		using  InputVectorConstPointerType =  typename TInputVectorImage::ConstPointer;
		using  OutputPointerType = typename TOutputImage::Pointer;

		using InputConstIteratorType = itk::ImageRegionConstIterator< TInputImage >;
		using InputVectorConstNeighborhoodIteratorType = itk::ConstNeighborhoodIterator< TInputVectorImage >;
		using OutputIteratorType = itk::ImageRegionIterator< TOutputImage >;

        using NormalType = itk::Vector<double, TInputImage::ImageDimension>;
        using OutputImageRegionType = typename Superclass::OutputImageRegionType;


		virtual void SetGradientImage(InputVectorConstPointerType gradient)
		{
			this->ProcessObject::SetNthInput( 1, const_cast< TInputVectorImage * >(gradient.GetPointer()) );
		}

		virtual InputVectorConstPointerType GetGradientImage()
		{
			return ( static_cast< TInputVectorImage *>(this->ProcessObject::GetInput(1)) );
		}

	protected:
        AverageOutwardFluxImageFilter();
        ~AverageOutwardFluxImageFilter() = default;

        /// \brief Compute the AOF.
        void GenerateData() override;

        void PrintSelf(std::ostream& os, Indent indent) const;

		InputConstPointerType m_Distance;
		InputVectorConstPointerType m_Gradient;
		OutputPointerType m_AOF;
private:
		bool m_ObjectIsNegative;
        void NormalsToASphere();
        std::vector< itk::Vector<double, TInputImage::ImageDimension> > m_Points;
};

#include "itkAverageOutwardFluxImageFilter.hxx"

} // end namespace itk
#endif // _itkAverageOutwardFluxImageFilter_h
