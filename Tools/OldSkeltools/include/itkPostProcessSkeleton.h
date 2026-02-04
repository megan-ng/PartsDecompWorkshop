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
// Created by tabish on 2020-02-04.
//

#ifndef SKELTOOLS_ITKPOSTPROCESSSKELETON_H
#define SKELTOOLS_ITKPOSTPROCESSSKELETON_H

#include <itkImageToImageFilter.h>
#include <itkImageRegionConstIterator.h>
#include <itkImageRegionIterator.h>
#include <itkConstantBoundaryCondition.h>

#include <queue>

namespace itk
{
    /// 1. manual instantation
    template< class TInputImage>
    class ITK_TEMPLATE_EXPORT  PostProcessSkeleton:
    public ImageToImageFilter<TInputImage, TInputImage >
{
    public:
    /** Standard class typedefs. */
    using Self = PostProcessSkeleton;
    using Superclass = ImageToImageFilter<TInputImage, TInputImage>;
    using Pointer = SmartPointer<Self>;
    using ConstPointer = SmartPointer<const Self>;



    /** Method for creation through the object factory */
    itkNewMacro(Self);

    /** Run-time type information (and related methods). */
    itkTypeMacro( PostProcessSkeleton, ImageToImageFilter );

    using InputPointerType = typename TInputImage::ConstPointer;
    using OutputPointerType = typename TInputImage::Pointer;

    using InputConstIteratorType = itk::ImageRegionConstIterator< TInputImage >;
    using OutputIteratorType = itk::ImageRegionIterator< TInputImage >;
    using IndexType = typename TInputImage::IndexType;
    using BoundaryConditionType = itk::ConstantBoundaryCondition<TInputImage>;

    protected:
        PostProcessSkeleton();
        ~PostProcessSkeleton() = default;


        void GenerateData() override;

        void PrintSelf(std::ostream& os, Indent indent) const;

    private:
        unsigned TopologicalLabel(IndexType index);
        unsigned ForegroudLabelling(IndexType& index);
        unsigned BackgroundLabelling(IndexType& index);

        std::vector<itk::Offset<3>> m_Neighbors26, m_Neighbors18;
        std::vector<std::vector<size_t>> m_Graph26, m_Graph18;
        std::vector<bool> m_n6;
        OutputPointerType m_Output;
        BoundaryConditionType m_Accessor;

    };

#include "itkPostProcessSkeleton.hxx"

} // end namespace itk

#endif //SKELTOOLS_ITKPOSTPROCESSSKELETON_H
