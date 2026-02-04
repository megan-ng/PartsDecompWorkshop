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
// Based on implementation from Monica Hernandez && Xavier Mellado in ITK Journal

#ifndef SKELTOOLS_ITKTOPOLOGYPRESERVINGTHINNING_H
#define SKELTOOLS_ITKTOPOLOGYPRESERVINGTHINNING_H
#include <ctime>

//STL
#include <functional>
#include <queue>

#include <itkImage.h>

#include <itkConstantBoundaryCondition.h>

#include <itkImageToImageFilter.h>

namespace itk
{

/// \brief Computation of the Medial curve of a tube like object.
///
/// The object is represented as a binary image. A topological prunning is performed guided
/// by the m_Distance transform to the object. The prunning stops when all voxels that are end points
/// are not simple. The average outward flux imposes an extra criterion for deletion. This class implements the centerline extraction
///	algorithm described in : "S. Bouix, K. Siddiqi, and A. Tannenbaum. Flux driven automatic centerline extraction. Technical Report
///	SOCS-04.2, School of Ccomputer Science, McGill University, 2004."

/// TODO:
/// 1. manual instantation
    template< typename TInputImage,
            typename TAverageOutwardFluxPixelType = float,
            typename TOutputPixelType = unsigned char>
    class ITK_TEMPLATE_EXPORT TopologyPreservingThinningBase:
            public ImageToImageFilter<TInputImage, ::itk::Image<TOutputPixelType,TInputImage::ImageDimension> >
    {
    public:
        ITK_DISALLOW_COPY_AND_ASSIGN(TopologyPreservingThinningBase);

        using TAverageOutwardFluxFImage =  ::itk::Image< TAverageOutwardFluxPixelType, TInputImage::ImageDimension>;
        using TOutputImage =  ::itk::Image< TOutputPixelType, TInputImage::ImageDimension >;

        using Self =  TopologyPreservingThinningBase;
        using Superclass = ImageToImageFilter<TInputImage,TOutputImage>;
        using Pointer = SmartPointer<Self>;
        using ConstPointer = SmartPointer<const Self>;



        itkTypeMacro(TopologyPreservingThinningBase, ImageToImageFilter );

        using OutputBCType = itk::ConstantBoundaryCondition< TOutputImage > ;

        using InputConstPointerType = typename TInputImage::ConstPointer ;
        using AOFConstPointerType = typename TAverageOutwardFluxFImage::ConstPointer ;
        using OutputPointerType = typename TOutputImage::Pointer ;

        using OutputIndexType = typename TOutputImage::IndexType ;
        using OutputSizeType = typename TOutputImage::SizeType ;
        using OutputRegionType = typename TOutputImage::RegionType ;
        using OutputPixelType = typename TOutputImage::PixelType ;

        using InputIndexType = typename TInputImage::IndexType ;
        using InputPixelType = typename TInputImage::PixelType ;

        using InputConstIteratorType = ::itk::ImageRegionConstIterator< TInputImage > ;
        using OutputIteratorType = itk::ImageRegionIterator< TOutputImage > ;
        using InputConstNeighborhoodIteratorType = itk::ConstNeighborhoodIterator< TInputImage > ;
        using AOFConstNeighborhoodIteratorType = itk::ConstNeighborhoodIterator< TAverageOutwardFluxFImage > ;
        using OutputNeighborhoodIteratorType = itk::NeighborhoodIterator< TOutputImage >;

        typedef
        struct Pixel
        {
        private:
            InputIndexType pixelIndex;
            InputPixelType pixelValue;

        public:
            InputPixelType GetPriority() const {return pixelValue;};
            void SetIndex(InputIndexType p) { pixelIndex=p;};
            void SetValue(InputPixelType v) {pixelValue=v;};
            InputIndexType GetIndex() {return pixelIndex;};
            InputPixelType GetValue() {return pixelValue;};
        }Pixel;

        struct Greater:public std::binary_function<Pixel, Pixel, bool>
        {
        public:
            bool operator()(const Pixel &p1, const Pixel &p2)  const
            {
                return p1.GetPriority() > p2.GetPriority();
            }
        };

        using HeapContainer = std::vector<Pixel> ;
        using HeapType = std::priority_queue<Pixel, HeapContainer, Greater > ;

        /** The dimension of the input and output images. */
        itkStaticConstMacro(InputImageDimension, unsigned int,
                            TInputImage::ImageDimension);
        itkStaticConstMacro(OutputImageDimension, unsigned int,
                            TOutputImage::ImageDimension);



        virtual void SetAverageOutwardFluxImage(AOFConstPointerType aofImage)
        {
            this->ProcessObject::SetNthInput( 1, const_cast< TAverageOutwardFluxFImage * >(aofImage.GetPointer()) );
        }

        virtual AOFConstPointerType GetAverageOutwardFluxImage()
        {
            return ( static_cast< TAverageOutwardFluxFImage *>(this->ProcessObject::GetInput(1)) );
        }

        /** Set the AOF threshold. */
        itkSetMacro( Threshold, double );

        /** Get the AOF threshold . */
        itkGetConstReferenceMacro( Threshold, double );

#ifdef ITK_USE_CONCEPT_CHECKING
        /** Begin concept checking */
        itkConceptMacro(SameDimensionCheck,
                        (Concept::SameDimension<InputImageDimension, OutputImageDimension>));
        itkConceptMacro(AOFIsFloatingPointCheck,
                        (Concept::IsFloatingPoint<TAverageOutwardFluxPixelType>));
        /** End concept checking */
#endif

    protected:
        TopologyPreservingThinningBase() = default;
        ~TopologyPreservingThinningBase() = default;
        ///\brief Returns true if p belongs to the object and at least one of its 27 neighbors
        /// belong to the background.
        bool IsBoundary( OutputIndexType p );

        ///\brief Returns true if its deletion from the object changes the local object topology
        /// in the 27 neighborhood.
        virtual bool IsIntSimple( OutputIndexType p );

        ///\brief Returns true if its deletion from the object changes the local background topology
        /// in the 18 neighborhood.
        virtual bool IsExtSimple( OutputIndexType p );

        ///\brief Returns true if the point has less than two object neighbors.
        virtual bool IsEnd( OutputIndexType p );

        ///\brief Computes the binary object from its  signed m_Distance transform representation.
        void DistanceToObject(bool insideNegative = true);

        ///\brief Computes the binary object from its  signed m_Distance transform representation.
        void FluxToSkeleton(bool insideNegative = true);
        //virtual void GenerateData();

        InputConstPointerType m_Distance; // Implicit representation of the object. Is at input 0.
        AOFConstPointerType m_Aof;        // Average outward flux. Is at input 1.
        double m_Threshold = -(60*2/3.14159)*0.4;             // Threshold for the average outward flux.
        OutputPointerType m_Queued;       // Image that stores queued labels in Update().
        OutputPointerType m_AuxQueued;    // Image that stores queued labels in IsIntSimple() and IsExtSimple().
        OutputPointerType m_Skeleton;     // Skeleton.
        OutputRegionType m_Region;
    };

}//end itk namespace

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkTopologyPreservingThinningBase.hxx"
#endif

#endif //SKELTOOLS_ITKTOPOLOGYPRESERVINGTHINNING_H
