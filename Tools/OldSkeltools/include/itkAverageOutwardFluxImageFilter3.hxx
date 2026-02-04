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
// Created by tabish on 2019-01-30.
//

#ifndef _itkAverageOutwardFluxImageFilter3_hxx
#define _itkAverageOutwardFluxImageFilter3_hxx

#include "itkAverageOutwardFluxImageFilter3.h"
namespace itk {

    template<class TInputImage, class TOutputPixelType>
    AverageOutwardFluxImageFilter3<TInputImage, TOutputPixelType>::AverageOutwardFluxImageFilter3() {
        this->DynamicMultiThreadingOn();
        this->NormalsToASphere();
    }

    template<class TInputImage, class TOutputPixelType>
    void
    AverageOutwardFluxImageFilter3<TInputImage, TOutputPixelType>::GenerateOutputInformation() {
        // this methods is overloaded so that if the output image is a
        // Scalar Image with the correct number of components are set.

        Superclass::GenerateOutputInformation();
        OutputImageType *output = this->GetOutput();

        if (!output) {
            return;
        }
        if (output->GetNumberOfComponentsPerPixel() != 1) {
            output->SetNumberOfComponentsPerPixel(1);
        }
    }

    template<class TInputImage, class TOutputPixelType>
    void
    AverageOutwardFluxImageFilter3<TInputImage, TOutputPixelType>::GenerateInputRequestedRegion() {
        // call the superclass' implementation of this method
        Superclass::GenerateInputRequestedRegion();

        // get pointers to the input and output
        InputPointerType inputPtr = const_cast< TInputImage * >( this->GetInput());
        OutputPointerType outputPtr = this->GetOutput();

        if (!inputPtr || !outputPtr) {
            return;
        }

        // get a copy of the input requested region (should equal the output
        // requested region)
        typename TInputImage::RegionType inputRequestedRegion;
        inputRequestedRegion = inputPtr->GetRequestedRegion();

        // pad the input requested region by the radius of sphere used to
        // compute the flux.
        inputRequestedRegion.PadByRadius(this->SphereRadius);

        // crop the input requested region at the input's largest possible region
        if (inputRequestedRegion.Crop(inputPtr->GetLargestPossibleRegion())) {
            inputPtr->SetRequestedRegion(inputRequestedRegion);
            return;
        } else {
            // Couldn't crop the region (requested region is outside the largest
            // possible region).  Throw an exception.

            // store what we tried to request (prior to trying to crop)
            inputPtr->SetRequestedRegion(inputRequestedRegion);

            // build an exception
            InvalidRequestedRegionError e(__FILE__, __LINE__);
            e.SetLocation(ITK_LOCATION);
            e.SetDescription("Requested region is (at least partially) outside the largest possible region.");
            e.SetDataObject(inputPtr);
            throw e;
        }
    }



    template<class TInputImage, class TOutputPixelType>
    void
    AverageOutwardFluxImageFilter3<TInputImage, TOutputPixelType>::NormalsToASphere() {
        ::std::random_device rd;  //Will be used to obtain a seed for the random number engine
        ::std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
        ::std::uniform_real_distribution<double> dis(0.0, 1.0);
        constexpr unsigned Dimension = TInputImage::ImageDimension;
        constexpr unsigned N = 60;
        ::std::vector<NormalType> forces;
        NormalType n;
        for (size_t d = 0; d < Dimension; ++d) n[d] = 0;
        forces.push_back(n);
        n[0] = 1.0;
        this->m_Points.push_back(n);
        for (size_t count = 0; count < N - 1; ++count) {
            for (size_t d = 0; d < Dimension; ++d) {
                n[d] = dis(gen);
            }
            n.Normalize();
            //hope that no two point are the same.
            this->m_Points.push_back(n);
            forces.push_back(0.0 * n);
        }

        double r;
        NormalType forceVector;
        int iteration = 0;
        do {
            for (size_t i = 1; i < N; ++i) {
                forces[i] *= 0.0;                        // Initialize force vector zero
                for (size_t j = 0; j < N; ++j) {
                    if (i == j) continue;
                    forceVector = this->m_Points[i] - this->m_Points[j];
                    r = forceVector.GetNorm();
                    r *= r;
                    forceVector /= r;
                    forces[i] += forceVector; // Get contributing forces from other particles.
                }
            }
            for (size_t i = 1; i < N; ++i) {
                //move point on sphere.
                this->m_Points[i] += forces[i];
                this->m_Points[i].Normalize();
            }
        } while (++iteration < 50);
        itkDebugMacro("Finished Computing normals to sphere.");
    }


    template<class TInputImage, class TOutputPixelType>
    void
    AverageOutwardFluxImageFilter3<TInputImage, TOutputPixelType>::DynamicThreadedGenerateData(
            const OutputImageRegionType &outputRegionForThread) {
        typename OutputImageType::Pointer output = this->GetOutput();
        typename TInputImage::ConstPointer input = this->GetInput();

        OutputIteratorType aofIt = OutputIteratorType(output, outputRegionForThread);
        using BoundaryConditionType = itk::ZeroFluxNeumannBoundaryCondition<TInputImage>;
        BoundaryConditionType accessor;
        double f = 0,inverseNorm;
        typename TInputImage::IndexType currentIndex, gradientIndex;
        for (aofIt.GoToBegin(); !aofIt.IsAtEnd(); ++aofIt) {
            f = 0.0;
            currentIndex = aofIt.GetIndex();
            for (auto point: this->m_Points) {
                //get gradient vector.
                for (size_t d = 0; d < TInputImage::ImageDimension; ++d) {
                    gradientIndex[d] = std::floor((static_cast<double>(currentIndex[d]) + point[d] + 0.5));
                }
                InputPixelType gradientVector = accessor.GetPixel(gradientIndex, input);
                inverseNorm = 0.0;
                for (size_t d = 0; d < TInputImage::ImageDimension; ++d) {
                    inverseNorm += (gradientVector[d] * gradientVector[d]);
                }
                inverseNorm = sqrt(inverseNorm);
                inverseNorm = (inverseNorm > 0) ? 1 / inverseNorm : 0;
                //compute dot product of normalized vectors
                for (size_t d = 0; d < TInputImage::ImageDimension; ++d) {
                    f -= (gradientVector[d] * inverseNorm * point[d]);
                }
            }

            aofIt.Set(f);
        }
    }


/**
*  Print Self
*/
    template<class TInputImage, class TOutputPixelType>
    void
    AverageOutwardFluxImageFilter3<TInputImage, TOutputPixelType>::PrintSelf(std::ostream &os, Indent indent) const {
        Superclass::PrintSelf(os, indent);
        os << indent << "AverageOutwardFluxImageFilter3." << std::endl;
    }

}

#endif
