import FWCore.ParameterSet.Config as cms

process = cms.Process("MUTAUANA")

process.load('Configuration.StandardSequences.Services_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
process.load("Configuration.StandardSequences.Geometry_cff")
process.load("Configuration.StandardSequences.MagneticField_cff")

from Configuration.PyReleaseValidation.autoCond import autoCond
process.GlobalTag.globaltag = cms.string( autoCond[ 'startup' ] )

process.load('JetMETCorrections.Configuration.DefaultJEC_cff')
process.load("CondCore.DBCommon.CondDBCommon_cfi")
process.jec = cms.ESSource("PoolDBESSource",
      DBParameters = cms.PSet(
        messageLevel = cms.untracked.int32(0)
        ),
      timetype = cms.string('runnumber'),
      toGet = cms.VPSet(
      cms.PSet(
            record = cms.string('JetCorrectionsRecord'),
            tag    = cms.string('JetCorrectorParametersCollection_Jec11V2_AK5PF'),
            label  = cms.untracked.string('AK5PF')
            )
      ),
      connect = cms.string('sqlite:Jec11V2.db')
)
process.es_prefer_jec = cms.ESPrefer('PoolDBESSource','jec')

runOnMC = True

if runOnMC:
    process.GlobalTag.globaltag = cms.string('START42_V12::All')
else:
    process.GlobalTag.globaltag = cms.string('GR_R_42_V14::All')
    
process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 10
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(200) )

process.source = cms.Source(
    "PoolSource",
    fileNames = cms.untracked.vstring(
    'file:./patTuples_MuTauStream.root'
    )
    )

process.options = cms.untracked.PSet(
    wantSummary = cms.untracked.bool(True)
    )

process.allEventsFilter = cms.EDFilter(
    "AllEventsFilter"
    )


process.rescaledMET = cms.EDProducer(
    "MEtRescalerProducer",
    metTag = cms.InputTag("patMETsPFlow"),
    jetTag = cms.InputTag("selectedPatJets"),
    electronTag = cms.InputTag(""),
    muonTag = cms.InputTag("muPtEtaIDIso"),
    tauTag = cms.InputTag("tauPtEtaIDAgMuAgElecIso"),
    unClusterShift = cms.double(0.10),
    tauShift = cms.double(0.03),
    muonShift = cms.double(0.01),
    electronShift = cms.double(0.02),
    jetThreshold  = cms.double(10),
    verbose = cms.bool(False)
    )

process.rescaledTaus = cms.EDProducer(
    "TauRescalerProducer",
    inputCollection = cms.InputTag("tauPtEtaIDAgMuAgElecIso"),
    shift = cms.double(0.03)
    )
process.rescaledMuons = cms.EDProducer(
    "MuonRescalerProducer",
    inputCollection = cms.InputTag("muPtEtaIDIso"),
    shift = cms.double(0.01)
    )

process.load("LLRAnalysis.Utilities.diTausReconstruction_cff")
process.diTau = process.allMuTauPairs.clone()
process.diTau.srcLeg1 = cms.InputTag("muPtEtaID")
process.diTau.srcLeg2 = cms.InputTag("tauPtEtaIDAgMuAgElec")
process.diTau.srcMET  = cms.InputTag("patMETsPFlow")
process.diTau.dRmin12  = cms.double(0.5)
process.diTau.doSVreco = cms.bool(True)

if not runOnMC:
    process.diTau.srcGenParticles = ""
        
process.selectedDiTau = cms.EDFilter(
    "MuTauPairSelector",
    src = cms.InputTag("diTau"),
    cut = cms.string("dR12>0.5")
    )

#######################################################################

process.diTauUDDDD = process.diTau.clone(doSVreco = cms.bool(True),
                                         srcLeg1 = cms.InputTag("rescaledMuons","D"),
                                         srcLeg2 = cms.InputTag("rescaledTaus","D"),
                                         srcMET  = cms.InputTag("rescaledMET","UDDDD")
                                         )
process.selectedDiTauUDDDD = process.selectedDiTau.clone(src = cms.InputTag("diTauUDDDD") )

process.diTauDUUUU = process.diTau.clone(doSVreco = cms.bool(True),
                                         srcLeg1 = cms.InputTag("rescaledMuons","U"),
                                         srcLeg2 = cms.InputTag("rescaledTaus","U"),
                                         srcMET  = cms.InputTag("rescaledMET","DUUUU")
                                         )
process.selectedDiTauDUUUU = process.selectedDiTau.clone(src = cms.InputTag("diTauDUUUU") )

#######################################################################


process.diTauMassNominal = cms.EDProducer(
    "MuTauMassProducer",
    diTauTag = cms.InputTag("selectedDiTau")
    )
process.diTauMassUDDDD = process.diTauMassNominal.clone( diTauTag = cms.InputTag("selectedDiTauUDDDD") )
process.diTauMassDUUUU = process.diTauMassNominal.clone( diTauTag = cms.InputTag("selectedDiTauDUUUU") )



#######################################################################


process.tauPtEtaIDAgMuAgElecIso  = cms.EDFilter(
    "PATTauSelector",
    src = cms.InputTag("tauPtEtaIDAgMuAgElec"),
    cut = cms.string("tauID('byLooseCombinedIsolationDeltaBetaCorr')>0.5"),
    filter = cms.bool(False)
    )
process.tauPtEtaIDAgMuAgElecIsoCounter = cms.EDFilter(
    "CandViewCountFilter",
    src = cms.InputTag("tauPtEtaIDAgMuAgElecIso"),
    minNumber = cms.uint32(1),
    maxNumber = cms.uint32(999),
    )
process.muPtEtaIDIso  = cms.EDFilter(
    "PATMuonSelector",
    src = cms.InputTag("muPtEtaID"),
    cut = cms.string("userFloat('PFRelIsoDB04v2')<0.1"),
    filter = cms.bool(False)
    )
process.muPtEtaIDIsoCounter = cms.EDFilter(
    "CandViewCountFilter",
    src = cms.InputTag("muPtEtaIDIso"),
    minNumber = cms.uint32(1),
    maxNumber = cms.uint32(999),
    )
process.filterSequence = cms.Sequence(
    process.tauPtEtaIDAgMuAgElecIso*process.tauPtEtaIDAgMuAgElecIsoCounter*
    process.muPtEtaIDIso*process.muPtEtaIDIsoCounter
    )

#######################################################################


process.muTauStreamAnalyzer = cms.EDAnalyzer(
    "MuTauStreamAnalyzer",
    diTaus =  cms.InputTag("selectedDiTau"),
    jets =  cms.InputTag("selectedPatJets"),
    triggerResults = cms.InputTag("patTriggerEvent"),
    isMC = cms.bool(runOnMC),
    deltaRLegJet  = cms.untracked.double(0.5),
    minCorrPt = cms.untracked.double(15.),
    minJetID  = cms.untracked.double(0.5), # 1=loose,2=medium,3=tight
    applyTauSignalSel =  cms.bool( True ),
    verbose =  cms.untracked.bool( False ),
    )

process.analysis = cms.Sequence(
    process.allEventsFilter*
    process.filterSequence*
    process.rescaledMET*
    process.rescaledTaus*
    process.rescaledMuons*
    (process.diTau+process.diTauUDDDD+process.diTauDUUUU)*
    (process.selectedDiTau+process.selectedDiTauUDDDD+process.selectedDiTauDUUUU)*
    (process.diTauMassNominal*process.diTauMassUDDDD*process.diTauMassDUUUU)
    )

process.p = cms.Path(
    process.analysis
    )

process.out = cms.OutputModule(
    "PoolOutputModule",
    outputCommands = cms.untracked.vstring( 'drop *',
                                            'keep *_patMETsPFlow_*_*',
                                            'keep *_rescaledMET_*_*',
                                            'keep *_diTauMassNominal_*_*',
                                            'keep *_diTauMassUDDDD_*_*',
                                            'keep *_diTauMassDUUUU_*_*',
                                            #'keep *_selectedDiTau_*_*',
                                            #'keep *_diTauUDDDD_*_*',
                                            #'keep *_diTauDUUUU_*_*',
                                            ),
    fileName = cms.untracked.string('patTuplesSkimmed_MuTauStream_systematics.root'),
    )

process.TFileService = cms.Service("TFileService",
                                   fileName = cms.string("treeMuTauStream_systematics.root")
                                   )

process.outpath = cms.EndPath(process.out)
#process.outpath = cms.EndPath()
