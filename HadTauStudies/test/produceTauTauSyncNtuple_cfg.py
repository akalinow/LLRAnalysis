import FWCore.ParameterSet.Config as cms

import LLRAnalysis.HadTauStudies.tools.eos as eos

import os

process = cms.PSet()

process.fwliteInput = cms.PSet(
    fileNames = cms.vstring(),
    
    ##maxEvents = cms.int32(100000),
    maxEvents = cms.int32(-1),
    
    outputEvery = cms.uint32(100000)
)

#----------------------------------------------------------------------------------------------------
##inputFilePath = "/store/user/veelken/CMSSW_5_3_x/Ntuples/AHtoTauTau/2013Dec07/v0_2/nom/HiggsSUSYGluGlu130/"
##inputFilePath = "/store/user/veelken/CMSSW_5_3_x/Ntuples/AHtoTauTau/2013Dec07/v0_2/nom/HiggsSUSYBB130/"
##inputFilePath = "/tmp/veelken/"
##inputFilePath = "/data2/veelken/CMSSW_5_3_x/Ntuples/AHtoTauTau/2013Dec10/v0_4/nom/HiggsSUSYGluGlu130/"
##
##inputFileNames = None
##if inputFilePath.find("/store") != -1:
##    inputFileNames = [ os.path.join(inputFilePath, file_info['file']) for file_info in eos.lsl(inputFilePath) ]
##else:
##    inputFileNames = [ os.path.join(inputFilePath, file) for file in os.listdir(inputFilePath) ]
##print " inputFileNames = %s" % inputFileNames
##
##process.fwliteInput.fileNames = cms.vstring(inputFileNames)
process.fwliteInput.fileNames = cms.vstring('ntuple_HadTauStream.root')
#----------------------------------------------------------------------------------------------------

process.produceTauTauSyncNtuple = cms.PSet(

    isHiggsMC = cms.bool(True),

    isEmbed = cms.bool(False),
    embedType = cms.string("PfEmbed"),

    inputTreeName = cms.string('tauTauNtupleProducer/H2TauTauTreeProducerTauTau'),
    outputTreeName = cms.string('H2TauTauSyncTreeTauTau'),

    addBranches = cms.PSet(
        NUP = cms.string("NUP/F->I"),
        run = cms.string("run/l"),
        event = cms.string("event/l"),
        lumi = cms.string("lumi/l"),
        npv = cms.string("nVert/I"),
        npu = cms.string("nPUbx0/I"),
        rho = cms.string("rho/F->D"),
        m_sv = cms.string("svfitMass/Formula->D"),
        mvis = cms.string("visMass/F->D"),
        m_sv_Up = cms.string("svfitMass/Formula->D"),
        m_sv_Down = cms.string("svfitMass/Formula->D"),
        gen_1_pt = cms.string("l1GenPt/F->D"),
        pt_1 = cms.string("l1Pt/F->D"),
        phi_1 = cms.string("l1Phi/F->D"),
        eta_1 = cms.string("l1Eta/F->D"),
        m_1 = cms.string("l1Mass/F->D"),
        q_1 = cms.string("l1Charge/F->D"),
        iso_1 = cms.string("l1RawDB3HIso/Formula->D"),
        antiele_1 = cms.string("l1againstElectronMVA3raw/Formula->D"),
        ##d0_1 = cms.string("l1dxy/F->D"),
        dZ_1 = cms.string("(l1VertexZ - vertexZ)/Formula->D"),
        mt_1 = cms.string("mt1/F->D"),
        idweight_1 = cms.string("1.0/Formula->D"),
        isoweight_1 = cms.string("1.0/Formula->D"),        
        byCombinedIsolationDeltaBetaCorrRaw3Hits_1 = cms.string("l1RawDB3HIso/Formula->D"),
        againstElectronMVA3raw_1 = cms.string("l1againstElectronMVA3raw/Formula->D"),
        againstElectronMVA3category_1 = cms.string("l1againstElectronMVA3category/Formula->D"),
        againstMuonsLoose2_1 = cms.string("l1againstMuonLoose2/F->D"),
        againstMuonsTight2_1 = cms.string("l1againstMuonTight2/F->D"),
        agsinstElectronLoose_1 = cms.string("l1againstElectronLoose/F->D"),
        gen_2_pt = cms.string("l2GenPt/F->D"),
        pt_2 = cms.string("l2Pt/F->D"),
        phi_2 = cms.string("l2Phi/F->D"),
        eta_2 = cms.string("l2Eta/F->D"),
        m_2 = cms.string("l2Mass/F->D"),
        q_2 = cms.string("l2Charge/F->D"),
        iso_2 = cms.string("l2RawDB3HIso/Formula->D"),
        antiele_2 = cms.string("l2againstElectronMVA3raw/Formula->D"),
        ##d0_2 = cms.string("l2dxy/F->D"),
        dZ_2 = cms.string("(l2VertexZ - vertexZ)/Formula->D"),
        mt_2 = cms.string("mt2/F->D"),
        idweight_2 = cms.string("1.0/Formula->D"),
        isoweight_2 = cms.string("1.0/Formula->D"),        
        byCombinedIsolationDeltaBetaCorrRaw3Hits_2 = cms.string("l2RawDB3HIso/Formula->D"),
        againstElectronMVA3raw_2 = cms.string("l2againstElectronMVA3raw/Formula->D"),
        againstElectronMVA3category_2 = cms.string("l2againstElectronMVA3category/Formula->D"),
        againstMuonsLoose2_2 = cms.string("l2againstMuonLoose2/F->D"),
        againstMuonsTight2_2 = cms.string("l2againstMuonTight2/F->D"),
        agsinstElectronLoose_2 = cms.string("l2againstElectronLoose/F->D"),        
        met = cms.string("met/Formula->D"),
        metphi = cms.string("metPhi/Formula->D"),
        mvamet = cms.string("met/Formula->D"),
        mvametphi = cms.string("metPhi/Formula->D"),
        pzetavis = cms.string("pZetaVis/Formula->D"),
        pzetamiss = cms.string("(pZeta - pZetaVis)/Formula->D"),
        metcov00 = cms.string("mvacov00/Formula->D"),
        metcov01 = cms.string("mvacov01/Formula->D"),
        metcov10 = cms.string("mvacov10/Formula->D"),
        metcov11 = cms.string("mvacov11/Formula->D"),
        mvacov00 = cms.string("mvacov00/Formula->D"),
        mvacov01 = cms.string("mvacov01/Formula->D"),
        mvacov10 = cms.string("mvacov10/Formula->D"),
        mvacov11 = cms.string("mvacov11/Formula->D"),        
        jpt_1 = cms.string("jet1Pt/F->D"),
        jeta_1 = cms.string("jet1Eta/F->D"),
        jphi_1 = cms.string("jet1Phi/F->D"),
        jptraw_1 = cms.string("jet1rawPt/F->D"),
        jptunc_1 = cms.string("jet1PtErr/F->D"),
        jmva_1 = cms.string("jet1PUJetIdMVA/F->D"),
        jpass_1 = cms.string("jet1PUJetId/F->D"),
        jpt_2 = cms.string("jet2Pt/F->D"),
        jeta_2 = cms.string("jet2Eta/F->D"),
        jphi_2 = cms.string("jet2Phi/F->D"),
        jptraw_2 = cms.string("jet2rawPt/F->D"),
        jptunc_2 = cms.string("jet2PtErr/F->D"),
        jmva_2 = cms.string("jet2PUJetIdMVA/F->D"),
        jpass_2 = cms.string("jet2PUJetId/F->D"),
        bpt = cms.string("bjet1Pt/F->D"),
        beta = cms.string("bjet1Eta/F->D"),
        bphi = cms.string("bjet1Phi/F->D"),
        njetingap = cms.string("nCentralJets/I"),
        nbtag = cms.string("nbJets/I"),
        njets = cms.string("nJets/I"),
        l1TrigMatched_diTau = cms.string("l1TrigMatched_diTau/I"),
        l2TrigMatched_diTau = cms.string("l2TrigMatched_diTau/I"),
        l1TrigMatched_diTauJet = cms.string("l1TrigMatched_diTauJet/I"),
        l2TrigMatched_diTauJet = cms.string("l2TrigMatched_diTauJet/I"),
        jetTrigMatched_diTauJet = cms.string("jetTrigMatched_diTauJet/I"),
        triggerWeight_diTauJet = cms.string("triggerWeight_diTauJet/F->D"),
        triggerEffMC_diTauJet = cms.string("triggerEffMC_diTauJet/F->D"),
        triggerEffData_diTauJet = cms.string("triggerEffData_diTauJet/F->D"),
        triggerWeight_diTau = cms.string("triggerWeight_diTau/F->D"),
        triggerEffMC_diTau = cms.string("triggerEffMC_diTau/F->D"),
        triggerEffData_diTau = cms.string("triggerEffData_diTau/F->D"),
        muon1Pt = cms.string("muon1Pt/F->D"),
        electron1Pt = cms.string("electron1Pt/F->D")
    ),

    outputFileName = cms.string('H2TauTauSyncTreeTauTau.root'),

    verbosity = cms.int32(0)
)
