import FWCore.ParameterSet.Config as cms
import copy

from TauAnalysis.CandidateTools.tools.objProdConfigurator import *
from TauAnalysis.CandidateTools.resolutions_cfi import *
from TauAnalysis.CandidateTools.nSVfitAlgorithmDiTau_cfi import *
from TauAnalysis.CandidateTools.nSVfitAlgorithmTauDecayKineMC_cfi import *
from RecoMET.METProducers.METSigParams_cfi import *
from TauAnalysis.CandidateTools.nSVfitAlgorithmVisPtCutCorrections_cfi import *

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
    doMtautauMin = cms.bool(False),   
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


############# Update to latest version

nSVfitResonanceLikelihoodPolarization.LR.formula = cms.string("[0]")
nSVfitResonanceLikelihoodPolarization.RL.formula = cms.string("[0]")
nSVfitResonanceLikelihoodPolarization.LL.formula = cms.string("[0]")
nSVfitResonanceLikelihoodPolarization.RR.formula = cms.string("[0]")

#nSVfitResonanceLikelihoodPolarization
allElecTauPairs.nSVfit.psKine_MEt_logM_fit.config.event.resonances.A.likelihoodFunctions = cms.VPSet()

nSVfitTauLikelihoodMatrixElement.applySinThetaFactor       = cms.bool(False)
nSVfitTauLikelihoodPhaseSpace.applySinThetaFactor          = cms.bool(False)
nSVfitElectronLikelihoodPhaseSpace.applySinThetaFactor     = cms.bool(False)
nSVfitElectronLikelihoodMatrixElement.applySinThetaFactor  = cms.bool(False)

allElecTauPairs.nSVfit.psKine_MEt_logM_fit.config.event.resonances.A.daughters.leg1.likelihoodFunctions = cms.VPSet(nSVfitElectronLikelihoodMatrixElement)
#allElecTauPairs.nSVfit.psKine_MEt_logM_fit.config.event.resonances.A.daughters.leg1.likelihoodFunctions = cms.VPSet(nSVfitElectronLikelihoodPhaseSpace)
allElecTauPairs.nSVfit.psKine_MEt_logM_fit.config.event.resonances.A.daughters.leg1.likelihoodFunctions[0].applyVisPtCutCorrection = cms.bool(False)
allElecTauPairs.nSVfit.psKine_MEt_logM_fit.config.event.resonances.A.daughters.leg1.likelihoodFunctions[0].visPtCutThreshold = cms.double(15)
allElecTauPairs.nSVfit.psKine_MEt_logM_fit.config.event.resonances.A.daughters.leg1.likelihoodFunctions[0].visPtCutCorrection = visPtCutCorrectionMuon15

#allElecTauPairs.nSVfit.psKine_MEt_logM_fit.config.event.resonances.A.daughters.leg2.likelihoodFunctions = cms.VPSet(nSVfitTauLikelihoodMatrixElement)
allElecTauPairs.nSVfit.psKine_MEt_logM_fit.config.event.resonances.A.daughters.leg2.likelihoodFunctions = cms.VPSet(nSVfitTauLikelihoodPhaseSpace)
allElecTauPairs.nSVfit.psKine_MEt_logM_fit.config.event.resonances.A.daughters.leg2.likelihoodFunctions[0].applyVisPtCutCorrection = cms.bool(False)
allElecTauPairs.nSVfit.psKine_MEt_logM_fit.config.event.resonances.A.daughters.leg2.likelihoodFunctions[0].visPtCutThreshold = cms.double(20)
allElecTauPairs.nSVfit.psKine_MEt_logM_fit.config.event.resonances.A.daughters.leg2.likelihoodFunctions[0].visPtCutCorrection = visPtCutCorrectionTau20

#allElecTauPairs.nSVfit.psKine_MEt_logM_fit.config.event.resonances.A.builder.polStates = cms.vstring("RR","LL")
allElecTauPairs.nSVfit.psKine_MEt_logM_fit.config.event.resonances.A.builder.polStates = cms.vstring("undefined")

allElecTauPairs.nSVfit.psKine_MEt_logM_int = cms.PSet()
allElecTauPairs.nSVfit.psKine_MEt_logM_int.config = allElecTauPairs.nSVfit.psKine_MEt_logM_fit.config
allElecTauPairs.nSVfit.psKine_MEt_logM_int.algorithm = nSVfitProducerByIntegration.algorithm

'''
allElecTauPairs.nSVfit.psKine_MEt_logM_int = cms.PSet()
allElecTauPairs.nSVfit.psKine_MEt_logM_int.config = allElecTauPairs.nSVfit.psKine_MEt_logM_fit.config
allElecTauPairs.nSVfit.psKine_MEt_logM_int.algorithm = nSVfitProducerByIntegration.algorithm
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
    doMtautauMin = cms.bool(False),       
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

############# Update to latest version

nSVfitResonanceLikelihoodPolarization.LR.formula = cms.string("[0]")
nSVfitResonanceLikelihoodPolarization.RL.formula = cms.string("[0]")
nSVfitResonanceLikelihoodPolarization.LL.formula = cms.string("[0]")
nSVfitResonanceLikelihoodPolarization.RR.formula = cms.string("[0]")

#nSVfitResonanceLikelihoodPolarization
allMuTauPairs.nSVfit.psKine_MEt_logM_fit.config.event.resonances.A.likelihoodFunctions = cms.VPSet()

nSVfitTauLikelihoodMatrixElement.applySinThetaFactor   = cms.bool(False)
nSVfitTauLikelihoodPhaseSpace.applySinThetaFactor      = cms.bool(False)
nSVfitMuonLikelihoodPhaseSpace.applySinThetaFactor     = cms.bool(False)
nSVfitMuonLikelihoodMatrixElement.applySinThetaFactor  = cms.bool(False)

allMuTauPairs.nSVfit.psKine_MEt_logM_fit.config.event.resonances.A.daughters.leg1.likelihoodFunctions = cms.VPSet(nSVfitMuonLikelihoodMatrixElement)
#allMuTauPairs.nSVfit.psKine_MEt_logM_fit.config.event.resonances.A.daughters.leg1.likelihoodFunctions = cms.VPSet(nSVfitMuonLikelihoodPhaseSpace)
allMuTauPairs.nSVfit.psKine_MEt_logM_fit.config.event.resonances.A.daughters.leg1.likelihoodFunctions[0].applyVisPtCutCorrection = cms.bool(False)
allMuTauPairs.nSVfit.psKine_MEt_logM_fit.config.event.resonances.A.daughters.leg1.likelihoodFunctions[0].visPtCutThreshold = cms.double(15)
allMuTauPairs.nSVfit.psKine_MEt_logM_fit.config.event.resonances.A.daughters.leg1.likelihoodFunctions[0].visPtCutCorrection = visPtCutCorrectionMuon15

#allMuTauPairs.nSVfit.psKine_MEt_logM_fit.config.event.resonances.A.daughters.leg2.likelihoodFunctions = cms.VPSet(nSVfitTauLikelihoodMatrixElement)
allMuTauPairs.nSVfit.psKine_MEt_logM_fit.config.event.resonances.A.daughters.leg2.likelihoodFunctions = cms.VPSet(nSVfitTauLikelihoodPhaseSpace)
allMuTauPairs.nSVfit.psKine_MEt_logM_fit.config.event.resonances.A.daughters.leg2.likelihoodFunctions[0].applyVisPtCutCorrection = cms.bool(False)
allMuTauPairs.nSVfit.psKine_MEt_logM_fit.config.event.resonances.A.daughters.leg2.likelihoodFunctions[0].visPtCutThreshold = cms.double(20)
allMuTauPairs.nSVfit.psKine_MEt_logM_fit.config.event.resonances.A.daughters.leg2.likelihoodFunctions[0].visPtCutCorrection = visPtCutCorrectionTau20

#allMuTauPairs.nSVfit.psKine_MEt_logM_fit.config.event.resonances.A.builder.polStates = cms.vstring("RR","LL")
allMuTauPairs.nSVfit.psKine_MEt_logM_fit.config.event.resonances.A.builder.polStates = cms.vstring("undefined")


allMuTauPairs.nSVfit.psKine_MEt_logM_int = cms.PSet()
allMuTauPairs.nSVfit.psKine_MEt_logM_int.config = allMuTauPairs.nSVfit.psKine_MEt_logM_fit.config
allMuTauPairs.nSVfit.psKine_MEt_logM_int.algorithm = nSVfitProducerByIntegration.algorithm


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

'''
allElecMuPairs.nSVfit.psKine_MEt_logM_int = cms.PSet()
allElecMuPairs.nSVfit.psKine_MEt_logM_int.config = allElecMuPairs.nSVfit.psKine_MEt_logM_fit.config
allElecMuPairs.nSVfit.psKine_MEt_logM_int.algorithm = nSVfitProducerByIntegration.algorithm
'''
#--------------------------------------------------------------------------------

elecMuPairProdConfigurator = objProdConfigurator(
    allElecMuPairs,
    pyModuleName = __name__
    )

produceElecMuPairs = elecTauPairProdConfigurator.configure(pyNameSpace = locals())

##################################################################################
