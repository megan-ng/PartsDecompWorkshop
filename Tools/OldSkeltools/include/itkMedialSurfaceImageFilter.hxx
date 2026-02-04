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

#ifndef SKELTOOLS_ITKMEDIALSURFACEIMAGEFILTER_HXX
#define SKELTOOLS_ITKMEDIALSURFACEIMAGEFILTER_HXX

#include <itkShapedNeighborhoodIterator.h>
#include "itkMedialSurfaceImageFilter.h"
#include "utils.h"
namespace itk{

    //--------------------------------------------------
template< class TInputImage, class TAverageOutwardFluxPixelType, class TOutputPixelType>
bool MedialSurfaceImageFilter<TInputImage, TAverageOutwardFluxPixelType, TOutputPixelType>::IsSurfaceEnd( OutputIndexType p )
{

    using ShapedIteratorType = itk::ShapedNeighborhoodIterator<TOutputImage>;
    typename ShapedIteratorType::RadiusType radius;
    radius.Fill(1);
    ShapedIteratorType nit(radius, this->m_Skeleton, this->m_Skeleton->GetRequestedRegion());

    nit.SetLocation(p);

    // xy plane...
    std::vector<typename ShapedIteratorType::OffsetType > plane1;
    plane1.push_back({ { 1, 0, 0 } });
    plane1.push_back({ { 1, 1, 0 } });
    plane1.push_back({ { 0, 1, 0 } });
    plane1.push_back({ {-1, 1, 0 } });
    plane1.push_back({ {-1, 0, 0 } });
    plane1.push_back({ {-1,-1, 0 } });
    plane1.push_back({ { 0,-1, 0 } });
    plane1.push_back({ { 1,-1, 0 } });

    for(auto point: plane1) nit.ActivateOffset(point);
    //std::cout << "plane1 has " << nit.GetActiveIndexListSize() << " active indices." << std::endl;

    int nbr = 0;
    typename ShapedIteratorType::Iterator i;
    for (i = nit.Begin(); ! i.IsAtEnd(); ++i)
        if(i.Get() == 1)
            ++nbr;

    //std::cout << "Plane 1 has " << nbr << "neighbors" << std::endl;

    if (nbr < 2) return true;
    for(auto point: plane1) nit.DeactivateOffset(point);
    //.........................

    // yz plane...
    std::vector<typename ShapedIteratorType::OffsetType > plane2;
    plane2.push_back({ { 0, 1, 0 } });
    plane2.push_back({ { 0, 1, 1 } });
    plane2.push_back({ { 0, 0, 1 } });
    plane2.push_back({ { 0,-1, 1 } });
    plane2.push_back({ { 0,-1, 0 } });
    plane2.push_back({ { 0,-1,-1 } });
    plane2.push_back({ { 0, 0,-1 } });
    plane2.push_back({ { 0, 1,-1 } });

    for(auto point: plane2) nit.ActivateOffset(point);
    //std::cout << "plane2 has " << nit.GetActiveIndexListSize() << " active indices." << std::endl;

    nbr = 0;
    for (i = nit.Begin(); ! i.IsAtEnd(); ++i)
        if(i.Get() == 1)
            ++nbr;

    //std::cout << "Plane 2 has " << nbr << "neighbors" << std::endl;

    if (nbr < 2) return true;
    for(auto point: plane2) nit.DeactivateOffset(point);

    // xz plane...
    std::vector<typename ShapedIteratorType::OffsetType > plane3;
    plane3.push_back({ { 1, 0, 0 } });
    plane3.push_back({ { 1, 0, 1 } });
    plane3.push_back({ { 0, 0, 1 } });
    plane3.push_back({ {-1, 0, 1 } });
    plane3.push_back({ {-1, 0, 0 } });
    plane3.push_back({ {-1, 0,-1 } });
    plane3.push_back({ { 0, 0,-1 } });
    plane3.push_back({ { 1, 0,-1 } });

    for(auto point: plane3) nit.ActivateOffset(point);
    //std::cout << "plane3 has " << nit.GetActiveIndexListSize() << " active indices." << std::endl;

    nbr = 0;
    for (i = nit.Begin(); ! i.IsAtEnd(); ++i)
        if(i.Get() == 1)
            ++nbr;

    //std::cout << "Plane 3 has " << nbr << "neighbors" << std::endl;

    if (nbr < 2) return true;
    for(auto point: plane3) nit.DeactivateOffset(point);

    // xy diagonal...
    std::vector<typename ShapedIteratorType::OffsetType > plane4;
    plane4.push_back({ {-1,-1, 1 } });
    plane4.push_back({ { 0, 0, 1 } });
    plane4.push_back({ { 1, 1, 1 } });
    plane4.push_back({ {-1,-1, 0 } });
    plane4.push_back({ { 1, 1, 0 } });
    plane4.push_back({ {-1,-1,-1 } });
    plane4.push_back({ { 0, 0,-1 } });
    plane4.push_back({ { 1, 1,-1 } });

    for(auto point: plane4) nit.ActivateOffset(point);
    //std::cout << "plane4 has " << nit.GetActiveIndexListSize() << " active indices." << std::endl;

    nbr = 0;
    for (i = nit.Begin(); ! i.IsAtEnd(); ++i)
        if(i.Get() == 1)
            ++nbr;

    //std::cout << "Plane 4 has " << nbr << "neighbors" << std::endl;

    if (nbr < 2) return true;
    for(auto point: plane4) nit.DeactivateOffset(point);

    // cross xy diagonal...
    std::vector<typename ShapedIteratorType::OffsetType > plane5;
    plane5.push_back({ {-1, 1, 1 } });
    plane5.push_back({ { 0, 0, 1 } });
    plane5.push_back({ { 1,-1, 1 } });
    plane5.push_back({ {-1, 1, 0 } });
    plane5.push_back({ { 1,-1, 0 } });
    plane5.push_back({ {-1, 1,-1 } });
    plane5.push_back({ { 0, 0,-1 } });
    plane5.push_back({ { 1,-1,-1 } });

    for(auto point: plane5) nit.ActivateOffset(point);
    //std::cout << "plane5 has " << nit.GetActiveIndexListSize() << " active indices." << std::endl;

    nbr = 0;
    for (i = nit.Begin(); ! i.IsAtEnd(); ++i)
        if(i.Get() == 1)
            ++nbr;

    //std::cout << "Plane 5 has " << nbr << "neighbors" << std::endl;

    if (nbr < 2) return true;
    for(auto point: plane5) nit.DeactivateOffset(point);

    //...............................
    // yz diagonal...
    std::vector<typename ShapedIteratorType::OffsetType > plane6;
    plane6.push_back({ { 1,-1,-1, } });
    plane6.push_back({ { 1, 0, 0, } });
    plane6.push_back({ { 1, 1, 1, } });
    plane6.push_back({ { 0,-1,-1, } });
    plane6.push_back({ { 0, 1, 1, } });
    plane6.push_back({ {-1,-1,-1, } });
    plane6.push_back({ {-1, 0, 0, } });
    plane6.push_back({ {-1, 1, 1, } });

    for(auto point: plane6) nit.ActivateOffset(point);
    //std::cout << "plane6 has " << nit.GetActiveIndexListSize() << " active indices." << std::endl;

    nbr = 0;
    for (i = nit.Begin(); ! i.IsAtEnd(); ++i)
        if(i.Get() == 1)
            ++nbr;

    //std::cout << "Plane 6 has " << nbr << "neighbors" << std::endl;

    if (nbr < 2) return true;
    for(auto point: plane6) nit.DeactivateOffset(point);

    // cross yz diagonal...
    std::vector<typename ShapedIteratorType::OffsetType > plane7;
    plane7.push_back({ { 1,-1, 1 } });
    plane7.push_back({ { 1, 0, 0 } });
    plane7.push_back({ { 1, 1,-1 } });
    plane7.push_back({ { 0,-1, 1 } });
    plane7.push_back({ { 0, 1,-1 } });
    plane7.push_back({ {-1,-1, 1 } });
    plane7.push_back({ {-1, 0, 0 } });
    plane7.push_back({ {-1, 1,-1 } });

    for(auto point: plane7) nit.ActivateOffset(point);
    //std::cout << "plane7 has " << nit.GetActiveIndexListSize() << " active indices." << std::endl;

    nbr = 0;
    for (i = nit.Begin(); ! i.IsAtEnd(); ++i)
        if(i.Get() == 1)
            ++nbr;

    //std::cout << "Plane 7 has " << nbr << "neighbors" << std::endl;

    if (nbr < 2) return true;
    for(auto point: plane7) nit.DeactivateOffset(point);

    //...............................
    // xz diagonal...
    std::vector<typename ShapedIteratorType::OffsetType > plane8;
    plane8.push_back({ {-1, 1,-1, } });
    plane8.push_back({ { 0, 1, 0, } });
    plane8.push_back({ { 1, 1, 1, } });
    plane8.push_back({ {-1, 0,-1, } });
    plane8.push_back({ { 1, 0, 1, } });
    plane8.push_back({ {-1,-1,-1, } });
    plane8.push_back({ { 0,-1, 0, } });
    plane8.push_back({ { 1,-1, 1, } });

    for(auto point: plane8) nit.ActivateOffset(point);
    //std::cout << "plane8 has " << nit.GetActiveIndexListSize() << " active indices." << std::endl;

    nbr = 0;
    for (i = nit.Begin(); ! i.IsAtEnd(); ++i)
        if(i.Get() == 1)
            ++nbr;

    //std::cout << "Plane 8 has " << nbr << "neighbors" << std::endl;

    if (nbr < 2) return true;
    for(auto point: plane8) nit.DeactivateOffset(point);

    // cross xz diagonal...
    std::vector<typename ShapedIteratorType::OffsetType > plane9;
    plane9.push_back({ {-1, 1, 1 } });
    plane9.push_back({ { 0, 1, 0 } });
    plane9.push_back({ { 1, 1,-1 } });
    plane9.push_back({ {-1, 0, 1 } });
    plane9.push_back({ { 1, 0,-1 } });
    plane9.push_back({ {-1,-1, 1 } });
    plane9.push_back({ { 0,-1, 0 } });
    plane9.push_back({ { 1,-1,-1 } });

    for(auto point: plane9) nit.ActivateOffset(point);
    //std::cout << "plane9 has " << nit.GetActiveIndexListSize() << " active indices." << std::endl;

    nbr = 0;
    for (i = nit.Begin(); ! i.IsAtEnd(); ++i)
        if(i.Get() == 1)
            ++nbr;

    //std::cout << "Plane 9 has " << nbr << "neighbors" << std::endl;

    if (nbr < 2) return true;
    for(auto point: plane9) nit.DeactivateOffset(point);

    return false;
}



    /*
template<typename TInputImage, typename TAverageOutwardFluxPixelType, typename TOutputPixelType>
void MedialSurfaceImageFilter<TInputImage, TAverageOutwardFluxPixelType, TOutputPixelType>::GenerateData() {
   this->m_Distance = dynamic_cast<const TInputImage  *>( ProcessObject::GetInput(0) );

   this->m_Skeleton = dynamic_cast< TOutputImage * >(  this->ProcessObject::GetOutput(0) );
   this->m_Skeleton->SetSpacing( this->m_Distance->GetSpacing() );
   this->m_Skeleton->SetOrigin( this->m_Distance->GetOrigin() );
   this->m_Skeleton->SetRegions( this->m_Distance->GetRequestedRegion() );
   this->m_Skeleton->Allocate();

   typename TOutputImage::Pointer temp = TOutputImage::New();
   temp->SetSpacing( this->m_Distance->GetSpacing() );
   temp->SetOrigin( this->m_Distance->GetOrigin() );
   temp->SetRegions( this->m_Distance->GetRequestedRegion() );
   temp->Allocate();
   OutputIteratorType skit( this->m_Skeleton, this->m_Skeleton->GetRequestedRegion() );
   OutputIteratorType tempit( temp, temp->GetRequestedRegion() );
   InputConstIteratorType dit( this->m_Distance, this->m_Distance->GetRequestedRegion() );

   for ( skit.GoToBegin(), dit.GoToBegin() ; !skit.IsAtEnd() && !dit.IsAtEnd(); ++skit, ++dit )
   {
       if(dit.Get() < 0)
           skit.Set(1);
       else
           skit.Set(0);
   }
    InputIndexType q;
    for ( skit.GoToBegin(), tempit.GoToBegin() ; !skit.IsAtEnd() && !tempit.IsAtEnd(); ++tempit, ++skit )
    {
        q = skit.GetIndex();
        if (skit.Get() == 1 && this->IsSurfaceEnd(q))
            tempit.Set(1);
        else
            tempit.Set(0);
    }
    writeImage<TOutputImage>("/home/leonardo/tabish/data/trash/mask.tif",temp);

}
*/

template<typename TInputImage, typename TAverageOutwardFluxPixelType, typename TOutputPixelType>
void MedialSurfaceImageFilter<TInputImage, TAverageOutwardFluxPixelType, TOutputPixelType>::GenerateData()
{
    this->m_Distance = dynamic_cast<const TInputImage  *>( ProcessObject::GetInput(0) );
    this->m_Aof = dynamic_cast<const TInputImage  *>( ProcessObject::GetInput(1) );

    this->m_Skeleton = dynamic_cast< TOutputImage * >(  this->ProcessObject::GetOutput(0) );

    // Create the region of the auxuliar queued image
    OutputSizeType size;
    OutputIndexType start;

    for ( size_t i = 0; i < TOutputImage::ImageDimension; i++ )
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
    //this->DistanceToObject();
    this->FluxToSkeleton();
    // Topological prunning

    //Iterators
    //InputConstIteratorType dit( this->m_Distance, this->m_Distance->GetRequestedRegion() );
    InputConstIteratorType ait( this->m_Aof, this->m_Aof->GetRequestedRegion() );
    OutputIteratorType skit( this->m_Skeleton, this->m_Skeleton->GetRequestedRegion() );
    OutputIteratorType qit( this->m_Queued, this->m_Queued->GetRequestedRegion() );

    //First step...
    InputIndexType q;
    InputPixelType priority=0;
    HeapType heap;
    Pixel node;

    for ( skit.GoToBegin(), ait.GoToBegin(), qit.GoToBegin(); !skit.IsAtEnd() && !ait.IsAtEnd() && !qit.IsAtEnd(); ++skit, ++ait, ++qit )
    {
        q = skit.GetIndex();
        if ( this->IsBoundary( q ) )
        {
            if ( this->IsIntSimple( q ) && this->IsExtSimple( q ) )
            {
                //Simple pixel
                node.SetIndex( q );
                priority = ait.Get();
                node.SetValue( priority );
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
    typename OutputNeighborhoodIteratorType::RadiusType radius;
//    radius.Fill(1);
//    InputConstNeighborhoodIteratorType dnit( radius, this->m_Distance, this->m_Distance->GetRequestedRegion() );
    radius.Fill(1);
    AOFConstNeighborhoodIteratorType aofnit( radius, this->m_Aof, this->m_Aof->GetRequestedRegion() );
    radius.Fill(1);
    OutputNeighborhoodIteratorType sknit( radius, this->m_Skeleton, this->m_Skeleton->GetRequestedRegion() );
    radius.Fill(1);
    OutputNeighborhoodIteratorType qnit( radius, this->m_Queued, this->m_Queued->GetRequestedRegion() );
    InputIndexType y;

    while ( !heap.empty() )
    {

        node=heap.top();
        heap.pop();

        q = node.GetIndex();
        qnit.SetLocation( q );
        qnit.SetCenterPixel( 0 );

        if ( this->IsIntSimple( q ) && this->IsExtSimple( q ) )
        {
            aofnit.SetLocation(q);
            if ( ( this->IsEnd( q ) ) && ( aofnit.GetCenterPixel() < this->m_Threshold ))
            {
                //std::cout<<q<<" Is medial "<<aofnit.GetCenterPixel()<<std::endl;
            }
            else
            {
                //std::cout<<q<<" Is deleted"<<std::endl;
                sknit.SetLocation( q );
                sknit.SetCenterPixel( 0 ); //Deletion from object
                // This iterator of nh goes out of the image, but since we have the
                // boundary condition to zero, when asking for a pixel from outside we
                // return 0 and we will not do anything
                // In queued therefore we will not write out of range

                for ( unsigned int i = 0; i < sknit.Size(); i++ )
                {
                    if ( sknit.GetPixel( i ) == 1 ) // check neighbors (center already set to 0)
                    {
                        //Object pixel
                        y = sknit.GetIndex(i );
                        if ( qnit.GetPixel( i ) == 0 )//Not queued pixel
                        {
                            if (this->IsIntSimple(y ) && this->IsExtSimple(y ) )
                            {
                                priority = aofnit.GetPixel( i );
                                node.SetIndex( y );
                                node.SetValue( priority );
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

/**
 *  Print Self
 */
template< class TInputImage, class TAverageOutwardFluxPixelType, class TOutputPixelType>
void MedialSurfaceImageFilter<TInputImage, TAverageOutwardFluxPixelType, TOutputPixelType>
::PrintSelf(std::ostream& os, Indent indent) const
{
    Superclass::PrintSelf(os,indent);

    os << indent << "MedialSurfaceImageFilter." << std::endl;
    os << indent << "Threshold         : " << this->m_Threshold << std::endl;
}

}
#endif //SKELTOOLS_ITKMEDIALSURFACEIMAGEFILTER_HXX
