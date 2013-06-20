import FWCore.ParameterSet.Config as cms
from PhysicsTools.PatAlgos.tools.helpers import cloneProcessingSnippet

def getDiTauMassByLeptonPair(process, muonColl, electronColl, tauColl, runOnMC=True, useMarkov=True, useRecoil=True, doSVFitReco=True, postfix="") :

    isMuTau = False
    isElecTau = False
    if muonColl != "":
        isMuTau = True
    elif electronColl != "":
        isElecTau = True
        
    process.load("JetMETCorrections.METPUSubtraction.mvaPFMET_cff")
    process.pfMEtMVA.minNumLeptons = cms.int32(2)
    if runOnMC:
        process.calibratedAK5PFJetsForPFMEtMVA.correctors = cms.vstring("ak5PFL1FastL2L3")
    else:
        process.calibratedAK5PFJetsForPFMEtMVA.correctors = cms.vstring("ak5PFL1FastL2L3Residual")
        
    process.metRecoilCorrector = cms.EDProducer(
        "MEtRecoilCorrectorProducer",
        genParticleTag      = cms.InputTag("genParticles"),
        jetTag              = cms.InputTag("selectedPatJets"),
        metTag              = cms.InputTag("patMETsPFlow"),
        electronTag         = cms.InputTag(""),
        muonTag             = cms.InputTag("muPtEtaIDIso"),
        tauTag              = cms.InputTag("tauPtEtaIDAgMuAgElecIso"),
        inputFileNamezmm42X = cms.FileInPath("LLRAnalysis/Utilities/data/recoilv7/RecoilCorrector_v7/recoilfits/recoilfit_zmm53X_2012_njet.root"),
        inputFileNamedatamm = cms.FileInPath("LLRAnalysis/Utilities/data/recoilv7/RecoilCorrector_v7/recoilfits/recoilfit_datamm53X_2012_njet.root"),
        inputFileNamewjets  = cms.FileInPath("LLRAnalysis/Utilities/data/recoilv7/RecoilCorrector_v7/recoilfits/recoilfit_wjets53X_20pv_njet.root"),
        inputFileNamezjets  = cms.FileInPath("LLRAnalysis/Utilities/data/recoilv7/RecoilCorrector_v7/recoilfits/recoilfit_zmm53X_2012_njet.root"),
        inputFileNamehiggs  = cms.FileInPath("LLRAnalysis/Utilities/data/recoilv7/RecoilCorrector_v7/recoilfits/recoilfit_higgs53X_20pv_njet.root"),
        numOfSigmas         = cms.double(1.0),
        minJetPt            = cms.double(30.0),
        verbose             = cms.bool(False),
        isMC                = cms.bool(runOnMC),
    )

    process.rescaledMET = cms.EDProducer(
        "MEtRescalerProducer",
        metTag         = cms.InputTag("metRecoilCorrector",  "N"),
        jetTag         = cms.InputTag("selectedPatJets"),
        electronTag    = cms.InputTag(""),
        muonTag        = cms.InputTag("muPtEtaIDIso"),
        tauTag         = cms.InputTag("tauPtEtaIDAgMuAgElecIso"),
        unClusterShift = cms.double(0.10),
        tauShift       = cms.vdouble(0.03,0.03),
        muonShift      = cms.vdouble(0.01,0.01),
        electronShift  = cms.vdouble(0.01,0.025),
        jetThreshold   = cms.double(10),
        numOfSigmas    = cms.double(1.0),
        verbose = cms.bool(False)
    )

    process.load("PhysicsTools.PatAlgos.producersLayer1.metProducer_cfi")
    process.patPFMetByMVA = process.patMETs.clone(
            metSource = cms.InputTag('pfMEtMVA'),
            addMuonCorrections = cms.bool(False),
            genMETSource = cms.InputTag('genMetTrue'),
            addGenMET = cms.bool(runOnMC)
        )
    
    process.pfMEtMVACov = cms.EDProducer(
        "PFMEtSignCovMatrixUnembedder",
        src = cms.InputTag("patPFMetByMVA")
    )

    if isMuTau:
        process.load("TauAnalysis.CandidateTools.muTauPairProduction_cff")
        process.diTau = process.allMuTauPairs.clone()
        process.diTau.srcLeg1  = cms.InputTag(muonColl)
    elif isElecTau:    
        process.load("TauAnalysis.CandidateTools.elecTauPairProduction_cff")
        process.diTau = process.allElecTauPairs.clone()
        process.diTau.srcLeg1  = cms.InputTag(electronColl)
    process.diTau.srcLeg2  = cms.InputTag("tauPtEtaIDAgMuAgElecIso")
    process.diTau.srcMET   = cms.InputTag("metRecoilCorrector",  "N")
    process.diTau.srcPrimaryVertex = cms.InputTag("offlinePrimaryVertices")
    process.diTau.dRmin12  = cms.double(0.5)
    process.diTau.doSVreco = cms.bool(doSVFitReco)
    if useMarkov:
        process.diTau.nSVfit.psKine_MEt_int.algorithm = cms.PSet(
            pluginName = cms.string("nSVfitAlgorithmByIntegration2"),
            pluginType = cms.string("NSVfitAlgorithmByIntegration2"),
            markovChainOptions = cms.PSet(
                mode = cms.string("Metropolis"),
                initMode = cms.string("Gaus"),
                numIterBurnin = cms.uint32(10000),
                numIterSampling = cms.uint32(100000),
                numIterSimAnnealingPhase1 = cms.uint32(2000),
                numIterSimAnnealingPhase2 = cms.uint32(6000),
                T0 = cms.double(15.),
                alpha = cms.double(0.999),
                numChains = cms.uint32(1),
                numBatches = cms.uint32(1),
                L = cms.uint32(1),
                epsilon0 = cms.double(1.e-2),
                nu = cms.double(0.71)
                ),
            max_or_median = cms.string("max"),
            verbosity = cms.int32(0)
            )
        process.diTau.nSVfit.psKine_MEt_int.config.event.srcPrimaryVertex = cms.InputTag("offlinePrimaryVertices")
    delattr(process.diTau.nSVfit, "psKine_MEt_logM_fit")
    if not runOnMC:
        process.diTau.srcGenParticles = ""

    ###############################################################
    diTauInputList = []
    runMETByPairsSequence = cms.Sequence()
    for idxLep in range(10):
        for idxTau in range(10):

            if isMuTau:
                moduleNameMu = "muPtEtaIDIso%i%i%s" % (idxLep, idxTau, postfix)
                moduleMu = cms.EDProducer("SinglePatMuonPicker",
                                          src = cms.InputTag(muonColl),
                                          itemNumber = cms.uint32(idxLep),
                                          verbose = cms.untracked.bool(False)
                                          )
                setattr(process, moduleNameMu, moduleMu)
                runMETByPairsSequence += moduleMu
            elif isElecTau:   
                 moduleNameEle = "elePtEtaIDIso%i%i%s" % (idxLep, idxTau, postfix)
                 moduleEle = cms.EDProducer("SinglePatElectronPicker",
                                            src = cms.InputTag(electronColl),
                                            itemNumber = cms.uint32(idxLep),
                                            verbose = cms.untracked.bool(False)
                                           )
                 setattr(process, moduleNameEle, moduleEle)
                 runMETByPairsSequence += moduleEle
                                                                 
            moduleNameTau = "tauPtEtaIDAgMuAgElecIso%i%i%s" % (idxLep, idxTau ,postfix)
            moduleTau = cms.EDProducer("SinglePatTauPicker",
                                       src = cms.InputTag(tauColl),
                                       itemNumber = cms.uint32(idxTau),
                                       verbose = cms.untracked.bool(False)
                                       )
            setattr(process, moduleNameTau, moduleTau)
            runMETByPairsSequence += moduleTau
            moduleNameMVAMET = "pfMEtMVA%i%i%s" % (idxLep, idxTau ,postfix)
            moduleMVAMET = process.pfMEtMVA.clone()
            if isMuTau:
                moduleMVAMET.srcLeptons = cms.VInputTag( cms.InputTag(moduleNameMu),  cms.InputTag(moduleNameTau))
            elif isElecTau:
                moduleMVAMET.srcLeptons = cms.VInputTag( cms.InputTag(moduleNameEle),  cms.InputTag(moduleNameTau))
            setattr(process, moduleNameMVAMET, moduleMVAMET)
            runMETByPairsSequence += moduleMVAMET
            moduleNamePatMetMVA = "patPFMetByMVA%i%i%s" % (idxLep, idxTau, postfix)
            modulePatMetMVA = process.patPFMetByMVA.clone(metSource = cms.InputTag(moduleNameMVAMET))
            setattr(process, moduleNamePatMetMVA, modulePatMetMVA)
            runMETByPairsSequence += modulePatMetMVA
            moduleNameMetCov = "pfMEtMVACov%i%i%s" % (idxLep, idxTau, postfix)
            moduleMetCov = process.pfMEtMVACov.clone(src = cms.InputTag(moduleNamePatMetMVA))
            setattr(process, moduleNameMetCov, moduleMetCov)
            runMETByPairsSequence += moduleMetCov
            moduleNameDiTau = "diTau%i%i%s" % (idxLep, idxTau, postfix)
            moduleDiTau = process.diTau.clone(
                srcLeg2  = cms.InputTag(moduleNameTau),
                srcMET   = cms.InputTag(moduleNamePatMetMVA)
                )
            if isMuTau:
                moduleDiTau.srcLeg1 = cms.InputTag(moduleNameMu)
            elif isElecTau:
                moduleDiTau.srcLeg1 = cms.InputTag(moduleNameEle)
            #setattr(process, moduleNameDiTau, moduleDiTau)
            metCollection = cms.InputTag(moduleNamePatMetMVA)
            if useRecoil :
                moduleNameRecoilMET = "metRecoilCorrector%i%i%s" % (idxLep, idxTau, postfix)
                moduleRecoilMET = process.metRecoilCorrector.clone()
                moduleRecoilMET.metTag = cms.InputTag(moduleNamePatMetMVA)
                if isMuTau:
                    moduleRecoilMET.electronTag  = cms.InputTag("")
                    moduleRecoilMET.muonTag = cms.InputTag(muonColl)
                elif isElecTau:
                    moduleRecoilMET.electronTag  = cms.InputTag(electronColl)
                    moduleRecoilMET.muonTag = cms.InputTag("")
                moduleRecoilMET.tauTag  = cms.InputTag(tauColl)
                setattr(process, moduleNameRecoilMET, moduleRecoilMET)
                runMETByPairsSequence += moduleRecoilMET

                moduleDiTau.srcMET   = cms.InputTag(moduleNameRecoilMET, "N")
                moduleDiTau.nSVfit.psKine_MEt_int.config.event.srcMEt = cms.InputTag(moduleNameRecoilMET, "N")
                moduleDiTau.nSVfit.psKine_MEt_int.config.event.likelihoodFunctions[0].srcMEtCovMatrix = cms.InputTag(moduleNameMetCov)
                metCollection = cms.InputTag(moduleNameRecoilMET, "N")
                                
            else :
                moduleDiTau.nSVfit.psKine_MEt_int.config.event.srcMEt = cms.InputTag(moduleNamePatMetMVA)
                moduleDiTau.nSVfit.psKine_MEt_int.config.event.likelihoodFunctions[0].srcMEtCovMatrix = cms.InputTag(moduleNameMetCov) 
                
            setattr(process, moduleNameDiTau, moduleDiTau)
            runMETByPairsSequence += moduleDiTau
            
            diTauInputList.append(cms.PSet(srcDiTau = cms.InputTag(moduleNameDiTau),
                                           srcMET = metCollection
                                           )
                                  )
            
            
    ################################################################3
    #moduleEvtContentName = "printEventContent%s" % postfix
    #moduleEventContent = cms.EDAnalyzer("EventContentAnalyzer")
    #setattr(process, moduleEvtContentName, moduleEventContent)
    #runMETByPairsSequence += moduleEventContent
    MergedDiTausName  = "MergedDiTaus%s" % postfix        
    if isMuTau:
        MergedDiTaus = cms.EDProducer("PATMuTauPairToMEtAssociationProducer",
                                      inputs = cms.VPSet(diTauInputList)
                                      )
    elif isElecTau:
        MergedDiTaus = cms.EDProducer("PATElecTauPairToMEtAssociationProducer",
                                      inputs = cms.VPSet(diTauInputList)
                                      )
    setattr(process, MergedDiTausName, MergedDiTaus) 
    runMETByPairsSequence += MergedDiTaus

    runMETByPairsSequenceName = "runMETByPairsSequence%s" % postfix
    setattr(process, runMETByPairsSequenceName, runMETByPairsSequence)
    #print runMETByPairsSequence
    return runMETByPairsSequence
