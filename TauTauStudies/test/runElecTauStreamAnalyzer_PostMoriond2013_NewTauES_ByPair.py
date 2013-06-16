import FWCore.ParameterSet.Config as cms

process = cms.Process("ELECTAUANA")

process.load('Configuration.StandardSequences.Services_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
process.load("Configuration.Geometry.GeometryIdeal_cff")
process.load("Configuration.StandardSequences.MagneticField_cff")

#from Configuration.PyReleaseValidation.autoCond import autoCond
#process.GlobalTag.globaltag = cms.string( autoCond[ 'startup' ] )

process.load('JetMETCorrections.Configuration.DefaultJEC_cff')

runOnMC     = True
runOnEmbed  = False
embedType   = "RhEmbedEleTauHighPt" #"PfEmbed" or "RhEmbed","MuTau" or "EleTau","LowPt","HighPt","FullRange"
reRunPatJets = True
applyTauESCorr= True
doSVFitReco = True
usePFMEtMVA = True
useRecoil   = True
useAntiZee   = True
useLepTauPAT = True
useMarkov   = True
runMoriond = True

#if runOnEmbed and runOnMC:
#    print "Running on Embedded, runOnMC should be switched off"
#    runOnMC=False

if runOnMC:
    print "Running on MC"
else:
    print "Running on Data"

if (not runOnMC) and (not runOnEmbed) and applyTauESCorr:
    print "Running on Data, Tau ESCorr should be switched off"
    applyTauESCorr=False 

if useLepTauPAT:
    print "Running on common LepTau PAT-tuples"
else:
    print "Running on old ElecTau PAT-tuples"

if useMarkov:
    print "Use SVFit with Markov chain integration"
else:
    print "Use SVFit with VEGAS integration"

if runOnMC:
    process.GlobalTag.globaltag = cms.string('START53_V23::All')
else:
    process.GlobalTag.globaltag = cms.string('FT_53_V21_AN4::All')
    
    
process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 10
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )
process.options = cms.untracked.PSet( wantSummary = cms.untracked.bool(True) )

process.source = cms.Source(
    "PoolSource",
    fileNames = cms.untracked.vstring(
    #'file:patTuples_LepTauStream.root'
    #'file:VBFH125.root'
    #'file:data2012D.root'    
    #'root://polgrid4.in2p3.fr//dpm/in2p3.fr/home/cms/trivcat/store/user/mbluj/VBF_HToTauTau_M-125_8TeV-powheg-pythia6/LepTauStream-07Dec2012_VBFH125-LepTau-powheg-PUS10_pat/fbab02682d6b416ae6da687406f89be0/patTuples_LepTauStream_100_1_PYQ.root'
    #'file:/data_CMS/cms/htautau/PostMoriond/pat/MC/file_GG125_patTuples_LepTauStream_10_1_Rs2.root'
    #'file:/data_CMS/cms/htautau/PostMoriond/PAT/MC/AntiZee/patTuples_LepTauStream_99_1_dw0.root'
    #'file:/data_CMS/cms/htautau/PostMoriond/PAT/RecHitEmbed/patTuples_LepTauStream_EmbedLowPt_etau_2012D.root'
    #'file:/data_CMS/cms/htautau/PostMoriond/PAT/RecHitEmbed/patTuples_LepTauStream_Embed_etau_2012B.root'
    #'file:/data_CMS/cms/htautau/PostMoriond/PAT/PFEmbed/patTuples_LepTauStream_30_1_8a9.root'
    #'file:/data_CMS/cms/htautau/PostMoriond/PAT/MC/patTuples_LepTauStream_VBFH125_PAT_v2.root'
    #'file:/data_CMS/cms/htautau/PostMoriond/PAT/MC/file_DYJets_patTuples_LepTauStream_9_1_lQm.root'
    #'file:/data_CMS/cms/htautau/PostMoriond/pat/Data/file_Data_2012D_PRV1_HTT_06Mar2013_PAT_v1_p2_patTuples_LepTauStream_78_1_2KS.root'
    'root://polgrid4.in2p3.fr//dpm/in2p3.fr/home/cms/trivcat/store/user/bluj/TauPlusX/Data_2012D_PRV1_HTT_06Mar13_PAT_v2_p1/633d9a9cc3632fa03920e1c68550a01b/patTuples_LepTauStream_9_2_XRa.root'
   )
    )

#process.source.eventsToProcess = cms.untracked.VEventRange(
#    '1:470223'
#    )

process.allEventsFilter = cms.EDFilter(
    "AllEventsFilter"
    )

#######################################################################
#quark/gluon jets
process.load('QuarkGluonTagger.EightTeV.QGTagger_RecoJets_cff')  
process.QGTagger.srcJets = cms.InputTag("selectedPatJets")
process.QGTagger.isPatJet = cms.untracked.bool(True) 
#Switch for when PFJets with CHS are used (only supported for LD):
#process.QGTagger.useCHS  = cms.untracked.bool(True) 
#If an uncorrected jet source is used as input, you can correct the pt on the fly inside the QGTagger:
#process.QGTagger.jec     = cms.untracked.string('ak5PFL1FastL2L3')

###################################################################################
'''
#----------------------------------------------------------------------------------
# produce No-PU MET

process.load("JetMETCorrections.METPUSubtraction.noPileUpPFMET_cff")

doSmearJets = None
if runOnMC:
    doSmearJets = True
else:
    doSmearJets = False

from PhysicsTools.PatUtils.tools.runNoPileUpMEtUncertainties import runNoPileUpMEtUncertainties
runNoPileUpMEtUncertainties(
    process,
    electronCollection = cms.InputTag('UserIsoElectrons'),
    photonCollection = '',
    muonCollection = '',
    tauCollection = tausForPFMEtMVA,
    jetCollection = cms.InputTag('selectedPatJets'),     
    doApplyChargedHadronSubtraction = False,
    doSmearJets = doSmearJets,
    jecUncertaintyTag = "SubTotalMC",
    addToPatDefaultSequence = False
)

if runOnMC:
    process.calibratedAK5PFJetsForNoPileUpPFMEt.correctors = cms.vstring("ak5PFL1FastL2L3")
else:
    process.calibratedAK5PFJetsForNoPileUpPFMEt.correctors = cms.vstring("ak5PFL1FastL2L3Residual")

process.producePFMEtNoPileUp = cms.Sequence(process.pfNoPileUpMEtUncertaintySequence)
#----------------------------------------------------------------------------------

#----------------------------------------------------------------------------------
# produce Type-1 corrected PFMET

process.load("PhysicsTools.PatUtils.patPFMETCorrections_cff")

process.load("JetMETCorrections.Type1MET.pfMETsysShiftCorrections_cfi")
sysShiftCorrParameter = None
if runOnMC:
    sysShiftCorrParameter = process.pfMEtSysShiftCorrParameters_2012runABCvsNvtx_mc
else:
    sysShiftCorrParameter = process.pfMEtSysShiftCorrParameters_2012runABCvsNvtx_data

from PhysicsTools.PatUtils.tools.runType1PFMEtUncertainties import runType1PFMEtUncertainties
runType1PFMEtUncertainties(
    process,
    electronCollection = cms.InputTag('UserIsoElectrons'),
    photonCollection = '',
    muonCollection = '',
    tauCollection = tausForPFMEtMVA,
    jetCollection = cms.InputTag('selectedPatJets'),        
    doSmearJets = doSmearJets,
    jecUncertaintyTag = "SubTotalMC",
    makeType1corrPFMEt = True,
    makeType1p2corrPFMEt = False,
    doApplyType0corr = True,
    sysShiftCorrParameter = sysShiftCorrParameter,
    doApplySysShiftCorr = True,
    addToPatDefaultSequence = False
)

# CV: collections of MET and jets used as input for SysShiftMETcorrInputProducer
#     are not fully consistent, but we anyway use parametrization of MET x/y shift as function of Nvtx
process.pfMEtSysShiftCorr.srcMEt = cms.InputTag('patMETs')
process.pfMEtSysShiftCorr.srcJets = cms.InputTag('selectedPatJets')

##from PhysicsTools.PatAlgos.tools.helpers import massSearchReplaceAnyInputTag
##massSearchReplaceAnyInputTag(process.producePatPFMETCorrections, cms.InputTag('patPFMet'), cms.InputTag('patMETs'))
process.patPFMet.addGenMET = cms.bool(runOnMC)
process.patPFMetJetEnUp.addGenMET = cms.bool(runOnMC)
process.patPFMetJetEnDown.addGenMET = cms.bool(runOnMC)
process.patPFMetElectronEnUp.addGenMET = cms.bool(runOnMC)
process.patPFMetElectronEnDown.addGenMET = cms.bool(runOnMC)
process.patPFMetTauEnUp.addGenMET = cms.bool(runOnMC)
process.patPFMetTauEnDown.addGenMET = cms.bool(runOnMC)
process.patPFMetNoPileUp.addGenMET = cms.bool(runOnMC)
process.patPFMetNoPileUpJetEnUp.addGenMET = cms.bool(runOnMC)
process.patPFMetNoPileUpJetEnDown.addGenMET = cms.bool(runOnMC)
process.patPFMetNoPileUpElectronEnUp.addGenMET = cms.bool(runOnMC)
process.patPFMetNoPileUpElectronEnDown.addGenMET = cms.bool(runOnMC)
process.patPFMetNoPileUpTauEnUp.addGenMET = cms.bool(runOnMC)
process.patPFMetNoPileUpTauEnDown.addGenMET = cms.bool(runOnMC)
process.patPFMetNoPileUpUnclusteredEnUp.addGenMET = cms.bool(runOnMC)
process.patPFMetNoPileUpUnclusteredEnDown.addGenMET = cms.bool(runOnMC)
##process.producePatPFMETCorrections.remove(process.patPFMet)

process.produceType1corrPFMEt = cms.Sequence()
if runOnMC:
    process.patPFJetMETtype1p2Corr.jetCorrLabel = cms.string("L3Absolute")
    process.produceType1corrPFMEt += process.pfType1MEtUncertaintySequence
else:
    # CV: apply data/MC residual correction to "unclustered energy"
    process.calibratedPFCandidates = cms.EDProducer("PFCandResidualCorrProducer",
        src = cms.InputTag('particleFlow'),
        residualCorrLabel = cms.string("ak5PFResidual"),
        residualCorrEtaMax = cms.double(9.9),
        extraCorrFactor = cms.double(1.05)
    )
    process.produceType1corrPFMEt += process.calibratedPFCandidates
    process.pfCandidateToVertexAssociation.PFCandidateCollection = cms.InputTag('calibratedPFCandidates')
    process.patPFJetMETtype1p2Corr.type2ResidualCorrLabel = cms.string("ak5PFResidual")
    process.patPFJetMETtype1p2Corr.type2ResidualCorrEtaMax = cms.double(9.9)
    process.pfCandMETresidualCorr = process.pfCandMETcorr.clone(                
        residualCorrLabel = cms.string("ak5PFResidual"),
        residualCorrEtaMax = cms.double(9.9),
        residualCorrOffset = cms.double(1.),
        extraCorrFactor = cms.double(1.05)
    )
    process.producePatPFMETCorrections.replace(process.pfCandMETcorr, process.pfCandMETcorr + process.pfCandMETresidualCorr)
    process.patType1CorrectedPFMet.srcType1Corrections.append(cms.InputTag('pfCandMETresidualCorr'))
    process.produceType1corrPFMEt += process.pfType1MEtUncertaintySequence
#----------------------------------------------------------------------------------
'''
#----------------------------------------------------------------------------------
# produce CaloMEtNoHF (MC corrected by data/MC difference in CaloMET response)
process.load("LLRAnalysis.TauTauStudies.calibrateCaloMETandL1ETMforEmbedded_cff")
process.load("LLRAnalysis.TauTauStudies.sumCaloTowersInEtaSlices_cfi")

process.produceCaloMEtNoHF = cms.Sequence(process.uncorrectedL1ETM)

if runOnEmbed:
    process.produceCaloMEtNoHF += process.calibrateCaloMETandL1ETMforEmbedded
if runOnMC:
    process.metNoHFresidualCorrected.residualCorrLabel = cms.string("ak5CaloResidual")
    process.metNoHFresidualCorrected.extraCorrFactor = cms.double(1.05)
    process.metNoHFresidualCorrected.isMC = cms.bool(True)
    process.produceCaloMEtNoHF += process.metNoHFresidualCorrected
    process.metNoHFresidualCorrectedUp = process.metNoHFresidualCorrected.clone(
        extraCorrFactor = cms.double(1.10)
    )
    process.produceCaloMEtNoHF += process.metNoHFresidualCorrectedUp
    process.metNoHFresidualCorrectedDown = process.metNoHFresidualCorrected.clone(
        extraCorrFactor = cms.double(1.0)
    )
    process.produceCaloMEtNoHF += process.metNoHFresidualCorrectedDown
else:
    process.metNoHFresidualCorrected.residualCorrLabel = cms.string("")
    process.metNoHFresidualCorrected.extraCorrFactor = cms.double(1.0)
    process.metNoHFresidualCorrected.isMC = cms.bool(False)
    process.produceCaloMEtNoHF += process.metNoHFresidualCorrected
#----------------------------------------------------------------------------------

#----------------------------------------------------------------------------------
# produce PU Jet Ids

from RecoJets.JetProducers.PileupJetID_cfi import *
stdalgos = cms.VPSet(full_53x,cutbased,PhilV1)
process.puJetMva = cms.EDProducer('PileupJetIdProducer',
                                  produceJetIds = cms.bool(True),
                                  jetids = cms.InputTag(""),
                                  runMvas = cms.bool(True),
                                  jets = cms.InputTag("ak5PFJets"),
                                  vertexes = cms.InputTag("offlinePrimaryVertices"),
                                  algos = cms.VPSet(stdalgos),
                                  rho     = cms.InputTag("kt6PFJets","rho"),
                                  jec     = cms.string("AK5PF"),
                                  applyJec = cms.bool(True),
                                  inputIsCorrected = cms.bool(False),
                                  residualsFromTxt = cms.bool(False),
                                  residualsTxt     = cms.FileInPath("RecoJets/JetProducers/data/dummy.txt"),
                                  )

process.puJetIdSequence = cms.Sequence(process.puJetMva)

#----------------------------------------------------------------------------------

#process.puJetIdAndMvaMet = cms.Sequence(process.puJetIdSequence *
#                                        (process.pfMEtMVAsequence*process.patPFMetByMVA))

######################################################
#ReRun patJet
process.runPatJets = cms.Sequence()

if reRunPatJets:
    #process.load("PhysicsTools.PatAlgos.producersLayer1.jetProducer_cff")
    process.load("PhysicsTools.PatAlgos.patSequences_cff")
    from PhysicsTools.PatAlgos.tools.jetTools import *

    switchJetCollection(process,cms.InputTag('ak5PFJets'),
                        doJTA        = True,
                        doBTagging   = True,
                        jetCorrLabel = ('AK5PF', ['L2Relative', 'L3Absolute',]),
                        doType1MET   = False,
                        genJetCollection=cms.InputTag("ak5GenJets"),
                        doJetID      = True,
                        jetIdLabel   = 'ak5',
                        outputModules = []
                        )

    JEClevels = cms.vstring(['L2Relative', 'L3Absolute'])
    if runOnMC:
        JEClevels = ['L1FastJet', 'L2Relative', 'L3Absolute']
    else:
        JEClevels = ['L1FastJet', 'L2Relative', 'L3Absolute', 'L2L3Residual']
        process.makePatJets.remove(process.patJetPartonMatch)
        process.makePatJets.remove(process.patJetGenJetMatch)
        process.makePatJets.remove(process.patJetFlavourId)
        process.patJets.addGenJetMatch = False
        process.patJets.addGenPartonMatch = False
        process.patJets.addPartonJetMatch = False
        process.patJets.getJetMCFlavour = False
        
    process.patJetCorrFactors.levels = JEClevels
    process.patJetCorrFactors.rho    = cms.InputTag('kt6PFJets','rho')
    process.patJetCorrFactors.useRho = True
    
    #process.patJetCorrFactorsL1Offset = process.patJetCorrFactors.clone(
    #    levels = cms.vstring('L1Offset',
    #                         'L2Relative',
    #                         'L3Absolute')
    #    )
    #process.patJetCorrFactorsL1Offset.useRho = False #MB
    #if runOnMC:
    #    process.patJetCorrFactorsL1Offset.levels = ['L1Offset', 'L2Relative', 'L3Absolute']
    #else:
    #    process.patJetCorrFactorsL1Offset.levels = ['L1Offset', 'L2Relative', 'L3Absolute', 'L2L3Residual']
    #    
    #process.patJets.jetCorrFactorsSource = cms.VInputTag(cms.InputTag("patJetCorrFactors"),
    #                                                     cms.InputTag("patJetCorrFactorsL1Offset"))
    #process.makePatJets.replace(process.patJetCorrFactors,
    #                            process.patJetCorrFactors+process.patJetCorrFactorsL1Offset)
    
    process.runPatJets = cms.Sequence(process.makePatJets*process.selectedPatJets)

###################################################################################
process.rescaledTaus = cms.EDProducer(
    "TauRescalerProducer",
    inputCollection = cms.InputTag("tauPtEtaIDAgMuAgElecIsoPtRel"),
    shift           = cms.vdouble(0.03,0.03),
    numOfSigmas     = cms.double(1.0)
    )
process.rescaledElectrons = cms.EDProducer(
    "ElectronRescalerProducer",
    inputCollection = cms.InputTag("elecPtEtaIDIsoPtRel"),
    shift           = cms.vdouble(0.01,0.025),
    numOfSigmas     = cms.double(1.0),
    )
process.rescaledElectronsRel = cms.EDProducer(
    "ElectronRescalerProducer",
    inputCollection = cms.InputTag("elecPtEtaRelID"),
    shift           = cms.vdouble(0.01,0.025),
    numOfSigmas     = cms.double(1.0),
    )

#######################################################################
from LLRAnalysis.TauTauStudies.runMETByLeptonPairs_cfi import *
getDiTauMassByLeptonPair(process, "", "elecPtEtaIDIso", "tauPtEtaIDAgMuAgElecIso", runOnMC, useMarkov, useRecoil, doSVFitReco)

process.selectedDiTau = cms.EDFilter(
    "ElecTauPairSelector",
    src = cms.InputTag("MergedDiTaus:diTaus"),
    cut = cms.string("dR12>0.5")
    )
process.selectedDiTauCounter = cms.EDFilter(
    "CandViewCountFilter",
    src = cms.InputTag("selectedDiTau"),
    minNumber = cms.uint32(1),
    maxNumber = cms.uint32(999),
    )
#######################################################################
getDiTauMassByLeptonPair(process, "", "elecPtEtaIDIso", "tauPtEtaIDAgMuAgElecIsoTauUp", runOnMC, useMarkov, useRecoil, doSVFitReco, "TauUp")

process.selectedDiTauTauUp = process.selectedDiTau.clone(src = cms.InputTag("MergedDiTausTauUp:diTaus") )
process.selectedDiTauTauUpCounter = process.selectedDiTauCounter.clone(src =  cms.InputTag("selectedDiTauTauUp"))

#######################################################################
getDiTauMassByLeptonPair(process, "", "elecPtEtaIDIso", "tauPtEtaIDAgMuAgElecIsoTauDown", runOnMC, useMarkov, useRecoil, doSVFitReco, "TauDown")

process.selectedDiTauTauDown = process.selectedDiTau.clone(src = cms.InputTag("MergedDiTausTauDown:diTaus") )
process.selectedDiTauTauDownCounter = process.selectedDiTauCounter.clone(src =  cms.InputTag("selectedDiTauTauDown"))
#######################################################################
getDiTauMassByLeptonPair(process, "", "elecPtEtaIDIsoElecUp", "tauPtEtaIDAgMuAgElecIso", runOnMC, useMarkov, useRecoil, doSVFitReco, "ElecUp")

process.selectedDiTauElecUp = process.selectedDiTau.clone(src = cms.InputTag("MergedDiTausElecUp:diTaus") )
process.selectedDiTauElecUpCounter = process.selectedDiTauCounter.clone(src =  cms.InputTag("selectedDiTauElecUp"))

#######################################################################
getDiTauMassByLeptonPair(process, "", "elecPtEtaIDIsoElecDown", "tauPtEtaIDAgMuAgElecIso", runOnMC, useMarkov, useRecoil, doSVFitReco, "ElecDown")

process.selectedDiTauElecDown = process.selectedDiTau.clone(src = cms.InputTag("MergedDiTausElecDown:diTaus") )
process.selectedDiTauElecDownCounter = process.selectedDiTauCounter.clone(src =  cms.InputTag("selectedDiTauElecDown"))
#######################################################################
'''
#AN, these are not used anywhere
MVA = "((pt<=20 && abs(superClusterPosition.Eta)>=0.0 && abs(superClusterPosition.Eta)<1.0 && userFloat('mva')>0.133) ||" + \
      " (pt<=20 && abs(superClusterPosition.Eta)>=1.0 && abs(superClusterPosition.Eta)<1.5 && userFloat('mva')>0.465) ||" + \
      " (pt<=20 && abs(superClusterPosition.Eta)>=1.5 && abs(superClusterPosition.Eta)<2.5 && userFloat('mva')>0.518) ||" + \
      " (pt>20  && abs(superClusterPosition.Eta)>=0.0 && abs(superClusterPosition.Eta)<1.0 && userFloat('mva')>0.942) ||" + \
      " (pt>20  && abs(superClusterPosition.Eta)>=1.0 && abs(superClusterPosition.Eta)<1.5 && userFloat('mva')>0.947) ||" + \
      " (pt>20  && abs(superClusterPosition.Eta)>=1.5 && abs(superClusterPosition.Eta)<2.5 && userFloat('mva')>0.878) )"

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
'''
###########################################################################
process.tauPtEtaIDAgMuLAgElec = cms.EDFilter( #apply AntiLep cuts
    "PATTauSelector",
    src = cms.InputTag("tauPtEtaIDAgMuLAgElec"),
    cut = cms.string("tauID('againstMuonLoose2')>0.5 "+
##                      " && tauID('againstElectronTightMVA3')>0.5 "
                     " && tauID('againstElectronLooseMVA3')>0.5 "
                     ),
    filter = cms.bool(False)
    )
process.tauPtEtaIDAgMuAgElecScaled = cms.EDProducer(
    "TauESCorrector",
    tauTag = cms.InputTag("tauPtEtaIDAgMuAgElec")
    #verbose         = cms.bool(True)
    )
if useLepTauPAT:
    process.tauPtEtaIDAgMuAgElecScaled.tauTag = "tauPtEtaIDAgMuLAgElec"
    
process.tauPtEtaIDAgMuAgElecIso  = cms.EDFilter(
    "PATTauSelector",
    src = cms.InputTag("tauPtEtaIDAgMuAgElec"),
    cut = cms.string("pt>20 && abs(eta)<2.3"+
                     " && tauID('byLooseIsolationMVA2')>-0.5"+
                     #" && (tauID('againstElectronTight')>0.5 || tauID('againstElectronMVA')>0.5)"
                     #" && tauID('againstElectronMVA')>0.5"
##                      " && tauID('againstElectronTightMVA3')>0.5 && tauID('againstMuonLoose2')>0.5"
                     " && tauID('againstElectronLooseMVA3')>0.5 && tauID('againstMuonLoose2')>0.5"
                     ),
    filter = cms.bool(False)
    )
process.tauPtEtaIDAgMuAgElecIsoPtRel  = cms.EDFilter(
    "PATTauSelector",
    src = cms.InputTag("tauPtEtaIDAgMuAgElec"),
    cut = cms.string("pt>19 && abs(eta)<2.3"+
                     " && tauID('byLooseIsolationMVA2')>-0.5"+
                     #" && (tauID('againstElectronTight')>0.5 || tauID('againstElectronMVA')>0.5 )"
                     #" && tauID('againstElectronMVA')>0.5"
##                      " && tauID('againstElectronTightMVA3')>0.5 && tauID('againstMuonLoose2')>0.5"
                     " && tauID('againstElectronLooseMVA3')>0.5 && tauID('againstMuonLoose2')>0.5"
                     ),
    filter = cms.bool(False)
    )
if useLepTauPAT:
    process.tauPtEtaIDAgMuAgElecIso.src = "tauPtEtaIDAgMuLAgElec"
    process.tauPtEtaIDAgMuAgElecIsoPtRel.src = "tauPtEtaIDAgMuLAgElec"
if applyTauESCorr:
    process.tauPtEtaIDAgMuAgElecIso.src = cms.InputTag("tauPtEtaIDAgMuAgElecScaled")
    process.tauPtEtaIDAgMuAgElecIsoPtRel.src = cms.InputTag("tauPtEtaIDAgMuAgElecScaled")
if runMoriond:
    process.tauPtEtaIDAgMuLAgElec.cut = cms.string("tauID('againstMuonLoose2')>0.5 || tauID('againstMuonLoose')>0.5")
    process.tauPtEtaIDAgMuAgElecIso.cut = cms.string("pt>20 && abs(eta)<2.3"+
                                                     " && (tauID('byLooseIsolationMVA2')>-0.5 || tauID('byLooseIsolationMVA')>-0.5)"+
##                                                      " && (tauID('againstElectronMVA')>0.5 || tauID('againstElectronTightMVA3')>0.5)"
                                                     " && (tauID('againstElectronMVA')>0.5 || tauID('againstElectronLooseMVA3')>0.5)"
                                                    )
    process.tauPtEtaIDAgMuAgElecIsoPtRel.cut = cms.string("pt>19 && abs(eta)<2.3"+
                                                          " && (tauID('byLooseIsolationMVA2')>-0.5 || tauID('byLooseIsolationMVA')>-0.5)"+
##                                                           " && (tauID('againstElectronMVA')>0.5 || tauID('againstElectronTightMVA3')>0.5)"
                                                          " && (tauID('againstElectronMVA')>0.5 || tauID('againstElectronLooseMVA3')>0.5)"
                                                          )
    
process.tauPtEtaIDAgMuAgElecIsoCounter = cms.EDFilter(
    "CandViewCountFilter",
    src = cms.InputTag("tauPtEtaIDAgMuAgElecIso"),
    minNumber = cms.uint32(1),
    maxNumber = cms.uint32(999),
    )
process.tauPtEtaIDAgMuAgElecIsoTauUp  =  process.tauPtEtaIDAgMuAgElecIso.clone(
    src = cms.InputTag("rescaledTaus", "U")
    )
process.tauPtEtaIDAgMuAgElecIsoTauUpCounter = process.tauPtEtaIDAgMuAgElecIsoCounter.clone(
    src = cms.InputTag("tauPtEtaIDAgMuAgElecIsoTauUp"),
    )
process.tauPtEtaIDAgMuAgElecIsoTauDown  =  process.tauPtEtaIDAgMuAgElecIso.clone(
    src = cms.InputTag("rescaledTaus", "D")
    )
process.tauPtEtaIDAgMuAgElecIsoTauDownCounter = process.tauPtEtaIDAgMuAgElecIsoCounter.clone(
    src = cms.InputTag("tauPtEtaIDAgMuAgElecIsoTauDown"),
    )

#################################################
#process.elecPtEtaID = cms.EDProducer(
#    "ElectronsUserEmbeddedIso",
#    electronTag = cms.InputTag("elecPtEtaID"),
#    )
#################################################
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
process.elecPtEtaID = cms.EDFilter(
    "PATElectronSelector",
    src = cms.InputTag("elecPtEtaID"),
    cut = cms.string(''),
    ##cut = cms.string(MVALoose),#for back compatibility
    filter = cms.bool(False)
    )
process.elecPtEtaIDIso  = cms.EDFilter(
    "PATElectronSelector",
    src = cms.InputTag("elecPtEtaID"),
    #MBcut = cms.string("userFloat('PFRelIsoDB04v3')<0.5 && pt>24 && abs(eta)<2.1"+
    cut = cms.string("userFloat('PFRelIsoDB04v3')<0.5 && pt>14 && abs(eta)<2.1"+
                     "&& userInt('antiConv')>0.5 && userInt('nHits')<1"),
    filter = cms.bool(False)
    )
process.elecPtEtaIDIsoPtRel  = cms.EDFilter(
    "PATElectronSelector",
    src = cms.InputTag("elecPtEtaID"),
    #MBcut = cms.string("userFloat('PFRelIsoDB04v3')<0.5 && pt>23 && abs(eta)<2.1"+
    cut = cms.string("userFloat('PFRelIsoDB04v3')<0.5 && pt>13 && abs(eta)<2.1"+
                     "&& userInt('antiConv')>0.5 && userInt('nHits')<1"),
    filter = cms.bool(False)
    )

process.elecPtEtaIDIsoCounter = cms.EDFilter(
    "CandViewCountFilter",
    src = cms.InputTag("elecPtEtaIDIso"),
    minNumber = cms.uint32(1),
    maxNumber = cms.uint32(999),
    )
process.elecPtEtaIDIsoElecUp = process.elecPtEtaIDIso.clone(
    src = cms.InputTag("rescaledElectrons","U")
    )
process.elecPtEtaIDIsoElecUpCounter = process.elecPtEtaIDIsoCounter.clone(
    src = cms.InputTag("elecPtEtaIDIsoElecUp"),
    )
process.elecPtEtaIDIsoElecDown = process.elecPtEtaIDIso.clone(
    src = cms.InputTag("rescaledElectrons","D")
    )
process.elecPtEtaIDIsoElecDownCounter = process.elecPtEtaIDIsoCounter.clone(
    src = cms.InputTag("elecPtEtaIDIsoElecDown"),
    )

#################################################
#process.elecPtEtaRelIDInt = cms.EDProducer(
#            "ElectronsUserEmbeddedIso",
#            electronTag = cms.InputTag("elecPtEtaRelID"),
#            )
#################################################

process.elecPtEtaRelID = process.elecPtEtaIDIso.clone(
    #src = cms.InputTag("elecPtEtaRelIDInt"),
    src = cms.InputTag("elecPtEtaRelID"),
    #MBcut = cms.string("pt>15")
    cut = cms.string("pt>12 && abs(userFloat('dxyWrtPV'))<0.045")
    )
process.elecPtEtaRelIDElecUp   = process.elecPtEtaRelID.clone(
    src = cms.InputTag("rescaledElectronsRel","U")
    )
process.elecPtEtaRelIDElecDown = process.elecPtEtaRelID.clone(
    src = cms.InputTag("rescaledElectronsRel","D")
    )
##Update 3rdLepVeto cuts##
process.electronsForVeto = cms.EDFilter(
    "PATElectronSelector",
    src = cms.InputTag("electronsForVeto"),
    cut = cms.string("userFloat('nHits')==0 && userInt('antiConv')>0.5"),
                     ## +" && "+MVALoose),#for back compatibility
    filter = cms.bool(False)
    )
#########

process.filterSequence = cms.Sequence(
    (process.tauPtEtaIDAgMuAgElecIso       * process.tauPtEtaIDAgMuAgElecIsoCounter) +
    (process.tauPtEtaIDAgMuAgElecIsoTauUp  * process.tauPtEtaIDAgMuAgElecIsoTauUpCounter) +
    (process.tauPtEtaIDAgMuAgElecIsoTauDown* process.tauPtEtaIDAgMuAgElecIsoTauDownCounter) +
    (process.elecPtEtaIDIso                * process.elecPtEtaIDIsoCounter) +
    (process.elecPtEtaIDIsoElecUp          * process.elecPtEtaIDIsoElecUpCounter) +
    (process.elecPtEtaIDIsoElecDown        * process.elecPtEtaIDIsoElecDownCounter) +
    (process.elecPtEtaRelID+process.elecPtEtaRelIDElecUp+process.elecPtEtaRelIDElecDown)
    )

#######################################################################


process.elecTauStreamAnalyzer = cms.EDAnalyzer(
    "ElecTauStreamAnalyzer",
    diTaus             = cms.InputTag("selectedDiTau"),
    jets               = cms.InputTag("selectedPatJets"),
    newJets            = cms.InputTag(""),
    met                = cms.InputTag("metRecoilCorrector00",  "N"),
    rawMet             = cms.InputTag("patMETsPFlow"),
    mvaMet             = cms.InputTag("patPFMetByMVA00"),
    metCov             = cms.InputTag("pfMEtMVACov00"),
    electrons          = cms.InputTag("elecPtEtaIDIso"),
    electronsRel       = cms.InputTag("elecPtEtaRelID"),
    electronsAntiZee   = cms.InputTag("selectedPatElectronsUserEmbeddedIso"),
    vertices           = cms.InputTag("selectedPrimaryVertices"),
    triggerResults     = cms.InputTag("patTriggerEvent"),
    genParticles       = cms.InputTag("genParticles"),
    genTaus            = cms.InputTag("tauGenJetsSelectorAllHadrons"),
    isMC               = cms.bool(runOnMC),
    isRhEmb            = cms.untracked.bool(runOnEmbed and "RhEmbed" in embedType),
    deltaRLegJet       = cms.untracked.double(0.5),
    minCorrPt          = cms.untracked.double(15.),
    minJetID           = cms.untracked.double(0.5), # 1=loose,2=medium,3=tight
    verbose            = cms.untracked.bool( False ),
    doElecIsoMVA       = cms.untracked.bool( False ),
    doIsoMVAOrdering = cms.untracked.bool(False)
    )

if not useAntiZee:
    process.elecTauStreamAnalyzer.electronsAntiZee = cms.InputTag("elecPtEtaRelID")
    
if usePFMEtMVA:
    if useRecoil :
        process.elecTauStreamAnalyzer.met = cms.InputTag("metRecoilCorrector00",  "N")
    else :
        process.elecTauStreamAnalyzer.met = cms.InputTag("patPFMetByMVA00")

process.elecTauStreamAnalyzerElecUp    = process.elecTauStreamAnalyzer.clone(
    diTaus       =  cms.InputTag("selectedDiTauElecUp"),
    #met          =  cms.InputTag("rescaledMETelectron","NUNNN"),
    electrons    =  cms.InputTag("elecPtEtaIDIsoElecUp"),
    electronsRel =  cms.InputTag("elecPtEtaRelIDElecUp"),
    )
process.elecTauStreamAnalyzerElecDown  = process.elecTauStreamAnalyzer.clone(
    diTaus       =  cms.InputTag("selectedDiTauElecDown"),
    #met          =  cms.InputTag("rescaledMETelectron","NDNNN"),
    electrons    =  cms.InputTag("elecPtEtaIDIsoElecDown"),
    electronsRel =  cms.InputTag("elecPtEtaRelIDElecDown"),
    )
process.elecTauStreamAnalyzerTauUp     = process.elecTauStreamAnalyzer.clone(
    diTaus =  cms.InputTag("selectedDiTauTauUp"),
    #met    =  cms.InputTag("rescaledMETtau","NNNUN")
    )
process.elecTauStreamAnalyzerTauDown   = process.elecTauStreamAnalyzer.clone(
    diTaus =  cms.InputTag("selectedDiTauTauDown"),
    #met    =  cms.InputTag("rescaledMETtau","NNNDN")
    )
#######################################################################
# weights for embedding
process.kineWeightsForEmbed = cms.Sequence()
if runOnEmbed:
    if "RhEmbed" in embedType:
        process.load("TauAnalysis.MCEmbeddingTools.embeddingKineReweight_cff")
        if not runOnMC:
            process.kineWeightsForEmbed += process.embeddingKineReweightSequenceRECembedding
        else:
            process.kineWeightsForEmbed += process.embeddingKineReweightSequenceGENembedding
        if "MuTau" in embedType:
            if "LowPt" in embedType:
                process.embeddingKineReweightRECembedding.inputFileName = cms.FileInPath('TauAnalysis/MCEmbeddingTools/data/embeddingKineReweight_muPt7to25tauPtGt18_recEmbedded.root')
                process.embeddingKineReweightGENembedding.inputFileName = cms.FileInPath("TauAnalysis/MCEmbeddingTools/data/embeddingKineReweight_muPt7to25tauPtGt18_genEmbedded.root")
            if "HighPt" in embedType:
                process.embeddingKineReweightRECembedding.inputFileName = cms.FileInPath('TauAnalysis/MCEmbeddingTools/data/embeddingKineReweight_muPtGt16tauPtGt18_recEmbedded.root')
                process.embeddingKineReweightGENembedding.inputFileName = cms.FileInPath("TauAnalysis/MCEmbeddingTools/data/embeddingKineReweight_muPtGt16tauPtGt18_genEmbedded.root")
            if "FullRange" in embedType:
                process.embeddingKineReweightRECembedding.inputFileName = cms.FileInPath('TauAnalysis/MCEmbeddingTools/data/embeddingKineReweight_recEmbedding_mutau.root')
                process.embeddingKineReweightGENembedding.inputFileName = cms.FileInPath("TauAnalysis/MCEmbeddingTools/data/embeddingKineReweight_genEmbedding_mutau.root")
        if "EleTau" in embedType:
            if "LowPt" in embedType:
                process.embeddingKineReweightRECembedding.inputFileName = cms.FileInPath('TauAnalysis/MCEmbeddingTools/data/embeddingKineReweight_ePt9to30tauPtGt18_recEmbedded.root')
                process.embeddingKineReweightGENembedding.inputFileName = cms.FileInPath("TauAnalysis/MCEmbeddingTools/data/embeddingKineReweight_ePt9to30tauPtGt18_genEmbedded.root")
            if "HighPt" in embedType:
                process.embeddingKineReweightRECembedding.inputFileName = cms.FileInPath('TauAnalysis/MCEmbeddingTools/data/embeddingKineReweight_ePtGt20tauPtGt18_recEmbedded.root')
                process.embeddingKineReweightGENembedding.inputFileName = cms.FileInPath("TauAnalysis/MCEmbeddingTools/data/embeddingKineReweight_ePtGt20tauPtGt18_genEmbedded.root")
            if "FullRange" in embedType:
                process.embeddingKineReweightRECembedding.inputFileName = cms.FileInPath('TauAnalysis/MCEmbeddingTools/data/embeddingKineReweight_recEmbedding_etau.root')
                process.embeddingKineReweightGENembedding.inputFileName = cms.FileInPath("TauAnalysis/MCEmbeddingTools/data/embeddingKineReweight_genEmbedding_etau.root")

#######################################################################
process.seqNominal = cms.Sequence(
    process.allEventsFilter*
    process.runPatJets*
    (process.tauPtEtaIDAgMuLAgElec*process.tauPtEtaIDAgMuAgElecScaled*
     process.tauPtEtaIDAgMuAgElecIso*process.tauPtEtaIDAgMuAgElecIsoCounter)*
    process.elecPtEtaID* #MB
    (process.elecPtEtaIDIso *process.elecPtEtaIDIsoCounter) *
    process.elecPtEtaRelID *
    process.electronsForVeto *
    #(process.pfMEtMVAsequence*process.patPFMetByMVA)*
    #(process.LeptonsForMVAMEt*process.puJetIdAndMvaMet)*
    process.puJetIdSequence *
    #process.produceType1corrPFMEt*
    #process.producePFMEtNoPileUp*
    process.produceCaloMEtNoHF*
    #process.metRecoilCorrector*
    #process.pfMEtMVACov*
    #process.diTau*process.selectedDiTau*process.selectedDiTauCounter*
    process.calibratedAK5PFJetsForPFMEtMVA*
    process.runMETByPairsSequence*
    process.selectedDiTau*process.selectedDiTauCounter*
    process.QuarkGluonTagger* #quark/gluon jets
    process.kineWeightsForEmbed*#IN
    process.elecTauStreamAnalyzer
    )
process.seqElecUp = cms.Sequence(
    process.allEventsFilter*
    process.runPatJets*
    process.elecPtEtaID* #MB
    process.elecPtEtaIDIsoPtRel *
    (process.tauPtEtaIDAgMuLAgElec*process.tauPtEtaIDAgMuAgElecScaled*
     process.tauPtEtaIDAgMuAgElecIso*process.tauPtEtaIDAgMuAgElecIsoCounter)*
    #(process.pfMEtMVAsequence*process.patPFMetByMVA)*
    #(process.LeptonsForMVAMEt*process.puJetIdAndMvaMet)*
    process.puJetIdSequence *
    process.produceCaloMEtNoHF*
    #process.metRecoilCorrector*
    #(process.rescaledMETelectron+process.rescaledElectrons+process.rescaledElectronsRel)*
    (process.rescaledElectrons+process.rescaledElectronsRel)*
    (process.elecPtEtaIDIsoElecUp*process.elecPtEtaIDIsoElecUpCounter) *
    process.elecPtEtaRelIDElecUp *
    process.electronsForVeto *
    #process.pfMEtMVACov*
    #process.diTauElecUp*process.selectedDiTauElecUp*process.selectedDiTauElecUpCounter*
    process.calibratedAK5PFJetsForPFMEtMVA*
    process.runMETByPairsSequenceElecUp*
    process.selectedDiTauElecUp*process.selectedDiTauElecUpCounter*
    process.QuarkGluonTagger* #quark/gluon jets
    process.kineWeightsForEmbed*#IN
    process.elecTauStreamAnalyzerElecUp
    )
process.seqElecDown = cms.Sequence(
    process.allEventsFilter*
    process.runPatJets*
    process.elecPtEtaID* #MB
    process.elecPtEtaIDIsoPtRel *
    (process.tauPtEtaIDAgMuLAgElec*process.tauPtEtaIDAgMuAgElecScaled*
     process.tauPtEtaIDAgMuAgElecIso*process.tauPtEtaIDAgMuAgElecIsoCounter)*
    #(process.pfMEtMVAsequence*process.patPFMetByMVA)*
    #(process.LeptonsForMVAMEt*process.puJetIdAndMvaMet)*
    process.puJetIdSequence *
    process.produceCaloMEtNoHF*
    #process.metRecoilCorrector*
    #(process.rescaledMETelectron+process.rescaledElectrons+process.rescaledElectronsRel)*
    (process.rescaledElectrons+process.rescaledElectronsRel)*
    (process.elecPtEtaIDIsoElecDown*process.elecPtEtaIDIsoElecDownCounter) *
    process.elecPtEtaRelIDElecDown *
    process.electronsForVeto *
    #process.pfMEtMVACov*
    #process.diTauElecDown*process.selectedDiTauElecDown*process.selectedDiTauElecDownCounter*
    process.calibratedAK5PFJetsForPFMEtMVA*
    process.runMETByPairsSequenceElecDown*
    process.selectedDiTauElecDown*process.selectedDiTauElecDownCounter*
    process.QuarkGluonTagger* #quark/gluon jets
    process.kineWeightsForEmbed*#IN
    process.elecTauStreamAnalyzerElecDown
    )

process.seqTauUp = cms.Sequence(
    process.allEventsFilter*
    process.runPatJets*
    process.elecPtEtaID* #MB
    (process.elecPtEtaIDIso*process.elecPtEtaIDIsoCounter) *
    process.tauPtEtaIDAgMuLAgElec*process.tauPtEtaIDAgMuAgElecScaled*process.tauPtEtaIDAgMuAgElecIsoPtRel*
    process.elecPtEtaRelID *
    process.electronsForVeto *
    #(process.pfMEtMVAsequence*process.patPFMetByMVA)*
    #(process.LeptonsForMVAMEt*process.puJetIdAndMvaMet)*
    process.puJetIdSequence *
    process.produceCaloMEtNoHF*
    #process.metRecoilCorrector*
    #(process.rescaledMETtau+process.rescaledTaus)*
    #(process.tauPtEtaIDAgMuAgElecIsoTauUp*process.tauPtEtaIDAgMuAgElecIsoTauUpCounter)*
    #process.pfMEtMVACov*
    #process.diTauTauUp*process.selectedDiTauTauUp*process.selectedDiTauTauUpCounter*
    (process.rescaledTaus)*
    (process.tauPtEtaIDAgMuAgElecIsoTauUp*process.tauPtEtaIDAgMuAgElecIsoTauUpCounter)*
    process.calibratedAK5PFJetsForPFMEtMVA*
    process.runMETByPairsSequenceTauUp*
    process.selectedDiTauTauUp*process.selectedDiTauTauUpCounter*
    process.QuarkGluonTagger* #quark/gluon jets
    process.kineWeightsForEmbed*#IN
    process.elecTauStreamAnalyzerTauUp
    )
process.seqTauDown = cms.Sequence(
    process.allEventsFilter*
    process.runPatJets*
    process.elecPtEtaID* #MB
    (process.elecPtEtaIDIso*process.elecPtEtaIDIsoCounter) *
    process.tauPtEtaIDAgMuLAgElec*process.tauPtEtaIDAgMuAgElecScaled*process.tauPtEtaIDAgMuAgElecIsoPtRel*
    process.elecPtEtaRelID *
    process.electronsForVeto *
    #(process.pfMEtMVAsequence*process.patPFMetByMVA)*
    #(process.LeptonsForMVAMEt*process.puJetIdAndMvaMet)*
    process.puJetIdSequence *
    process.produceCaloMEtNoHF*
    #process.metRecoilCorrector*
    #(process.rescaledMETtau+process.rescaledTaus)*
    #(process.tauPtEtaIDAgMuAgElecIsoTauDown*process.tauPtEtaIDAgMuAgElecIsoTauDownCounter)*
    #process.pfMEtMVACov*
    #process.diTauTauDown*process.selectedDiTauTauDown*process.selectedDiTauTauDownCounter*
    (process.rescaledTaus)*
    (process.tauPtEtaIDAgMuAgElecIsoTauDown*process.tauPtEtaIDAgMuAgElecIsoTauDownCounter)*
    process.calibratedAK5PFJetsForPFMEtMVA*
    process.runMETByPairsSequenceTauDown*
    process.selectedDiTauTauDown*process.selectedDiTauTauDownCounter*
    process.QuarkGluonTagger* #quark/gluon jets
    process.kineWeightsForEmbed*#IN
    process.elecTauStreamAnalyzerTauDown
    )


#######################################################################

if runOnMC:
    process.pNominal            = cms.Path( process.seqNominal )
    process.pTauUp              = cms.Path( process.seqTauUp)
    process.pTauDown            = cms.Path( process.seqTauDown )
    #process.pElecUp                  = cms.Path( process.seqElecUp)    # TO BE STUDIED LATER
    #process.pElecDown                = cms.Path( process.seqElecDown)  # TO BE STUDIED LATER

else:
    process.pNominal            = cms.Path( process.seqNominal )
    if runOnEmbed:
        process.pTauUp          = cms.Path( process.seqTauUp)
        process.pTauDown        = cms.Path( process.seqTauDown )
        #process.pElecUp         = cms.Path( process.seqElecUp)
        #process.pElecDown       = cms.Path( process.seqElecDown)

#######################################################################

process.out = cms.OutputModule(
    "PoolOutputModule",
    outputCommands = cms.untracked.vstring( 'drop *'),
    fileName = cms.untracked.string('patTuplesSkimmed_ElecTauStream.root'),
    )

process.TFileService = cms.Service(
    "TFileService",
    fileName = cms.string("treeElecTauStream.root")
    )

# before starting to process 1st event, print event content
##process.printEventContent = cms.EDAnalyzer("EventContentAnalyzer")
##process.filterFirstEvent = cms.EDFilter("EventCountFilter",
##    numEvents = cms.int32(1)
##)
##process.printFirstEventContentPath = cms.Path(process.filterFirstEvent + process.printEventContent)

process.outpath = cms.EndPath()

##
#processDumpFile = open('runElecTauStreamAnalyzer_Moriond2013_NewTauES.dump', 'w')
#print >> processDumpFile, process.dumpPython()

#  LocalWords:  selectedDiTauElecDownCounter
