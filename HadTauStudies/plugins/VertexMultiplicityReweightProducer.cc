#include "LLRAnalysis/HadTauStudies/plugins/VertexMultiplicityReweightProducer.h"

VertexMultiplicityReweightProducer::VertexMultiplicityReweightProducer(const edm::ParameterSet& cfg)
  : extractor_(0)
{
  std::string pluginType = "VertexMultiplicityReweightExtractor";
  extractor_ = ObjValExtractorPluginFactory::get()->create(pluginType, cfg);

  produces<double>("");
}

VertexMultiplicityReweightProducer::~VertexMultiplicityReweightProducer() 
{
  delete extractor_;
}

void VertexMultiplicityReweightProducer::produce(edm::Event& evt, const edm::EventSetup& es)
{
  std::auto_ptr<double> weightPtr(new double((*extractor_)(evt)));
  evt.put(weightPtr);
}

#include "FWCore/Framework/interface/MakerMacros.h"

DEFINE_FWK_MODULE(VertexMultiplicityReweightProducer);


