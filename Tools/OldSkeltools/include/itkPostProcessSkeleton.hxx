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

#ifndef SKELTOOLS_ITKPOSTPROCESSSKELETON_HXX
#define SKELTOOLS_ITKPOSTPROCESSSKELETON_HXX
template< class TInputImage>
PostProcessSkeleton<TInputImage>::PostProcessSkeleton(){
    m_Neighbors18 = {
            {{-1,-1,0}},
            {{-1,0,-1}},
            {{-1,0,0}},
            {{-1,0,1}},
            {{-1,1,0}},
            {{0,-1,-1}},
            {{0,-1,0}},
            {{0,-1,1}},
            {{0,0,-1}},
            {{0,0,1}},
            {{0,1,-1}},
            {{0,1,0}},
            {{0,1,1}},
            {{1,-1,0}},
            {{1,0,-1}},
            {{1,0,0}},
            {{1,0,1}},
            {{1,1,0}}
    };

    m_Neighbors26 = {
            {{-1,-1,-1}},
            {{-1,-1,0}},
            {{-1,-1,1}},
            {{-1,0,-1}},
            {{-1,0,0}},
            {{-1,0,1}},
            {{-1,1,-1}},
            {{-1,1,0}},
            {{-1,1,1}},
            {{0,-1,-1}},
            {{0,-1,0}},
            {{0,-1,1}},
            {{0,0,-1}},
            {{0,0,1}},
            {{0,1,-1}},
            {{0,1,0}},
            {{0,1,1}},
            {{1,-1,-1}},
            {{1,-1,0}},
            {{1,-1,1}},
            {{1,0,-1}},
            {{1,0,0}},
            {{1,0,1}},
            {{1,1,-1}},
            {{1,1,0}},
            {{1,1,1}}
    };
    m_Graph26 = {
            {1,2,4,5,10,11,13},
            {1,2,3,4,5,6,10,11,12,13,14},
            {2,3,5,6,11,12,14},
            {1,2,4,5,7,8,10,11,13,15,16},
            {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17},
            {2,3,5,6,8,9,11,12,14,16,17},
            {4,5,7,8,13,15,16},
            {4,5,6,7,8,9,13,14,15,16,17},
            {5,6,8,9,14,16,17},
            {1,2,4,5,10,11,13,18,19,21,22},
            {1,2,3,4,5,6,10,11,12,13,14,18,19,20,21,22,23},
            {2,3,5,6,11,12,14,19,20,22,23},
            {1,2,4,5,7,8,10,11,13,15,16,18,19,21,22,24,25},
            {2,3,5,6,8,9,11,12,14,16,17,19,20,22,23,25,26},
            {4,5,7,8,13,15,16,21,22,24,25},
            {4,5,6,7,8,9,13,14,15,16,17,21,22,23,24,25,26},
            {5,6,8,9,14,16,17,22,23,25,26},
            {10,11,13,18,19,21,22},
            {10,11,12,13,14,18,19,20,21,22,23},
            {11,12,14,19,20,22,23},
            {10,11,13,15,16,18,19,21,22,24,25},
            {10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26},
            {11,12,14,16,17,19,20,22,23,25,26},
            {13,15,16,21,22,24,25},
            {13,14,15,16,17,21,22,23,24,25,26},
            {14,16,17,22,23,25,26}
    };
    m_Graph18 = {
            {1,3,7},
            {2,3,9},
            {1,2,3,4,5},
            {3,4,10},
            {3,5,12},
            {6,7,9},
            {1,6,7,8,14},
            {7,8,10},
            {2,6,9,11,15},
            {4,8,10,13,17},
            {9,11,12},
            {5,11,12,13,18},
            {10,12,13},
            {7,14,16},
            {9,15,16},
            {14,15,16,17,18},
            {10,16,17},
            {12,16,18}
    };
    m_n6 = {false,false,true,false,false,false,true,false,true,true,false,true,false,false,false,true,false,false};
}

template< class TInputImage>
unsigned
PostProcessSkeleton<TInputImage>::BackgroundLabelling(IndexType& index){


    unsigned regions = 0;
    std::queue<size_t> Q;
    std::vector<bool> visited(18,false);
    for(size_t i = 0; i < m_Neighbors18.size(); ++i){ // starting point
        if( (m_n6[i]) && (! visited[i]) &&                       // not already visited
            (m_Accessor.GetPixel(index + m_Neighbors18[i], this->m_Output) <= 0)){     // is outside object
            ++regions; // new component
            Q.push(i);
            visited[i] = true;
            while(Q.size() > 0){
                size_t current = Q.front();
                Q.pop();
                visited[current] = true;
                for(size_t neighbor : m_Graph18[current]){
                    --neighbor;
                    if((!visited[neighbor]) &&
                       (m_Accessor.GetPixel(index + m_Neighbors18[neighbor], this->m_Output) <= 0)){
                        visited[neighbor] = true;
                        Q.push(neighbor);
                    }
                }
            }

        }
    }
    return regions;
}

template< class TInputImage>
unsigned
PostProcessSkeleton<TInputImage>::ForegroudLabelling(IndexType& index){
    unsigned regions = 0;
    std::queue<size_t> Q;
    std::vector<bool> visited(26,false);
    for(size_t i = 0; i < m_Neighbors26.size(); ++i){ // starting point
        if( (! visited[i]) &&                       // not already visited
            (m_Accessor.GetPixel(index + m_Neighbors26[i], this->m_Output) > 0)){     // is in object
            ++regions; // new component
            Q.push(i);
            visited[i] = true;
            while(Q.size() > 0){
                size_t current = Q.front();
                Q.pop();
                visited[current] = true;
                for(size_t neighbor : m_Graph26[current]){
                    --neighbor;
                    if((!visited[neighbor]) &&
                       (m_Accessor.GetPixel(index + m_Neighbors26[neighbor], this->m_Output) > 0)){
                        visited[neighbor] = true;
                        Q.push(neighbor);
                    }
                }
            }

        }
    }
    return regions;
}

template< class TInputImage>
unsigned
PostProcessSkeleton<TInputImage>::TopologicalLabel(IndexType index){
    unsigned Cstar = this->ForegroudLabelling(index);
    unsigned Cbar = this->BackgroundLabelling(index);
    unsigned label;
    if (Cbar==0)
        label =   2 ; //interior point
    else if (Cstar==0)
        label =   3; //isolated point
    else if ((Cbar==1)&&(Cstar==1))
        label =   4; //simple point
    else if ((Cbar==1)&&(Cstar==2))
        label =   5; //candidate curve point
    else if ((Cbar==1)&&(Cstar>2))
        label =   6; //junction of curves
    else if ((Cbar==2)&&(Cstar==1))
        label =   7; //candidate surface point
    else if ((Cbar==2)&&(Cstar>=2))
        label =   8;  //junction between curve(s) and surface
    else if ((Cbar>2)&&(Cstar==1))
        label =   9; //junction of surfaces
    else if ((Cbar>2)&&(Cstar>=2))
        label =  10; //junction between surface(s) and curve
    else
        label = 0;

    return label;
}

template< class TInputImage>
void
PostProcessSkeleton<TInputImage>::GenerateData() {
    this->AllocateOutputs();
    InputPointerType input = this->GetInput();
    this->m_Output = this->GetOutput();

    InputConstIteratorType skit = InputConstIteratorType(input, input->GetLargestPossibleRegion());
    OutputIteratorType outIt = OutputIteratorType(m_Output, m_Output->GetLargestPossibleRegion());

    skit.GoToBegin();
    outIt.GoToBegin();
    while(!skit.IsAtEnd()){
        outIt.Set(skit.Get());
        ++skit;
        ++outIt;
    }
    outIt.GoToBegin();
    //TODO: FIX this....
    while(!outIt.IsAtEnd()) {
        if (outIt.Get() > 0) {
            IndexType index = outIt.GetIndex();
            unsigned label = this->TopologicalLabel(index);
            if (label == 3 || label == 5 || label == 6) {
                outIt.Set(0);
            }
        }
        ++outIt;
    }
}

/**
*  Print Self
*/
template< class TInputImage>
void
PostProcessSkeleton<TInputImage>::PrintSelf(std::ostream& os, Indent indent) const
{
    Superclass::PrintSelf(os,indent);
    os << indent << "PostProcessSkeleton." << std::endl;
}

#endif //SKELTOOLS_ITKPOSTPROCESSSKELETON_HXX
