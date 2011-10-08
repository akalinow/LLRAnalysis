import FWCore.ParameterSet.Config as cms
import copy

from TauAnalysis.CandidateTools.tools.objProdConfigurator import *
from TauAnalysis.CandidateTools.resolutions_cfi import *
from TauAnalysis.CandidateTools.nSVfitAlgorithmDiTau_cfi import *
from TauAnalysis.CandidateTools.nSVfitAlgorithmTauDecayKineMC_cfi import *
from RecoMET.METProducers.METSigParams_cfi import *

#--------------------------------------------------------------------------------
# produce combinations of electron + tau-jet pairs
#--------------------------------------------------------------------------------

allElecTauPairs = cms.EDProducer(
    "PATElecTauPairProducer",
    useLeadingTausOnly = cms.bool(False),
    srcLeg1 = cms.InputTag('selectedPatElectrons'),
    srcLeg2 = cms.InputTag('selectedPatTaus'),
    dRmin12 = cms.double(0.3),
    srcMET = cms.InputTag('patMETs'),
    srcPrimaryVertex = cms.InputTag("offlinePrimaryVertices"),
    srcBeamSpot = cms.InputTag("offlineBeamSpot"),
    srcGenParticles = cms.InputTag('genParticles'),
    recoMode = cms.string(""),
    doSVreco = cms.bool(True),
    nSVfit = cms.PSet(),
    scaleFuncImprovedCollinearApprox = cms.string('1'),
    doPFMEtSign = cms.bool(True),
    pfMEtSign = cms.PSet(
    srcPFJets = cms.InputTag('ak5PFJets'),
    srcPFCandidates = cms.InputTag('particleFlow'),
    resolution = METSignificance_params,
    dRoverlapPFJet = cms.double(0.3),
    dRoverlapPFCandidate = cms.double(0.1)
    ),
    verbosity = cms.untracked.int32(0),
    )


allElecTauPairs.nSVfit.psKine_MEt_logM_fit = cms.PSet()
allElecTauPairs.nSVfit.psKine_MEt_logM_fit.config = copy.deepcopy(nSVfitConfig_template)
allElecTauPairs.nSVfit.psKine_MEt_logM_fit.config.event.resonances.A.daughters.leg1 = cms.PSet(
        src = allElecTauPairs.srcLeg1,
            likelihoodFunctions = cms.VPSet(nSVfitElectronLikelihoodPhaseSpace),
            builder = nSVfitTauToElecBuilder
        )
allElecTauPairs.nSVfit.psKine_MEt_logM_fit.config.event.resonances.A.daughters.leg2 = cms.PSet(
        src = allElecTauPairs.srcLeg2,
            likelihoodFunctions = cms.VPSet(nSVfitTauLikelihoodPhaseSpace),
            builder = nSVfitTauToHadBuilder
        )
allElecTauPairs.nSVfit.psKine_MEt_logM_fit.algorithm = cms.PSet(
        pluginName = cms.string("nSVfitAlgorithmByLikelihoodMaximization"),
            pluginType = cms.string("NSVfitAlgorithmByLikelihoodMaximization"),
            minimizer  = cms.vstring("Minuit2", "Migrad"),
            maxObjFunctionCalls = cms.uint32(5000),
            verbosity = cms.int32(0)
        )


allElecTauPairs.nSVfit.psKine_MEt_logM_int = cms.PSet()
allElecTauPairs.nSVfit.psKine_MEt_logM_int.config = allElecTauPairs.nSVfit.psKine_MEt_logM_fit.config
allElecTauPairs.nSVfit.psKine_MEt_logM_int.algorithm = nSVfitProducerByIntegration.algorithm
'''
cms.PSet(
        pluginName = cms.string("nSVfitAlgorithmByIntegration"),
            pluginType = cms.string("NSVfitAlgorithmByIntegration"),
            parameters = cms.PSet(
            mass_A = cms.PSet(
                min = cms.double(20.),
                            max = cms.double(750.),
                            stepSize = cms.double(5.),
                            replace = cms.string("leg1.x"),
                            by = cms.string("(A.p4.mass/mass_A)*(A.p4.mass/mass_A)/leg2.x")
                        )
                ),
            vegasOptions = cms.PSet(
            numCalls = cms.uint32(10000)
                )
        )
'''
#--------------------------------------------------------------------------------

elecTauPairProdConfigurator = objProdConfigurator(
            allElecTauPairs,
            pyModuleName = __name__
            )

produceElecTauPairs = elecTauPairProdConfigurator.configure(pyNameSpace = locals())

##################################################################################
#--------------------------------------------------------------------------------
# produce combinations of muon + tau-jet pairs
#--------------------------------------------------------------------------------
##################################################################################

allMuTauPairs = cms.EDProducer(
    "PATMuTauPairProducer",
    useLeadingTausOnly = cms.bool(False),
    srcLeg1 = cms.InputTag('selectedPatMuons'),
    srcLeg2 = cms.InputTag('selectedPatTaus'),
    dRmin12 = cms.double(0.3),
    srcMET = cms.InputTag('patMETs'),
    srcPrimaryVertex = cms.InputTag("offlinePrimaryVertices"),
    srcBeamSpot = cms.InputTag("offlineBeamSpot"),
    srcGenParticles = cms.InputTag('genParticles'),
    recoMode = cms.string(""),
    doSVreco = cms.bool(True),
    nSVfit = cms.PSet(),
    scaleFuncImprovedCollinearApprox = cms.string('1'),
    doPFMEtSign = cms.bool(True),
    pfMEtSign = cms.PSet(
    srcPFJets = cms.InputTag('ak5PFJets'),
    srcPFCandidates = cms.InputTag('particleFlow'),
    resolution = METSignificance_params,
    dRoverlapPFJet = cms.double(0.3),
    dRoverlapPFCandidate = cms.double(0.1)
    ),
    verbosity = cms.untracked.int32(0)
    )

#--------------------------------------------------------------------------------
# configure (new) SVfit algorithm
# (using combination of PS + MET likelihoods + logM regularization term
#  to reconstruct mass of tau lepton pair, as described in CMS AN-11-165)
allMuTauPairs.nSVfit.psKine_MEt_logM_fit = cms.PSet()
allMuTauPairs.nSVfit.psKine_MEt_logM_fit.config = copy.deepcopy(nSVfitConfig_template)
allMuTauPairs.nSVfit.psKine_MEt_logM_fit.config.event.resonances.A.daughters.leg1 = cms.PSet(
        src = allMuTauPairs.srcLeg1,
            likelihoodFunctions = cms.VPSet(nSVfitMuonLikelihoodPhaseSpace),
            builder = nSVfitTauToMuBuilder
        )
allMuTauPairs.nSVfit.psKine_MEt_logM_fit.config.event.resonances.A.daughters.leg2 = cms.PSet(
        src = allMuTauPairs.srcLeg2,
            likelihoodFunctions = cms.VPSet(nSVfitTauLikelihoodPhaseSpace),
            builder = nSVfitTauToHadBuilder
        )
allMuTauPairs.nSVfit.psKine_MEt_logM_fit.algorithm = cms.PSet(
        pluginName = cms.string("nSVfitAlgorithmByLikelihoodMaximization"),
            pluginType = cms.string("NSVfitAlgorithmByLikelihoodMaximization"),
            minimizer  = cms.vstring("Minuit2", "Migrad"),
            maxObjFunctionCalls = cms.uint32(5000),
            verbosity = cms.int32(0)
        )


allMuTauPairs.nSVfit.psKine_MEt_logM_int = cms.PSet()
allMuTauPairs.nSVfit.psKine_MEt_logM_int.config = allMuTauPairs.nSVfit.psKine_MEt_logM_fit.config
allMuTauPairs.nSVfit.psKine_MEt_logM_int.algorithm = nSVfitProducerByIntegration.algorithm
'''
cms.PSet(
    pluginName = cms.string("nSVfitAlgorithmByIntegration"),
    pluginType = cms.string("NSVfitAlgorithmByIntegration"),
    parameters = cms.PSet(
    mass_A = cms.PSet(
    min = cms.double(20.),
    max = cms.double(750.),
    #stepSize = cms.double(5.),
    stepSizeFactor = cms.double(1.025), # nextM = max(stepSizeFactor*currentM, minStepSize)
    minStepSize = cms.double(2.5),      
    replace = cms.string("leg1.x"),
    by = cms.string("(A.p4.mass/mass_A)*(A.p4.mass/mass_A)/leg2.x")
    )
    ),
    vegasOptions = cms.PSet(
    #numCalls = cms.uint32(10000)
    numCallsGridOpt = cms.uint32(1000),
    numCallsIntEval = cms.uint32(10000),
    maxChi2         = cms.double(2.),
    maxIntEvalIter  = cms.uint32(5),                                          
    precision       = cms.double(0.00001)
    )
    )
'''

muTauPairProdConfigurator = objProdConfigurator(
    allMuTauPairs,
    pyModuleName = __name__
    )

produceMuTauPairs = muTauPairProdConfigurator.configure(pyNameSpace = locals())

##################################################################################
#--------------------------------------------------------------------------------
# produce combinations of electron + muon pairs
#--------------------------------------------------------------------------------
##################################################################################

allElecMuPairs = cms.EDProducer(
    "PATElecMuPairProducer",
    useLeadingTausOnly = cms.bool(False),
    srcLeg1 = cms.InputTag('selectedPatElectrons'),
    srcLeg2 = cms.InputTag('selectedPatMuons'),
    dRmin12 = cms.double(0.3),
    srcMET = cms.InputTag('patMETs'),
    srcPrimaryVertex = cms.InputTag("offlinePrimaryVertices"),
    srcBeamSpot = cms.InputTag("offlineBeamSpot"),
    srcGenParticles = cms.InputTag('genParticles'),
    recoMode = cms.string(""),
    doSVreco = cms.bool(True),
    nSVfit = cms.PSet(),
    scaleFuncImprovedCollinearApprox = cms.string('1'),
    doPFMEtSign = cms.bool(True),
    pfMEtSign = cms.PSet(
    srcPFJets = cms.InputTag('ak5PFJets'),
    srcPFCandidates = cms.InputTag('particleFlow'),
    resolution = METSignificance_params,
    dRoverlapPFJet = cms.double(0.3),
    dRoverlapPFCandidate = cms.double(0.1)
    ),
    verbosity = cms.untracked.int32(0),
    )


allElecMuPairs.nSVfit.psKine_MEt_logM_fit = cms.PSet()
allElecMuPairs.nSVfit.psKine_MEt_logM_fit.config = copy.deepcopy(nSVfitConfig_template)
allElecMuPairs.nSVfit.psKine_MEt_logM_fit.config.event.resonances.A.daughters.leg1 = cms.PSet(
        src = allElecMuPairs.srcLeg1,
            likelihoodFunctions = cms.VPSet(nSVfitElectronLikelihoodPhaseSpace),
            builder = nSVfitTauToElecBuilder
        )
allElecMuPairs.nSVfit.psKine_MEt_logM_fit.config.event.resonances.A.daughters.leg2 = cms.PSet(
        src = allElecMuPairs.srcLeg2,
            likelihoodFunctions = cms.VPSet(nSVfitMuonLikelihoodPhaseSpace),
            builder = nSVfitTauToHadBuilder
        )
allElecMuPairs.nSVfit.psKine_MEt_logM_fit.algorithm = cms.PSet(
        pluginName = cms.string("nSVfitAlgorithmByLikelihoodMaximization"),
            pluginType = cms.string("NSVfitAlgorithmByLikelihoodMaximization"),
            minimizer  = cms.vstring("Minuit2", "Migrad"),
            maxObjFunctionCalls = cms.uint32(5000),
            verbosity = cms.int32(0)
        )


allElecMuPairs.nSVfit.psKine_MEt_logM_int = cms.PSet()
allElecMuPairs.nSVfit.psKine_MEt_logM_int.config = allElecMuPairs.nSVfit.psKine_MEt_logM_fit.config
allElecMuPairs.nSVfit.psKine_MEt_logM_int.algorithm = nSVfitProducerByIntegration.algorithm
'''
cms.PSet(
        pluginName = cms.string("nSVfitAlgorithmByIntegration"),
            pluginType = cms.string("NSVfitAlgorithmByIntegration"),
            parameters = cms.PSet(
            mass_A = cms.PSet(
                min = cms.double(20.),
                            max = cms.double(750.),
                            stepSize = cms.double(5.),
                            replace = cms.string("leg1.x"),
                            by = cms.string("(A.p4.mass/mass_A)*(A.p4.mass/mass_A)/leg2.x")
                        )
                ),
            vegasOptions = cms.PSet(
            numCalls = cms.uint32(10000)
                )
        )
'''
#--------------------------------------------------------------------------------

elecMuPairProdConfigurator = objProdConfigurator(
    allElecMuPairs,
    pyModuleName = __name__
    )

produceElecMuPairs = elecTauPairProdConfigurator.configure(pyNameSpace = locals())

##################################################################################
