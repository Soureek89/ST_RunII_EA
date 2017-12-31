from WMCore.Configuration import Configuration
config = Configuration()

config.section_("General")

config.General.requestName = 'TChannel_Powheg_JESJERsyst'
#config.General.requestName = 'TbarChannel_Powheg_JESJERsyst'
#config.General.requestName = 'TWChannel_JESJERsyst'
#config.General.requestName = 'TbarWChannel_JESJERsyst'
#config.General.requestName = 'SChannel_JESJERsyst'
#config.General.requestName = 'TTbar_JESJERsyst'
#config.General.requestName = 'WJets_aMCatNLO_Inclusive_JESJERsyst'
#config.General.requestName = 'Wp0Jets_aMCatNLO'
#config.General.requestName = 'Wp1Jets_aMCatNLO'
#config.General.requestName = 'Wp2Jets_aMCatNLO'

#config.General.requestName = 'DYJets_JESJERsyst'
#config.General.requestName = 'QCD_JESJERsyst'
#config.General.requestName = 'WW1L1Nu2Q_JESJERsyst'
#config.General.requestName = 'WW2L2Nu_JESJERsyst'
#config.General.requestName = 'WZ1L1Nu2Q_JESJERsyst'
#config.General.requestName = 'WZ2L2Q_JESJERsyst'
#config.General.requestName = 'ZZ2L2Q_JESJERsyst'

### Data
#config.General.requestName = 'Data_RunB_v3_ReReco_v3'
#config.General.requestName = 'Data_RunC_v1_ReReco_v3'
#config.General.requestName = 'Data_RunD_v1_ReReco_v3'
#config.General.requestName = 'Data_RunE_v1_ReReco_v3'
#config.General.requestName = 'Data_RunF_v1_ReReco_v3'
#config.General.requestName = 'Data_RunG_v1_ReReco_v3'
#config.General.requestName = 'Data_RunH_v1_PromptReco'
#config.General.requestName = 'Data_RunH_v2_PromptReco'
#config.General.requestName = 'Data_RunH_v3_PromptReco'

#config.General.requestName = 'TChannel_Mass166p5_v2'
#config.General.requestName = 'TbarChannel_Mass166p5_v2'
#config.General.requestName = 'TTbar_Mass166p5'

#config.General.requestName = 'TChannel_Mass169p5'
#config.General.requestName = 'TbarChannel_Mass169p5'
#config.General.requestName = 'TTbar_Mass169p5'

#config.General.requestName = 'TChannel_Mass171p5'
#config.General.requestName = 'TbarChannel_Mass171p5'
#config.General.requestName = 'TTbar_Mass171p5'

#config.General.requestName = 'TChannel_Mass173p5'
#config.General.requestName = 'TbarChannel_Mass173p5'
#config.General.requestName = 'TTbar_Mass173p5'

#config.General.requestName = 'TChannel_Mass175p5'
#config.General.requestName = 'TbarChannel_Mass175p5'
#config.General.requestName = 'TTbar_Mass175p5'

#config.General.requestName = 'TChannel_Mass178p5_v2'
#config.General.requestName = 'TbarChannel_Mass178p5_v2'
#config.General.requestName = 'TTbar_Mass178p5'

#config.General.requestName = 'TChannel_hDampUp'
#config.General.requestName = 'TChannel_hDampDown'
#config.General.requestName = 'TChannel_ScaleUp'
#config.General.requestName = 'TChannel_ScaleDown'
#config.General.requestName = 'TChannel_Herwig'

#config.General.requestName = 'TbarChannel_hDampUp'
#config.General.requestName = 'TbarChannel_hDampDown'
#config.General.requestName = 'TbarChannel_ScaleUp'
#config.General.requestName = 'TbarChannel_ScaleDown'
#config.General.requestName = 'TbarChannel_Herwig'

config.General.transferLogs = True
#config.General.transferOutputs = True


config.section_("JobType")
config.JobType.allowUndistributedCMSSW = True
config.JobType.pluginName = 'Analysis'

### For MC ####

config.JobType.psetName = 'tree_amcatnlo_cfg.py'

config.JobType.pyCfgParams =["maxEvents=-1","outputLabel=SystTrees_TChannel_Powheg_Summer16_80X.root","isData=False","useLHE=True", "lhes=externalLHEProducer"]
#config.JobType.pyCfgParams =["maxEvents=-1","outputLabel=SystTrees_TbarChannel_Powheg_Summer16_80X.root","isData=False","useLHE=True", "lhes=externalLHEProducer"]
#config.JobType.pyCfgParams =["maxEvents=-1","outputLabel=SystTrees_TWChannel_Powheg_Summer16_80X.root","isData=False","useLHE=True","lhes=externalLHEProducer"]
#config.JobType.pyCfgParams =["maxEvents=-1","outputLabel=SystTrees_TbarWChannel_Powheg_Summer16_80X.root","isData=False","useLHE=True", "lhes=externalLHEProducer"]
#config.JobType.pyCfgParams =["maxEvents=-1","outputLabel=SystTrees_SChannel_aMCatNLO_Summer16_80X.root","isData=False","useLHE=True","lhes=externalLHEProducer"]
#config.JobType.pyCfgParams =["maxEvents=-1","outputLabel=SystTrees_TTbar_Summer16_80X.root","isData=False","useLHE=True","lhes=externalLHEProducer"]

#config.JobType.pyCfgParams =["maxEvents=-1","outputLabel=SystTrees_WJets_aMCatNLO_Inclusive_Summer16_80X.root","isData=False","useLHE=True", "lhes=externalLHEProducer"]
#config.JobType.pyCfgParams =["maxEvents=-1","outputLabel=Trees_Wp0Jets_aMCatNLO_Summer16_80X.root","isData=False","useLHE=True", "lhes=externalLHEProducer"]
#config.JobType.pyCfgParams =["maxEvents=-1","outputLabel=Trees_Wp1Jets_aMCatNLO_Summer16_80X.root","isData=False","useLHE=True", "lhes=externalLHEProducer"]
#config.JobType.pyCfgParams =["maxEvents=-1","outputLabel=Trees_Wp2Jets_aMCatNLO_Summer16_80X.root","isData=False","useLHE=True", "lhes=externalLHEProducer"]

#config.JobType.pyCfgParams =["maxEvents=-1","outputLabel=SystTrees_DYJets_aMCatNLO_Summer16_80X.root","isData=False","useLHE=True", "lhes=externalLHEProducer"]
#config.JobType.pyCfgParams =["maxEvents=-1","outputLabel=SystTrees_QCD_Summer16_80X.root","isData=False","useLHE=False"]

#config.JobType.pyCfgParams =["maxEvents=-1","outputLabel=SystTrees_WW1L1Nu2Q_aMCatNLO_Summer16_80X.root","isData=False","useLHE=True", "lhes=externalLHEProducer"]
#config.JobType.pyCfgParams =["maxEvents=-1","outputLabel=SystTrees_WW2L2Nu_Powheg_Summer16_80X.root","isData=False","useLHE=True", "lhes=externalLHEProducer"]
#config.JobType.pyCfgParams =["maxEvents=-1","outputLabel=SystTrees_WZ1L1Nu2Q_aMCatNLO_Summer16_80X.root","isData=False","useLHE=True", "lhes=externalLHEProducer"]
#config.JobType.pyCfgParams =["maxEvents=-1","outputLabel=SystTrees_WZ2L2Q_aMCatNLO_Summer16_80X.root","isData=False","useLHE=True", "lhes=externalLHEProducer"]
#config.JobType.pyCfgParams =["maxEvents=-1","outputLabel=SystTrees_ZZ2L2Q_aMCatNLO_Summer16_80X.root","isData=False","useLHE=True", "lhes=externalLHEProducer"]

#config.JobType.pyCfgParams =["maxEvents=-1","outputLabel=Trees_TChannel_Mass166p5_Summer16_80X.root","isData=False","useLHE=True","lhes=externalLHEProducer"]
#config.JobType.pyCfgParams =["maxEvents=-1","outputLabel=Trees_TChannel_Mass169p5_Summer16_80X.root","isData=False","useLHE=True","lhes=externalLHEProducer"]
#config.JobType.pyCfgParams =["maxEvents=-1","outputLabel=Trees_TChannel_Mass171p5_Summer16_80X.root","isData=False","useLHE=True","lhes=externalLHEProducer"]
#config.JobType.pyCfgParams =["maxEvents=-1","outputLabel=Trees_TChannel_Mass173p5_Summer16_80X.root","isData=False","useLHE=True","lhes=externalLHEProducer"]
#config.JobType.pyCfgParams =["maxEvents=-1","outputLabel=Trees_TChannel_Mass175p5_Summer16_80X.root","isData=False","useLHE=True","lhes=externalLHEProducer"]
#config.JobType.pyCfgParams =["maxEvents=-1","outputLabel=Trees_TChannel_Mass178p5_Summer16_80X.root","isData=False","useLHE=True","lhes=externalLHEProducer"]

#config.JobType.pyCfgParams =["maxEvents=-1","outputLabel=Trees_TbarChannel_Mass166p5_Summer16_80X.root","isData=False","useLHE=True","lhes=externalLHEProducer"]
#config.JobType.pyCfgParams =["maxEvents=-1","outputLabel=Trees_TbarChannel_Mass169p5_Summer16_80X.root","isData=False","useLHE=True","lhes=externalLHEProducer"]
#config.JobType.pyCfgParams =["maxEvents=-1","outputLabel=Trees_TbarChannel_Mass171p5_Summer16_80X.root","isData=False","useLHE=True","lhes=externalLHEProducer"]
#config.JobType.pyCfgParams =["maxEvents=-1","outputLabel=Trees_TbarChannel_Mass173p5_Summer16_80X.root","isData=False","useLHE=True","lhes=externalLHEProducer"]
#config.JobType.pyCfgParams =["maxEvents=-1","outputLabel=Trees_TbarChannel_Mass175p5_Summer16_80X.root","isData=False","useLHE=True","lhes=externalLHEProducer"]
#config.JobType.pyCfgParams =["maxEvents=-1","outputLabel=Trees_TbarChannel_Mass178p5_Summer16_80X.root","isData=False","useLHE=True","lhes=externalLHEProducer"]

#config.JobType.pyCfgParams =["maxEvents=-1","outputLabel=Trees_TTbar_Mass166p5_Summer16_80X.root","isData=False","useLHE=True","lhes=externalLHEProducer"]
#config.JobType.pyCfgParams =["maxEvents=-1","outputLabel=Trees_TTbar_Mass169p5_Summer16_80X.root","isData=False","useLHE=True","lhes=externalLHEProducer"]
#config.JobType.pyCfgParams =["maxEvents=-1","outputLabel=Trees_TTbar_Mass171p5_Summer16_80X.root","isData=False","useLHE=True","lhes=externalLHEProducer"]
#config.JobType.pyCfgParams =["maxEvents=-1","outputLabel=Trees_TTbar_Mass173p5_Summer16_80X.root","isData=False","useLHE=True","lhes=externalLHEProducer"]
#config.JobType.pyCfgParams =["maxEvents=-1","outputLabel=Trees_TTbar_Mass175p5_Summer16_80X.root","isData=False","useLHE=True","lhes=externalLHEProducer"]
#config.JobType.pyCfgParams =["maxEvents=-1","outputLabel=Trees_TTbar_Mass178p5_Summer16_80X.root","isData=False","useLHE=True","lhes=externalLHEProducer"]

#config.JobType.pyCfgParams =["maxEvents=-1","outputLabel=Trees_TChannel_hDampUp_Summer16_80X.root","isData=False","useLHE=True","lhes=externalLHEProducer"]
#config.JobType.pyCfgParams =["maxEvents=-1","outputLabel=Trees_TChannel_hDampDown_Summer16_80X.root","isData=False","useLHE=True","lhes=externalLHEProducer"]
#config.JobType.pyCfgParams =["maxEvents=-1","outputLabel=Trees_TChannel_ScaleUp_Summer16_80X.root","isData=False","useLHE=True","lhes=externalLHEProducer"]
#config.JobType.pyCfgParams =["maxEvents=-1","outputLabel=Trees_TChannel_ScaleDown_Summer16_80X.root","isData=False","useLHE=True","lhes=externalLHEProducer"]
#config.JobType.pyCfgParams =["maxEvents=-1","outputLabel=Trees_TChannel_Herwig_Summer16_80X.root","isData=False","useLHE=True","lhes=externalLHEProducer"]

#config.JobType.pyCfgParams =["maxEvents=-1","outputLabel=Trees_TbarChannel_hDampUp_Summer16_80X.root","isData=False","useLHE=True","lhes=externalLHEProducer"]
#config.JobType.pyCfgParams =["maxEvents=-1","outputLabel=Trees_TbarChannel_hDampDown_Summer16_80X.root","isData=False","useLHE=True","lhes=externalLHEProducer"]
#config.JobType.pyCfgParams =["maxEvents=-1","outputLabel=Trees_TbarChannel_ScaleUp_Summer16_80X.root","isData=False","useLHE=True","lhes=externalLHEProducer"]
#config.JobType.pyCfgParams =["maxEvents=-1","outputLabel=Trees_TbarChannel_ScaleDown_Summer16_80X.root","isData=False","useLHE=True","lhes=externalLHEProducer"]
#config.JobType.pyCfgParams =["maxEvents=-1","outputLabel=Trees_TbarChannel_Herwig_Summer16_80X.root","isData=False","useLHE=True","lhes=externalLHEProducer"]


### For Data ###
#config.JobType.psetName = 'tree_data.py'

#config.JobType.pyCfgParams =["maxEvts=-1","outputLabel=Trees_Data_RunB_v3_ReReco_80X.root","useLHE=False","isData=True","Era=RunB"]
#config.JobType.pyCfgParams =["maxEvts=-1","outputLabel=Trees_Data_RunC_v1_ReReco_80X.root","useLHE=False","isData=True","Era=RunC"]
#config.JobType.pyCfgParams =["maxEvts=-1","outputLabel=Trees_Data_RunD_v1_ReReco_80X.root","useLHE=False","isData=True","Era=RunD"]
#config.JobType.pyCfgParams =["maxEvts=-1","outputLabel=Trees_Data_RunE_v1_ReReco_80X.root","useLHE=False","isData=True","Era=RunE"]
#config.JobType.pyCfgParams =["maxEvts=-1","outputLabel=Trees_Data_RunF_v1_ReReco_80X.root","useLHE=False","isData=True","Era=RunF"]
#config.JobType.pyCfgParams =["maxEvts=-1","outputLabel=Trees_Data_RunG_v1_ReReco_80X.root","useLHE=False","isData=True","Era=RunG"] 

#config.JobType.pyCfgParams =["maxEvts=-1","outputLabel=Trees_Data_RunH_v1_PromptReco_80X.root","useLHE=False","isData=True","Era=RunH"]
#config.JobType.pyCfgParams =["maxEvts=-1","outputLabel=Trees_Data_RunH_v2_PromptReco_80X.root","useLHE=False","isData=True","Era=RunH"]
#config.JobType.pyCfgParams =["maxEvts=-1","outputLabel=Trees_Data_RunH_v3_PromptReco_80X.root","useLHE=False","isData=True","Era=RunH"]

### Common to Data/MC ####

config.JobType.inputFiles = ['Summer16JEC/Summer16_23Sep2016BCDV4_DATA_L1FastJet_AK4PFchs.txt','Summer16JEC/Summer16_23Sep2016BCDV4_DATA_L2Residual_AK4PFchs.txt','Summer16JEC/Summer16_23Sep2016BCDV4_DATA_L3Absolute_AK4PFchs.txt','Summer16JEC/Summer16_23Sep2016BCDV4_DATA_L2L3Residual_AK4PFchs.txt','Summer16JEC/Summer16_23Sep2016BCDV4_DATA_UncertaintySources_AK4PFchs.txt','Summer16JEC/Summer16_23Sep2016EFV4_DATA_L1FastJet_AK4PFchs.txt','Summer16JEC/Summer16_23Sep2016EFV4_DATA_L2Residual_AK4PFchs.txt','Summer16JEC/Summer16_23Sep2016EFV4_DATA_L3Absolute_AK4PFchs.txt','Summer16JEC/Summer16_23Sep2016EFV4_DATA_L2L3Residual_AK4PFchs.txt','Summer16JEC/Summer16_23Sep2016EFV4_DATA_UncertaintySources_AK4PFchs.txt','Summer16JEC/Summer16_23Sep2016GV4_DATA_L1FastJet_AK4PFchs.txt','Summer16JEC/Summer16_23Sep2016GV4_DATA_L2Residual_AK4PFchs.txt','Summer16JEC/Summer16_23Sep2016GV4_DATA_L3Absolute_AK4PFchs.txt','Summer16JEC/Summer16_23Sep2016GV4_DATA_L2L3Residual_AK4PFchs.txt','Summer16JEC/Summer16_23Sep2016GV4_DATA_UncertaintySources_AK4PFchs.txt','Summer16JEC/Summer16_23Sep2016HV4_DATA_L1FastJet_AK4PFchs.txt','Summer16JEC/Summer16_23Sep2016HV4_DATA_L2Residual_AK4PFchs.txt','Summer16JEC/Summer16_23Sep2016HV4_DATA_L3Absolute_AK4PFchs.txt','Summer16JEC/Summer16_23Sep2016HV4_DATA_L2L3Residual_AK4PFchs.txt','Summer16JEC/Summer16_23Sep2016HV4_DATA_UncertaintySources_AK4PFchs.txt','Summer16JEC/Summer16_23Sep2016V4_MC_L1FastJet_AK4PFchs.txt','Summer16JEC/Summer16_23Sep2016V4_MC_L2Relative_AK4PFchs.txt','Summer16JEC/Summer16_23Sep2016V4_MC_L3Absolute_AK4PFchs.txt','Summer16JEC/Summer16_23Sep2016V4_MC_UncertaintySources_AK4PFchs.txt','pileUpDistrSummer16_25ns.root','DataPileupHistogram_69p2mbMinBias.root','DataPileupHistogram_69p2mbMinBias_down.root','DataPileupHistogram_69p2mbMinBias_up.root','cMVAv2_Moriond17_B_H.csv','btagging_cmva.root']

#config.JobType.inputFiles = ['Summer16JEC/Summer16_23Sep2016HV4_DATA_L1FastJet_AK4PFchs.txt','Summer16JEC/Summer16_23Sep2016HV4_DATA_L2Residual_AK4PFchs.txt','Summer16JEC/Summer16_23Sep2016HV4_DATA_L3Absolute_AK4PFchs.txt','Summer16JEC/Summer16_23Sep2016HV4_DATA_L2L3Residual_AK4PFchs.txt','Summer16JEC/Summer16_23Sep2016HV4_DATA_UncertaintySources_AK4PFchs.txt','Summer16JEC/Summer16_23Sep2016V4_MC_L1FastJet_AK4PFchs.txt','Summer16JEC/Summer16_23Sep2016V4_MC_L2Relative_AK4PFchs.txt','Summer16JEC/Summer16_23Sep2016V4_MC_L3Absolute_AK4PFchs.txt','Summer16JEC/Summer16_23Sep2016V4_MC_UncertaintySources_AK4PFchs.txt','pileUpDistrSummer16_25ns.root','DataPileupHistogram_69p2mbMinBias.root','DataPileupHistogram_69p2mbMinBias_down.root','DataPileupHistogram_69p2mbMinBias_up.root','cMVAv2_Moriond17_B_H.csv','btagging_cmva.root']

config.section_("Data")
### TChannel aMC@NLO+Pythia8
#config.Data.inputDataset = 

### TChannel powheg+Pythia8
config.Data.inputDataset = '/ST_t-channel_top_4f_inclusiveDecays_13TeV-powhegV2-madspin-pythia8_TuneCUETP8M1/smitra-TChannel_Powheg_EDMTuple_Summer16-de822f6bc13a69d387b17bc05e2b65fc/USER'

### TbarChannel powheg+Pythia8
#config.Data.inputDataset = '/ST_t-channel_antitop_4f_inclusiveDecays_13TeV-powhegV2-madspin-pythia8_TuneCUETP8M1/smitra-TbarChannel_Powheg_EDMTuple_Summer16-de822f6bc13a69d387b17bc05e2b65fc/USER'

### TbarWChannel
#config.Data.inputDataset = '/ST_tW_antitop_5f_inclusiveDecays_13TeV-powheg-pythia8_TuneCUETP8M2T4/smitra-TbarWChannel_EDMTuple_Summer16-de822f6bc13a69d387b17bc05e2b65fc/USER'

### TWChannel 
#config.Data.inputDataset = '/ST_tW_top_5f_inclusiveDecays_13TeV-powheg-pythia8_TuneCUETP8M2T4/smitra-TWChannel_EDMTuple_Summer16-de822f6bc13a69d387b17bc05e2b65fc/USER'

### SChannel
#config.Data.inputDataset = '/ST_s-channel_4f_leptonDecays_13TeV-amcatnlo-pythia8_TuneCUETP8M1/smitra-SChannel_EDMTuple_Summer16-de822f6bc13a69d387b17bc05e2b65fc/USER'

### TTbar
#config.Data.inputDataset = '/TT_TuneCUETP8M2T4_13TeV-powheg-pythia8/smitra-TTbar_EDMTuple_Summer16-de822f6bc13a69d387b17bc05e2b65fc/USER'

### W+Jets MG
#config.Data.inputDataset = ''

### W+Jets aMC@NLO Inclusive Jet
#config.Data.inputDataset = '/WJetsToLNu_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/smitra-WJets_aMCatNLO_EDMTuple_Summer16-de822f6bc13a69d387b17bc05e2b65fc/USER'
#config.Data.inputDataset = '/WToLNu_0J_13TeV-amcatnloFXFX-pythia8/smitra-Wp0Jets_aMCatNLO_EDMTuple_Summer16-de822f6bc13a69d387b17bc05e2b65fc/USER'
#config.Data.inputDataset = '/WToLNu_1J_13TeV-amcatnloFXFX-pythia8/smitra-Wp1Jets_aMCatNLO_EDMTuple_Summer16-de822f6bc13a69d387b17bc05e2b65fc/USER'
#config.Data.inputDataset = '/WToLNu_2J_13TeV-amcatnloFXFX-pythia8/smitra-Wp2Jets_aMCatNLO_EDMTuple_Summer16-de822f6bc13a69d387b17bc05e2b65fc/USER'


### DY+Jets
#config.Data.inputDataset = '/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/smitra-DYJets_aMCatNLO_EDMTuple_Summer16-de822f6bc13a69d387b17bc05e2b65fc/USER'

### QCD
#config.Data.inputDataset = '/QCD_Pt-20toInf_MuEnrichedPt15_TuneCUETP8M1_13TeV_pythia8/smitra-QCD_EDMTuple_Summer16-de822f6bc13a69d387b17bc05e2b65fc/USER'

### WW
#config.Data.inputDataset = '/WWTo1L1Nu2Q_13TeV_amcatnloFXFX_madspin_pythia8/smitra-WW1L1Nu2Q_EDMTuple_Summer16-de822f6bc13a69d387b17bc05e2b65fc/USER'
#config.Data.inputDataset = '/WWTo2L2Nu_13TeV-powheg/smitra-WW2L2Nu_EDMTuple_Summer16-de822f6bc13a69d387b17bc05e2b65fc/USER'

### WZ
#config.Data.inputDataset = '/WZTo1L1Nu2Q_13TeV_amcatnloFXFX_madspin_pythia8/smitra-WZ1L1Nu2Q_EDMTuple_Summer16-de822f6bc13a69d387b17bc05e2b65fc/USER'
#config.Data.inputDataset = '/WZTo2L2Q_13TeV_amcatnloFXFX_madspin_pythia8/smitra-WZ2L2Q_EDMTuple_Summer16-de822f6bc13a69d387b17bc05e2b65fc/USER'

### ZZ
#config.Data.inputDataset = '/ZZTo2L2Q_13TeV_amcatnloFXFX_madspin_pythia8/smitra-ZZ2L2Q_EDMTuple_Summer16-de822f6bc13a69d387b17bc05e2b65fc/USER'


### Systemtic Samples

### TChannel aMCatNLO
#config.Data.inputDataset = '/ST_t-channel_top_4f_leptonDecays_13TeV-powheg-pythia8_TuneCUETP8M1/RunIISpring16MiniAODv2-PUSpring16_80X_mcRun2_asymptotic_2016_miniAODv2_v0-v1/MINIAODSIM'
### TbarChannel aMCatNLO
#config.Data.inputDataset = '/ST_t-channel_antitop_4f_leptonDecays_13TeV-powheg-pythia8_TuneCUETP8M1/RunIISpring16MiniAODv2-PUSpring16_80X_mcRun2_asymptotic_2016_miniAODv2_v0-v1/MINIAODSIM' 

### TChannel Herwig++
#config.Data.inputDataset = '/ST_t-channel_top_4f_inclusiveDecays_13TeV-powhegV2-madspin-herwigpp/smitra-TChannel_Herwig_EDMTuple_Summer16-025cf3290a7040bdc212a7512ae6278f/USER'
### TbarChannel Herwig++
#config.Data.inputDataset = '/ST_t-channel_antitop_4f_inclusiveDecays_13TeV-powhegV2-madspin-herwigpp/smitra-TbarChannel_Herwig_EDMTuple_Summer16-025cf3290a7040bdc212a7512ae6278f/USER'


### TChannel_Mass_Down
#config.Data.inputDataset = '/ST_t-channel_top_4f_mtop1665_inclusiveDecays_13TeV-powhegV2-madspin-pythia8/smitra-TChannel_Mass166p5_EDMTuple_Summer16-025cf3290a7040bdc212a7512ae6278f/USER'
#config.Data.inputDataset = '/ST_t-channel_top_4f_mtop1695_inclusiveDecays_13TeV-powhegV2-madspin-pythia8/smitra-TChannel_Mass169p5_EDMTuple_Summer16-de822f6bc13a69d387b17bc05e2b65fc/USER'
#config.Data.inputDataset = '/ST_t-channel_top_4f_mtop1715_inclusiveDecays_13TeV-powhegV2-madspin-pythia8/smitra-TChannel_Mass171p5_EDMTuple_Summer16-de822f6bc13a69d387b17bc05e2b65fc/USER'

### TbarChannel_Mass_Down
#config.Data.inputDataset = '/ST_t-channel_antitop_4f_mtop1665_inclusiveDecays_13TeV-powhegV2-madspin-pythia8/smitra-TbarChannel_Mass166p5_EDMTuple_Summer16-025cf3290a7040bdc212a7512ae6278f/USER'
#config.Data.inputDataset = '/ST_t-channel_antitop_4f_mtop1695_inclusiveDecays_13TeV-powhegV2-madspin-pythia8/smitra-TbarChannel_Mass169p5_EDMTuple_Summer16-de822f6bc13a69d387b17bc05e2b65fc/USER
#config.Data.inputDataset = '/ST_t-channel_antitop_4f_mtop1715_inclusiveDecays_13TeV-powhegV2-madspin-pythia8/smitra-TbarChannel_Mass171p5_EDMTuple_Summer16-de822f6bc13a69d387b17bc05e2b65fc/USER'


### TChannel Mass_Up ###
#config.Data.inputDataset = '/ST_t-channel_top_4f_mtop1735_inclusiveDecays_13TeV-powhegV2-madspin-pythia8/smitra-TChannel_Mass173p5_EDMTuple_Summer16-de822f6bc13a69d387b17bc05e2b65fc/USER'
#config.Data.inputDataset = '/ST_t-channel_top_4f_mtop1755_inclusiveDecays_13TeV-powhegV2-madspin-pythia8/smitra-TChannel_Mass175_5_EDMTuple_Summer16-de822f6bc13a69d387b17bc05e2b65fc/USER'
#config.Data.inputDataset = '/ST_t-channel_top_4f_mtop1785_inclusiveDecays_13TeV-powhegV2-madspin-pythia8/smitra-TChannel_Mass178p5_EDMTuple_Summer16-025cf3290a7040bdc212a7512ae6278f/USER'

### TbarChannel_Mass_Up ###
#config.Data.inputDataset = '/ST_t-channel_antitop_4f_mtop1755_inclusiveDecays_13TeV-powhegV2-madspin-pythia8/smitra-TbarChannel_Mass175p5_EDMTuple_Summer16-de822f6bc13a69d387b17bc05e2b65fc/USER'
#config.Data.inputDataset = '/ST_t-channel_antitop_4f_mtop1735_inclusiveDecays_13TeV-powhegV2-madspin-pythia8/smitra-TbarChannel_Mass173p5_EDMTuple_Summer16-de822f6bc13a69d387b17bc05e2b65fc/USER'
#config.Data.inputDataset = '/ST_t-channel_antitop_4f_mtop1785_inclusiveDecays_13TeV-powhegV2-madspin-pythia8/smitra-TbarChannel_Mass178p5_EDMTuple_Summer16-025cf3290a7040bdc212a7512ae6278f/USER'

### TChannel Scale Up
#config.Data.inputDataset = '/ST_t-channel_top_4f_scaleup_inclusiveDecays_13TeV-powhegV2-madspin-pythia8/smitra-TChannel_ScaleUp_EDMTuple_Summer16-025cf3290a7040bdc212a7512ae6278f/USER'
### TChannel Scale Down
#config.Data.inputDataset = '/ST_t-channel_top_4f_scaledown_inclusiveDecays_13TeV-powhegV2-madspin-pythia8/smitra-TChannel_ScaleDown_EDMTuple_Summer16-025cf3290a7040bdc212a7512ae6278f/USER'

### TbarChannel Scale Up
#config.Data.inputDataset = '/ST_t-channel_antitop_4f_scaleup_inclusiveDecays_13TeV-powhegV2-madspin-pythia8/smitra-TbarChannel_ScaleUp_EDMTuple_Summer16-025cf3290a7040bdc212a7512ae6278f/USER'
### TbarChannel Scale Down 
#config.Data.inputDataset = '/ST_t-channel_antitop_4f_scaledown_inclusiveDecays_13TeV-powhegV2-madspin-pythia8/smitra-TbarChannel_ScaleDown_EDMTuple_Summer16-025cf3290a7040bdc212a7512ae6278f/USER'


### TTbar Mass_Up
#config.Data.inputDataset = '/TT_TuneCUETP8M2T4_mtop1785_13TeV-powheg-pythia8/smitra-TTbar_Mass178p5_EDMTuple_Summer16-025cf3290a7040bdc212a7512ae6278f/USER'
#config.Data.inputDataset = '/TT_TuneCUETP8M2T4_mtop1755_13TeV-powheg-pythia8/smitra-TTbar_Mass175p5_EDMTuple_Summer16-de822f6bc13a69d387b17bc05e2b65fc/USER'
#config.Data.inputDataset = '/TT_TuneCUETP8M2T4_mtop1735_13TeV-powheg-pythia8/smitra-TTbar_Mass173p5_EDMTuple_Summer16-de822f6bc13a69d387b17bc05e2b65fc/USER'

### TTbar Mass_Down
#config.Data.inputDataset = '/TT_TuneCUETP8M2T4_mtop1665_13TeV-powheg-pythia8/smitra-TTbar_Mass166p5_EDMTuple_Summer16-025cf3290a7040bdc212a7512ae6278f/USER'
#config.Data.inputDataset = '/TT_TuneCUETP8M2T4_mtop1695_13TeV-powheg-pythia8/smitra-TTbar_Mass169p5_EDMTuple_Summer16-de822f6bc13a69d387b17bc05e2b65fc/USER'
#config.Data.inputDataset = '/TT_TuneCUETP8M2T4_mtop1715_13TeV-powheg-pythia8/smitra-TTbar_Mass171p5_EDMTuple_Summer16-de822f6bc13a69d387b17bc05e2b65fc/USER'

### TTbar Scale Up
#config.Data.inputDataset = '/TT_TuneCUETP8M1_13TeV-powheg-scaleup-pythia8/RunIIFall15MiniAODv2-PU25nsData2015v1_76X_mcRun2_asymptotic_v12_ext3-v1/MINIAODSIM'
### TTbar Scale Down
#config.Data.inputDataset = '/TT_TuneCUETP8M1_13TeV-powheg-scaledown-pythia8/RunIIFall15MiniAODv2-PU25nsData2015v1_76X_mcRun2_asymptotic_v12_ext3-v1/MINIAODSIM'

### TChannel hDamp Up
#config.Data.inputDataset = '/ST_t-channel_top_4f_hdampup_inclusiveDecays_13TeV-powhegV2-madspin-pythia8/smitra-TChannel_hDampUp_EDMTuple_Summer16-025cf3290a7040bdc212a7512ae6278f/USER'
### TChannel hDamp Down
#config.Data.inputDataset = '/ST_t-channel_top_4f_hdampdown_inclusiveDecays_13TeV-powhegV2-madspin-pythia8/smitra-TChannel_hDampDown_EDMTuple_Summer16-025cf3290a7040bdc212a7512ae6278f/USER'

### TbarChannel hDamp Up
#config.Data.inputDataset = '/ST_t-channel_antitop_4f_hdampup_inclusiveDecays_13TeV-powhegV2-madspin-pythia8/smitra-TbarChannel_hDampUp_EDMTuple_Summer16-025cf3290a7040bdc212a7512ae6278f/USER'
### TChannel hDamp Down
#config.Data.inputDataset = '/ST_t-channel_antitop_4f_hdampdown_inclusiveDecays_13TeV-powhegV2-madspin-pythia8/smitra-TbarChannel_hDampDown_EDMTuple_Summer16-025cf3290a7040bdc212a7512ae6278f/USER'


#### Data 
#config.Data.inputDataset = '/SingleMuon/smitra-SingleMu_2016B_23SepReReco_v3-8e3fb509c5d1052c5b3ae79f14116dd7/USER'
#config.Data.inputDataset = '/SingleMuon/smitra-SingleMu_2016C_23SepReReco_v1-8e3fb509c5d1052c5b3ae79f14116dd7/USER'
#config.Data.inputDataset = '/SingleMuon/smitra-SingleMu_2016D_23SepReReco_v1-8e3fb509c5d1052c5b3ae79f14116dd7/USER'
#config.Data.inputDataset = '/SingleMuon/smitra-SingleMu_2016E_23SepReReco_v1-aacd84a8d761fd6e95770505c181bffc/USER'
#config.Data.inputDataset = '/SingleMuon/smitra-SingleMu_2016F_23SepReReco_v1-aacd84a8d761fd6e95770505c181bffc/USER'
#config.Data.inputDataset = '/SingleMuon/smitra-SingleMu_2016G_23SepReReco_v1-56c7d11e670cf8cfec530dfb67866b49/USER'

#config.Data.inputDataset = '/SingleMuon/smitra-SingleMu_2016H_PromptReco_v1-53e040755c08f56bf10964da750b5f1c/USER'
#config.Data.inputDataset = '/SingleMuon/smitra-SingleMu_2016H_PromptReco_v2-53e040755c08f56bf10964da750b5f1c/USER' 
#config.Data.inputDataset = '/SingleMuon/smitra-SingleMu_2016H_PromptReco_v3-53e040755c08f56bf10964da750b5f1c/USER'


config.Data.inputDBS = 'phys03'
config.Data.splitting = 'FileBased'
#config.Data.splitting = 'LumiBased'
config.Data.unitsPerJob = 1 
#config.Data.lumiMask = 'https://cms-service-dqm.web.cern.ch/cms-service-dqm/CAF/certification/Collisions16/13TeV/ReReco/Final/Cert_271036-284044_13TeV_23Sep2016ReReco_Collisions16_JSON.txt'

#config.Data.outLFNDirBase = '/store/user/smitra/25ns/TopMass/2017/Trees_80X/TChannel_Powheg/'
#config.Data.outLFNDirBase = '/store/user/smitra/25ns/TopMass/2017/Trees_80X/TbarChannel_Powheg/'
#config.Data.outLFNDirBase = '/store/user/smitra/25ns/TopMass/2017/Trees_80X/TWChannel_Powheg/'
#config.Data.outLFNDirBase = '/store/user/smitra/25ns/TopMass/2017/Trees_80X/TbarWChannel_Powheg/'
#config.Data.outLFNDirBase = '/store/user/smitra/25ns/TopMass/2017/Trees_80X/SChannel_aMCatNLO/'
#config.Data.outLFNDirBase = '/store/user/smitra/25ns/TopMass/2017/Trees_80X/TTbar/'

#config.Data.outLFNDirBase = '/store/user/smitra/25ns/TopMass/2017/Trees_80X/WJets_Inclusive_aMCatNLO/'
#config.Data.outLFNDirBase = '/store/user/smitra/25ns/TopMass/2017/Trees_80X/Wp0Jets/'
#config.Data.outLFNDirBase = '/store/user/smitra/25ns/TopMass/2017/Trees_80X/Wp1Jets/'
#config.Data.outLFNDirBase = '/store/user/smitra/25ns/TopMass/2017/Trees_80X/Wp2Jets/'

#config.Data.outLFNDirBase = '/store/user/smitra/25ns/TopMass/2017/Trees_80X/DYJets_aMCatNLO/'
#config.Data.outLFNDirBase = '/store/user/smitra/25ns/TopMass/2017/Trees_80X/QCD/'

#config.Data.outLFNDirBase = '/store/user/smitra/25ns/TopMass/2017/Trees_80X/WW1L1Nu2Q/'
#config.Data.outLFNDirBase = '/store/user/smitra/25ns/TopMass/2017/Trees_80X/WW2L2Nu/'
#config.Data.outLFNDirBase = '/store/user/smitra/25ns/TopMass/2017/Trees_80X/WZ1L1Nu2Q/'
#config.Data.outLFNDirBase = '/store/user/smitra/25ns/TopMass/2017/Trees_80X/WZ2L2Q/'
#config.Data.outLFNDirBase = '/store/user/smitra/25ns/TopMass/2017/Trees_80X/ZZ2L2Q/'

#config.Data.outLFNDirBase = '/store/user/smitra/25ns/TopMass/2017/Trees_80X/TChannel_Mass166p5/'
#config.Data.outLFNDirBase = '/store/user/smitra/25ns/TopMass/2017/Trees_80X/TChannel_Mass169p5/'
#config.Data.outLFNDirBase = '/store/user/smitra/25ns/TopMass/2017/Trees_80X/TChannel_Mass171p5/'
#config.Data.outLFNDirBase = '/store/user/smitra/25ns/TopMass/2017/Trees_80X/TChannel_Mass173p5/'
#config.Data.outLFNDirBase = '/store/user/smitra/25ns/TopMass/2017/Trees_80X/TChannel_Mass175p5/'
#config.Data.outLFNDirBase = '/store/user/smitra/25ns/TopMass/2017/Trees_80X/TChannel_Mass178p5/'

#config.Data.outLFNDirBase = '/store/user/smitra/25ns/TopMass/2017/Trees_80X/TbarChannel_Mass166p5/'
#config.Data.outLFNDirBase = '/store/user/smitra/25ns/TopMass/2017/Trees_80X/TbarChannel_Mass169p5/'
#config.Data.outLFNDirBase = '/store/user/smitra/25ns/TopMass/2017/Trees_80X/TbarChannel_Mass171p5/'
#config.Data.outLFNDirBase = '/store/user/smitra/25ns/TopMass/2017/Trees_80X/TbarChannel_Mass173p5/'
#config.Data.outLFNDirBase = '/store/user/smitra/25ns/TopMass/2017/Trees_80X/TbarChannel_Mass175p5/'
#config.Data.outLFNDirBase = '/store/user/smitra/25ns/TopMass/2017/Trees_80X/TbarChannel_Mass178p5/'

#config.Data.outLFNDirBase = '/store/user/smitra/25ns/TopMass/2017/Trees_80X/TTbar_Mass166p5/'
#config.Data.outLFNDirBase = '/store/user/smitra/25ns/TopMass/2017/Trees_80X/TTbar_Mass169p5/'
#config.Data.outLFNDirBase = '/store/user/smitra/25ns/TopMass/2017/Trees_80X/TTbar_Mass171p5/'
#config.Data.outLFNDirBase = '/store/user/smitra/25ns/TopMass/2017/Trees_80X/TTbar_Mass173p5/'
#config.Data.outLFNDirBase = '/store/user/smitra/25ns/TopMass/2017/Trees_80X/TTbar_Mass175p5/'
#config.Data.outLFNDirBase = '/store/user/smitra/25ns/TopMass/2017/Trees_80X/TTbar_Mass178p5/'

#config.Data.outLFNDirBase = '/store/user/smitra/25ns/TopMass/2017/Trees_80X/TChannel_Herwig/'
#config.Data.outLFNDirBase = '/store/user/smitra/25ns/TopMass/2017/Trees_80X/TbarChannel_Herwig/'

#config.Data.outLFNDirBase = '/store/user/smitra/25ns/TopMass/2017/Trees_80X/TChannel_hDampUp/'
#config.Data.outLFNDirBase = '/store/user/smitra/25ns/TopMass/2017/Trees_80X/TChannel_hDampDown/'

#config.Data.outLFNDirBase = '/store/user/smitra/25ns/TopMass/2017/Trees_80X/TbarChannel_hDampUp/'
#config.Data.outLFNDirBase = '/store/user/smitra/25ns/TopMass/2017/Trees_80X/TbarChannel_hDampDown/'

#config.Data.outLFNDirBase = '/store/user/smitra/25ns/TopMass/2017/Trees_80X/TChannel_ScaleUp/'
#config.Data.outLFNDirBase = '/store/user/smitra/25ns/TopMass/2017/Trees_80X/TChannel_ScaleDown/'

#config.Data.outLFNDirBase = '/store/user/smitra/25ns/TopMass/2017/Trees_80X/TbarChannel_ScaleUp/'
#config.Data.outLFNDirBase = '/store/user/smitra/25ns/TopMass/2017/Trees_80X/TbarChannel_ScaleDown/'

config.Data.outLFNDirBase = '/store/user/smitra/25ns/TopMass/2017/Trees_80X/Syst/TChannel/'
#config.Data.outLFNDirBase = '/store/user/smitra/25ns/TopMass/2017/Trees_80X/Syst/TbarChannel/'
#config.Data.outLFNDirBase = '/store/user/smitra/25ns/TopMass/2017/Trees_80X/Syst/TWChannel/'
#config.Data.outLFNDirBase = '/store/user/smitra/25ns/TopMass/2017/Trees_80X/Syst/TbarWChannel/'
#config.Data.outLFNDirBase = '/store/user/smitra/25ns/TopMass/2017/Trees_80X/Syst/SChannel/'
#config.Data.outLFNDirBase = '/store/user/smitra/25ns/TopMass/2017/Trees_80X/Syst/TTbar/'
#config.Data.outLFNDirBase = '/store/user/smitra/25ns/TopMass/2017/Trees_80X/Syst/WJets_Inclusive/'
#config.Data.outLFNDirBase = '/store/user/smitra/25ns/TopMass/2017/Trees_80X/Syst/DYJets/'
#config.Data.outLFNDirBase = '/store/user/smitra/25ns/TopMass/2017/Trees_80X/Syst/QCD/'
#config.Data.outLFNDirBase = '/store/user/smitra/25ns/TopMass/2017/Trees_80X/Syst/WW1L1Nu2Q/'
#config.Data.outLFNDirBase = '/store/user/smitra/25ns/TopMass/2017/Trees_80X/Syst/WW2L2Nu/'
#config.Data.outLFNDirBase = '/store/user/smitra/25ns/TopMass/2017/Trees_80X/Syst/WZ1L1Nu2Q/'
#config.Data.outLFNDirBase = '/store/user/smitra/25ns/TopMass/2017/Trees_80X/Syst/WZ2L2Q/'
#config.Data.outLFNDirBase = '/store/user/smitra/25ns/TopMass/2017/Trees_80X/Syst/ZZ2L2Q/'

### Data ###
#config.Data.outLFNDirBase = '/store/user/smitra/25ns/TopMass/2017/Trees_80X/Data_RunB_v3_ReReco/'
#config.Data.outLFNDirBase = '/store/user/smitra/25ns/TopMass/2017/Trees_80X/Data_RunC_ReReco/'
#config.Data.outLFNDirBase = '/store/user/smitra/25ns/TopMass/2017/Trees_80X/Data_RunD_ReReco/'
#config.Data.outLFNDirBase = '/store/user/smitra/25ns/TopMass/2017/Trees_80X/Data_RunE_ReReco/'
#config.Data.outLFNDirBase = '/store/user/smitra/25ns/TopMass/2017/Trees_80X/Data_RunF_ReReco/'
#config.Data.outLFNDirBase = '/store/user/smitra/25ns/TopMass/2017/Trees_80X/Data_RunG_ReReco/'
#config.Data.outLFNDirBase = '/store/user/smitra/25ns/TopMass/2017/Trees_80X/Data_RunH_v1_PromptReco/'
#config.Data.outLFNDirBase = '/store/user/smitra/25ns/TopMass/2017/Trees_80X/Data_RunH_v2_PromptReco/'
#config.Data.outLFNDirBase = '/store/user/smitra/25ns/TopMass/2017/Trees_80X/Data_RunH_v3_PromptReco/'

config.Data.ignoreLocality = True

config.section_("Site")
config.Site.storageSite = 'T2_IN_TIFR'
