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

#ifndef SKELTOOLS_ITKTOPOLOGYPRESERVINGTHINNINGBASE_HXX
#define SKELTOOLS_ITKTOPOLOGYPRESERVINGTHINNINGBASE_HXX

#include <itkNeighborhoodIterator.h>
#include <itkImageRegionIterator.h>

#include "itkTopologyPreservingThinningBase.h"

namespace itk {


    template<class TInputImage, class TAverageOutwardFluxPixelType, class TOutputPixelType>
    bool TopologyPreservingThinningBase<TInputImage, TAverageOutwardFluxPixelType, TOutputPixelType>::IsBoundary(
            OutputIndexType p) {
        bool IsBoundaryPixel = false;
        typename OutputNeighborhoodIteratorType::RadiusType radius;
        radius.Fill(1);
        OutputNeighborhoodIteratorType nit(radius, this->m_Skeleton, this->m_Skeleton->GetRequestedRegion());

        OutputBCType cbc;
        nit.OverrideBoundaryCondition(&cbc);

        nit.SetLocation(p);

        if (nit.GetCenterPixel() == 1) {
            for (unsigned int i = 0; i < nit.Size() && !IsBoundaryPixel; i++) {
                if (nit.GetPixel(i) == 0) {
                    IsBoundaryPixel = true;
                    break;
                }
            }
        }
        return IsBoundaryPixel;
    }


    //An object pixel is simple for the object's topology if its deletion from the object does not change the
    //topology in the pixel's neighborhood. The algorithm proceeds finding the first 26 connected point to
    //the pixel in the object. A flood fill algorithm goes through the adjacent object pixels different
    //from p using the 26 connectivity. Finally, if the number of flooded points equals to the number of
    //object pixels-1 the local topology does not change and, therefore, the pixel is simple for the object.
    //--------------------------------------------------
    template< class TInputImage, class TAverageOutwardFluxPixelType, class TOutputPixelType>
    bool TopologyPreservingThinningBase<TInputImage, TAverageOutwardFluxPixelType, TOutputPixelType>::IsIntSimple( OutputIndexType p )
    {
            // Compute new index of the auxiliar queued image
            OutputIndexType start;

            for ( size_t i = 0; i < TOutputImage::ImageDimension; i++ )
            {
                start[i] = p[i]-1;
            }
            this->m_Region.SetIndex( start );
            this->m_AuxQueued->SetRegions( m_Region );

            typename OutputNeighborhoodIteratorType::RadiusType radius;
            radius.Fill(1);
            OutputNeighborhoodIteratorType nit( radius, this->m_Skeleton, this->m_Skeleton->GetRequestedRegion());
            radius.Fill(0);
            OutputNeighborhoodIteratorType qnit( radius, this->m_AuxQueued, this->m_AuxQueued->GetRequestedRegion());

            OutputBCType cbc;
            nit.OverrideBoundaryCondition(&cbc);

            nit.SetLocation(p);

            int in = 0;
            for ( unsigned int i = 0; i < nit.Size(); i++ )
            {
                if ( nit.GetPixel( i ) == 1 )
                    in++;
            }

            if( in == 1 || in == pow((float)3,(int)TInputImage::ImageDimension) )
            { // p is isolated or an interior point and therefore, not simple
                return false;
            }
            else
            {
                //Flood-fill

                typename TOutputImage::PixelType priority=0;
                HeapType heap;
                Pixel node;

                OutputIteratorType aux( this->m_AuxQueued , this->m_AuxQueued->GetRequestedRegion() );

                for ( aux.GoToBegin(); !aux.IsAtEnd(); ++aux )
                    aux.Set( 0 );

                qnit.SetLocation( nit.GetIndex() );
                qnit.SetCenterPixel( 1 );

                OutputIndexType q;

                for( unsigned int i = 0; i < nit.Size(); i++ )
                {
                    if ( nit.GetPixel( i ) == 1 )
                    {
                        //object pixel
                        qnit.SetLocation( nit.GetIndex( i ) );
                        if( qnit.GetCenterPixel() == 0 )
                        {
                            // not queued
                            node.SetIndex( nit.GetIndex( i ) );
                            node.SetValue( priority++ );
                            heap.push( node );

                            qnit.SetCenterPixel( 1 );
                            i = nit.Size();
                        }
                    }
                }

                //Just inserted the first found object pixel in N26*

                while( !heap.empty() )
                {
                    node = heap.top();
                    heap.pop();

                    nit.SetLocation( node.GetIndex() );

                    for( unsigned int i = 0; i < nit.Size(); i++ )
                    {
                        if( nit.GetPixel( i ) == 1 )
                        {
                            //object pixel
                            q = nit.GetIndex( i );
                            qnit.SetLocation( q );
                            bool cond = true;
                            for (unsigned int d = 0; d < TInputImage::ImageDimension && cond; d++)
                            {
                                cond = cond && (abs( double(p[d] - q[d]) ) <= 1);
                            }
                            if ( ( cond ) && qnit.GetCenterPixel() == 0 )
                            {
                                // 26* neighborhood - not queued
                                node.SetIndex( q );
                                node.SetValue( priority++ );
                                heap.push( node );
                                qnit.SetCenterPixel( 1 );
                            }
                        }
                    }
                }

                return priority == in - 1;
            }
    }


    //An object pixel is simple for the background's topology if its deletion from the object does not
//change the background's topology in the pixel's neighborhood. The algorithm proceeds finding
//the first pixel in the background. A flood fill algorithm goes through the adjacent background pixels
//different from p using the 6 connectivity in the 18 neighborhood. Finally, if the number of flooded
//points equals to the number of 6 connected background pixels-1 the local topology of the background
//does not change and, therefore, the pixel is simple for the background.
//--------------------------------------------------
    template< class TInputImage, class TAverageOutwardFluxPixelType, class TOutputPixelType>
    bool TopologyPreservingThinningBase<TInputImage, TAverageOutwardFluxPixelType, TOutputPixelType>::IsExtSimple( OutputIndexType p )
    {
            // Compute new index of the auxiliar queued image
            OutputIndexType start;

            for ( size_t i = 0; i < TOutputImage::ImageDimension; i++ )
            {
                start[i] = p[i]-1;
            }
            this->m_Region.SetIndex( start );

            this->m_AuxQueued->SetRegions( m_Region );


            typename OutputNeighborhoodIteratorType::RadiusType radius;
            radius.Fill(1);
            OutputNeighborhoodIteratorType nit(radius, this->m_Skeleton, this->m_Skeleton->GetRequestedRegion());
            radius.Fill(0);
            OutputNeighborhoodIteratorType qnit(radius, this->m_AuxQueued, this->m_AuxQueued->GetRequestedRegion());

            OutputBCType cbc;
            nit.OverrideBoundaryCondition(&cbc);

            nit.SetLocation(p);

            int out = 0;

            OutputIndexType q,r;
            OutputPixelType priority=0;
            HeapType heap;

            Pixel node;

            OutputIteratorType aux( this->m_AuxQueued , this->m_AuxQueued->GetRequestedRegion() );

            for ( aux.GoToBegin(); !aux.IsAtEnd(); ++aux )
                aux.Set( 0 );

            out = 0;
            node.SetIndex( p );
            node.SetValue( out++ );
            heap.push( node );

            qnit.SetLocation( p );
            qnit.SetCenterPixel( 1 );

            while ( !heap.empty() )
            {
                node = heap.top();
                heap.pop();
                r = node.GetIndex();
                nit.SetLocation( r );

                for( unsigned int i = 0; i < nit.Size(); i++ )
                {
                    if ( nit.GetPixel( i ) == 0 )
                    {
                        q = nit.GetIndex( i );
                        qnit.SetLocation( q );
                        bool cond1 = true;
                        bool cond2 = false;
                        for (unsigned int d = 0; d < TInputImage::ImageDimension && cond1; d++)
                        {
                            cond1 = cond1 && (abs( double(p[d] - q[d]) ) <= 1);
                        }
                        for (unsigned int d = 0; d < TInputImage::ImageDimension && !cond2; d++)
                        {
                            cond2 = cond2 || (p[d] - q[d] == 0);
                        }

                        if ( ( cond1 ) && ( cond2 ) )
                        {
                            bool cond3 = false;
                            for (unsigned int d = 0; d < TInputImage::ImageDimension && !cond3; d++)
                            {
                                cond3 = cond3 || ( r[d] - q[d] == 0 && r[(d+1)%(TInputImage::ImageDimension)] - q[(d+1)%(TInputImage::ImageDimension)] == 0 );
                            }
                            //18 neighbor of p
                            if ( cond3 )
                            {
                                //6 connected to r
                                if ( qnit.GetCenterPixel() == 0 )
                                {
                                    node.SetIndex( q );
                                    node.SetValue( out++ );
                                    heap.push( node );
                                    qnit.SetCenterPixel( 1 );
                                }
                            }
                        }
                    }
                }
            }

            if ( out == 0 || out == 18 )
                return false;
            else
            {

                //Flood-fill

                nit.SetLocation(p);

                for ( aux.GoToBegin(); !aux.IsAtEnd(); ++aux )
                    aux.Set( 0 );

                qnit.SetLocation( p );
                qnit.SetCenterPixel( 1 );

                for ( unsigned int i = 0; i < nit.Size(); i++ )
                {
                    if ( nit.GetPixel( i ) == 0 )
                    {
                        //background pixel
                        q = nit.GetIndex( i );
                        qnit.SetLocation( q );
                        bool cond1 = true;
                        bool cond2 = false;
                        for (unsigned int d = 0; d < TInputImage::ImageDimension && cond1; d++)
                        {
                            cond1 = cond1 && (abs( double(p[d] - q[d]) ) <= 1);
                        }
                        for (unsigned int d = 0; d < TInputImage::ImageDimension && !cond2; d++)
                        {
                            cond2 = cond2 || (p[d] - q[d] == 0);
                        }

                        if ( ( cond1 ) && ( cond2 ) )
                        {
                            bool cond3 = false;
                            for (unsigned int d = 0; d < TInputImage::ImageDimension && !cond3; d++)
                            {
                                cond3 = cond3 || ( p[d] - q[d] == 0 && p[(d+1)%(TInputImage::ImageDimension)] - q[(d+1)%(TInputImage::ImageDimension)] == 0 );
                            }
                            //18 neighbor of p
                            if ( cond3 )
                            {
                                //6 connected to p
                                if ( qnit.GetCenterPixel() == 0 )
                                {
                                    // not queued
                                    node.SetIndex( q );
                                    node.SetValue( priority++ );
                                    heap.push( node );

                                    qnit.SetCenterPixel( 1 );
                                    i = nit.Size();
                                }
                            }
                        }
                    }
                }

                //Just inserted the first found background pixel in N18

                while( !heap.empty() )
                {
                    node = heap.top();
                    heap.pop();
                    r = node.GetIndex();
                    nit.SetLocation( r );

                    for( unsigned int i = 0; i < nit.Size(); i++ )
                    {
                        if ( nit.GetPixel( i ) == 0 )
                        {
                            q = nit.GetIndex( i );
                            qnit.SetLocation( q );
                            bool cond1 = true;
                            bool cond2 = false;
                            for (unsigned int d = 0; d < TInputImage::ImageDimension && cond1; d++)
                            {
                                cond1 = cond1 && (abs( double(p[d] - q[d]) ) <= 1);
                            }
                            for (unsigned int d = 0; d < TInputImage::ImageDimension && !cond2; d++)
                            {
                                cond2 = cond2 || (p[d] - q[d] == 0);
                            }

                            if ( ( cond1 ) && ( cond2 ) )
                            {
                                bool cond3 = false;
                                for (unsigned int d = 0; d < TInputImage::ImageDimension && !cond3; d++)
                                {
                                    cond3 = cond3 || ( r[d] - q[d] == 0 && r[(d+1)%(TInputImage::ImageDimension)] - q[(d+1)%(TInputImage::ImageDimension)] == 0 );
                                }
                                //18 neighbor of r
                                if ( cond3 )
                                {
                                    //6 connected
                                    if ( qnit.GetCenterPixel() == 0 )
                                    {
                                        node.SetIndex( q );
                                        node.SetValue( priority++ );
                                        heap.push( node );
                                        qnit.SetCenterPixel( 1 );
                                    }
                                }
                            }
                        }
                    }
                }

                if( priority == out-1 )
                    return true;
                else
                    return false;
            }

    }

    //p is a medial axis end point if in 26* there is only one foreground points
//--------------------------------------------------
    template< class TInputImage, class TAverageOutwardFluxPixelType, class TOutputPixelType>
    bool TopologyPreservingThinningBase<TInputImage, TAverageOutwardFluxPixelType, TOutputPixelType>::IsEnd( OutputIndexType p )
    {

            typename OutputNeighborhoodIteratorType::RadiusType radius;
            radius.Fill(1);
            OutputNeighborhoodIteratorType nit(radius, this->m_Skeleton, this->m_Skeleton->GetRequestedRegion());

            OutputBCType cbc;
            nit.OverrideBoundaryCondition(&cbc);

            nit.SetLocation(p);

            int n=0;
            for( unsigned int i = 0; i < nit.Size(); i++ )
            {
                if ( nit.GetIndex() != nit.GetIndex( i ) && nit.GetPixel( i ) == 1 ) //Belonging to the object - 26* connected
                    n++;
            }

        return n < 2;

    }

//Computation o the binary image representing the object from its signed m_Distance representation.
    template< class TInputImage, class TAverageOutwardFluxPixelType, class TOutputPixelType>
    void TopologyPreservingThinningBase<TInputImage, TAverageOutwardFluxPixelType, TOutputPixelType>::DistanceToObject(bool insideNegative)
    {
        InputConstIteratorType dit(this->m_Distance, this->m_Distance->GetRequestedRegion());
        OutputIteratorType skit(this->m_Skeleton, this->m_Skeleton->GetRequestedRegion());
        OutputPixelType inside  = insideNegative ? 1 : 0;
        for ( dit.GoToBegin(), skit.GoToBegin(); !dit.IsAtEnd(), !skit.IsAtEnd(); ++dit, ++skit )
        {
            if ( dit.Get() < 0 )//threshold aof instead..
                skit.Set( inside );
            else
                skit.Set( 1 - inside );
        }

    }

    template< class TInputImage, class TAverageOutwardFluxPixelType, class TOutputPixelType>
    void TopologyPreservingThinningBase<TInputImage, TAverageOutwardFluxPixelType, TOutputPixelType>::FluxToSkeleton(bool insideNegative)
    {
        InputConstIteratorType aofIt(this->m_Aof, this->m_Aof->GetRequestedRegion());
        OutputIteratorType skit(this->m_Skeleton, this->m_Skeleton->GetRequestedRegion());
        OutputPixelType inside  = insideNegative ? 1 : 0;
        for ( aofIt.GoToBegin(), skit.GoToBegin(); !aofIt.IsAtEnd(), !skit.IsAtEnd(); ++aofIt, ++skit )
        {
            if ( aofIt.Get() < this->m_Threshold )//threshold aof instead..
                skit.Set( inside );
            else
                skit.Set( 1 - inside );
        }
    }


}// namespace itk




#endif //SKELTOOLS_ITKTOPOLOGYPRESERVINGTHINNINGBASE_HXX
