### *****************************************************************************************
### Usage:
###
### cmsRun topplusdmanaEDMntuples_cfg.py maxEvts=N sample="mySample/sample.root" version="71" outputLabel="myoutput"
###
### Default values for the options are set:
### maxEvts     = -1
### sample      = 'file:/scratch/decosa/ttDM/testSample/tlbsm_53x_v3_mc_10_1_qPV.root'
### outputLabel = 'analysisTTDM.root'
### *****************************************************************************************
import FWCore.ParameterSet.Config as cms
import FWCore.ParameterSet.VarParsing as opts



options = opts.VarParsing ('analysis')

options.register('maxEvts',
                 -1,# default value: process all events
                 opts.VarParsing.multiplicity.singleton,
                 opts.VarParsing.varType.int,
                 'Number of events to process')

options.register('sample',
                 [#'file:/tmp/oiorio/B2GEDMNtuple_1.root'
#                 'file:../../edm_mc/B2GEDMNtuple.root'
#                 'file:/afs/cern.ch/work/n/nfalterm/public/B2GEDMNtuple.root'
#                  'root://xrootd.ba.infn.it///store/user/decosa/ttDM/CMSSW_7_4_X/TT_TuneCUETP8M1_13TeV-powheg-pythia8/TT_TuneCUETP8M1_13TeV/150926_070344/0000/B2GEDMNtuple_1.root'
],
                 opts.VarParsing.multiplicity.singleton,
                 opts.VarParsing.varType.string,
                 'Sample to analyze')

options.register('outputLabel',
                 'treesTest_NewSmall.root',
                 opts.VarParsing.multiplicity.singleton,
                 opts.VarParsing.varType.string,
                 'Output label')

options.register('isData',
                 False,
                 opts.VarParsing.multiplicity.singleton,
                 opts.VarParsing.varType.bool,
                 'Is data?')

options.register('useLHE',
                 True,
                 opts.VarParsing.multiplicity.singleton,
                 opts.VarParsing.varType.bool,
                 'Keep LHEProducts')

options.register('lhes',
#                 'source',
                 'externalLHEProducer',
                 opts.VarParsing.multiplicity.singleton,
                 opts.VarParsing.varType.string,
                 'name from generator')

options.register('syst',
                 ['noSyst'],
#                 ['noSyst','jer__up','jer__down','jes__up','jes__down','unclusteredMet__up','unclusteredMet__down'],
                 opts.VarParsing.multiplicity.singleton,
                 opts.VarParsing.varType.string,
                 'systematic trees')


options.register('globalTag',
                 '74X_mcRun2_asymptotic_v2',
                 opts.VarParsing.multiplicity.singleton,
                 opts.VarParsing.varType.string,
                 'global tag to be used')

options.parseArguments()

if(options.isData):options.useLHE = False

process = cms.Process("ttDManalysisTrees")

process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.categories.append('HLTrigReport')
### Output Report
process.options = cms.untracked.PSet( wantSummary = cms.untracked.bool(True) )
### Number of maximum events to process
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(options.maxEvts) )
### Source file
process.source = cms.Source("PoolSource",
        fileNames = cms.untracked.vstring(
        options.sample
        )
)

process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff')
from Configuration.AlCa.GlobalTag_condDBv2 import GlobalTag
print "# using GT ", options.globalTag
process.GlobalTag.globaltag = options.globalTag

#from Configuration.AlCa.GlobalTag import GlobalTag as customiseGlobalTag
#process.GlobalTag = customiseGlobalTag(process.GlobalTag, globaltag = 'auto:run2_mc_50nsGRun')
#process.GlobalTag.connect   = 'frontier://FrontierProd/CMS_COND_31X_GLOBALTAG'
#process.GlobalTag.pfnPrefix = cms.untracked.string('frontier://FrontierProd/')

#for pset in process.GlobalTag.toGet.value():
#    pset.connect = pset.connect.value().replace('frontier://FrontierProd/', 'frontier://FrontierProd/')
#    #   Fix for multi-run processing:
#    process.GlobalTag.RefreshEachRun = cms.untracked.bool( False )
#    process.GlobalTag.ReconnectEachRun = cms.untracked.bool( False )

    

### Rootplizer

process.TFileService = cms.Service("TFileService", fileName = cms.string(options.outputLabel))
process.load("Analysis.ST_RunII_EA.topplusdmedmRootTreeMaker_cff")
#process.DMTreesDumper.lhes =cms.InputTag("externalLHEProducer")
process.DMTreesDumper.lhes =cms.InputTag(options.lhes)
process.DMTreesDumper.channelInfo.useLHE =(options.useLHE)
process.DMTreesDumper.systematics =(options.syst)
process.DMTreesDumper.changeJECs = cms.untracked.bool(False)# JEC via GT
process.DMTreesDumper.useMETNoHF = cms.untracked.bool(True)
#process.DMTreesDumper.addPV = cms.untracked.bool(True)
process.DMTreesDumper.channelInfo.useLHEWeights =cms.untracked.bool(False)
process.DMTreesDumper.isData = cms.untracked.bool(False)#This adds the L2L3Residuals
process.DMTreesDumper.doPU= cms.bool(True);
process.DMTreesDumper.dataPUFile=cms.string("DistrSummer15_25ns");

process.analysisPath = cms.Path(
    process.DMTreesDumper
    )


