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

template< class TInputImage, class TOutputPixelType, class TInputVectorPixelType>
AverageOutwardFluxImageFilter<TInputImage, TOutputPixelType, TInputVectorPixelType>::AverageOutwardFluxImageFilter(){
    this->m_ObjectIsNegative = true;
    this->NormalsToASphere();
    this->DynamicMultiThreadingOn();
}

template< class TInputImage, class TOutputPixelType, class TInputVectorPixelType>
void
AverageOutwardFluxImageFilter<TInputImage, TOutputPixelType, TInputVectorPixelType>::NormalsToASphere(){
    std::random_device rd;  //Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    std::uniform_real_distribution<double > dis(0.0, 1.0);
    constexpr unsigned Dimension = TInputImage::ImageDimension;
    constexpr unsigned N = 60;
    ::std::vector< NormalType > forces;
    NormalType n;
    for(size_t d = 0; d <Dimension;  ++d) n[d] = 0;
    forces.push_back(n);
    n[0] = 1.0;
    this->m_Points.push_back(n);
    for(size_t count = 0; count < N-1; ++count){
        for(size_t d = 0; d < Dimension; ++d) {
            n[d] = dis(gen);
        }
        n.Normalize();
        //hope that no two point are the same.
        this->m_Points.push_back(n);
        forces.push_back(0.0*n);
    }

    double r;
    NormalType forceVector;
    int iteration = 0;
    do {
        for(size_t i=1;i<N;++i) {
            forces[i] *= 0.0;                        // Initialize force vector zero
            for(size_t j = 0; j<N ;++j){
                if (i == j) continue;
                forceVector = this->m_Points[i] - this->m_Points[j];
                r = forceVector.GetNorm();
                r *=r;
                forceVector /= r;
                forces[i] += forceVector; // Get contributing forces from other particles.
            }
        }
        for(size_t i=1;i<N;++i) {
            //move point on sphere.
            this->m_Points[i] += forces[i];
            this->m_Points[i].Normalize();
        }
    } while(++iteration<50);
    itkDebugMacro("Finished Computing normals to sphere.");
}

/*
template< class TInputImage, class TOutputPixelType, class TInputVectorPixelType>
void
AverageOutwardFluxImageFilter<TInputImage, TOutputPixelType, TInputVectorPixelType>::GenerateData() {
    this->m_Distance = dynamic_cast<const TInputImage *>( ProcessObject::GetInput(0));
    this->m_Gradient = dynamic_cast<const TInputVectorImage *>( ProcessObject::GetInput(1));

    this->m_AOF = dynamic_cast< TOutputImage * >(  this->ProcessObject::GetOutput(0));

    // Initialize output image
    this->m_AOF->SetSpacing(this->m_Distance->GetSpacing());
    this->m_AOF->SetOrigin(this->m_Distance->GetOrigin());
    this->m_AOF->SetRegions(this->m_Distance->GetRequestedRegion());
    this->m_AOF->Allocate();

    // AOF and neighborhood iterators
    InputConstIteratorType dit = InputConstIteratorType(this->m_Distance, this->m_Distance->GetRequestedRegion());
    OutputIteratorType aofit = OutputIteratorType(this->m_AOF, this->m_AOF->GetRequestedRegion());

    typename InputVectorConstNeighborhoodIteratorType::RadiusType radius;
    radius.Fill(1);
    InputVectorConstNeighborhoodIteratorType nit(radius, this->m_Gradient, this->m_Gradient->GetRequestedRegion());

    // Computation of the average outward flux inside the object
    double f = 0, norm = 0, objectThreshold = 8.5;
    int sign = this->m_ObjectIsNegative ? -1 : 1;

    vector<double> n(TInputImage::ImageDimension);

    for (nit.GoToBegin(), dit.GoToBegin(), aofit.GoToBegin(); !nit.IsAtEnd(); ++nit, ++dit, ++aofit) {
        f = 0.0;
        // if (object is negative) compute for m_Distance < -objectThreshold else m_Distance > objectThreshold
        // => -m_Distance > objectThreshold else m_Distance > objectThreshold.
        if (sign * dit.Get() >  objectThreshold) {
            int aofdirections = 0;
            //nit_size =  3.0^(TInputImage::ImageDimension)
            for (int i = 0; i < nit.Size(); ++i) {
                if (nit.GetIndex() != nit.GetIndex(i)) {
                    norm = 0.0;
                    for (unsigned char d = 0; d < n.size(); d++) {
                        // Compute the normal of the sphere centered in p in the i-th 27 neighborhood
                        n[d] = nit.GetIndex(i)[d] - nit.GetIndex()[d];
                    }
                    for (unsigned char d = 0; d < n.size(); d++) {
                        norm += n[d] * n[d];
                    }
                    norm = sqrt(norm);

                    for (unsigned char d = 0; d < n.size(); d++) {
                        n[d] = n[d] / norm;
                    }

                    // Compute dot product with outward normal.
                    for (unsigned char d = 0; d < n.size(); d++) {
                        f += sign * nit.GetPixel(i)[d] * n[d];
                    }
                    aofdirections++;
                }
            }
            f /= (aofdirections > 0) ? aofdirections : 1;
        }
        aofit.Set(f);
    }
}
*/


template< class TInputImage, class TOutputPixelType, class TInputVectorPixelType>
void
AverageOutwardFluxImageFilter<TInputImage, TOutputPixelType, TInputVectorPixelType>::GenerateData() {
    this->m_Distance = dynamic_cast<const TInputImage *>( ProcessObject::GetInput(0));
    this->m_Gradient = dynamic_cast<const TInputVectorImage *>( ProcessObject::GetInput(1));

    this->m_AOF = dynamic_cast< TOutputImage * >(  this->ProcessObject::GetOutput(0));

    // Initialize output image
    this->m_AOF->SetSpacing(this->m_Distance->GetSpacing());
    this->m_AOF->SetOrigin(this->m_Distance->GetOrigin());
    this->m_AOF->SetRegions(this->m_Distance->GetRequestedRegion());
    this->m_AOF->Allocate();
    // AOF and neighborhood iterators
    InputConstIteratorType dit = InputConstIteratorType(this->m_Distance, this->m_Distance->GetRequestedRegion());
    OutputIteratorType aofit = OutputIteratorType(this->m_AOF, this->m_AOF->GetRequestedRegion());


    // Computation of the average outward flux inside the object
    double f = 0, objectThreshold = 8.5,inverseNorm;
    int sign = this->m_ObjectIsNegative ? -1 : 1;

    using BoundaryConditionType = itk::ZeroFluxNeumannBoundaryCondition<TInputVectorImage>;
    BoundaryConditionType accessor;
    typename TInputImage::IndexType currentIndex,gradientIndex;
    for (aofit.GoToBegin(), dit.GoToBegin(); !aofit.IsAtEnd() && !dit.IsAtEnd(); ++aofit, ++dit) {
        f = 0.0;
        currentIndex = aofit.GetIndex();
        if (sign * dit.Get() >  objectThreshold) {
            for (auto point: this->m_Points) {
                //get spoke vector.
                for (size_t d = 0; d < TInputImage::ImageDimension; ++d) {
                    gradientIndex[d] = std::floor((static_cast<double>(currentIndex[d]) + point[d] + 0.5));
                }
                TInputVectorPixelType gradientVector = accessor.GetPixel(gradientIndex, this->m_Gradient);
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
        }
        aofit.Set(f);
    }
}
/**
*  Print Self
*/
template< class TInputImage, class TOutputPixelType, class TInputVectorPixelType>
void
AverageOutwardFluxImageFilter<TInputImage, TOutputPixelType, TInputVectorPixelType>::PrintSelf(std::ostream& os, Indent indent) const
{
	Superclass::PrintSelf(os,indent);
	os << indent << "AverageOutwardFluxImageFilter." << std::endl;
}

