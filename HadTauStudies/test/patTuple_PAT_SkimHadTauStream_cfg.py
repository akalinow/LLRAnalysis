###############################################################################
## Pat-tuple for tau+tau analysis
## 
## Last modificaion: 30/11/2013 Christian
## TODO: synch tests
##
##
###############################################################################

from PhysicsTools.PatAlgos.patTemplate_cfg import *

process.load('Configuration.StandardSequences.Services_cff')
#MB needed?:
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
process.load('JetMETCorrections.Configuration.DefaultJEC_cff')
process.load("JetMETCorrections.Configuration.JetCorrectionServices_cff")

postfix     = "PFlow"
runOnMC     = True
runOnEmbed  = False
embedType   = "RhEmbed" #"PfEmbed" or "RhEmbed"

#from Configuration.PyReleaseValidation.autoCond import autoCond
#process.GlobalTag.globaltag = cms.string( autoCond[ 'startup' ] )

if runOnMC:
    process.GlobalTag.globaltag = cms.string('START53_V23::All')
else:
    process.GlobalTag.globaltag = cms.string('FT_53_V21_AN4::All')

#--------------------------------------------------------------------------------
# define "hooks" for replacing configuration parameters
# in case running jobs on the CERN batch system/grid
#
#__runOnMC = $runOnMC
#__runOnEmbed = $runOnEmbed
#__embedType = $embedType
#
#--------------------------------------------------------------------------------

process.options   = cms.untracked.PSet( wantSummary = cms.untracked.bool(True))
process.MessageLogger.cerr.FwkReport.reportEvery = 1000

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )

process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(
        ##'/store/user/veelken/CMSSW_5_3_x/skims/simZprime2500toTauTau_RECO_2_2_0t6.root'
        'file:/data1/veelken/CMSSW_5_3_x/skims/selEvents_simHiggsSUSYGluGlu130_tautau_selEventFromRiccardo_AOD.root'
    ),
    dropDescendantsOfDroppedBranches=cms.untracked.bool(False),
    inputCommands=cms.untracked.vstring(
        'keep *',
        'drop patTaus_*_*_*',
        'drop *PFTau*_*_*_*'
    )
)


#process.add_(cms.Service("PrintLoadingPlugins"))

#process.source.eventsToProcess = cms.untracked.VEventRange(
#    '1:751063'
#    )
################### event content ##################

process.printEventContent = cms.EDAnalyzer("EventContentAnalyzer")

################### filters log  ####################

process.allEventsCounter = cms.EDFilter("AllEventsFilter")

process.primaryVertexCounter = cms.EDFilter("AllEventsFilter")

process.atLeastOneDiTauCounter = cms.EDFilter("AllEventsFilter")

process.atLeastTwoTausPtEtaIDCounter = cms.EDFilter("AllEventsFilter")
process.atLeastTwoTausPtEtaIDRelIsoCounter = cms.EDFilter("AllEventsFilter")
process.atLeastOneTauPtEtaIDAgMuFilter = cms.EDFilter("AllEventsFilter")
process.atLeastOneTauPtEtaIDAgMuAgElecCounter = cms.EDFilter("AllEventsFilter")

################### HLT trigger  ####################

process.hltFilterSequence = cms.Sequence()
if runOnEmbed :
    # CV: In case of Embedded samples do *not* apply diTau triggers,
    #     as HLT trigger paths are not properly simulated for "hybrid" event in case of PF/RH Embedding.
    #     Instead, weight Embedded events by trigger efficiency turn-on.
    #     Require Embedded events to pass diMuon trigger,
    #     as HLT trigger selection cuts have not been applied during Embedded sample production.
    process.HLTFilterMuMu = cms.EDFilter("HLTHighLevel",
        TriggerResultsTag = cms.InputTag("TriggerResults", "", "HLT"),
        HLTPaths = cms.vstring(
            'HLT_Mu17_Mu8_v*',
        ),
        eventSetupPathsKey = cms.string(''),
        andOr = cms.bool(True), # OR
        throw = cms.bool(False)
    )
    process.hltFilterSequence += process.HLTFilterMuMu
else :
    process.HLTFilterTauTau = cms.EDFilter("HLTHighLevel",
        TriggerResultsTag = cms.InputTag("TriggerResults", "", "HLT"),
        HLTPaths = cms.vstring(
            "HLT_DoubleMediumIsoPFTau35_Trk5_eta2p1_v*",
            "HLT_DoubleMediumIsoPFTau35_Trk1_eta2p1_v*",
            "HLT_DoubleMediumIsoPFTau30_Trk5_eta2p1_Jet30_v*",
            "HLT_DoubleMediumIsoPFTau30_Trk1_eta2p1_Jet30_v*"
        ),
        eventSetupPathsKey = cms.string(''),
        andOr = cms.bool(True), # OR
        throw = cms.bool(False)
    )
    process.hltFilterSequence += process.HLTFilterTauTau

### cone-based tau to mimic trigger //CUSTOM
process.load("LLRAnalysis.TauTauStudies.emulateHLTPFTau_cff")

################### gen listing  ####################

process.load("SimGeneral.HepPDTESSource.pythiapdt_cfi")
process.printTree1 = cms.EDAnalyzer("ParticleListDrawer",
    src = cms.InputTag("genParticles"),
    maxEventsToPrint  = cms.untracked.int32(1)
)

################### jet sequence ####################
#MB needed??
process.load('RecoJets.Configuration.RecoPFJets_cff')

################### bTag ##############################

process.load('RecoBTag.Configuration.RecoBTag_cff')
process.load('RecoJets.JetAssociationProducers.ak5JTA_cff')
if runOnEmbed:
    #process.load('RecoBTag.Configuration.RecoBTag_cff')
    #process.load('RecoJets.JetAssociationProducers.ak5JTA_cff')
    process.ak5JetTracksAssociatorAtVertex.jets   = cms.InputTag("ak5PFJets")
    if "PfEmbed" in embedType:
        process.ak5JetTracksAssociatorAtVertex.tracks = cms.InputTag("tmfTracks")

## Plus, add this to your path:
#process.ak5JetTracksAssociatorAtVertex*process.btagging

################### vertex sequence ####################

process.selectedPrimaryVertices = cms.EDFilter("VertexSelector",
    src = cms.InputTag('offlinePrimaryVertices'),
    cut = cms.string("isValid & ndof >= 4 & z > -24 & z < +24 & position.Rho < 2."),
    filter = cms.bool(False)                                          
)

process.primaryVertexFilter = cms.EDFilter("VertexCountFilter",
    src = cms.InputTag('selectedPrimaryVertices'),
    minNumber = cms.uint32(1),
    maxNumber = cms.uint32(999),
)

################### pat specific ####################

from PhysicsTools.PatAlgos.tools.helpers import massSearchReplaceAnyInputTag

from PhysicsTools.PatAlgos.tools.coreTools import *
if not runOnMC:
    removeMCMatching(process, ["All"])

removeSpecificPATObjects(process, ['Photons'], outputInProcess = None)
removeCleaning(process, outputInProcess = None)

restrictInputToAOD(process, ['All'])

from LLRAnalysis.Utilities.customizePAT  import *
addSelectedPFlowParticle(process)

from PhysicsTools.PatAlgos.tools.metTools import *
addPfMET(process, postfix)
'''
##MB standard corrections for PFMET
##MB FIXME: check with Christian
##MB FIXME: are the following corrections properly stacked??
#type-0 + type-1
process.pfType1CorrectedMet.applyType0Corrections = False #->added by other meas(see below). Is it taken by PatMEt as correction level which can be removed?
process.pfType1CorrectedMet.applyType1Corrections = True
process.pfType1CorrectedMet.applyType2Corrections = False
#define type-1
process.pfJetMETcorr.skipEM=False
process.pfJetMETcorr.skipMuons=False
process.pfJetMETcorr.skipMuonSelection='isGlobalMuon' #or default 'isGlobalMuon | isStandAloneMuon'?
#define type-0
process.load("JetMETCorrections.Type1MET.pfMETCorrectionType0_cfi")
process.pfType1CorrectedMet.srcType1Corrections = cms.VInputTag(
    cms.InputTag('pfMETcorrType0'),
    cms.InputTag('pfJetMETcorr', 'type1')        
)
process.producePFMETCorrections.replace(process.pfType1CorrectedMet,
                                        process.type0PFMEtCorrection+process.pfType1CorrectedMet)
#MET x/y Shift Correction (for phi modulation)
process.load("JetMETCorrections.Type1MET.pfMETsysShiftCorrections_cfi")
if (not runOnMC) or runOnEmbed:
    process.pfMEtSysShiftCorr.parameter = process.pfMEtSysShiftCorrParameters_2012runAvsNvtx_data
process.pfType1CorrectedMet.srcType1Corrections.append(cms.InputTag('pfMEtSysShiftCorr') )
process.producePFMETCorrections.replace(process.pfType1CorrectedMet,
                                        process.pfMEtSysShiftCorrSequence+process.pfType1CorrectedMet)
##
'''
from PhysicsTools.PatAlgos.tools.jetTools import *

switchJetCollection(process,cms.InputTag('ak5PFJets'),
                    doJTA        = True,
                    doBTagging   = True,
                    jetCorrLabel = ('AK5PF', ['L2Relative', 'L3Absolute',]),
                    doType1MET   = False,
                    genJetCollection=cms.InputTag("ak5GenJets"),
                    doJetID      = True,
                    jetIdLabel   = 'ak5'
                    )

JEClevels = cms.vstring(['L2Relative', 'L3Absolute'])
if runOnMC:
    JEClevels = ['L1FastJet', 'L2Relative', 'L3Absolute']
else:
    JEClevels = ['L1FastJet', 'L2Relative', 'L3Absolute', 'L2L3Residual']

process.patJetCorrFactors.levels = JEClevels
process.patJetCorrFactors.rho    = cms.InputTag('kt6PFJets','rho')
process.patJetCorrFactors.useRho = True

process.patJets.jetCorrFactorsSource = cms.VInputTag(cms.InputTag("patJetCorrFactors"))

if runOnMC:
    process.load("RecoJets.Configuration.GenJetParticles_cff")
    process.load("RecoJets.Configuration.RecoGenJets_cff")
    process.genJetsNoNu = cms.Sequence(process.genParticlesForJetsNoNu*
                                       process.ak5GenJetsNoNu)
    process.patDefaultSequence.replace(process.patJetGenJetMatch,
                                       process.genJetsNoNu*
                                       process.patJetGenJetMatch)
    process.patJetGenJetMatch.matched = cms.InputTag("ak5GenJetsNoNu")


#################### tau sequence #######################

process.load("RecoTauTag.Configuration.RecoPFTauTag_cff")

from PhysicsTools.PatAlgos.tools.tauTools import *
#MB needed??
switchToPFTauHPS(process)

'''
#MB needed? tracks and PFlow kept and gen
getattr(process,"patTaus").embedIsolationTracks             = cms.bool(True)
getattr(process,"patTaus").embedSignalTracks                = cms.bool(True)
getattr(process,"patTaus").embedLeadTrack                   = cms.bool(True)
getattr(process,"patTaus").embedLeadPFCand                  = cms.bool(True)
getattr(process,"patTaus").embedLeadPFChargedHadrCand       = cms.bool(True)
getattr(process,"patTaus").embedLeadPFNeutralCand           = cms.bool(True)
getattr(process,"patTaus").embedSignalPFCands               = cms.bool(True)
getattr(process,"patTaus").embedSignalPFChargedHadrCands    = cms.bool(True)
getattr(process,"patTaus").embedSignalPFNeutralHadrCands    = cms.bool(True)
getattr(process,"patTaus").embedSignalPFGammaCands          = cms.bool(True)
getattr(process,"patTaus").embedIsolationPFCands            = cms.bool(True)
getattr(process,"patTaus").embedIsolationPFChargedHadrCands = cms.bool(True)
getattr(process,"patTaus").embedIsolationPFNeutralHadrCands = cms.bool(True)
getattr(process,"patTaus").embedIsolationPFGammaCands       = cms.bool(True)

getattr(process,"patTaus").embedGenMatch                    = cms.bool(True)
getattr(process,"patTaus").embedGenJetMatch                 = cms.bool(True)
'''

process.tauMatch.maxDeltaR                = 0.15
process.tauMatch.resolveAmbiguities       = cms.bool(False)
process.tauGenJetMatch.resolveAmbiguities = cms.bool(False)
process.tauGenJetMatch.maxDeltaR          = 0.15
process.tauGenJetMatch.maxDPtRel          = 999

##################################################################

from CommonTools.ParticleFlow.Tools.pfIsolation import setupPFMuonIso, setupPFElectronIso
process.muIsoSequence       = setupPFMuonIso(process,'muons')
process.electronIsoSequence = setupPFElectronIso(process,'gsfElectrons')
from CommonTools.ParticleFlow.pfParticleSelection_cff import pfParticleSelectionSequence
process.pfParticleSelectionSequence = pfParticleSelectionSequence
process.pfIsolationSequence = cms.Sequence(
    process.pfParticleSelectionSequence*
    process.muIsoSequence* process.electronIsoSequence
)
        
#Custom cone size for Electron isolation
process.elPFIsoValueChargedAll04PFIdPFIso.deposits[0].vetos = cms.vstring(
    'EcalBarrel:ConeVeto(0.01)', 'EcalEndcaps:ConeVeto(0.015)',
)
process.elPFIsoValueGamma04PFIdPFIso.deposits[0].vetos = cms.vstring(
    'EcalBarrel:ConeVeto(0.08)', 'EcalEndcaps:ConeVeto(0.08)',
)
process.elPFIsoValuePU04PFIdPFIso.deposits[0].vetos = cms.vstring()
process.elPFIsoValueNeutral04PFIdPFIso.deposits[0].vetos = cms.vstring()

process.elPFIsoValueChargedAll04NoPFIdPFIso.deposits[0].vetos = cms.vstring(
    'EcalBarrel:ConeVeto(0.01)', 'EcalEndcaps:ConeVeto(0.015)',
)
process.elPFIsoValueGamma04NoPFIdPFIso.deposits[0].vetos = cms.vstring(
    'EcalBarrel:ConeVeto(0.08)', 'EcalEndcaps:ConeVeto(0.08)',
)
process.elPFIsoValuePU04NoPFIdPFIso.deposits[0].vetos = cms.vstring()
process.elPFIsoValueNeutral04PFIdPFIso.deposits[0].vetos = cms.vstring()

#Custom cone size for Muon isolation
process.muPFIsoValueChargedAll04PFIso.deposits[0].vetos = cms.vstring(
    '0.0001', 'Threshold(0.0)'
)

process.patMuons.isoDeposits = cms.PSet(
    pfAllParticles   = cms.InputTag("muPFIsoDepositPUPFIso"),      # all PU   CH+MU+E
    pfChargedHadrons = cms.InputTag("muPFIsoDepositChargedPFIso"), # all noPU CH
    pfNeutralHadrons = cms.InputTag("muPFIsoDepositNeutralPFIso"), # all NH
    pfPhotons        = cms.InputTag("muPFIsoDepositGammaPFIso"),   # all PH
    user = cms.VInputTag(
        cms.InputTag("muPFIsoDepositChargedAllPFIso"),                 # all noPU CH+MU+E
    )
)
process.patMuons.isolationValues = cms.PSet(
    pfAllParticles   = cms.InputTag("muPFIsoValuePU04PFIso"),
    pfChargedHadrons = cms.InputTag("muPFIsoValueCharged04PFIso"),
    pfNeutralHadrons = cms.InputTag("muPFIsoValueNeutral04PFIso"),
    pfPhotons        = cms.InputTag("muPFIsoValueGamma04PFIso"),
    user = cms.VInputTag(
        cms.InputTag("muPFIsoValueChargedAll04PFIso"),
    )
)

process.patElectrons.isoDeposits = cms.PSet(
    pfAllParticles   = cms.InputTag("elPFIsoDepositPUPFIso"),      # all PU   CH+MU+E
    pfChargedHadrons = cms.InputTag("elPFIsoDepositChargedPFIso"), # all noPU CH
    pfNeutralHadrons = cms.InputTag("elPFIsoDepositNeutralPFIso"), # all NH
    pfPhotons        = cms.InputTag("elPFIsoDepositGammaPFIso"),   # all PH
    user = cms.VInputTag(
        cms.InputTag("elPFIsoDepositChargedAllPFIso"),                 # all noPU CH+MU+E
    )
)
process.patElectrons.isolationValues = cms.PSet(
    pfAllParticles   = cms.InputTag("elPFIsoValuePU04PFIdPFIso"),
    pfChargedHadrons = cms.InputTag("elPFIsoValueCharged04PFIdPFIso"),
    pfNeutralHadrons = cms.InputTag("elPFIsoValueNeutral04PFIdPFIso"),
    pfPhotons        = cms.InputTag("elPFIsoValueGamma04PFIdPFIso"),
    user = cms.VInputTag(
        cms.InputTag("elPFIsoValueChargedAll04PFIdPFIso"),
    )
)
process.patElectrons.isolationValuesNoPFId = cms.PSet(
    pfAllParticles   = cms.InputTag("elPFIsoValuePU04NoPFIdPFIso"),
    pfChargedHadrons = cms.InputTag("elPFIsoValueCharged04NoPFIdPFIso"),
    pfNeutralHadrons = cms.InputTag("elPFIsoValueNeutral04NoPFIdPFIso"),
    pfPhotons        = cms.InputTag("elPFIsoValueGamma04NoPFIdPFIso"),
    user = cms.VInputTag(
        cms.InputTag("elPFIsoValueChargedAll04NoPFIdPFIso")
    )
)

######################## pat::jet ################################

getattr(process,"selectedPatJets").cut = cms.string('pt>10 && abs(eta)<5.0')


######################## pat::trigger ############################

from PhysicsTools.PatAlgos.tools.trigTools import *
switchOnTrigger( process )
process.patTriggerEvent.processName = '*'
if runOnEmbed:
    process.patTriggerEvent.processName = 'HLT'

if hasattr(process,"patTrigger"):
    process.patTrigger.processName = '*'
    if runOnEmbed:
        process.patTrigger.processName = 'HLT'
        
######################## embedding ###############################

process.selectedPatMuonsUserEmbedded = cms.EDProducer("MuonsUserEmbedded",
    muonTag           = cms.InputTag("selectedPatMuons"),
    vertexTag         = cms.InputTag("offlinePrimaryVertices"),
    fitUnbiasedVertex = cms.bool(False)
)

process.selectedPatElectronsUserEmbedded = cms.EDProducer("ElectronsUserEmbedded",
    electronTag = cms.InputTag("selectedPatElectrons"),
    vertexTag   = cms.InputTag("offlinePrimaryVertices"),
    isMC        = cms.bool(runOnMC),
    doMVADaniele= cms.bool(True),
    inputFileName0v2 = cms.FileInPath('LLRAnalysis/Utilities/data/mvaEleId/Electrons_BDTG_TrigV0_Cat1.weights.xml'),
    inputFileName1v2 = cms.FileInPath('LLRAnalysis/Utilities/data/mvaEleId/Electrons_BDTG_TrigV0_Cat2.weights.xml'),
    inputFileName2v2 = cms.FileInPath('LLRAnalysis/Utilities/data/mvaEleId/Electrons_BDTG_TrigV0_Cat3.weights.xml'),
    inputFileName3v2 = cms.FileInPath('LLRAnalysis/Utilities/data/mvaEleId/Electrons_BDTG_TrigV0_Cat4.weights.xml'),
    inputFileName4v2 = cms.FileInPath('LLRAnalysis/Utilities/data/mvaEleId/Electrons_BDTG_TrigV0_Cat5.weights.xml'),
    inputFileName5v2 = cms.FileInPath('LLRAnalysis/Utilities/data/mvaEleId/Electrons_BDTG_TrigV0_Cat6.weights.xml'),
    inputFileName0v3 = cms.FileInPath('LLRAnalysis/Utilities/data/mvaEleId/Electrons_BDTG_NonTrigV0_Cat1.weights.xml'),
    inputFileName1v3 = cms.FileInPath('LLRAnalysis/Utilities/data/mvaEleId/Electrons_BDTG_NonTrigV0_Cat2.weights.xml'),
    inputFileName2v3 = cms.FileInPath('LLRAnalysis/Utilities/data/mvaEleId/Electrons_BDTG_NonTrigV0_Cat3.weights.xml'),
    inputFileName3v3 = cms.FileInPath('LLRAnalysis/Utilities/data/mvaEleId/Electrons_BDTG_NonTrigV0_Cat4.weights.xml'),
    inputFileName4v3 = cms.FileInPath('LLRAnalysis/Utilities/data/mvaEleId/Electrons_BDTG_NonTrigV0_Cat5.weights.xml'),
    inputFileName5v3 = cms.FileInPath('LLRAnalysis/Utilities/data/mvaEleId/Electrons_BDTG_NonTrigV0_Cat6.weights.xml'),
    inputFileName0v4 = cms.FileInPath('LLRAnalysis/Utilities/data/mvaEleId/Electrons_BDTG_TrigNoIPV0_2012_Cat1.weights.xml'),
    inputFileName1v4 = cms.FileInPath('LLRAnalysis/Utilities/data/mvaEleId/Electrons_BDTG_TrigNoIPV0_2012_Cat2.weights.xml'),
    inputFileName2v4 = cms.FileInPath('LLRAnalysis/Utilities/data/mvaEleId/Electrons_BDTG_TrigNoIPV0_2012_Cat3.weights.xml'),
    inputFileName3v4 = cms.FileInPath('LLRAnalysis/Utilities/data/mvaEleId/Electrons_BDTG_TrigNoIPV0_2012_Cat4.weights.xml'),
    inputFileName4v4 = cms.FileInPath('LLRAnalysis/Utilities/data/mvaEleId/Electrons_BDTG_TrigNoIPV0_2012_Cat5.weights.xml'),
    inputFileName5v4 = cms.FileInPath('LLRAnalysis/Utilities/data/mvaEleId/Electrons_BDTG_TrigNoIPV0_2012_Cat6.weights.xml'),
)

process.selectedPatElectronsUserEmbeddedIso = cms.EDProducer("ElectronsUserEmbeddedIso",
    electronTag = cms.InputTag("selectedPatElectronsUserEmbedded")
)

process.selectedPatTausUserEmbedded = cms.EDProducer("TausUserEmbedded",
    tauTag    = cms.InputTag("selectedPatTaus"),
    vertexTag = cms.InputTag("offlinePrimaryVertices")
)

####################### mu+tau pairing ##################################

process.atLeastOneDiTau = cms.EDProducer("CandViewShallowCloneCombiner",
    decay = cms.string("selectedPatTausUserEmbedded selectedPatTausUserEmbedded"),
    cut = cms.string(
        "sqrt((daughter(0).eta - daughter(1).eta)*(daughter(0).eta - daughter(1).eta) +" +
        "min( abs(daughter(0).phi - daughter(1).phi), 2*3.1415926 - abs(daughter(0).phi - daughter(1).phi) ) * " +
        "min( abs(daughter(0).phi - daughter(1).phi), 2*3.1415926 - abs(daughter(0).phi - daughter(1).phi) ) ) > 0.5"
    ),
    checkCharge = cms.bool(False)
)
process.atLeastOneDiTauFilter = cms.EDFilter("CandViewCountFilter",
    src = cms.InputTag("atLeastOneDiTau"),
    minNumber = cms.uint32(1),
    maxNumber = cms.uint32(999),
)

process.muPtEta = cms.EDFilter("PATMuonSelector",
    src = cms.InputTag("selectedPatMuonsUserEmbedded"),
    cut = cms.string("pt > 7 & abs(eta) < 2.4"),
    filter = cms.bool(False)
)

process.muPtEtaRelID = cms.EDFilter("PATMuonSelector",
    src = cms.InputTag("selectedPatMuonsUserEmbedded"),
    cut = cms.string(
        "pt > 7 & abs(eta) < 2.4 & isGlobalMuon & isPFMuon & isTrackerMuon" +
        " & abs(userFloat('dzWrtPV')) < 0.2 && abs(userFloat('dxyWrtPV')) < 0.045"
    ),
    filter = cms.bool(False)
)

process.muPtEtaRelIDRelIso = cms.EDFilter("PATMuonSelector",
    src = cms.InputTag("selectedPatMuonsUserEmbedded"),
    cut = cms.string(process.muPtEtaRelID.cut.value() + " && userFloat('PFRelIsoDB04v2')<0.20"),
    filter = cms.bool(False)
)

process.muPtEtaID = cms.EDFilter("PATMuonSelector",
    src = cms.InputTag("selectedPatMuonsUserEmbedded"),
    cut = cms.string(
        process.muPtEta.cut.value()+
        " & abs(userFloat('dxyWrtPV')) < 0.045 & abs(userFloat('dzWrtPV')) < 0.2" +
        " & (" +
        " isGlobalMuon & isPFMuon" +
        " & globalTrack.isNonnull " +
        " & globalTrack.normalizedChi2 < 10" +
        " & globalTrack.hitPattern.numberOfValidMuonHits > 0" +                     
        " & numberOfMatchedStations > 1" +                     
        " & innerTrack.hitPattern.numberOfValidPixelHits > 0" +
        " & track.hitPattern.trackerLayersWithMeasurement > 5)"
    ),
    filter = cms.bool(False)
)

process.muonsForVeto =  cms.EDFilter("PATMuonSelector",
    src = cms.InputTag("selectedPatMuonsUserEmbedded"),
    cut = cms.string(
        "pt > 8 & abs(eta) < 2.4" +
        " & abs(userFloat('dxyWrtPV')) < 0.045 & abs(userFloat('dzWrtPV')) < 0.2" +
        " & (" +
        " isGlobalMuon & isPFMuon" +
        " & globalTrack.isNonnull " +
        " & globalTrack.normalizedChi2 < 10" +
        " & globalTrack.hitPattern.numberOfValidMuonHits > 0" +                     
        " & numberOfMatchedStations > 1" +                     
        " & innerTrack.hitPattern.numberOfValidPixelHits > 0" +
        " & track.hitPattern.trackerLayersWithMeasurement > 5)" +
        " & userFloat('PFRelIsoDB04v2') < 0.30"
    ),
    filter = cms.bool(False)
)

process.tauPtEtaID = cms.EDFilter("PATTauSelector",
    src = cms.InputTag("selectedPatTausUserEmbedded"),
    cut = cms.string(
        "pt > 40 & abs(eta) < 2.3" +
        " & (tauID('decayModeFindingNewDMs') > 0.5 | tauID('decayModeFindingOldDMs') > 0.5)" + 
        " & abs(userFloat('dzWrtPV')) < 0.2"
    ),
    filter = cms.bool(False)
)
process.atLeastTwoTausPtEtaIDFilter = cms.EDFilter("CandViewCountFilter",
    src = cms.InputTag("tauPtEtaID"),
    minNumber = cms.uint32(2),
    maxNumber = cms.uint32(999)
)

process.tauPtEtaIDRelIso = cms.EDFilter("PATTauSelector",
    src = cms.InputTag("tauPtEtaID"),
    cut = cms.string(
        "tauID('byLooseCombinedIsolationDeltaBetaCorr3Hits') > 0.5 | tauID('byCombinedIsolationDeltaBetaCorrRaw3Hits') < 8.0" +
        " | tauID('byLooseIsolationMVA3newDMwLT') > 0.5 | tauID('byVLooseIsolationMVA3newDMwLT') > 0.5" +
        " | tauID('byLooseIsolationMVA3oldDMwLT') > 0.5 | tauID('byVLooseIsolationMVA3oldDMwLT') > 0.5"
    ),
    filter = cms.bool(False)
)
process.atLeastTwoTausPtEtaIDRelIsoFilter = cms.EDFilter("CandViewCountFilter",
    src = cms.InputTag("tauPtEtaIDRelIso"),
    minNumber = cms.uint32(2),
    maxNumber = cms.uint32(999)
)

process.tausForVeto = cms.EDFilter("PATTauSelector",
    src = cms.InputTag("selectedPatTausUserEmbedded"),
    cut = cms.string(
        "pt > 20 & abs(eta) < 2.3" +
        " & (tauID('decayModeFindingNewDMs') > 0.5 | tauID('decayModeFindingOldDMs') > 0.5)" +
        " & abs(userFloat('dzWrtPV')) < 0.2" +
        " & (tauID('byLooseCombinedIsolationDeltaBetaCorr3Hits') > 0.5 | tauID('byLooseIsolationMVA3newDMwLT') > 0.5 | tauID('byLooseIsolationMVA3oldDMwLT') > 0.5)"
    ), 
    filter = cms.bool(False)
)

process.tauPtEtaIDAgMu = cms.EDFilter("PATTauSelector",
    src = cms.InputTag("tauPtEtaID"),
    cut = cms.string(
        "tauID('againstMuonLoose3') > 0.5 | tauID('againstMuonLooseMVA') > 0.5"
    ), 
    filter = cms.bool(False)
)
process.atLeastOneTauPtEtaIDAgMuFilter = cms.EDFilter("CandViewCountFilter",
    src = cms.InputTag("tauPtEtaIDAgMu"),
    minNumber = cms.uint32(1),
    maxNumber = cms.uint32(999)
)

process.tauPtEtaIDAgElec = cms.EDFilter("PATTauSelector",
    src = cms.InputTag("tauPtEtaID"),
    cut = cms.string(
        "tauID('againstElectronLoose') > 0.5 | tauID('againstElectronLooseMVA5') > 0.5"
    ), 
    filter = cms.bool(False)
)
process.atLeastOneTauPtEtaIDAgMuAgElecFilter = cms.EDFilter("CandViewCountFilter",
    src = cms.InputTag("tauPtEtaIDAgElec"),
    minNumber = cms.uint32(1),
    maxNumber = cms.uint32(999)
)

####################### e+tau pairing ##################################

simpleCutsVeto = "(userFloat('nHits')<=999"+ \
                 " && (" + \
                 " (isEB && userFloat('sihih')<0.010 && userFloat('dPhi')<0.80 && "+ \
                 "          userFloat('dEta') <0.007 && userFloat('HoE') <0.15)"   + \
                 " || "  + \
                 " (isEE && userFloat('sihih')<0.030 && userFloat('dPhi')<0.70 && "+ \
                 "          userFloat('dEta') <0.010 && userFloat('HoE') <999)"   + \
                 "     )"+ \
                 ")"
simpleCutsWP95 = "(userFloat('nHits')<=1"+ \
                 " && (" + \
                 " (isEB && userFloat('sihih')<0.010 && userFloat('dPhi')<0.80 && "+ \
                 "          userFloat('dEta') <0.007 && userFloat('HoE') <0.15)"   + \
                 " || "  + \
                 " (isEE && userFloat('sihih')<0.030 && userFloat('dPhi')<0.70 && "+ \
                 "          userFloat('dEta') <0.010 && userFloat('HoE') <0.07)"   + \
                 "     )"+ \
                 ")"
simpleCutsWP80 = "(userFloat('nHits')==0 && userInt('antiConv')>0.5 "+ \
                 " && ("   + \
                 " (pt>=20 && ("    + \
                 "               (isEB && userFloat('sihih')<0.010 && userFloat('dPhi')<0.06 && "  + \
                 "                        userFloat('dEta')< 0.004 && userFloat('HoE') <0.04)"     + \
                 "               ||"+ \
                 "               (isEE && userFloat('sihih')<0.030 && userFloat('dPhi')<0.030 && " + \
                 "                        userFloat('dEta') <0.007 && userFloat('HoE') <0.025) )) "+ \
                 "     || "+ \
                 " (pt<20 && (fbrem>0.15 || (abs(superClusterPosition.Eta)<1. && eSuperClusterOverP>0.95) ) && ( "+ \
                 "               (isEB && userFloat('sihih')<0.010 && userFloat('dPhi')<0.030 && " + \
                 "                        userFloat('dEta') <0.004 && userFloat('HoE') <0.025) "   + \
                 "               ||"+ \
                 "               (isEE && userFloat('sihih')<0.030 && userFloat('dPhi')<0.020 &&"  + \
                 "                        userFloat('dEta') <0.005 && userFloat('HoE') <0.025) ))" + \
                 "    )"   + \
                 ")"
##Loose MVA ID used for e-mu and 3d lepton veto selection
MVALoose = "((pt<=20 && abs(superCluster.eta)>=0.0 && abs(superCluster.eta)<0.8 && userFloat('mvaPOGNonTrig')>0.925) ||" + \
           "(pt<=20 && abs(superCluster.eta)>=0.8 && abs(superCluster.eta)<1.479 && userFloat('mvaPOGNonTrig')>0.915) ||" + \
           "(pt<=20 && abs(superCluster.eta)>=1.479 && abs(superCluster.eta)<2.5 && userFloat('mvaPOGNonTrig')>0.965) ||" + \
           "(pt>20  && abs(superCluster.eta)>=0.0 && abs(superCluster.eta)<0.8 && userFloat('mvaPOGNonTrig')>0.905) ||" + \
           "(pt>20  && abs(superCluster.eta)>=0.8 && abs(superCluster.eta)<1.479 && userFloat('mvaPOGNonTrig')>0.955) ||" + \
           "(pt>20  && abs(superCluster.eta)>=1.479 && abs(superCluster.eta)<2.5 && userFloat('mvaPOGNonTrig')>0.975))"
##Loose MVA ID: new e-Id, same FR as old one
MVALooseNew = "((pt<=20 && abs(superCluster.eta)>=0.0 && abs(superCluster.eta)<0.8 && userFloat('mvaPOGTrigNoIP')>-0.5375) ||" + \
              "(pt<=20 && abs(superCluster.eta)>=0.8 && abs(superCluster.eta)<1.479 && userFloat('mvaPOGTrigNoIP')>-0.375) ||" + \
              "(pt<=20 && abs(superCluster.eta)>=1.479 && abs(superCluster.eta)<2.5 && userFloat('mvaPOGTrigNoIP')>-0.025) ||" + \
              "(pt>20  && abs(superCluster.eta)>=0.0 && abs(superCluster.eta)<0.8 && userFloat('mvaPOGTrigNoIP')>0.325) ||" + \
              "(pt>20  && abs(superCluster.eta)>=0.8 && abs(superCluster.eta)<1.479 && userFloat('mvaPOGTrigNoIP')>0.775) ||" + \
              "(pt>20  && abs(superCluster.eta)>=1.479 && abs(superCluster.eta)<2.5 && userFloat('mvaPOGTrigNoIP')>0.775))"

process.elecPtEta = cms.EDFilter("PATElectronSelector",
    src = cms.InputTag("selectedPatElectronsUserEmbeddedIso"),
    cut = cms.string("pt > 12 & abs(eta) < 2.5"),
    filter = cms.bool(False)
)

process.elecPtEtaID = cms.EDFilter("PATElectronSelector",
    src = cms.InputTag("selectedPatElectronsUserEmbeddedIso"),
    cut = cms.string(
        process.elecPtEta.cut.value()+
        " & abs(userFloat('dxyWrtPV')) < 0.045 & abs(userFloat('dzWrtPV')) < 0.2" +
        " & userFloat('nHits') == 0 & userInt('antiConv')>0.5 & " + 
        "("+MVALoose + " | " + MVALooseNew + ")"
    ),
    filter = cms.bool(False)
)

process.elecPtEtaRelID = cms.EDFilter("PATElectronSelector",
    src = cms.InputTag("selectedPatElectronsUserEmbeddedIso"),
    cut = cms.string(
        "pt > 12 & abs(eta) < 2.5 & " +
        "abs(userFloat('dxyWrtPV')) < 0.045 & abs(userFloat('dzWrtPV')) < 0.2 & " +
        #"abs(userFloat('dzWrtPV')) < 0.2 &&"+
        simpleCutsVeto
    ),
    filter = cms.bool(False)
)

process.elecPtEtaRelIDRelIso = cms.EDFilter("PATElectronSelector",
    src = cms.InputTag("selectedPatElectronsUserEmbeddedIso"),
    cut = cms.string(
        process.elecPtEtaRelID.cut.value() +
        " & userFloat('PFRelIsoDB04v3') < 0.20"
    ),
    filter = cms.bool(False)
)

process.electronsForVeto = cms.EDFilter("PATElectronSelector",
    src = cms.InputTag("selectedPatElectronsUserEmbeddedIso"),
    cut = cms.string(
        "pt > 10 && abs(eta) < 2.5" +
        " & abs(userFloat('dxyWrtPV')) < 0.045 & abs(userFloat('dzWrtPV')) < 0.2 & " +
        #" && ( ( pt > 20 && ( (abs(superCluster.eta)<0.80 && userFloat('mvaPOGNonTrig')>0.905) || (abs(superCluster.eta)<1.479 && abs(superCluster.eta)>0.80 && userFloat('mvaPOGNonTrig')>0.955) || (abs(superCluster.eta)>1.479 && userFloat('mvaPOGNonTrig')>0.975) )) || (pt < 20 && ( (abs(superCluster.eta)<0.80 && userFloat('mvaPOGNonTrig')>0.925) || (abs(superCluster.eta)<1.479 && abs(superCluster.eta)>0.80 && userFloat('mvaPOGNonTrig')>0.915) || (abs(superCluster.eta)>1.479 && userFloat('mvaPOGNonTrig')>0.965) )))"+
        "(" + MVALoose + " | " + MVALooseNew + ")"
        " & userFloat('nHits') == 0 & userInt('antiConv') > 0.5" +
        " & userFloat('PFRelIsoDB04v3') < 0.30"
    ),
    filter = cms.bool(False)
)

###################### final sequences ##############################

process.atLeastOneGoodVertexSequence = cms.Sequence(
    process.selectedPrimaryVertices *
    process.primaryVertexFilter *
    process.primaryVertexCounter
)

process.patLeptonsUserEmbeddedSequence = cms.Sequence(
    process.selectedPatMuonsUserEmbedded *
    process.selectedPatElectronsUserEmbedded *
    process.selectedPatElectronsUserEmbeddedIso *
    process.selectedPatTausUserEmbedded
)

process.atLeastOneDiTauSequence = cms.Sequence(
    process.atLeastOneDiTau *
    process.atLeastOneDiTauFilter *
    process.atLeastOneDiTauCounter
)

process.muLegSequence = cms.Sequence(
    process.muPtEta *
    process.muPtEtaID *
    process.muPtEtaRelID * process.muPtEtaRelIDRelIso
)

process.tauLegSequence = cms.Sequence(
    process.tauPtEtaID * process.atLeastTwoTausPtEtaIDFilter * process.atLeastTwoTausPtEtaIDCounter *
    process.tauPtEtaIDRelIso * process.atLeastTwoTausPtEtaIDRelIsoFilter * process.atLeastTwoTausPtEtaIDRelIsoCounter *
    process.tauPtEtaIDAgMu * process.atLeastOneTauPtEtaIDAgMuFilter *
    process.tauPtEtaIDAgElec * process.atLeastOneTauPtEtaIDAgMuAgElecFilter * process.atLeastOneTauPtEtaIDAgMuAgElecCounter
)

process.elecLegSequence = cms.Sequence(
    process.elecPtEta *
    process.elecPtEtaID *
    process.elecPtEtaRelID * process.elecPtEtaRelIDRelIso
)

process.looseLeptonsSequence = cms.Sequence(
    process.muPtEtaID * process.muPtEtaRelID *
    process.elecPtEtaID * process.elecPtEtaRelID
)
process.leptonsForVetoSequence = cms.Sequence(
    process.muonsForVeto * process.electronsForVeto * process.tausForVeto
)

####################### x-cleaning of jets #########################
##MB FIXME: it is correct clean like this? higher lep/tau pT threshold? loose iso?
##MB: needed??
'''
process.deltaRJetMuons = cms.EDProducer(
    "DeltaRNearestMuonComputer",
    probes = cms.InputTag("selectedPatJets"),
    objects = cms.InputTag("muPtEtaID"),
    )
process.selectedPatJetsNoMuons = cms.EDProducer(
    "JetsCleaner",
    jets =  cms.InputTag("selectedPatJets"),
    valueMap = cms.InputTag("deltaRJetMuons"),
    minDeltaR = cms.double(0.5)
    )

process.deltaRJetTaus = cms.EDProducer(
    "DeltaRNearestTauComputer",
    probes = cms.InputTag("selectedPatJetsNoMuons"),
    objects = cms.InputTag("tauPtEtaIDAgMuAgElec"),
    )
process.selectedPatJetsNoMuonsNoTaus = cms.EDProducer(
    "JetsCleaner",
    jets =  cms.InputTag("selectedPatJetsNoMuons"),
    valueMap = cms.InputTag("deltaRJetTaus"),
    minDeltaR = cms.double(0.3)
    )

process.jetCleaningSequence = cms.Sequence(
    process.deltaRJetMuons*process.selectedPatJetsNoMuons*
    process.deltaRJetTaus*process.selectedPatJetsNoMuonsNoTaus
    )
'''
########################## paths ###############################
process.commonOfflineSequence = cms.Sequence(
    process.atLeastOneGoodVertexSequence *
    process.PFTau *
    #process.pfParticleSelectionSequence *
    #process.muIsoSequence *
    #process.electronIsoSequence *
    process.pfIsolationSequence *
    (process.ak5JetTracksAssociatorAtVertex * process.btagging) *
    process.patDefaultSequence *
    ##process.kt6PFJetsNeutral *
    process.patLeptonsUserEmbeddedSequence *
    process.looseLeptonsSequence *
    process.leptonsForVetoSequence
    #+process.hltPFTauSequence
)

process.skimTauTau = cms.Sequence(
    process.allEventsCounter *
    process.hltFilterSequence *
    process.commonOfflineSequence *
    process.atLeastOneDiTauSequence *
    process.muLegSequence *
    process.tauLegSequence
    ##+process.jetCleaningSequence
    ##+process.printTree1
)

'''
#MB it is usually incorrect replace offlinePrimaryVertices by selectedPrimaryVertices
massSearchReplaceAnyInputTag(process.skimMuTau1,
                             "offlinePrimaryVertices",
                             "selectedPrimaryVertices",
                             verbose=False)
massSearchReplaceAnyInputTag(process.skimMuTau2,
                             "offlinePrimaryVertices",
                             "selectedPrimaryVertices",
                             verbose=False)
massSearchReplaceAnyInputTag(process.skimElecTau1,
                             "offlinePrimaryVertices",
                             "selectedPrimaryVertices",
                             verbose=False)
massSearchReplaceAnyInputTag(process.skimElecTau2,
                             "offlinePrimaryVertices",
                             "selectedPrimaryVertices",
                             verbose=False)

process.selectedPrimaryVertices.src = cms.InputTag('offlinePrimaryVertices')
process.hltAK5PFJetTracksAssociatorAtVertex.pvSrc = "offlinePrimaryVertices"
process.hltPFTauTagInfo.PVProducer = "offlinePrimaryVertices"
process.hltPFTaus.PVProducer = "offlinePrimaryVertices"
process.hltPFTauLooseIsolationDiscriminator.qualityCuts.primaryVertexSrc = "offlinePrimaryVertices"
'''
#MB use selectedPrimaryVertices only in pat-object embedding moduces
process.selectedPatMuonsUserEmbedded.vertexTag = "selectedPrimaryVertices"
process.selectedPatElectronsUserEmbedded.vertexTag = "selectedPrimaryVertices"
process.selectedPatTausUserEmbedded.vertexTag = "selectedPrimaryVertices"

##Arun:  change vertex for the PFLow isolation?
massSearchReplaceAnyInputTag(process.pfIsolationSequence, "offlinePrimaryVertices", "selectedPrimaryVertices", verbose = False)

if not runOnEmbed:
    process.commonOfflineSequence.remove(process.ak5JetTracksAssociatorAtVertex)
    process.commonOfflineSequence.remove(process.btagging)

if runOnEmbed:
    if "RhEmbed" in embedType:
        process.load("TauAnalysis.MCEmbeddingTools.embeddingKineReweight_cff")
        if not runOnMC:
            process.commonOfflineSequence += process.embeddingKineReweightSequenceRECembedding
        else:
            process.commonOfflineSequence += process.embeddingKineReweightSequenceGENembedding
        process.embeddingKineReweightRECembedding.inputFileName = cms.FileInPath('TauAnalysis/MCEmbeddingTools/data/embeddingKineReweight_tautau_recEmbedded.root')
        process.embeddingKineReweightGENembedding.inputFileName = cms.FileInPath("TauAnalysis/MCEmbeddingTools/data/embeddingKineReweight_tautau_genEmbedded.root")

#process.p = cms.Path(process.printEventContent+process.skim)
process.pTauTau = cms.Path(process.skimTauTau)

########################## output ###############################

from PhysicsTools.PatAlgos.patEventContent_cff import patEventContentNoCleaning
process.out.outputCommands = cms.untracked.vstring(
    'drop *',
    *patEventContentNoCleaning
) 
process.out.outputCommands.extend(cms.vstring(
    'keep *_TriggerResults_*_*',
    'keep *_hltTriggerSummaryAOD_*_*',
    'keep recoGenParticles_genParticles*_*_*',
    'keep *_patTriggerEvent_*_*',
    'keep *_patTrigger_*_*',
    'keep *_selectedPatJets_*_*',
    'keep *_ak5PFJets_*_*',#MB??
    'keep *_genParticles_*_*',
    'keep *_particleFlow__*',
    'keep *_offlinePrimaryVertices_*_*',
    'keep *_selectedPrimaryVertices_*_*',
    #MB'keep *_offlinePrimaryVerticesWithBS_*_*',
    'keep *_offlineBeamSpot_*_*',
    'keep *_patMETsPFlow_*_*',
    'keep *_patPFMetByMVA_*_*',
    'keep *_metNoHF_*_*',
    'keep *_tauGenJetsSelectorAllHadrons_*_*',
    'keep *_kt6PFJets_rho_*',
    'keep *_kt6PFJetsCentral_rho_*',
    'keep *_kt6PFJetsNeutral_rho_*',
    'keep *_kt6PFJetsForRhoComputationVoronoi_rho_*',
    'keep *_muPtEtaID_*_*',
    'keep *_muPtEtaRelID_*_*',
    'keep *_muons_*_*',
    'keep *_globalMuons_*_*', #AN: needed for noPU MET
    'keep *_elecPtEtaID_*_*',
    'keep *_elecPtEtaRelID_*_*',
    'keep *_electronsForVeto_*_*',
    'keep *_muonsForVeto_*_*',
    'keep *_tausForVeto_*_*',
    'keep *_addPileupInfo_*_*',
    'keep *_generalTracks_*_*',
    'keep *_tmfTracks_*_*',
    'keep *_generalTracksORG_*_*', #AN: needed for rechit Embed
    'keep *_electronGsfTracks_*_*',
    'keep recoTrackExtras_*_*_*',
    'keep recoGsfTrackExtras_*_*_*',
    'keep *_tauPtEtaID_*_*',
    'keep *_generator_*_*',
    'keep *_source_*_*',
    'keep *_l1extraParticles_*_*',#MB+
    'keep *_selectedHltPatTaus_*_*',#MB+
    'keep *_reducedEcalRecHitsEB_*_*',
    'keep *_reducedEcalRecHitsEE_*_*',
    #'drop *_TriggerResults_*_HLT',#MB
    #'drop *_TriggerResults_*_RECO',#MB
    'drop *_selectedPatElectrons_*_*',
    'drop *_selectedPatMuons_*_*',
    'drop *_selectedPatTaus_*_*',
    #MB'drop *_patMETs_*_*',
    'keep *_selectedPatMuonsUserEmbedded_*_*', #AN+
    'drop *_selectedPatElectronsUserEmbedded_*_*',
    'keep *_selectedPatElectronsUserEmbeddedIso_*_*', #AN+
    'drop *_selectedPatTausUserEmbedded_*_*',
    'keep *_puJetId_*_*',
    'keep *_puJetMva_*_*',
    #MB'keep *_hfEMClusters_*_*',
    #MB'keep *_hybridSuperClusters_*_*',
    #MB'keep *_multi5x5BasicClusters_*_*',
    #MB'keep *_pfElectronTranslator_*_*',
    #MB'keep *_pfPhotonTranslator_*_*',
    #MB'keep *_hybridSuperClusters_*_*',
    #CV
    'keep *_particleFlowDisplacedVertex_*_*',
    'keep *_pfMet_*_*',
    'keep *_generalV0Candidates_Kshort_*',
    'keep *_generalV0Candidates_Lambda_*',
    'keep *_gsfElectrons_*_*',
    'keep *_allConversions_*_*',
    'keep *_genMetTrue_*_*',
    #To re-make patJets
    'keep *_jetTracksAssociatorAtVertex_*_*',
    'keep *_ak5GenJets_*_*',
    'keep *_*JetTagsAOD_*_*',
    'keep *_TauSpinnerReco_TauSpinnerWT*_*',
    'keep *_ZmumuEvtSelEffCorrWeightProducer_weight_*',
    'keep *_muonRadiationCorrWeightProducer_weight_*',
    'keep *_muonRadiationCorrWeightProducer_weightUp_*',
    'keep *_muonRadiationCorrWeightProducer_weightDown_*',
    'keep *_embeddingKineReweightRECembedding_genTau2PtVsGenTau1Pt_*',
    'keep *_embeddingKineReweightRECembedding_genTau2EtaVsGenTau1Eta_*',
    'keep *_embeddingKineReweightRECembedding_genDiTauMassVsGenDiTauPt_*',
    'keep *_embeddingKineReweightGENembedding_genTau2PtVsGenTau1Pt_*',
    'keep *_embeddingKineReweightGENembedding_genTau2EtaVsGenTau1Eta_*',
    'keep *_embeddingKineReweightGENembedding_genDiTauMassVsGenDiTauPt_*',
    'keep *_generator_minVisPtFilter_*',
    'keep *_goldenZmumuCandidatesGe2IsoMuons_*_*',
))

process.TFileService = cms.Service("TFileService",
    fileName = cms.string("skimHadTauStream.root")
    ##fileName = cms.string("/data1/veelken/CMSSW_5_3_x/PATTuples/skimHadTauStream_selEvents_simHiggsSUSYGluGlu130_tautau_selEventFromRiccardo.root")
)

process.out.SelectEvents = cms.untracked.PSet(
    SelectEvents = cms.vstring('pTauTau')
)

process.out.fileName = cms.untracked.string('patTuple_HadTauStream.root')
##process.out.fileName = cms.untracked.string('/data1/veelken/CMSSW_5_3_x/PATTuples/patTuple_HadTauStream_selEvents_simHiggsSUSYGluGlu130_tautau_selEventFromRiccardo.root')

process.outpath = cms.EndPath(process.out)

processDumpFile = open('patTuplePATSkimHadTauStream.dump', 'w')
print >> processDumpFile, process.dumpPython()

