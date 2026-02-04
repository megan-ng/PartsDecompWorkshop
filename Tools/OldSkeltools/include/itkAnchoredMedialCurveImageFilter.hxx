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
//
// Based on implementation from Monica Hernandez && Xavier Mellado in ITK Journal


#ifndef SKELTOOLS_ITKANCHOREDMEDIALCURVEIMAGEFILTER_HXX
#define SKELTOOLS_ITKANCHOREDMEDIALCURVEIMAGEFILTER_HXX

#include "itkAnchoredMedialCurveImageFilter.h"

namespace itk{

    template<typename TImage, typename TAverageOutwardFluxImage>
    void AnchoredMedialCurveImageFilter<TImage, TAverageOutwardFluxImage>::GenerateData()
    {
        for(auto endpointIndex: this->m_EndpointsList)
        {
            this->m_Endpoints[this->GetInput()->ComputeOffset(endpointIndex)] = true;
        }
        Superclass::GenerateData();
    }

    template<typename TImage, typename TAverageOutwardFluxImage>
    bool AnchoredMedialCurveImageFilter<TImage, TAverageOutwardFluxImage>::IsExtSimple(typename TImage::IndexType p)
    {
        return topology::computeCbar<TImage>(this->m_Skeleton, p) == 1;
    }

    template<typename TImage, typename TAverageOutwardFluxImage>
    bool AnchoredMedialCurveImageFilter<TImage, TAverageOutwardFluxImage>::IsIntSimple(typename TImage::IndexType p)
    {
        return topology::computeCstar<TImage>(this->m_Skeleton, p) == 1;
    }

    template<typename TImage, typename TAverageOutwardFluxImage>
    bool AnchoredMedialCurveImageFilter<TImage, TAverageOutwardFluxImage>::IsEnd(typename TImage::IndexType p)
    {
        if(!this->usePreComputedEndpoints)
        {
            return Superclass::IsEnd(p);
        }

        return this->m_Endpoints.find(this->m_Distance->ComputeOffset(p)) != this->m_Endpoints.end();
    }
}
#endif //SKELTOOLS_ITKANCHOREDMEDIALCURVEIMAGEFILTER_HXX
