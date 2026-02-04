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
// Created by tabish on 2021-07-27.
//

#ifndef SKELTOOLS_MEDIAL_HXX
#define SKELTOOLS_MEDIAL_HXX

template<typename DistanceImageType, typename FluxImageType, typename OutputImageType>
typename OutputImageType::Pointer
computeMedialCurve(const itk::CommandLineArgumentParser::Pointer &parser,
                   const typename DistanceImageType::Pointer &distanceMap, 
                   const typename FluxImageType::Pointer &aof,
                   const itk::Logger::Pointer &logger){

    std::string outputFoldername;
    std::string endpointsPath;

    parser->GetCommandLineArgument("-outputFolder",outputFoldername);

    using MedialCurveFilterType = itk::AnchoredMedialCurveImageFilter<DistanceImageType, FluxImageType>;
    typename MedialCurveFilterType::Pointer medialCurveFilter = MedialCurveFilterType::New();

    logger->Info("Computing medial curve\n");

    bool fixedEndpointsProvided = parser->GetCommandLineArgument("-endpoints", endpointsPath);

    if(fixedEndpointsProvided) {
        logger->Info("Using fixed endpoints from: " + endpointsPath + '\n');

        typename DistanceImageType::IndexType pixelIndex;
        std::vector<typename DistanceImageType::IndexType> endpoints;

        auto array = readCSV<typename DistanceImageType::PixelType>(endpointsPath);

        int numEndpoints = array.rows();
        int dimensions = array.columns();

        for(int endpointIndex = 0; endpointIndex<numEndpoints; endpointIndex++) {
            for(int dimension = 0; dimension<dimensions; dimension++) {
                pixelIndex[dimension] = array(endpointIndex, dimension);
            }

            endpoints.push_back(pixelIndex);
        }

        medialCurveFilter->SetEndpointLocations(endpoints);
    }

    medialCurveFilter->SetInput(distanceMap);
    medialCurveFilter->SetAverageOutwardFluxImage(aof);
    
    using OutputCastFilterType = itk::CastImageFilter<DistanceImageType, OutputImageType>;
    typename OutputCastFilterType::Pointer outputCastFilter = OutputCastFilterType::New();
    outputCastFilter->SetInput(medialCurveFilter->GetOutput());

    outputCastFilter->Update();

    typename OutputImageType::Pointer medialCurve = outputCastFilter->GetOutput();
    writeImage<OutputImageType>(fs::path(outputFoldername) / "medialCurve.mha", medialCurve);

    return medialCurve;
}

template<typename DistanceImageType, typename FluxImageType, typename OutputImageType>
typename OutputImageType::Pointer
computeMedialSurface(const itk::CommandLineArgumentParser::Pointer &parser,
                     const typename DistanceImageType::Pointer &distanceMap, 
                     const typename FluxImageType::Pointer &aof,
                     const itk::Logger::Pointer &logger){

    std::string outputFoldername;
    parser->GetCommandLineArgument("-outputFolder", outputFoldername);

    using ThresholdFilterType = itk::BinaryThresholdImageFilter< FluxImageType,OutputImageType>;
    typename ThresholdFilterType::Pointer thresholdFilter = ThresholdFilterType::New();

    thresholdFilter->SetLowerThreshold(-40*0.4);
    thresholdFilter->SetUpperThreshold(std::numeric_limits<typename FluxImageType::PixelType>::max());
    //outside value here is where the aof is sufficiently negative enought aka the skeleton
    thresholdFilter->SetOutsideValue(1);
    thresholdFilter->SetInsideValue(0);
    thresholdFilter->SetInput(aof);

    typename OutputImageType::Pointer thickSkeleton = thresholdFilter->GetOutput();
    thickSkeleton->Update();
    // writeImage<OutputImageType>(fs::path(outputFoldername) / "approxMedialSurfaceBefore.mha", thickSkeleton);
    //TODO: Correct post processing...
    using PostProcessor = itk::PostProcessSkeleton<OutputImageType>;
    typename PostProcessor::Pointer pp = PostProcessor::New();
    pp->SetInput(thickSkeleton);
    pp->Update();
    typename OutputImageType::Pointer medialSurface = pp->GetOutput();
    writeImage<OutputImageType>(fs::path(outputFoldername) / "medialSurfaceThreshold.mha", medialSurface);

    logger->Info("Thickness mapped medial surface computation\n");
    using MaskOutputFilterType = itk::MultiplyImageFilter<OutputImageType, DistanceImageType, DistanceImageType>;
    typename MaskOutputFilterType::Pointer maskSkeletonFilter = MaskOutputFilterType::New();
    maskSkeletonFilter->SetInput1(medialSurface);
    maskSkeletonFilter->SetInput2(distanceMap);
    typename MaskOutputFilterType::OutputImageType::Pointer medialSurfaceWithThickness = maskSkeletonFilter->GetOutput();
    std::string medialSurfaceWithThicknessFilename = fs::path(outputFoldername) / "medialSurfaceWithThickness.mha";
    writeImage<typename MaskOutputFilterType::OutputImageType>(medialSurfaceWithThicknessFilename, medialSurfaceWithThickness);

    std::string inputFilename;
    parser->GetCommandLineArgument("-input",inputFilename);

    mapWeightedSkeletonToBoundary(fs::path(outputFoldername) / "medialSurfaceWithThickness.mha", inputFilename);

    return medialSurface;
}


template<typename SkeletonImageType, typename BoundaryImageType>
typename SkeletonImageType::Pointer
skeletonToBoundaryMap(typename SkeletonImageType::Pointer weightedSkeleton, typename BoundaryImageType::Pointer boundary){
    std::cout << "Started Mapping skeltal weight to boundary" << std::endl;
    using SkeletonIteratorType = itk::ImageRegionConstIteratorWithIndex<SkeletonImageType>;
    SkeletonIteratorType skIt = SkeletonIteratorType(weightedSkeleton, weightedSkeleton->GetLargestPossibleRegion());

    using BoundaryIteratorType = itk::ImageRegionConstIteratorWithIndex<BoundaryImageType>;
    BoundaryIteratorType boundaryIt = BoundaryIteratorType(boundary, boundary->GetLargestPossibleRegion());

    typename SkeletonImageType::Pointer weightedBoundary = SkeletonImageType::New();
    weightedBoundary->SetOrigin(weightedSkeleton->GetOrigin());
    weightedBoundary->SetSpacing(weightedSkeleton->GetSpacing());
    weightedBoundary->SetRegions(weightedSkeleton->GetLargestPossibleRegion());
    weightedBoundary->Allocate();

    using OutputIteratorType = itk::ImageRegionIterator<SkeletonImageType>;
    OutputIteratorType outputIt = OutputIteratorType(weightedBoundary, weightedBoundary->GetLargestPossibleRegion());

    using MeasurementVectorType = itk::Point<float, 3>;
    using SampleType = itk::Statistics::ListSample<MeasurementVectorType>;

    typename SampleType::Pointer samples = SampleType::New();
    //samples->SetMeasurementVectorSize(2);

    skIt.GoToBegin();
    MeasurementVectorType mv;
    while(!skIt.IsAtEnd()){
        if(skIt.Get() <  -0.1) {
            weightedSkeleton->TransformIndexToPhysicalPoint(skIt.GetIndex(), mv);
            samples->PushBack(mv);
        }
        ++skIt;
    }
    std::cout << "Added Skeletal points to sample list" << std::endl;

    using TreeGeneratorType = itk::Statistics::KdTreeGenerator<SampleType>;
    typename TreeGeneratorType::Pointer treeGenerator = TreeGeneratorType::New();

    treeGenerator->SetSample(samples);
    treeGenerator->SetBucketSize(16);
    treeGenerator->Update();
    using TreeType = TreeGeneratorType::KdTreeType;
    //using NodeType = TreeType::KdTreeNodeType;

    typename TreeType::Pointer tree = treeGenerator->GetOutput();
    std::cout << "Generated Tree of size " << tree->Size() << std::endl;

    MeasurementVectorType queryPoint;
    unsigned int  numberOfNeighbors = 1;
    typename TreeType::InstanceIdentifierVectorType neighbors;
    typename SkeletonImageType::IndexType index;

    boundaryIt.GoToBegin();
    outputIt.GoToBegin();
    while(!boundaryIt.IsAtEnd()){
        if(boundaryIt.Get() > 0){
            //std::cout << "Mapping point " << boundaryIt.GetIndex();
            boundary->TransformIndexToPhysicalPoint(boundaryIt.GetIndex(), queryPoint);
            tree->Search(queryPoint, numberOfNeighbors, neighbors);
            mv = tree->GetMeasurementVector(neighbors[0]);
            weightedSkeleton->TransformPhysicalPointToIndex(mv, index);
            //std::cout << " -> " << index << std::endl;
            outputIt.Set(-1*weightedSkeleton->GetPixel(index));
        }else{
            outputIt.Set(0);
        }
        ++boundaryIt;
        ++outputIt;
    }
    std::cout << " Finished Mapping each boundary point " << std::endl;
    return weightedBoundary;
}


template<class TDistanceImage>
typename TDistanceImage::Pointer
computeAstrocyteSignedDistanceMapWithoutSpokes(const itk::CommandLineArgumentParser::Pointer &parser,
                                               const itk::Logger::Pointer &logger){
    logger->Info("Starting computation of m_Distance map without spokes\n");
    using AstrocyteImageType = itk::Image<unsigned char, 3>;
    using DistanceImageType = TDistanceImage;
    typename AstrocyteImageType::SpacingType spacing;
    spacing[0] = 4.1341146;
    spacing[1] = 4.1341146;
    spacing[2] = 8;
    std::string outputFolderName, inputFilename;
    parser->GetCommandLineArgument("-outputFolder", outputFolderName);
    parser->GetCommandLineArgument("-input", inputFilename);

    logger->Info("Reading astrocyte image...\n");
    //typename AstrocyteImageType::Pointer astrocyte = readImage<AstrocyteImageType>(outputFolderName + "astrocyte.mha");
    using AstrocyteReader =  itk::ImageFileReader<AstrocyteImageType>;
    typename AstrocyteReader::Pointer astrocyteReader = AstrocyteReader::New();
    astrocyteReader->SetFileName(inputFilename);
    using InformationChangeFilterType = itk::ChangeInformationImageFilter<AstrocyteImageType>;
    InformationChangeFilterType::Pointer astrocyte = InformationChangeFilterType::New();
    astrocyte->SetInput(astrocyteReader->GetOutput());
    astrocyte->SetOutputSpacing(spacing);
    astrocyte->ChangeSpacingOn();

    using GaussianFilterType = itk::DiscreteGaussianImageFilter<AstrocyteImageType , DistanceImageType >;
    typename GaussianFilterType::Pointer smoother = GaussianFilterType::New();
    smoother->SetInput(astrocyte->GetOutput());
    double variance[3] = {5, 5, 2.5};
    smoother->SetVariance(variance);
    smoother->SetUseImageSpacingOff();


    using ThresholdFilterType = itk::BinaryThresholdImageFilter< DistanceImageType , AstrocyteImageType >;
    typename ThresholdFilterType::Pointer thresholdFilter = ThresholdFilterType::New();

    thresholdFilter->SetLowerThreshold(0.5);
    thresholdFilter->SetUpperThreshold(std::numeric_limits<typename DistanceImageType::PixelType>::max());
    thresholdFilter->SetOutsideValue(1);
    thresholdFilter->SetInsideValue(0);
    thresholdFilter->SetInput(smoother->GetOutput());
    //thresholdFilter->Update();

    logger->Info("Started Signed m_Distance map computation \n");
    using SignedDistanceMapImageFilterType = itk::SignedMaurerDistanceMapImageFilter<AstrocyteImageType, DistanceImageType>;
    typename SignedDistanceMapImageFilterType::Pointer distanceMapImageFilter = SignedDistanceMapImageFilterType::New();
    distanceMapImageFilter->SetInput(thresholdFilter->GetOutput());
    // inside true because threshold inverts the astrocyte.
    distanceMapImageFilter->SetInsideIsPositive(true);
    logger->Info("Start writing cpt file \n");
    using WriterType = itk::ImageFileWriter<DistanceImageType>;
    typename WriterType::Pointer w = WriterType::New();
    // w->SetFileName(fs::path(outputFolderName) / "signedDistanceMap.mha");
    w->SetInput(distanceMapImageFilter->GetOutput());
    w->SetNumberOfStreamDivisions(32);
    w->Update();
    logger->Info("Wrote Signed distance function Field\n");
    typename DistanceImageType::Pointer distanceMap = distanceMapImageFilter->GetOutput();
    return distanceMap;
}


template<class TDistanceImage>
std::pair< typename TDistanceImage::Pointer,
        typename itk::Image<itk::Vector<float, TDistanceImage::ImageDimension>,TDistanceImage::ImageDimension>::Pointer>
computeAstrocyteSignedDistanceMap(const itk::CommandLineArgumentParser::Pointer &parser,
                                  const itk::Logger::Pointer &logger){
    logger->Info("Starting computation of m_Distance map \n");
    using AstrocyteImageType = itk::Image<unsigned char, 3>;
    using DistanceImageType = TDistanceImage;
    typename AstrocyteImageType::SpacingType spacing;
    spacing[0] = 4.1341146;
    spacing[1] = 4.1341146;
    spacing[2] = 8;
    std::string  inputFilename;
    parser->GetCommandLineArgument("-input", inputFilename);

    logger->Info("Reading input image...\n" );
    using ReaderType = itk::ImageFileReader<AstrocyteImageType>;
    ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName(inputFilename);
    reader->Update();

    using InformationChangeFilterTypeFloat = itk::ChangeInformationImageFilter<AstrocyteImageType>;
    InformationChangeFilterTypeFloat::Pointer changeSpacing = InformationChangeFilterTypeFloat::New();
    changeSpacing->SetInput(reader->GetOutput());
    changeSpacing->SetOutputSpacing(spacing);
    changeSpacing->ChangeSpacingOn();
    typename AstrocyteImageType::Pointer image = changeSpacing->GetOutput();

    using GaussianFilterType = itk::DiscreteGaussianImageFilter<AstrocyteImageType , DistanceImageType >;
    typename GaussianFilterType::Pointer smoother = GaussianFilterType::New();
    smoother->SetInput(image);
    double variance[3] = {5, 5, 2.5};
    smoother->SetVariance(variance);
    smoother->SetUseImageSpacingOff();


    using ThresholdFilterType = itk::BinaryThresholdImageFilter< DistanceImageType , AstrocyteImageType >;
    typename ThresholdFilterType::Pointer thresholdFilter = ThresholdFilterType::New();

    thresholdFilter->SetLowerThreshold(0.5);
    thresholdFilter->SetUpperThreshold(std::numeric_limits<typename ThresholdFilterType::InputImageType::PixelType>::max());
    thresholdFilter->SetOutsideValue(1);
    thresholdFilter->SetInsideValue(0);
    thresholdFilter->SetInput(smoother->GetOutput());

    logger->Info("Started Signed m_Distance map computation \n");
    using SignedDistanceMapImageFilterType = itk::SignedDanielssonDistanceMapImageFilter<AstrocyteImageType, DistanceImageType>;
    typename SignedDistanceMapImageFilterType::Pointer distanceMapImageFilter = SignedDistanceMapImageFilterType::New();
    distanceMapImageFilter->SetInput(thresholdFilter->GetOutput());
    // inside true because threshold inverts the astrocyte.
    distanceMapImageFilter->SetInsideIsPositive(true);
    distanceMapImageFilter->Update();
    typename DistanceImageType::Pointer distanceMap = distanceMapImageFilter->GetOutput();
    using OffSetImageType =  typename SignedDistanceMapImageFilterType::VectorImageType ;
    typename OffSetImageType::Pointer closestPointTransform = distanceMapImageFilter->GetVectorDistanceMap();

    //Casting Offset Image to Floating Point Vector field.
    using FieldImageType = itk::Image<itk::Vector<float>,TDistanceImage::ImageDimension>;
    typename FieldImageType::Pointer castField = FieldImageType::New();
    castField->SetOrigin(distanceMap->GetOrigin());
    castField->SetSpacing(distanceMap->GetSpacing());
    castField->SetRegions(distanceMap->GetLargestPossibleRegion());
    castField->Allocate();
    itk::ImageRegionIterator<FieldImageType > wit(castField, castField->GetLargestPossibleRegion());
    itk::ImageRegionIterator<DistanceImageType > dit(distanceMap, distanceMap->GetLargestPossibleRegion());
    itk::ImageRegionConstIterator<OffSetImageType> cpit(closestPointTransform, closestPointTransform->GetLargestPossibleRegion());
    typename FieldImageType::PixelType castValue;
    dit.GoToBegin();
    wit.GoToBegin();
    cpit.GoToBegin();
    while(!wit.IsAtEnd()){
    auto current = cpit.Get();
    auto dist = dit.Get();
    float multiplier = dist < -1.5*spacing[2] ? 1:0;
    for(size_t d = 0; d < OffSetImageType::ImageDimension; ++d){
    castValue[d] = static_cast<float>(current.GetElement(d)) * multiplier;
    }
    wit.Set(castValue);
    ++cpit;
    ++wit;
    ++dit;
    }

    std::pair<typename TDistanceImage::Pointer, typename FieldImageType::Pointer> retVal;
    retVal.first = distanceMap;
    retVal.second = castField;
    return retVal;
}




template<typename TInputValueType,
        unsigned int VImageDimension,
        typename TOutputValueType> //use std::conditional here..
typename itk::Image<TOutputValueType,VImageDimension>::Pointer
computeAOFFromSpokes(typename itk::Image<itk::Vector<TInputValueType, VImageDimension>, VImageDimension >::Pointer spokeField)
{
    std::cout << "Starting AOF computation using Spoke Vector Field" << std::endl;
    using OutputImageType = itk::Image<TOutputValueType,VImageDimension>;
    using SpokeFieldImageType = itk::Image<itk::Vector<TInputValueType, VImageDimension>, VImageDimension >;
    using AOFFilterType = itk::AverageOutwardFluxImageFilter2< SpokeFieldImageType, TOutputValueType >;
    typename AOFFilterType::Pointer aofFilter = AOFFilterType::New();
    aofFilter->SetInput(spokeField);
    aofFilter->Update();
    typename OutputImageType::Pointer aof = aofFilter->GetOutput();
    return aof;
}


template<class TDistanceImage>
typename TDistanceImage::Pointer computeAOF(const itk::CommandLineArgumentParser::Pointer &parser,
                                            const itk::Logger::Pointer &logger,
                                            bool lowMemory){
    using DistanceImageType = TDistanceImage;
    typename DistanceImageType::Pointer aof;
    std::string outputFolderName, inputFilename;
    parser->GetCommandLineArgument("-outputFolder", outputFolderName);
    parser->GetCommandLineArgument("-input", inputFilename);

    using Reader =  itk::ImageFileReader<DistanceImageType>;
    typename Reader::Pointer reader = Reader::New();
    // reader->SetFileName(fs::path(outputFolderName) / "signedDistanceMap.mha");
    typename DistanceImageType::SpacingType spacing;
    spacing[0] = 4.1341146;
    spacing[1] = 4.1341146;
    spacing[2] = 8;
    using InformationChangeFilterTypeFloat = itk::ChangeInformationImageFilter<DistanceImageType>;
    typename InformationChangeFilterTypeFloat::Pointer distanceMap = InformationChangeFilterTypeFloat::New();
    distanceMap->SetInput(reader->GetOutput());
    distanceMap->SetOutputSpacing(spacing);
    distanceMap->ChangeSpacingOn();
    std::cout << "Setup.. Gradient vector field..." << std::endl;
    using GradientFilterType = itk::GradientImageFilter<DistanceImageType >;
    typename GradientFilterType::Pointer gradientFilter = GradientFilterType::New();
    gradientFilter->SetInput(distanceMap->GetOutput());
    std::cout << "Starting AOF computation..." << std::endl;
    if(!lowMemory) {
        using AOFFilterType = itk::AverageOutwardFluxImageFilter<DistanceImageType>;
        typename AOFFilterType::Pointer aofFilter = AOFFilterType::New();
        aofFilter->SetInput(distanceMap->GetOutput());
        aofFilter->SetGradientImage(gradientFilter->GetOutput());
        aof = aofFilter->GetOutput();
        return aof;
    }else {
        std::cout << "Using Low memory aof computation will take longer. time" << std::endl;
        using AOFFilterType = itk::AverageOutwardFluxImageFilter3<typename GradientFilterType::OutputImageType, float>;
        typename AOFFilterType::Pointer aofFilter = AOFFilterType::New();
        aofFilter->SetInput(gradientFilter->GetOutput());
        using AstrocyteImageType = itk::Image<unsigned char, DistanceImageType::ImageDimension>;
        using AstrocyteReader =  itk::ImageFileReader<AstrocyteImageType>;
        typename AstrocyteReader::Pointer astrocyteReader = AstrocyteReader::New();
        astrocyteReader->SetFileName(inputFilename);
        using InformationChangeFilterType = itk::ChangeInformationImageFilter<AstrocyteImageType>;
        typename InformationChangeFilterType::Pointer astrocyte = InformationChangeFilterType::New();
        astrocyte->SetInput(astrocyteReader->GetOutput());
        astrocyte->SetOutputSpacing(spacing);
        astrocyte->ChangeSpacingOn();
        using MultiplyImageFilter = itk::MultiplyImageFilter<AstrocyteImageType ,DistanceImageType ,DistanceImageType>;
        typename MultiplyImageFilter::Pointer multiplyFilter = MultiplyImageFilter::New();
        multiplyFilter->SetInput1(astrocyte->GetOutput());
        multiplyFilter->SetInput2(aofFilter->GetOutput());
        // writeImage<DistanceImageType >(fs::path(outputFolderName) / "aof.mha",multiplyFilter->GetOutput(),8);
        return multiplyFilter->GetOutput();
    }
}


template<class TDistanceImage>
typename TDistanceImage::Pointer computeAOF(typename TDistanceImage::Pointer signedDistanceFunction, bool lowMemory){
    using DistanceImageType = TDistanceImage;
    typename DistanceImageType::Pointer aof;
    std::cout << "Computing Gradient vector field..." << std::endl;
    using GradientFilterType = itk::GradientImageFilter<DistanceImageType >;
    typename GradientFilterType::Pointer gradientFilter = GradientFilterType::New();
    gradientFilter->SetInput(signedDistanceFunction);
    //gradientFilter->Update();

    std::cout << "Starting AOF computation..." << std::endl;
    if(!lowMemory) {
        using AOFFilterType = itk::AverageOutwardFluxImageFilter<DistanceImageType>;
        typename AOFFilterType::Pointer aofFilter = AOFFilterType::New();
        aofFilter->SetInput(signedDistanceFunction);
        aofFilter->SetGradientImage(gradientFilter->GetOutput());
        aofFilter->Update();
        aof = aofFilter->GetOutput();
        return aof;
    }else {
        std::cout << "Using Low memory aof computation will take longer. time" << std::endl;
        using AOFFilterType = itk::AverageOutwardFluxImageFilter3<typename GradientFilterType::OutputImageType, float>;
        typename AOFFilterType::Pointer aofFilter = AOFFilterType::New();
        aofFilter->SetInput(gradientFilter->GetOutput());

        using ThresholdFilterType = itk::BinaryThresholdImageFilter<DistanceImageType, DistanceImageType >;
        typename ThresholdFilterType::Pointer distanceMaskFilter = ThresholdFilterType::New();
        distanceMaskFilter->SetInput(signedDistanceFunction);
        distanceMaskFilter->SetLowerThreshold(std::numeric_limits<float>::lowest());
        distanceMaskFilter->SetUpperThreshold(-8.5);
        distanceMaskFilter->SetOutsideValue(0.0);
        distanceMaskFilter->SetInsideValue(1.0);


        using MultiplyImageFilter = itk::MultiplyImageFilter<DistanceImageType,DistanceImageType ,DistanceImageType >;
        typename MultiplyImageFilter::Pointer multiplyFilter = MultiplyImageFilter::New();
        multiplyFilter->SetInput1(distanceMaskFilter->GetOutput());
        multiplyFilter->SetInput2(aofFilter->GetOutput());
        multiplyFilter->Update();

        return multiplyFilter->GetOutput();
    }
}
#endif //SKELTOOLS_MEDIAL_HXX
