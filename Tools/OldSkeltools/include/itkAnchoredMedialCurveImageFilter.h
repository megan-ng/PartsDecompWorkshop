//**********************************************************
//Copyright 2021 Affan Jilani
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
// Created by affan on 2021-11-25.
// Extension of filter based on implementation from Monica Hernandez && Xavier Mellado in ITK Journal

#ifndef SKELTOOLS_ITKANCHOREDMEDIALCURVEIMAGEFILTER_H
#define SKELTOOLS_ITKANCHOREDMEDIALCURVEIMAGEFILTER_H
#include <ctime>

//STL
#include <functional>
#include <queue>

#include <itkImage.h>
#include <itkNeighborhoodIterator.h>
#include <itkImageRegionIterator.h>
#include <itkConstantBoundaryCondition.h>

#include "itkTopologyPreservingThinningBase.h"
#include "topology.h"

namespace itk
{
    /// \brief Compute medial curve using cbar and cstar definition of simple points
    template<typename TImage, typename TAverageOutwardFluxImage>
    class ITK_TEMPLATE_EXPORT AnchoredMedialCurveImageFilter:public MedialCurveImageFilter<TImage, typename TAverageOutwardFluxImage::PixelType, typename TImage::PixelType>
    {
        public:
            ITK_DISALLOW_COPY_AND_ASSIGN(AnchoredMedialCurveImageFilter);

            /** Standard class type aliases. */
            using Self =  AnchoredMedialCurveImageFilter;
            using Superclass = MedialCurveImageFilter<TImage, typename TAverageOutwardFluxImage::PixelType, typename TImage::PixelType>;
            using Pointer = SmartPointer<Self>;
            using ConstPointer = SmartPointer<const Self>;

            using OutputPointerType = typename TImage::Pointer;
            using BoundaryConditionType = itk::ConstantBoundaryCondition<TImage>;

            /** Method for creation through the object factory. */
            itkNewMacro(Self);

            /** Run-time type information (and related methods). */
            itkTypeMacro(AnchoredMedialCurveImageFilter, MedialCurveImageFilter);

            void SetEndpointLocations(std::vector<typename TImage::IndexType> endpoints)
            {
                this->usePreComputedEndpoints = true;
                this->m_EndpointsList = endpoints;
            }

        protected:
            AnchoredMedialCurveImageFilter() = default;
            ~AnchoredMedialCurveImageFilter() = default;

            bool IsIntSimple(typename TImage::IndexType p) override;
            bool IsExtSimple(typename TImage::IndexType p) override;
            bool IsEnd(typename TImage::IndexType p) override;

            void GenerateData() override;

        private:
            std::vector<typename TImage::IndexType> m_EndpointsList;
            std::unordered_map<typename TImage::OffsetValueType, bool> m_Endpoints;
            bool usePreComputedEndpoints = false;
    };
} //end itk namespace

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkAnchoredMedialCurveImageFilter.hxx"
#endif

#endif //SKELTOOLS_ITKANCHOREDMEDIALCURVEIMAGEFILTER_H
