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


#ifndef SKELTOOLS_ITKMEDIALCURVEIMAGEFILTER_HXX
#define SKELTOOLS_ITKMEDIALCURVEIMAGEFILTER_HXX

#include "itkMedialCurveImageFilter.h"

namespace itk{

 template<typename TInputImage, typename TAverageOutwardFluxPixelType, typename TOutputPixelType>
void MedialCurveImageFilter<TInputImage, TAverageOutwardFluxPixelType, TOutputPixelType>::GenerateData()
{
    this->m_Distance = dynamic_cast<const TInputImage  *>( ProcessObject::GetInput(0) );
    this->m_Aof = dynamic_cast<const TInputImage  *>( ProcessObject::GetInput(1) );

    this->m_Skeleton = dynamic_cast< TOutputImage * >(  this->ProcessObject::GetOutput(0) );

    // Create the region of the auxuliar queued image
    OutputSizeType size;
    OutputIndexType start;

    for (size_t i = 0; i < TOutputImage::ImageDimension; i++ )
    {
        start[i]=0;
        size[i]=3;
    }
    this->m_Region.SetIndex( start );
    this->m_Region.SetSize( size );

    this->m_Skeleton->SetSpacing( this->m_Distance->GetSpacing() );
    this->m_Skeleton->SetOrigin( this->m_Distance->GetOrigin() );
    this->m_Skeleton->SetRegions( this->m_Distance->GetRequestedRegion() );
    this->m_Skeleton->Allocate();

    this->m_Queued = TOutputImage::New();
    this->m_Queued->SetSpacing( this->m_Distance->GetSpacing() );
    this->m_Queued->SetOrigin( this->m_Distance->GetOrigin() );
    this->m_Queued->SetRegions( this->m_Distance->GetRequestedRegion() );
    this->m_Queued->Allocate();

    this->m_AuxQueued = TOutputImage::New();
    this->m_AuxQueued->SetSpacing( this->m_Distance->GetSpacing() );
    this->m_AuxQueued->SetOrigin( this->m_Distance->GetOrigin() );
    this->m_AuxQueued->SetRegions( this->m_Region );
    this->m_AuxQueued->Allocate();

    // Initialization of binary object
    this->DistanceToObject();

    // Topological prunning

    //Iterators
    InputConstIteratorType dit( this->m_Distance, this->m_Distance->GetRequestedRegion() );
    OutputIteratorType skit( this->m_Skeleton, this->m_Skeleton->GetRequestedRegion() );
    OutputIteratorType qit( this->m_Queued, this->m_Queued->GetRequestedRegion() );

    typename OutputNeighborhoodIteratorType::RadiusType radius;
    radius.Fill(1);
    InputConstNeighborhoodIteratorType dnit( radius, this->m_Distance, this->m_Distance->GetRequestedRegion() );
    radius.Fill(1);
    OutputNeighborhoodIteratorType sknit( radius, this->m_Skeleton, this->m_Skeleton->GetRequestedRegion() );
    radius.Fill(1);
    OutputNeighborhoodIteratorType qnit( radius, this->m_Queued, this->m_Queued->GetRequestedRegion() );

    //First step...
    InputIndexType q;
    InputPixelType priority=0;
    HeapType heap;
    Pixel node;

    for ( skit.GoToBegin(), dit.GoToBegin(), qit.GoToBegin(); !skit.IsAtEnd() && !dit.IsAtEnd() && !qit.IsAtEnd(); ++skit, ++dit, ++qit )
    {
        q = skit.GetIndex();
        if ( this->IsBoundary( q ) )
        {
            if ( this->IsIntSimple( q ) && this->IsExtSimple( q ) )
            {
                //Simple pixel
                node.SetIndex( q );
                priority = dit.Get();
                node.SetValue( -priority );
                heap.push( node );
                qit.Set( 1 );
            }
            else qit.Set( 0 );
        }
        else{
            qit.Set( 0 );
        }
    }

    //Second step

    InputIndexType r;

    while ( !heap.empty() )
    {

        node=heap.top();
        heap.pop();

        q = node.GetIndex();

        qnit.SetLocation( q );
        qnit.SetCenterPixel( 0 );

        if ( this->IsIntSimple( q ) && this->IsExtSimple( q ) )
        {
            if ( this->IsEnd( q ) )
            {
                //std::cout<<q<<" Is medial "<<aofnit.GetCenterPixel()<<std::endl;
            }
            else
            {
                //std::cout<<q<<" Is deleted"<<std::endl;
                sknit.SetLocation( q );
                sknit.SetCenterPixel( 0 ); //Deletion from object
                dnit.SetLocation( q );
                // This iterator of nh goes out of the image, but since we have the
                // boundary condition to zero, when asking for a pixel from outside we
                // return 0 and we will not do anything
                // In queued therefore we will not write out of range

                for ( unsigned int i = 0; i < sknit.Size(); i++ )
                {
                    if ( sknit.GetPixel( i ) == 1 )
                    {
                        //Object pixel
                        r = sknit.GetIndex( i );
                        if ( qnit.GetPixel( i ) == 0 )
                        {
                            //Not queued pixel
                            if ( this->IsIntSimple( r ) && this->IsExtSimple( r ) )
                            {
                                priority = dnit.GetPixel( i );
                                node.SetIndex( r );
                                node.SetValue( -priority );
                                heap.push( node );
                                qnit.SetPixel( i, 1 );
                            }
                        }
                    }
                }
            }
        }
    }
}

template<typename TInputImage, typename TAverageOutwardFluxPixelType, typename TOutputPixelType>
bool MedialCurveImageFilter<TInputImage, TAverageOutwardFluxPixelType, TOutputPixelType>::IsEnd(OutputIndexType p)
{
    return ( ( this->m_Aof->GetPixel(p) < this->m_Threshold ) && ( Superclass::IsEnd(p) ) );
}

/**
 *  Print Self
 */
template< class TInputImage, class TAverageOutwardFluxPixelType, class TOutputPixelType>
void MedialCurveImageFilter<TInputImage, TAverageOutwardFluxPixelType, TOutputPixelType>
::PrintSelf(std::ostream& os, Indent indent) const
{
    Superclass::PrintSelf(os,indent);
    os << indent << "MedialCurveImageFilter." << std::endl;
    os << indent << "Threshold         : " << this->m_Threshold << std::endl;
}

}
#endif //SKELTOOLS_ITKMEDIALSURFACEIMAGEFILTER_HXX
