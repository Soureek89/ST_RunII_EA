/**
 *\Class DMAnalysisTreeMaker
 * 
 * Produces analysis trees from edm-ntuples adding extra variables for resolved and unresolved tops
 * For custom systematics scenarios
 * 
 * \Author A. Orso M. Iorio
 * 
 * 
 *\version  $Id:
 * 
 * 
*/ 

#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/EDMException.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "SimDataFormats/GeneratorProducts/interface/LHEEventProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"
#include "CondFormats/JetMETObjects/interface/FactorizedJetCorrector.h"
#include "CondFormats/JetMETObjects/interface/JetCorrectionUncertainty.h"
#include "CondFormats/JetMETObjects/interface/JetCorrectorParameters.h"
#include "DataFormats/Math/interface/LorentzVector.h"
#include "DataFormats/Math/interface/deltaPhi.h"
#include "DataFormats/Math/interface/deltaR.h"
#include <Math/VectorUtil.h>

#include "TFile.h"
#include "TTree.h"
#include "TMath.h"
#include<vector>
#include<algorithm>

//using namespace reco;
using namespace edm;
using namespace std;


namespace LHAPDF
{
void initPDFSet(int nset, const std::string &filename, int member = 0);
int numberPDF(int nset);
void usePDFMember(int nset, int member);
double xfx(int nset, double x, double Q, int fl);
double getXmin(int nset, int member);
double getXmax(int nset, int member);
double getQ2min(int nset, int member);
double getQ2max(int nset, int member);
void extrapolate(bool extrapolate = true);
}

class  DMAnalysisTreeMaker : public edm::EDAnalyzer 
{
public:
  explicit DMAnalysisTreeMaker( const edm::ParameterSet & );   

private:
  virtual void analyze(const edm::Event &, const edm::EventSetup & );
  vector<string> additionalVariables(string);
  string makeName(string label,string pref,string var);
  string makeBranchName(string label, string pref, string var);
  void initializePdf(string centralpdf,string variationpdf);
  void getEventPdf();
  bool getEventTriggers();
  void getEventLHEWeights();
  bool getMETFilters();
  double jetUncertainty(double pt, double eta, string syst);
  //  double smearPt(double pt, double genpt, double eta, string syst);
  double smear(double pt, double genpt, double eta, string syst);
  double resolSF(double eta, string syst);
  double getScaleFactor(double pt, double eta, double partonFlavour, string syst);
  

  bool isInVector(std::vector<std::string> v, std::string s);
  std::vector<edm::ParameterSet > physObjects;
  std::vector<edm::InputTag > variablesFloat, variablesInt, singleFloat, singleInt;
  edm::InputTag lhes_;
  edm::InputTag genprod_;
  edm::InputTag triggerNames_;
  edm::InputTag triggerBits_;
  edm::InputTag triggerPrescales_;
  edm::InputTag lumiBlock_;
  edm::InputTag runNumber_;
  edm::InputTag eventNumber_;
  edm::InputTag HBHEFilter_;

  edm::InputTag metNames_;
  edm::InputTag metBits_;

  edm::InputTag pvZ_,pvChi2_,pvNdof_,pvRho_;

  //  edm::LumiReWeighting LumiWeights_, LumiWeightsUp_, LumiWeightsDown_;


  TTree * treesBase;
  map<string, TTree * > trees;
  std::vector<string> names;
  std::vector<string> systematics;
  map< string , float[100] > vfloats_values;
  map< string , int[100] > vints_values;
  map< string , string > obs_to_obj;
  map< string , string > obj_to_pref;
  
  map< string , float > float_values;
  map< string , int > int_values;
  map< string , int > sizes;

  map< string , bool > got_label; 
  map< string , int > max_instances; 
  map< int, int > subj_jet_map;

  map<string, edm::Handle<std::vector<float> > > h_floats;
  map<string, edm::Handle<std::vector<int> > > h_ints;
  map<string, edm::Handle<float> > h_float;
  map<string, edm::Handle<int> >h_int;

  string mu_label, ele_label, jets_label, met_label, jetsnohf_label; 
 
   
  //MC info:
  edm::ParameterSet channelInfo;
  std::string channel;
  double crossSection, originalEvents;
  bool useLHEWeights, useLHE, useTriggers,cutOnTriggers, useMETFilters, addPV;
  bool addLHAPDFWeights;
  string centralPdfSet,variationPdfSet;
  std::vector<string> leptonicTriggers, hadronicTriggers, metFilters;
  int maxPdf, maxWeights;
  edm::Handle<LHEEventProduct > lhes;
  edm::Handle<GenEventInfoProduct> genprod;

  edm::Handle<std::vector<float> > triggerBits;
  edm::Handle<std::vector<string> > triggerNames;
  edm::Handle<std::vector<int> > triggerPrescales;

  edm::Handle<std::vector<float> > metBits;
  edm::Handle<std::vector<string> > metNames;
  edm::Handle<bool> HBHE;

  edm::Handle<unsigned int> lumiBlock;
  edm::Handle<unsigned int> runNumber;
  edm::Handle<ULong64_t> eventNumber;

  //  float pdf_weights[140];
  //float lhe_weights[20];  
  // std::string lhe_weights_id[20];  

  edm::Handle<std::vector<float> > pvZ,pvChi2,pvRho;
  edm::Handle<std::vector<int> > pvNdof;
  float nPV;

  //JEC info
  bool changeJECs;
  bool isData, useMETNoHF;
  edm::Handle<double> rho;
  double Rho;
  std::vector<double> jetScanCuts;
  std::vector<JetCorrectorParameters> jecPars;
  JetCorrectorParameters *jecParsL1, *jecParsL2, *jecParsL3, *jecParsL2L3Residuals;
  JetCorrectionUncertainty *jecUnc;
  FactorizedJetCorrector *jecCorr;


  bool isFirstEvent;

  class BTagWeight
  {
  private:
    int minTags;
    int maxTags;
  public:
    struct JetInfo
    {
      JetInfo(float mceff, float datasf) : eff(mceff), sf(datasf) {}
      float eff;
      float sf;
    };
    BTagWeight():
      minTags(0), maxTags(0)
    {
      ;
    }
    BTagWeight(int jmin, int jmax) :
      minTags(jmin) , maxTags(jmax) {}
    bool filter(int t);
    float weight(vector<JetInfo> jets, int tags);
    float weightWithVeto(vector<JetInfo> jetsTags, int tags, vector<JetInfo> jetsVetoes, int vetoes);
  };
  vector<BTagWeight::JetInfo> jsfscsvt, 
    jsfscsvt_b_tag_up, 
    jsfscsvt_b_tag_down, 
    jsfscsvt_mistag_up, 
    jsfscsvt_mistag_down;

  vector<BTagWeight::JetInfo> jsfscsvm, 
    jsfscsvm_b_tag_up, 
    jsfscsvm_b_tag_down, 
    jsfscsvm_mistag_up, 
    jsfscsvm_mistag_down;

  vector<BTagWeight::JetInfo> jsfscsvl, 
    jsfscsvl_b_tag_up, 
    jsfscsvl_b_tag_down, 
    jsfscsvl_mistag_up, 
    jsfscsvl_mistag_down;
  
  BTagWeight b_csvt_0_tags= BTagWeight(0,0),
    b_csvt_1_tag= BTagWeight(1,1),
    b_csvt_2_tags= BTagWeight(2,2);
  
  double b_weight_csvt_0_tags,
    b_weight_csvt_1_tag,
    b_weight_csvt_2_tags;
  double b_weight_csvt_0_tags_mistag_up,
    b_weight_csvt_1_tag_mistag_up,
    b_weight_csvt_2_tags_mistag_up;
  double b_weight_csvt_0_tags_mistag_down,
    b_weight_csvt_1_tag_mistag_down,
    b_weight_csvt_2_tags_mistag_down;
  double b_weight_csvt_0_tags_b_tag_down,
    b_weight_csvt_1_tag_b_tag_down,
    b_weight_csvt_2_tags_b_tag_down;
  double b_weight_csvt_0_tags_b_tag_up,
    b_weight_csvt_1_tag_b_tag_up,
    b_weight_csvt_2_tags_b_tag_up;

  BTagWeight b_csvm_0_tags= BTagWeight(0,0),
    b_csvm_1_tag= BTagWeight(1,1),
    b_csvm_2_tags= BTagWeight(2,2);

  double b_weight_csvm_0_tags,
    b_weight_csvm_1_tag,
    b_weight_csvm_2_tags;
  double b_weight_csvm_0_tags_mistag_up,
    b_weight_csvm_1_tag_mistag_up,
    b_weight_csvm_2_tags_mistag_up;
  double b_weight_csvm_0_tags_mistag_down,
    b_weight_csvm_1_tag_mistag_down,
    b_weight_csvm_2_tags_mistag_down;
  double b_weight_csvm_0_tags_b_tag_down,
    b_weight_csvm_1_tag_b_tag_down,
    b_weight_csvm_2_tags_b_tag_down;
  double b_weight_csvm_0_tags_b_tag_up,
    b_weight_csvm_1_tag_b_tag_up,
    b_weight_csvm_2_tags_b_tag_up;

  BTagWeight b_csvl_0_tags= BTagWeight(0,0),
    b_csvl_1_tag= BTagWeight(1,1),
    b_csvl_2_tags= BTagWeight(2,2);
  
  double b_weight_csvl_0_tags_mistag_up,
    b_weight_csvl_1_tag_mistag_up,
    b_weight_csvl_2_tags_mistag_up;
  double b_weight_csvl_0_tags_mistag_down,
    b_weight_csvl_1_tag_mistag_down,
    b_weight_csvl_2_tags_mistag_down;
  double b_weight_csvl_0_tags_b_tag_down,
    b_weight_csvl_1_tag_b_tag_down,
    b_weight_csvl_2_tags_b_tag_down;
  double b_weight_csvl_0_tags_b_tag_up,
    b_weight_csvl_1_tag_b_tag_up,
    b_weight_csvl_2_tags_b_tag_up;
  double b_weight_csvl_0_tags,
    b_weight_csvl_1_tag,
    b_weight_csvl_2_tags;

  double MCTagEfficiency(string algo, int flavor, double pt); 
  double TagScaleFactor(string algo, int flavor, string syst,double pt);
 
  //
  bool doBTagSF=true;
  
};


DMAnalysisTreeMaker::DMAnalysisTreeMaker(const edm::ParameterSet& iConfig){
  
  mu_label = iConfig.getParameter<std::string >("muLabel");
  ele_label = iConfig.getParameter<std::string >("eleLabel");
  jets_label = iConfig.getParameter<std::string >("jetsLabel");
  met_label = iConfig.getParameter<std::string >("metLabel");
  jetsnohf_label = iConfig.getParameter<std::string >("jetsnohfLabel");
  physObjects = iConfig.template getParameter<std::vector<edm::ParameterSet> >("physicsObjects");
  
  channelInfo = iConfig.getParameter<edm::ParameterSet >("channelInfo"); // The physics of the channel, e.g. the cross section, #original events, etc.
  channel = channelInfo.getParameter<std::string>("channel");
  crossSection = channelInfo.getParameter<double>("crossSection");
  originalEvents = channelInfo.getParameter<double>("originalEvents");
  useLHEWeights = channelInfo.getUntrackedParameter<bool>("useLHEWeights",false);
  //useLHE = channelInfo.getUntrackedParameter<bool>("useLHE",false);
  useLHE = channelInfo.getUntrackedParameter<bool>("useLHE",false);
  //addLHAPDFWeights = channelInfo.getUntrackedParameter<bool>("addLHAPDFWeights",false);
  addLHAPDFWeights = channelInfo.getUntrackedParameter<bool>("addLHAPDFWeights",true);
  
  genprod_ = iConfig.getParameter<edm::InputTag>( "genprod" );
  
  useTriggers = iConfig.getUntrackedParameter<bool>("useTriggers",true);
  cutOnTriggers = iConfig.getUntrackedParameter<bool>("cutOnTriggers",true);

  lumiBlock_ = iConfig.getParameter<edm::InputTag>("lumiBlock");
  runNumber_ = iConfig.getParameter<edm::InputTag>("runNumber");
  eventNumber_ = iConfig.getParameter<edm::InputTag>("eventNumber");

  if(useTriggers){
    triggerBits_ = iConfig.getParameter<edm::InputTag>("triggerBits");
    triggerNames_ = iConfig.getParameter<edm::InputTag>("triggerNames");
    triggerPrescales_ = iConfig.getParameter<edm::InputTag>("triggerPrescales");
    leptonicTriggers= channelInfo.getParameter<std::vector<string> >("leptonicTriggers");
    hadronicTriggers= channelInfo.getParameter<std::vector<string> >("hadronicTriggers");
  }
  useMETFilters = iConfig.getUntrackedParameter<bool>("useMETFilters",true);
  if(useMETFilters){
    metFilters = channelInfo.getParameter<std::vector<string> >("metFilters");
    metBits_ = iConfig.getParameter<edm::InputTag>("metBits");
    metNames_ = iConfig.getParameter<edm::InputTag>("metNames");
    HBHEFilter_ = iConfig.getParameter<edm::InputTag>("HBHEFilter");
  }

  addPV = iConfig.getUntrackedParameter<bool>("addPV",true);
  changeJECs = iConfig.getUntrackedParameter<bool>("changeJECs",false);
  isData = iConfig.getUntrackedParameter<bool>("isData",false);
  useMETNoHF = iConfig.getUntrackedParameter<bool>("useMETNoHF",false);
  if(addPV || changeJECs){
    pvZ_ = iConfig.getParameter<edm::InputTag >("vertexZ");
    pvChi2_ = iConfig.getParameter<edm::InputTag >("vertexChi2");
    pvNdof_ = iConfig.getParameter<edm::InputTag >("vertexNdof");
    pvRho_ = iConfig.getParameter<edm::InputTag >("vertexRho");
  }

  if(useLHEWeights){
    maxWeights = channelInfo.getUntrackedParameter<int>("maxWeights",9);//Usually we do have 9 weights for the scales, might vary depending on the lhe
  }

  
  if(addLHAPDFWeights){
    centralPdfSet = channelInfo.getUntrackedParameter<string>("pdfSet","CT10");
    //centralPdfSet = channelInfo.getUntrackedParameter<string>("pdfSet","NNPDF");
    variationPdfSet = channelInfo.getUntrackedParameter<string>("pdfSet","CT10");
    //variationPdfSet = channelInfo.getUntrackedParameter<string>("pdfSet","NNPDF");
    initializePdf(centralPdfSet,variationPdfSet);

  }
  

  systematics = iConfig.getParameter<std::vector<std::string> >("systematics");

  jetScanCuts = iConfig.getParameter<std::vector<double> >("jetScanCuts");
  
  
  std::vector<edm::ParameterSet >::const_iterator itPsets = physObjects.begin();

  bool addNominal=false;
  for (size_t s = 0; s<systematics.size();++s){
    if(systematics.at(s).find("noSyst")!=std::string::npos) {
      addNominal=true;
      break;
    }
  }
  if(systematics.size()==0){
    addNominal=true;
    systematics.push_back("noSyst");
  }//In case there's no syst specified, do the nominal scenario
  //addNominal=true;
  Service<TFileService> fs;
  TFileDirectory DMTrees;// = fs->mkdir( "systematics_trees" );


  if(addNominal){
    //Service<TFileService> fs;
    DMTrees = fs->mkdir( "systematics_trees" );
  }
  //  treesBase = new TTree("TreeBase", "TreeBase");
  trees["noSyst"] =  new TTree((channel+"__noSyst").c_str(),(channel+"__noSyst").c_str());
  
  
  lhes_ = iConfig.getParameter<edm::InputTag>( "lhes" );


  for (;itPsets!=physObjects.end();++itPsets){ 
    int maxI = itPsets->getUntrackedParameter< int >("maxInstances",10);
    variablesFloat = itPsets->template getParameter<std::vector<edm::InputTag> >("variablesF"); 
    variablesInt = itPsets->template getParameter<std::vector<edm::InputTag> >("variablesI"); 
    singleFloat = itPsets->template getParameter<std::vector<edm::InputTag> >("singleF"); 
    singleInt = itPsets->template getParameter<std::vector<edm::InputTag> >("singleI"); 
    string namelabel = itPsets->getParameter< string >("label");
    string nameprefix = itPsets->getParameter< string >("prefix");
    bool saveBaseVariables = itPsets->getUntrackedParameter<bool>("saveBaseVariables",true);
    std::vector<std::string > toSave= itPsets->getParameter<std::vector<std::string> >("toSave");
    std::vector<edm::InputTag >::const_iterator itF = variablesFloat.begin();
    std::vector<edm::InputTag >::const_iterator itI = variablesInt.begin();
    std::vector<edm::InputTag >::const_iterator itsF = singleFloat.begin();
    std::vector<edm::InputTag >::const_iterator itsI = singleInt.begin();

    stringstream max_instance_str;
    max_instance_str<<maxI;
    max_instances[namelabel]=maxI;
    string nameobs = namelabel;
    string prefix = nameprefix;
    for (;itF != variablesFloat.end();++itF){
      
      string name=itF->instance()+"_"+itF->label();
      string nameinstance=itF->instance();
      string nameshort=itF->instance();
      //if(nameobs!=itF->label())cout<<" warning! label not matching the module! check if members of pset are consistent. If intentional , ignore this warning";
      //cout << "nameobs "<< nameobs<< " name " << name <<" nameshort " <<nameshort << " strsizw "<< (nameshort+"["+max_instance_str.str()+"]/F").c_str()<<endl;
      //      std::cout << " label is " << itF->instance()<< " is in vector? "<< isInVector(toSave,itF->instance())<< endl;
      string nametobranch = makeBranchName(namelabel,prefix,nameinstance);
      name = nametobranch;
      nameshort = nametobranch;
      //      cout << " name for the branch is now: "<< nametobranch<<endl;
      //      if(saveBaseVariables|| isInVector(toSave,itF->instance())) trees["noSyst"]->Branch(nameshort.c_str(), &vfloats_values[name],(nameshort+"["+max_instance_str.str()+"]/F").c_str());
      if(saveBaseVariables|| isInVector(toSave,itF->instance())) trees["noSyst"]->Branch(nameshort.c_str(), &vfloats_values[name],(nameshort+"["+max_instance_str.str()+"]/F").c_str());
      names.push_back(name);
      obs_to_obj[name] = nameobs;
      obj_to_pref[nameobs] = prefix;
      cout << " branching name "<< name<< " for obs "<< nameobs << " instance "<< nameinstance << endl;
    }
    
    for (;itI != variablesInt.end();++itI){
      string name=itI->instance()+"_"+itI->label();
      string nameshort=itF->instance();
      string nametobranch = makeBranchName(namelabel,prefix,nameshort);
      name = nametobranch;
      nameshort = nametobranch;
      if(saveBaseVariables|| isInVector(toSave,itI->instance())) trees["noSyst"]->Branch(nameshort.c_str(), &vints_values[name],(nameshort+"["+max_instance_str.str()+"]/I").c_str());
      names.push_back(name);
      obs_to_obj[name] = nameobs;
      obj_to_pref[nameobs] = prefix;
    }  
    
    if (variablesFloat.size()>0){
      string nameshortv = namelabel;
      vector<string> extravars = additionalVariables(nameshortv);
      for(size_t addv = 0; addv < extravars.size();++addv){
	string name = nameshortv+"_"+extravars.at(addv);
	if (saveBaseVariables || isInVector(toSave, extravars.at(addv)) || isInVector(toSave, "allExtra") )trees["noSyst"]->Branch(name.c_str(), &vfloats_values[name],(name+"["+max_instance_str.str()+"]/F").c_str());
	obs_to_obj[name] = nameobs;
	obj_to_pref[nameobs] = prefix;
      }
    }
    names.push_back(nameobs);
    cout << "size part: nameobs is  "<< nameobs<<endl;
    trees["noSyst"]->Branch((nameobs+"_size").c_str(), &sizes[nameobs]);
    
    //Initialize single pset objects
    for (;itsF != singleFloat.end();++itsF){
      string name=itsF->instance()+itsF->label();
      string nameshort=itsF->instance();
      string nametobranch = makeBranchName(namelabel,prefix,nameshort);
      name = nametobranch;
      nameshort = nametobranch;
      if(saveBaseVariables|| isInVector(toSave,itsF->instance()))trees["noSyst"]->Branch(nameshort.c_str(), &float_values[name]);
    }
    for (;itsI != singleInt.end();++itsI){
      string name=itsI->instance()+itsI->label();
      string nameshort=itsI->instance();
      string nametobranch = makeBranchName(namelabel,prefix,nameshort);
      name = nametobranch;
      nameshort = nametobranch;
      if(saveBaseVariables|| isInVector(toSave,itsI->instance()))trees["noSyst"]->Branch(nameshort.c_str(), &int_values[name]);
    }
  }
  
  string nameshortv= "Event";
  vector<string> extravars = additionalVariables(nameshortv);
  for(size_t addv = 0; addv < extravars.size();++addv){
    string name = nameshortv+"_"+extravars.at(addv);
    trees["noSyst"]->Branch(name.c_str(), &float_values[name],(name+"/F").c_str());
  }
  
  //Prepare the trees cloning all branches and setting the correct names/titles:
  if(!addNominal){
    DMTrees = fs->mkdir( "systematics_trees" );
  }
  for(size_t s=0;s< systematics.size();++s){
    std::string syst  = systematics.at(s);
    if(syst=="noSyst")continue;
    trees[syst]= trees["noSyst"]->CloneTree();
    //trees[syst]= treesBase->CloneTree();
    trees[syst]->SetName((channel+"__"+syst).c_str());
    trees[syst]->SetTitle((channel+"__"+syst).c_str());
  }
  

  jecParsL1  = new JetCorrectorParameters("Summer15_50nsV4_MC_L1FastJet_AK4PFchs.txt");
  jecParsL2  = new JetCorrectorParameters("Summer15_50nsV4_MC_L2Relative_AK4PFchs.txt");
  jecParsL3  = new JetCorrectorParameters("Summer15_50nsV4_MC_L3Absolute_AK4PFchs.txt");
  jecParsL2L3Residuals  = new JetCorrectorParameters("Summer15_50nsV4_DATA_L2L3Residual_AK4PFchs.txt");
  jecPars.push_back(*jecParsL1);
  jecPars.push_back(*jecParsL2);
  jecPars.push_back(*jecParsL3);
  if(isData)jecPars.push_back(*jecParsL2L3Residuals);

  jecCorr = new FactorizedJetCorrector(jecPars);
  
  jecUnc  = new JetCorrectionUncertainty(*(new JetCorrectorParameters("Summer15_50nsV4_DATA_UncertaintySources_AK4PFchs.txt", "Total")));
  
  //  if(addNominal) systematics.push_back("noSyst");
 
}

void DMAnalysisTreeMaker::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) {

  std::vector<edm::ParameterSet >::const_iterator itPsets = physObjects.begin();

  //  if(addLHAPDFWeights){

  // event info
  iEvent.getByLabel(lumiBlock_,lumiBlock );
  iEvent.getByLabel(runNumber_,runNumber );
  iEvent.getByLabel(eventNumber_,eventNumber );

  if(useLHE){
    iEvent.getByLabel(lhes_, lhes);
  }
  if(addLHAPDFWeights){
    iEvent.getByLabel(genprod_, genprod);
  }

  //Part 0: trigger preselection
  if(useTriggers){
    iEvent.getByLabel(triggerBits_,triggerBits );
    iEvent.getByLabel(triggerNames_,triggerNames );
    iEvent.getByLabel(triggerPrescales_,triggerPrescales );
    bool triggerOr = getEventTriggers();
    if(cutOnTriggers && !triggerOr) return;
  }  
  if(useMETFilters){
    iEvent.getByLabel(metBits_,metBits );
    iEvent.getByLabel(metNames_,metNames );
    iEvent.getByLabel(HBHEFilter_ ,HBHE);
    if(isFirstEvent){
      for(size_t bt = 0; bt < metNames->size();++bt){
	std::string tname = metNames->at(bt);
	cout << "test tname "<< tname << " passes "<< metBits->at(bt)<< endl;
      }
      //cout << "test tname "<< tname <<endl;
      isFirstEvent = false;
    }
    getMETFilters();
  }
  
  if(changeJECs){
    iEvent.getByLabel("fixedGridRhoFastjetAll",rho);
    Rho = *rho; 
  }
  if( addPV || changeJECs){
    iEvent.getByLabel(pvZ_,pvZ);
    iEvent.getByLabel(pvChi2_,pvChi2);
    iEvent.getByLabel(pvNdof_,pvNdof);
    iEvent.getByLabel(pvRho_,pvRho);
    nPV = pvZ->size();
  }

    /*    std::cout << " initTriggers "<< endl;
    for(size_t bt = 0; bt < triggerNames->size();++bt){
      std::string tname = triggerNames->at(bt);
      float bit = triggerBits->at(bt);
      int presc = triggerPrescales->at(bt);
      std::cout << "name "<< tname << " bit "<< bit << " prescale "<<presc<<endl;
      }*/
    
    
  

  //Part 1 taking the obs values from the edm file
  for (;itPsets!=physObjects.end();++itPsets){ 
    variablesFloat = itPsets->template getParameter<std::vector<edm::InputTag> >("variablesF"); 
    variablesInt = itPsets->template getParameter<std::vector<edm::InputTag> >("variablesI"); 
    singleFloat = itPsets->template getParameter<std::vector<edm::InputTag> >("singleF"); 
    singleInt = itPsets->template getParameter<std::vector<edm::InputTag> >("singleI"); 
    std::vector<edm::InputTag >::const_iterator itF = variablesFloat.begin();
    std::vector<edm::InputTag >::const_iterator itI = variablesInt.begin();
    std::vector<edm::InputTag >::const_iterator itsF = singleFloat.begin();
    std::vector<edm::InputTag >::const_iterator itsI = singleInt.begin();
    string namelabel = itPsets->getParameter< string >("label");
    string nameprefix = itPsets->getParameter< string >("prefix");
    size_t maxInstance=(size_t)max_instances[namelabel];
    
    //Vectors of floats
    for (;itF != variablesFloat.end();++itF){
      string varname=itF->instance();
      
      string name = makeBranchName(namelabel,nameprefix,varname);
      
      //string namelabel;
      float tmp =1.0;
      iEvent.getByLabel(*(itF),h_floats[name]);
      //      cout << "name "<< name <<endl;
      for (size_t fi = 0;fi < maxInstance ;++fi){
	if(fi <h_floats[name]->size()){tmp = h_floats[name]->at(fi);}
	else { tmp = -9999.; }
	//	cout << " setting name "<< name<< " at instance "<< fi <<" to value "<< tmp <<endl;
	vfloats_values[name][fi]=tmp;
      }
      sizes[namelabel]=h_floats[name]->size();
      //      cout<< " size for "<< namelabel <<" is then "<< sizes[namelabel]<<endl; 
    }
    
    //    std::cout << " checkpoint floats"<<endl;
    //Vectors of ints
    for (;itI != variablesInt.end();++itI){
      string varname=itI->instance();
      string name = makeBranchName(namelabel,nameprefix,varname);
      int tmp = 1;
      iEvent.getByLabel(*(itI),h_ints[name]);
      for (size_t fi = 0;fi < maxInstance;++fi){
	if(fi <h_ints[name]->size()){tmp = h_ints[name]->at(fi);}
	else { tmp = -9999.; }
	vints_values[name][fi]=tmp;
      }
    }  
    
    //    std::cout << " checkpoint ints"<<endl;
    //Single floats/ints
    for (;itsF != singleFloat.end();++itsF){
      string varname=itsF->instance();
      string name = makeBranchName(namelabel,nameprefix,varname);

      iEvent.getByLabel(*(itsF),h_float[name]);
      float_values[name]=*h_float[name];
    }
    for (;itsI != singleInt.end();++itsI){
      string varname=itsI->instance();
      string name = makeBranchName(namelabel,nameprefix,varname);

      iEvent.getByLabel(*(itsI),h_int[name]);
      int_values[name]=*h_int[name];
    }
    //    std::cout << " checkpoint singles"<<endl;
  }

  //  std::cout << " checkpoint part 1"<<endl;


  //Part 2: selection and analysis-level changes
  //This might change for each particular systematics, 
  //e.g. for each jet energy scale variation, for MET or even lepton energy scale variations
  vector<TLorentzVector> electrons;
  vector<TLorentzVector> muons;
  vector<TLorentzVector> leptons;
  vector<TLorentzVector> loosemuons;
  vector<TLorentzVector> jets;
  vector<TLorentzVector> muons_t;

  for (size_t s = 0; s< systematics.size();++s){

    int ncsvl_tags=0,ncsvt_tags=0,ncsvm_tags=0;//, njets_tottag=0;    
    getEventTriggers();

    electrons.clear();
    muons.clear();
    loosemuons.clear();
    jets.clear();
    string syst = systematics.at(s);

    jsfscsvt.clear();
    jsfscsvt_b_tag_up.clear(); 
    jsfscsvt_b_tag_down.clear(); 
    jsfscsvt_mistag_up.clear(); 
    jsfscsvt_mistag_down.clear();

    jsfscsvm.clear(); 
    jsfscsvm_b_tag_up.clear(); 
    jsfscsvm_b_tag_down.clear(); 
    jsfscsvm_mistag_up.clear(); 
    jsfscsvm_mistag_down.clear();
    
    jsfscsvl.clear(); 
    jsfscsvl_b_tag_up.clear(); 
    jsfscsvl_b_tag_down.clear(); 
    jsfscsvl_mistag_up.clear();
    jsfscsvl_mistag_down.clear();

    /**************************
    Muons:
    **************************/
    for(int mu = 0;mu < max_instances[mu_label] ;++mu){
      string pref = obj_to_pref[mu_label];
      //      std::cout << " now checking name " << makeName(mu_label,pref,"IsTightMuon")<<std::endl;
      float isTight = vfloats_values[makeName(mu_label,pref,"IsTightMuon")][mu];
      float isLoose = vfloats_values[makeName(mu_label,pref,"IsLooseMuon")][mu];
      float isSoft = vfloats_values[makeName(mu_label,pref,"IsSoftMuon")][mu];
      //      float isVeto = vfloats_values[makeName(ele_label,pref,"isVeto")][el];
      float pt = vfloats_values[makeName(mu_label,pref,"Pt")][mu];
      float eta = vfloats_values[makeName(mu_label,pref,"Eta")][mu];
      float phi = vfloats_values[makeName(mu_label,pref,"Phi")][mu];
      float energy = vfloats_values[makeName(mu_label,pref,"E")][mu];
      float iso = vfloats_values[makeName(mu_label,pref,"Iso04")][mu];
      //      std::cout << " muon #"<<el<< " pt " << pt << " isTight/Loose/Soft?"<< isTight<<isSoft<<isLoose<<std::endl;
      
      if(isTight>0 && pt> 22 && abs(eta) < 2.1 && iso <0.06){
	++float_values["Event_nTightMuons"];
	TLorentzVector muon;
	muon.SetPtEtaPhiE(pt, eta, phi, energy);
	muons.push_back(muon);
	muons_t.push_back(muon);
	leptons.push_back(muon);
      }
      if(isLoose>0 && pt> 10 && abs(eta) < 2.1 && iso<0.2){
	++float_values["Event_nLooseMuons"];
      }
      if(isSoft>0 && pt> 30 && abs(eta) < 2.4){
	++float_values["Event_nSoftMuons"]; 
      }
      if(isLoose>0 && pt > 15){
	TLorentzVector muon;
	muon.SetPtEtaPhiE(pt, eta, phi, energy);
	loosemuons.push_back(muon);
      }

    }

    /**************************
    Electrons:
    **************************/
    for(int el = 0;el < max_instances[ele_label] ;++el){
      string pref = obj_to_pref[ele_label];
      float pt = vfloats_values[makeName(ele_label,pref,"Pt")][el];
      //      if(pt<0)continue;
      float isTight = vfloats_values[makeName(ele_label,pref,"isTight")][el];
      float isLoose = vfloats_values[makeName(ele_label,pref,"isLoose")][el];
      float isMedium = vfloats_values[makeName(ele_label,pref,"isMedium")][el];
      float isVeto = vfloats_values[makeName(ele_label,pref,"isVeto")][el];
      float eta = vfloats_values[makeName(ele_label,pref,"Eta")][el];
      float scEta = vfloats_values[makeName(ele_label,pref,"scEta")][el];
      float phi = vfloats_values[makeName(ele_label,pref,"Phi")][el];
      float energy = vfloats_values[makeName(ele_label,pref,"E")][el];      
      float iso = vfloats_values[makeName(ele_label,pref,"Iso03")][el];
      //      std::cout << " electron #"<<el<< " pt " << pt << " isTight/Mid/Loose/Veto?"<< isTight<<isMedium<<isLoose<<isVeto<<std::endl;

      bool passesDRmu = true;
      bool passesTightCuts = false;
      if(fabs(scEta)<=1.479){
	passesTightCuts = isTight >0.0 && iso < 0.074355 ;
      } //is barrel electron
      if (fabs(scEta)>1.479 && fabs(scEta)<2.5){
	passesTightCuts = isTight >0.0 && iso < 0.090185 ;
      }

      if(pt> 30 && abs(eta) < 2.5 && passesTightCuts){
	TLorentzVector ele;
	ele.SetPtEtaPhiE(pt, eta, phi, energy);	
	double minDR=999;
	double deltaR = 999;
	for (size_t m = 0; m < (size_t)loosemuons.size(); ++m){
	  deltaR = ele.DeltaR(loosemuons[m]);
	  minDR = min(minDR, deltaR);
	}
	if(!loosemuons.size()) minDR=999;
	if(minDR>0.1){ 
	  electrons.push_back(ele); 
	  leptons.push_back(ele);
	  ++float_values["Event_nTightElectrons"];

	}
	else {passesDRmu = false;}
      }

      if(isLoose>0 && pt> 30 && abs(eta) < 2.5){
	++float_values["Event_nLooseElectrons"];
      }
      if(isMedium>0 && pt> 30 && abs(eta) < 2.5){
	++float_values["Event_nMediumElectrons"]; 
      }
      if(isVeto>0 && pt> 20 && abs(eta) < 2.5){
	++float_values["Event_nVetoElectrons"]; 
      }

      vfloats_values[ele_label+"_PassesDRmu"][el]=(float)passesDRmu;

    } 

    //    cout << "syst "<<syst<<endl;
    /**************************
    Jets:
    **************************/
    double corrMetPx =0;
    double corrMetPy =0;
    double DUnclusteredMETPx=0.0;
    double DUnclusteredMETPy=0.0;

    for(int j = 0;j < max_instances[jets_label] ;++j){
      DUnclusteredMETPx=0.0;
      DUnclusteredMETPy=0.0;

      string pref = obj_to_pref[jets_label];
      float pt = vfloats_values[makeName(jets_label,pref,"Pt")][j];
      //      if(pt<0)continue;
      float genpt = vfloats_values[makeName(jets_label,pref,"GenJetPt")][j];
      float eta = vfloats_values[makeName(jets_label,pref,"Eta")][j];
      float phi = vfloats_values[makeName(jets_label,pref,"Phi")][j];
      float energy = vfloats_values[makeName(jets_label,pref,"E")][j];
      float ptCorr = -9999;
      float energyCorr = -9999;
      float smearfact = -9999;
      //      cout << "j is " <<j<< "label "<< jets_label << " maxinstances "<< max_instances[jets_label]<< "size "<< sizes[jets_label]<< " pt "<< vfloats_values[makeName(jets_label,pref,"Pt")][j]<< " eta "<< eta<< " phi "<< phi << " e "<< energy <<endl;
      float jecscale = vfloats_values[makeName(jets_label,pref,"jecFactor0")][j];
      float area = vfloats_values[makeName(jets_label,pref,"jetArea")][j];

      // if(pt>0){
      // 	ptCorr = smearPt(pt, genpt, eta, syst);
	
      // 	float unc = jetUncertainty(ptCorr,eta,syst);
      // 	//cout << "genpt? "<< genpt <<" pt ? "<< pt<<" ptcorr? "<< ptCorr<<"unc? "<< unc<<endl;
      // 	ptCorr = ptCorr * (1 + unc);
      // 	energyCorr = energy * (1 + unc);
      // }
      if(pt>0){
	TLorentzVector jetUncorr;
	
	jetUncorr.SetPtEtaPhiE(pt,eta,phi,energy);
	
	//	  cout << "jet "<< j <<" standard pt "<<pt<< " eta "<< eta << " zero correction "<< jecscale<<endl;;
	jetUncorr= jetUncorr*jecscale;


	DUnclusteredMETPx=0.1*jetUncorr.Pt()*cos(phi);
	DUnclusteredMETPy=0.1*jetUncorr.Pt()*sin(phi);
	if(changeJECs){
	  jecCorr->setJetEta(jetUncorr.Eta());
	  jecCorr->setJetPt(jetUncorr.Pt());
	  jecCorr->setJetA(area);
	  jecCorr->setRho(Rho);
	  jecCorr->setNPV(nPV);
	  double recorr =  jecCorr->getCorrection();
	  jetUncorr = jetUncorr *recorr;
	  //	  cout << " recorrection "<<recorr << " corrected Pt "<< jetUncorr.Pt()<< " eta "<< jetUncorr.Eta()<<endl;
	  pt = jetUncorr.Pt();
	  eta = jetUncorr.Eta();
	  energy = jetUncorr.Energy();
	  phi = jetUncorr.Phi();
	}
	
	
	smearfact = smear(pt, genpt, eta, syst);
	ptCorr = pt * smearfact;
	energyCorr = energy * smearfact;
	float unc = jetUncertainty(ptCorr,eta,syst);
	//	cout << "genpt? "<< genpt <<" pt ? "<< pt<<" ptcorr? "<< ptCorr<<"unc? "<< unc<<endl;
	if(unc != 0 && !useMETNoHF){ // for noHFMet we use the uncertainty from the NoHFJet set.
	  corrMetPx -=unc*(cos(phi)*ptCorr);
	  corrMetPy -=unc*(sin(phi)*ptCorr);
	}
	if(syst.find("unclusteredMet")!= std::string::npos && !useMETNoHF){
	  double signmet = 1.0; 
	  if(syst.find("down")!=std::string::npos) signmet=-1.0;
	  corrMetPx -=signmet*DUnclusteredMETPx;
	  corrMetPy -=signmet*DUnclusteredMETPy;
	}
	
      	if (syst.find("jer__")!=std::string::npos && (fabs(eta)<3.0 || (!useMETNoHF))){//For JER propagation to met we use standard jets, as reclusteded ones don't have genparticle information
	  corrMetPx -=pt * (smearfact-1) * cos(phi);
	  corrMetPy -=pt * (smearfact-1) * sin(phi);
	  
	}
	ptCorr = ptCorr * (1 + unc);
	energyCorr = energyCorr * (1 + unc);
      }
      //      ptCorr = ptCorr;
      //      energyCorr = energyCorr;
      float csv = vfloats_values[makeName(jets_label,pref,"CSV")][j];
      float partonFlavour = vfloats_values[makeName(jets_label,pref,"PartonFlavour")][j];
      int flavor = int(partonFlavour);
      //      vfloats_values[jets_label+"_CorrPt"][j]=ptCorr;
      ///      vfloats_values[jets_label+"_CorrE"][j]=energyCorr;
      vfloats_values[jets_label+"_CorrPt"][j]=ptCorr;
      vfloats_values[jets_label+"_CorrE"][j]=energyCorr;
      vfloats_values[jets_label+"_CorrEta"][j]=eta;
      vfloats_values[jets_label+"_CorrPhi"][j]=phi;
      // https://twiki.cern.ch/twiki/bin/viewauth/CMS/BTagPerformanceOP
      bool isCSVT = csv  > 0.970;
      bool isCSVM = csv  > 0.890;
      //      bool isCSVM = csv  > 0.814;
      bool isCSVL = csv  > 0.605;
      vfloats_values[jets_label+"_IsCSVT"][j]=isCSVT;
      vfloats_values[jets_label+"_IsCSVM"][j]=isCSVM;
      vfloats_values[jets_label+"_IsCSVL"][j]=isCSVL;
      
      float bsf = getScaleFactor(ptCorr,eta,partonFlavour,"noSyst");
      float bsfup = getScaleFactor(ptCorr,eta,partonFlavour,"up");
      float bsfdown = getScaleFactor(ptCorr,eta,partonFlavour,"down");
      
      vfloats_values[jets_label+"_BSF"][j]=bsf;
      vfloats_values[jets_label+"_BSFUp"][j]=bsfup;
      vfloats_values[jets_label+"_BSFDown"][j]=bsfdown;

      //***** JET ID ****      
      bool passesID = true;
      
      if(!(jecscale*energy > 0))passesID = false;
      else{
        double enZero=energy*jecscale;
	
	float nDau = vfloats_values[makeName(jets_label,pref,"numberOfDaughters")][j];
	float mu_frac = vfloats_values[makeName(jets_label,pref,"MuonEnergy")][j]/enZero;
	float chMulti = vfloats_values[makeName(jets_label,pref,"chargedMultiplicity")][j];
	float chHadEnFrac = vfloats_values[makeName(jets_label,pref,"chargedHadronEnergy")][j]/enZero;
	float chEmEnFrac = vfloats_values[makeName(jets_label,pref,"chargedEmEnergy")][j]/enZero;
	float neuEmEnFrac = vfloats_values[makeName(jets_label,pref,"neutralEmEnergy")][j]/enZero;
	float neuHadEnFrac = vfloats_values[makeName(jets_label,pref,"neutralHadronEnergy")][j]/enZero;
	float neuMulti = vfloats_values[makeName(jets_label,pref,"neutralMultiplicity")][j];

	
	// recommended at 8 TeV
	//passesID =  (nDau >1.0 && fabs(eta) < 4.7 && (fabs(eta)>=2.4 ||(chHadEnFrac>0.0 && chMulti>0 && chEmEnFrac<0.99) ) && neuEmEnFrac<0.99 && neuHadEnFrac <0.99 && mu_frac < 0.8);

	// temp fix for HF issues
	//passesID =  (nDau >1.0 && fabs(eta) < 4.7 && (fabs(eta)>=2.4 ||(chHadEnFrac>0.0 && chMulti>0 && chEmEnFrac<0.99 && neuEmEnFrac<0.99 && neuHadEnFrac <0.99) ) && mu_frac < 0.8);

	// new recommendations from 14/08
	if (fabs(eta)<=3.0) passesID =  (nDau >1.0 && fabs(eta) < 4.7 && (fabs(eta)>=2.4 ||(chHadEnFrac>0.0 && chMulti>0 && chEmEnFrac<0.99) ) && neuEmEnFrac<0.99 && neuHadEnFrac <0.99);
	else passesID = (fabs(eta) < 4.7 && neuMulti > 10 && neuEmEnFrac < 0.9 );

	vfloats_values[jets_label+"_JetID_numberOfDaughters"][j]=nDau;
	vfloats_values[jets_label+"_JetID_muonEnergyFraction"][j]=mu_frac;
	vfloats_values[jets_label+"_JetID_chargedMultiplicity"][j]=chMulti;
	vfloats_values[jets_label+"_JetID_chargedHadronEnergyFraction"][j]=chHadEnFrac;
	vfloats_values[jets_label+"_JetID_chargedEmEnergyFraction"][j]=chEmEnFrac;
	vfloats_values[jets_label+"_JetID_neutralEmEnergyFraction"][j]=neuEmEnFrac;
	vfloats_values[jets_label+"_JetID_neutralHadronEnergyFraction"][j]=neuHadEnFrac;
	vfloats_values[jets_label+"_JetID_neutralMultiplicity"][j]=neuMulti;

	/*
	cout << "### JET ID ###" << endl;
	cout << "ndau " << nDau << endl;
	cout << "mu_frac " << mu_frac << endl;
	cout << "chMulti " << chMulti << endl;
	cout << "chHadEnFrac " << chHadEnFrac << endl;
	cout << "chEmEnFrac " << chEmEnFrac << endl;
	cout << "neuEmEnFrac " << neuEmEnFrac << endl;
	cout << "neuHadEnFrac " << neuHadEnFrac << endl;
	cout << "passes loose id " << int(passesID) << endl;
	*/	
      }

      vfloats_values[jets_label+"_PassesID"][j]=(float)passesID;
      
      
      //remove overlap with tight electrons/muons
      double minDR=9999;
      double minDRtight=9999;
      double minDRThrEl=0.3;
      double minDRThrMu=0.3;
      bool passesDR=true;
      bool passesDRtight=true;

      for (size_t e = 0; e < (size_t)electrons.size(); ++e){
	minDR = min(minDR,deltaR(math::PtEtaPhiELorentzVector(electrons.at(e).Pt(),electrons.at(e).Eta(),electrons.at(e).Phi(),electrons.at(e).Energy() ) ,math::PtEtaPhiELorentzVector(ptCorr, eta, phi, energyCorr)));
	if(minDR<minDRThrEl)passesDR = false;
      }
      for (size_t m = 0; m < (size_t)muons.size(); ++m){
	minDR = min(minDR,deltaR(math::PtEtaPhiELorentzVector(muons.at(m).Pt(),muons.at(m).Eta(),muons.at(m).Phi(),muons.at(m).Energy() ) ,math::PtEtaPhiELorentzVector(ptCorr, eta, phi, energyCorr)));
	//minDR = min(minDR,deltaR(muons.at(m) ,math::PtEtaPhiELorentzVector(ptCorr, eta, phi, energyCorr)));
	if(minDR<minDRThrMu)passesDR = false;
      }
      
      for (size_t m = 0; m < (size_t)muons_t.size(); ++m){
	minDRtight = min(minDRtight,deltaR(math::PtEtaPhiELorentzVector(muons_t.at(m).Pt(),muons_t.at(m).Eta(),muons_t.at(m).Phi(),muons_t.at(m).Energy() ) ,math::PtEtaPhiELorentzVector(ptCorr, eta, phi, energyCorr)));
	if(minDRtight<minDRThrMu)passesDRtight = false;
      }

      vfloats_values[jets_label+"_MinDR"][j]=minDR;
      vfloats_values[jets_label+"_MinDRtight"][j]=minDRtight;      
      vfloats_values[jets_label+"_PassesDR"][j]=(float)passesDR;
      vfloats_values[jets_label+"_PassesDRtight"][j]=(float)passesDRtight;

      //vfloats_values[jets_label+"_IsTight"][j]=0.0;
      //vfloats_values[jets_label+"_IsLoose"][j]=0.0;
	
      if(passesID) vfloats_values[jets_label+"_IsLoose"][j]=1.0;
      if(passesDR) vfloats_values[jets_label+"_PassesDR"][j]=1.0;
      if(passesDRtight) vfloats_values[jets_label+"_PassesDRtight"][j]=1.0;

      if( passesID && passesDR) vfloats_values[jets_label+"_IsLoose"][j]=1.0;
      for (size_t ji = 0; ji < (size_t)jetScanCuts.size(); ++ji){
	stringstream j_n;
	double jetval = jetScanCuts.at(ji);
	j_n << "Cut" <<jetval;
	bool passesCut = ( ptCorr > jetval && fabs(eta) < 4.7);

	
	if(!passesID || !passesCut || !passesDR) continue;
	if(ji==0){
	  vfloats_values[jets_label+"_IsTight"][j]=1.0;
	  TLorentzVector jet;
	  jet.SetPtEtaPhiE(ptCorr, eta, phi, energyCorr);
	  jets.push_back(jet);

	  //double MCTagEFFiciency(int flavor); 
	  //  double TagScaleFactor(int flavor, string syst);    
   
	  //  jsfscsvt_b_tag_up;
	  //    jsfscsvt_b_tag_down, 
	  //    jsfscsvt_mistag_up, 
	  //    jsfscsvt_mistag_down;

	  //    partonFlavour = vfloats_values[makeName(jets_label,pref,"PartonFlavour")][j];    
	  
	}
	//      cout << " syst "<< syst<< " jet "<< j << " pt "<< ptCorr <<"cut "<< jetScanCuts.at(ji)<< " extra jet with pt "<< ptCorr<< "eventNJets before is" << float_values["Event_nJets"+j_n.str()]<< " csv "<< csv<< " isCSVM? "<< isCSVM<<endl;
	if(passesCut)	float_values["Event_nJets"+j_n.str()]+=1;
	//	cout<<  "after: "<< float_values["Event_nJets"+j_n.str()]<<endl;
	if(passesCut){
	  double csvteff = MCTagEfficiency("csvt",flavor, ptCorr);
	  double sfcsvt = TagScaleFactor("csvt", flavor, "noSyst", ptCorr);
	    
	  double csvleff = MCTagEfficiency("csvl",flavor,ptCorr);
	  double sfcsvl = TagScaleFactor("csvl", flavor, "noSyst", ptCorr);

	  double csvmeff = MCTagEfficiency("csvm",flavor,ptCorr);
	  double sfcsvm = TagScaleFactor("csvm", flavor, "noSyst", ptCorr);

	  double sfcsvt_mistag_up = TagScaleFactor("csvt", flavor, "mistag_up", ptCorr);
	  double sfcsvl_mistag_up = TagScaleFactor("csvl", flavor, "mistag_up", ptCorr);
	  double sfcsvm_mistag_up = TagScaleFactor("csvm", flavor, "mistag_up", ptCorr);

	  double sfcsvt_mistag_down = TagScaleFactor("csvt", flavor, "mistag_down", ptCorr);
	  double sfcsvl_mistag_down = TagScaleFactor("csvl", flavor, "mistag_down", ptCorr);
	  double sfcsvm_mistag_down = TagScaleFactor("csvm", flavor, "mistag_down", ptCorr);

	  double sfcsvt_b_tag_down = TagScaleFactor("csvt", flavor, "b_tag_down", ptCorr);
	  double sfcsvl_b_tag_down = TagScaleFactor("csvl", flavor, "b_tag_down", ptCorr);
	  double sfcsvm_b_tag_down = TagScaleFactor("csvm", flavor, "b_tag_down", ptCorr);
	    
	  double sfcsvt_b_tag_up = TagScaleFactor("csvt", flavor, "b_tag_up", ptCorr);
	  double sfcsvl_b_tag_up = TagScaleFactor("csvl", flavor, "b_tag_up", ptCorr);
	  double sfcsvm_b_tag_up = TagScaleFactor("csvm", flavor, "b_tag_up", ptCorr);

	  jsfscsvt.push_back(BTagWeight::JetInfo(csvteff, sfcsvt));
	  jsfscsvl.push_back(BTagWeight::JetInfo(csvleff, sfcsvl));
	  jsfscsvm.push_back(BTagWeight::JetInfo(csvmeff, sfcsvm));

	  jsfscsvt_mistag_up.push_back(BTagWeight::JetInfo(csvteff, sfcsvt_mistag_up));
	  jsfscsvl_mistag_up.push_back(BTagWeight::JetInfo(csvleff, sfcsvl_mistag_up));
	  jsfscsvm_mistag_up.push_back(BTagWeight::JetInfo(csvmeff, sfcsvm_mistag_up));

	  jsfscsvt_b_tag_up.push_back(BTagWeight::JetInfo(csvteff, sfcsvt_b_tag_up));
	  jsfscsvl_b_tag_up.push_back(BTagWeight::JetInfo(csvleff, sfcsvl_b_tag_up));
	  jsfscsvm_b_tag_up.push_back(BTagWeight::JetInfo(csvmeff, sfcsvm_b_tag_up));

	  jsfscsvt_mistag_down.push_back(BTagWeight::JetInfo(csvteff, sfcsvt_mistag_down));
	  jsfscsvl_mistag_down.push_back(BTagWeight::JetInfo(csvleff, sfcsvl_mistag_down));
	  jsfscsvm_mistag_down.push_back(BTagWeight::JetInfo(csvmeff, sfcsvm_mistag_down));

	  jsfscsvt_b_tag_down.push_back(BTagWeight::JetInfo(csvteff, sfcsvt_b_tag_down));
	  jsfscsvl_b_tag_down.push_back(BTagWeight::JetInfo(csvleff, sfcsvl_b_tag_down));
	  jsfscsvm_b_tag_down.push_back(BTagWeight::JetInfo(csvmeff, sfcsvm_b_tag_down));

	  //cout<< "jet :# "<< j << "sf is " <<sfcsvt << " eff is "<<csvteff<<endl;
	}


	//if(isCSVT && passesCut && fabs(eta) < 2.4) {
	if(isCSVT && passesID && passesDRtight && ptCorr > 40 && fabs(eta) < 2.4) {
	  float_values["Event_nCSVTJets"+j_n.str()]+=1.0;
	  ncsvt_tags +=1;
	}

	//if(isCSVL && passesCut && fabs(eta) < 2.4) { 
	if(isCSVL && passesID && passesDRtight && ptCorr > 40 && fabs(eta) < 2.4) {
	  ncsvl_tags +=1;
	}

	//if(isCSVM && passesCut && fabs(eta) < 2.4) { 
	if(isCSVM && passesID && passesDRtight && ptCorr > 40 && fabs(eta) < 2.4) {
	  float_values["Event_nCSVMJets"+j_n.str()]+=1.0;
	  if(ji==0){
	    ncsvm_tags +=1;
	    //    cout << " jet "<< j<< " isBJET "<<endl;
	    //TLorentzVector bjet;
	    //bjet.SetPtEtaPhiE(ptCorr, eta, phi, energyCorr);
	    //bjets.push_back(bjet);
	    //mapBJets[bjetidx]=j;
	    //++bjetidx;
	  }
	  //	  if(ji==0){
	    //	    cout << " jet "<< j<< " isBJET "<<endl;
	    //	    TLorentzVector bjet;
	    //  bjet.SetPtEtaPhiE(ptCorr, eta, phi, energyCorr);
	    //	    bjets.push_back(bjet);
	  //	  }
	}
	
	if(isCSVL && passesCut && abs(eta) < 2.4) float_values["Event_nCSVLJets"+j_n.str()]+=1;
      }
    } 
    
    if(useMETNoHF){
      double DUnclusteredMETPx=0.0;
      double DUnclusteredMETPy=0.0;
      for(int j = 0;j < max_instances[jetsnohf_label] ;++j){
	DUnclusteredMETPx=0.0;
	DUnclusteredMETPy=0.0;

	string pref = obj_to_pref[jetsnohf_label];
	float pt = vfloats_values[makeName(jetsnohf_label,pref,"Pt")][j];
	//      if(pt<0)continue;
	float eta = vfloats_values[makeName(jetsnohf_label,pref,"Eta")][j];
	float phi = vfloats_values[makeName(jetsnohf_label,pref,"Phi")][j];
	float energy = vfloats_values[makeName(jetsnohf_label,pref,"E")][j];
	float ptCorr = pt;
	//	float energyCorr = -9999;
	//	float smearfact = -9999;
	//	cout << "j is " <<j<< "label "<< jetsnohf_label << " maxinstances "<< max_instances[jetsnohf_label]<< "size "<< sizes[jetsnohf_label]<< " pt "<< vfloats_values[makeName(jetsnohf_label,pref,"Pt")][j]<< " eta "<< eta<< " phi "<< phi << " e "<< energy <<endl;
	float jecscale = vfloats_values[makeName(jetsnohf_label,pref,"jecFactor0")][j];
	float area = vfloats_values[makeName(jetsnohf_label,pref,"jetArea")][j];
	
	if(pt>0){
	  TLorentzVector jetUncorr;
	  
	  jetUncorr.SetPtEtaPhiE(pt,eta,phi,energy);
	  
	  //	  cout << "jet "<< j <<" standard pt "<<pt<< " eta "<< eta << " zero correction "<< jecscale<<endl;;
	  jetUncorr= jetUncorr*jecscale;
	  //	  cout << " uncorrected "<<jetUncorr.Pt()<<" ch jecs? "<< changeJECs<<endl;
	  //	  cout << " corrmetPx? "<<corrMetPx<<" Py? "<< corrMetPy<<endl;
	  //	  if(pt > 10){
	  DUnclusteredMETPx=0.1*jetUncorr.Pt()*cos(phi);
	  DUnclusteredMETPy=0.1*jetUncorr.Pt()*sin(phi);
	  ///	  }
	  //	  DUnclusteredMETPx=0.1*jetUncorr.Pt()*cos(phi);
	  //	  DUnclusteredMETPy=0.1*jetUncorr.Pt()*sin(phi);
	  if(changeJECs){
	    jecCorr->setJetEta(jetUncorr.Eta());
	    jecCorr->setJetPt(jetUncorr.Pt());
	    jecCorr->setJetA(area);
	    jecCorr->setRho(Rho);
	    jecCorr->setNPV(nPV);
	    
	    double recorr =  jecCorr->getCorrection();
	    jetUncorr = jetUncorr *recorr;
	    //	  cout << " recorrection "<<recorr << " corrected Pt "<< jetUncorr.Pt()<< " eta "<< jetUncorr.Eta()<<endl;
	    pt = jetUncorr.Pt();
	    eta = jetUncorr.Eta();
	    energy = jetUncorr.Energy();
	    phi = jetUncorr.Phi();
	  }
	  
	  float unc = jetUncertainty(pt,eta,syst);
	  if(syst.find("unclusteredMet")!= std::string::npos ){
	    double signmet = 1.0; 
	    if(syst.find("down")!=std::string::npos) signmet=-1.0;
	    corrMetPx -=signmet*DUnclusteredMETPx;
	    corrMetPy -=signmet*DUnclusteredMETPy;
	  }
	  if(unc != 0){
	    corrMetPx -=unc*(cos(phi)*ptCorr);
	    corrMetPy -=unc*(sin(phi)*ptCorr);
	  }
	}
      }
    }
    
    //    cout << " met "<<endl;
    //Met and mt
    string pref = obj_to_pref[met_label];
    float metpt = vfloats_values[makeName(met_label,pref,"Pt")][0];
    float metphi = vfloats_values[makeName(met_label,pref,"Phi")][0];
    
    
    float metPy = metpt*sin(metphi);
    float metPx = metpt*cos(metphi);
    metPx+=corrMetPx; metPy+=corrMetPy; // add JEC/JER contribution
    
    //Correcting the pt
    float metptCorr = sqrt(metPx*metPx + metPy*metPy);
    vfloats_values[met_label+"_CorrPt"][0]=metptCorr;

    //Correcting the phi
    float metphiCorr = metphi;
    if(metPx<0){
      if(metPy>0)metphiCorr = atan(metPy/metPx)+3.141592;
      if(metPy<0)metphiCorr = atan(metPy/metPx)-3.141592;
    }
    else  metphiCorr = (atan(metPy/metPx));

    vfloats_values[met_label+"_CorrPhi"][0]=metphiCorr;

    //BTagging part
    if(doBTagSF){
      //CSVT
      //0 tags
      b_weight_csvt_0_tags = b_csvt_0_tags.weight(jsfscsvt, ncsvt_tags);  
      b_weight_csvt_0_tags_mistag_up = b_csvt_0_tags.weight(jsfscsvt_mistag_up, ncsvt_tags);  
      b_weight_csvt_0_tags_mistag_down = b_csvt_0_tags.weight(jsfscsvt_mistag_down, ncsvt_tags);  
      b_weight_csvt_0_tags_b_tag_up = b_csvt_0_tags.weight(jsfscsvt_b_tag_up, ncsvt_tags);  
      b_weight_csvt_0_tags_b_tag_down = b_csvt_0_tags.weight(jsfscsvt_b_tag_down, ncsvt_tags);  
      
      //1 tag
      b_weight_csvt_1_tag = b_csvt_1_tag.weight(jsfscsvt, ncsvt_tags);  
      b_weight_csvt_1_tag_mistag_up = b_csvt_1_tag.weight(jsfscsvt_mistag_up, ncsvt_tags);  
      b_weight_csvt_1_tag_mistag_down = b_csvt_1_tag.weight(jsfscsvt_mistag_down, ncsvt_tags);  
      b_weight_csvt_1_tag_b_tag_up = b_csvt_1_tag.weight(jsfscsvt_b_tag_up, ncsvt_tags);  
      b_weight_csvt_1_tag_b_tag_down = b_csvt_1_tag.weight(jsfscsvt_b_tag_down, ncsvt_tags);  
      //cout <<"w1t check: is"<< b_weight_csvt_1_tag<<endl;
      
      //2 tags
      b_weight_csvt_2_tags = b_csvt_2_tags.weight(jsfscsvt, ncsvt_tags);  
      b_weight_csvt_2_tags_mistag_up = b_csvt_2_tags.weight(jsfscsvt_mistag_up, ncsvt_tags);  
      b_weight_csvt_2_tags_mistag_down = b_csvt_2_tags.weight(jsfscsvt_mistag_down, ncsvt_tags);  
      b_weight_csvt_2_tags_b_tag_up = b_csvt_2_tags.weight(jsfscsvt_b_tag_up, ncsvt_tags);  
      b_weight_csvt_2_tags_b_tag_down = b_csvt_2_tags.weight(jsfscsvt_b_tag_down, ncsvt_tags);  
      
      //      cout << " n tight tags "<< ncsvt_tags  << " w0tag "<< b_weight_csvt_0_tags<<" w1tag " << b_weight_csvt_1_tag <<" w2tags "<<b_weight_csvt_2_tags  <<endl;
      
      float_values["Event_bWeight0CSVL"]=b_weight_csvl_0_tags;
      float_values["Event_bWeight1CSVL"]=b_weight_csvl_1_tag;
      float_values["Event_bWeight2CSVL"]=b_weight_csvl_2_tags;
      
      float_values["Event_bWeight0CSVM"]=b_weight_csvm_0_tags;
      float_values["Event_bWeight1CSVM"]=b_weight_csvm_1_tag;
      float_values["Event_bWeight2CSVM"]=b_weight_csvm_2_tags;
      
      float_values["Event_bWeight0CSVT"]=b_weight_csvt_0_tags;
      float_values["Event_bWeight1CSVT"]=b_weight_csvt_1_tag;
      float_values["Event_bWeight2CSVT"]=b_weight_csvt_2_tags;
      
      float_values["Event_bWeight0CSVL"]=b_weight_csvl_0_tags;
      float_values["Event_bWeight1CSVL"]=b_weight_csvl_1_tag;
      float_values["Event_bWeight2CSVL"]=b_weight_csvl_2_tags;
      
      float_values["Event_bWeight0CSVM"]=b_weight_csvm_0_tags;
      float_values["Event_bWeight1CSVM"]=b_weight_csvm_1_tag;
      float_values["Event_bWeight2CSVM"]=b_weight_csvm_2_tags;
      
      float_values["Event_bWeight0CSVT"]=b_weight_csvt_0_tags;
      float_values["Event_bWeight1CSVT"]=b_weight_csvt_1_tag;
      float_values["Event_bWeight2CSVT"]=b_weight_csvt_2_tags;

      //Mistag
      float_values["Event_bWeightMisTagUp0CSVL"]=b_weight_csvl_0_tags_mistag_up;
      float_values["Event_bWeightMisTagUp1CSVL"]=b_weight_csvl_1_tag_mistag_up;
      float_values["Event_bWeightMisTagUp2CSVL"]=b_weight_csvl_2_tags_mistag_up;
      
      float_values["Event_bWeightMisTagUp0CSVM"]=b_weight_csvm_0_tags_mistag_up;
      float_values["Event_bWeightMisTagUp1CSVM"]=b_weight_csvm_1_tag_mistag_up;
      float_values["Event_bWeightMisTagUp2CSVM"]=b_weight_csvm_2_tags_mistag_up;
    
      float_values["Event_bWeightMisTagUp0CSVT"]=b_weight_csvt_0_tags_mistag_up;
      float_values["Event_bWeightMisTagUp1CSVT"]=b_weight_csvt_1_tag_mistag_up;
      float_values["Event_bWeightMisTagUp2CSVT"]=b_weight_csvt_2_tags_mistag_up;
      
      
      float_values["Event_bWeightMisTagDown0CSVL"]=b_weight_csvl_0_tags_mistag_down;
      float_values["Event_bWeightMisTagDown1CSVL"]=b_weight_csvl_1_tag_mistag_down;
      float_values["Event_bWeightMisTagDown2CSVL"]=b_weight_csvl_2_tags_mistag_down;
      
      float_values["Event_bWeightMisTagDown0CSVM"]=b_weight_csvm_0_tags_mistag_down;
      float_values["Event_bWeightMisTagDown1CSVM"]=b_weight_csvm_1_tag_mistag_down;
      float_values["Event_bWeightMisTagDown2CSVM"]=b_weight_csvm_2_tags_mistag_down;
      
      float_values["Event_bWeightMisTagDown0CSVT"]=b_weight_csvt_0_tags_mistag_down;
      float_values["Event_bWeightMisTagDown1CSVT"]=b_weight_csvt_1_tag_mistag_down;
      float_values["Event_bWeightMisTagDown2CSVT"]=b_weight_csvt_2_tags_mistag_down;

      //Btag
      float_values["Event_bWeightBTagUp0CSVL"]=b_weight_csvl_0_tags_b_tag_up;
      float_values["Event_bWeightBTagUp1CSVL"]=b_weight_csvl_1_tag_b_tag_up;
      float_values["Event_bWeightBTagUp2CSVL"]=b_weight_csvl_2_tags_b_tag_up;
      
      float_values["Event_bWeightBTagUp0CSVM"]=b_weight_csvm_0_tags_b_tag_up;
      float_values["Event_bWeightBTagUp1CSVM"]=b_weight_csvm_1_tag_b_tag_up;
      float_values["Event_bWeightBTagUp2CSVM"]=b_weight_csvm_2_tags_b_tag_up;
      
      float_values["Event_bWeightBTagUp0CSVT"]=b_weight_csvt_0_tags_b_tag_up;
      float_values["Event_bWeightBTagUp1CSVT"]=b_weight_csvt_1_tag_b_tag_up;
      float_values["Event_bWeightBTagUp2CSVT"]=b_weight_csvt_2_tags_b_tag_up;
      
      float_values["Event_bWeightBTagDown0CSVL"]=b_weight_csvl_0_tags_b_tag_down;
      float_values["Event_bWeightBTagDown1CSVL"]=b_weight_csvl_1_tag_b_tag_down;
      float_values["Event_bWeightBTagDown2CSVL"]=b_weight_csvl_2_tags_b_tag_down;
      
      float_values["Event_bWeightBTagDown0CSVM"]=b_weight_csvm_0_tags_b_tag_down;
      float_values["Event_bWeightBTagDown1CSVM"]=b_weight_csvm_1_tag_b_tag_down;
      float_values["Event_bWeightBTagDown2CSVM"]=b_weight_csvm_2_tags_b_tag_down;
      
      float_values["Event_bWeightBTagDown0CSVT"]=b_weight_csvt_0_tags_b_tag_down;
      float_values["Event_bWeightBTagDown1CSVT"]=b_weight_csvt_1_tag_b_tag_down;
      float_values["Event_bWeightBTagDown2CSVT"]=b_weight_csvt_2_tags_b_tag_down;
    }



      
    if(useLHE){
      //LHE and luminosity weights:
      float LHE_weight = lhes->hepeup().XWGTUP;
      float LHEWeightSign = LHE_weight/fabs(LHE_weight);
      float_values["Event_LHEWeightSign"]=LHEWeightSign;
      float_values["Event_LHEWeight"]=LHE_weight;
    }
    float weightLumi = crossSection/originalEvents;
    float_values["Event_weight"]=weightLumi;
  
  
    //Part 3: filling the additional variables
    //Reset event weights/#objects
    if(useLHEWeights){
      getEventLHEWeights();
    }
    
    if(addLHAPDFWeights){
      getEventPdf();
    }
    
    if(addPV){
      float nGoodPV = 0.0;
      for (size_t v = 0; v < pvZ->size();++v){
	bool isGoodPV = (
			 fabs(pvZ->at(v)) < 24.0 &&
			 pvNdof->at(v) > 4.0 &&
			 pvRho->at(v) <2.0
			 );
	if (isGoodPV)nGoodPV+=1.0;
      }	
      float_values["Event_nGoodPV"]=(float)(nGoodPV);
      float_values["Event_nPV"]=(float)(nPV);
    }

    //    if(useMETFilters){
    //      getMETFilters();
    //    }
    
    float_values["Event_passesHBHE"]=(float)(*HBHE);

    //technical event information
    float_values["Event_EventNumber"]=*eventNumber;
    float_values["Event_LumiBlock"]=*lumiBlock;
    float_values["Event_RunNumber"]=*runNumber;

    //    std::cout << " event ntight muons " << float_values["Event_nTightMuons"]<< std::endl;
    //    std::cout << " event ntight electrons " << float_values["Event_nTightElectrons"]<< std::endl;
    //    cout << "before filling jets label "<< jets_label << " maxinstances "<< max_instances[jets_label]<< "size "<< sizes[jets_label]<<endl;  
    trees[syst]->Fill();
    //Reset event weights/#objects
    string nameshortv= "Event";
    vector<string> extravars = additionalVariables(nameshortv);
    for(size_t addv = 0; addv < extravars.size();++addv){
      string name = nameshortv+"_"+extravars.at(addv);
      //      std::cout << " resetting variable "<< name<< " before "<< float_values[name];
      //      bool isTriggerVar
      //      if(isTriggerVar) continue;
      float_values[name]=0;
      //      std::cout << " after "<< Float_values[name]<<endl;
    }
  }

  /*
  for(int t = 0;t < max_instances[boosted_tops_label] ;++t){
    vfloats_values[boosted_tops_label+"_nCSVM"][t]=0;
    vfloats_values[boosted_tops_label+"_nJ"][t]=0;
  }
  */

  //treesBase->Fill(); 
}

string DMAnalysisTreeMaker::makeBranchName(string label, string pref, string var){
  string outVar = var;
  size_t prefPos=outVar.find(pref);
  size_t prefLength = pref.length();
  outVar.replace(prefPos,prefLength,label+"_");
  return outVar;
}

string DMAnalysisTreeMaker::makeName(string label,string pref,string var){
  return label+"_"+var;
  //  string outVar = var;
  //  size_t prefPos=outVar.find(pref);
  //  size_t prefLength = pref.length();
  //  std::cout << " outvar is "<< outVar<< " prefpos "<< prefPos<< " length "<< prefLength<< endl;
  //  std::cout << "it is " << label+"_"+var<<endl;
  //  outVar.replace(prefPos,prefLength,label+"_");

  //  outVar.replace(prefPos,prefLength,label+"_");
  //  return outVar;
  //  return makeBranchName(label,pref,var);
  //  return pref+var+"_"+label;
}

vector<string> DMAnalysisTreeMaker::additionalVariables(string object){
  vector<string> addvar;
  bool ismuon=object.find("muon")!=std::string::npos;
  bool iselectron=object.find("electron")!=std::string::npos;
  bool ismet=object.find("met")!=std::string::npos;
  bool isjet=object.find("jet")!=std::string::npos && object.find("AK4")!=std::string::npos;
  bool isak8=object.find("jet")!=std::string::npos && object.find("AK8")!=std::string::npos && object.find("sub")==std::string::npos;
  bool isak8subjet=object.find("jet")!=std::string::npos && object.find("AK8")!=std::string::npos && object.find("sub")!=std::string::npos;
  bool isevent=object.find("Event")!=std::string::npos;
  //bool isResolvedTopHad=object.find("resolvedTopHad")!=std::string::npos;
  //bool isResolvedTopSemiLep=object.find("resolvedTopSemiLep")!=std::string::npos;
  
  if(ismuon || iselectron){
    addvar.push_back("SFTrigger");
    addvar.push_back("SFReco");
    addvar.push_back("isQCD");
    //    addvar.push_back("isTightOffline");
    //    addvar.push_back("isLooseOffline");
  }
  if(iselectron){
    addvar.push_back("PassesDRmu");
  }
  if(ismet){
    //    addvar.push_back("Pt");
    //    addvar.push_back("Eta");
    //    addvar.push_back("Phi");
    //    addvar.push_back("E");
    addvar.push_back("CorrPt");
    addvar.push_back("CorrPhi");
  }
  if(isjet){
    addvar.push_back("CorrPt");
    //    addvar.push_back("CorrEta");
    //    addvar.push_back("CorrPhi");
    addvar.push_back("CorrE");
    addvar.push_back("MinDR");
    addvar.push_back("IsCSVT");
    addvar.push_back("IsCSVM");
    addvar.push_back("IsCSVL");
    //    addvar.push_back("BSF");
    //    addvar.push_back("BSFUp");
    //    addvar.push_back("BSFDown");
    addvar.push_back("PassesID");
    addvar.push_back("PassesDR");
    addvar.push_back("PassesDRtight");
    addvar.push_back("CorrMass");
    addvar.push_back("IsTight");
    addvar.push_back("IsLoose");
    //    addvar.push_back("CorrNJets");
    //    addvar.push_back("CorrPartonFlavour");

    addvar.push_back("JetID_numberOfDaughters");
    addvar.push_back("JetID_muonEnergyFraction");
    addvar.push_back("JetID_chargedMultiplicity");
    addvar.push_back("JetID_chargedHadronEnergyFraction");
    addvar.push_back("JetID_chargedEmEnergyFraction");
    addvar.push_back("JetID_neutralEmEnergyFraction");
    addvar.push_back("JetID_neutralHadronEnergyFraction");
    addvar.push_back("JetID_neutralMultiplicity");

  }
  if(isak8){
    addvar.push_back("CorrPt");
    addvar.push_back("CorrE");
    addvar.push_back("isType1");
    addvar.push_back("isType2");
    addvar.push_back("TopPt");
    addvar.push_back("TopEta");
    addvar.push_back("TopPhi");
    addvar.push_back("TopE");
    addvar.push_back("TopMass");
    addvar.push_back("TopWMass");
    addvar.push_back("nJ");
    addvar.push_back("nCSVM");
  }  
  if(isak8subjet){
    ;//    addvar.push_back("CorrPt");
  }
  /*
  if(isResolvedTopHad ){
    addvar.push_back("Pt");    addvar.push_back("Eta");    addvar.push_back("Phi");    addvar.push_back("E"); addvar.push_back("Mass"); 
    addvar.push_back("WMass"); addvar.push_back("BMPhi");  addvar.push_back("WMPhi");  addvar.push_back("TMPhi");  addvar.push_back("WBPhi");
    addvar.push_back("IndexB");    addvar.push_back("IndexJ1");    addvar.push_back("IndexJ2");
  }

  if(isResolvedTopSemiLep ){
    addvar.push_back("Pt");    addvar.push_back("Eta");    addvar.push_back("Phi");    addvar.push_back("E"); addvar.push_back("Mass");   
    addvar.push_back("MT");    addvar.push_back("LBMPhi");    addvar.push_back("LMPhi");    addvar.push_back("LBPhi");     addvar.push_back("BMPhi");  addvar.push_back("TMPhi"); 
    addvar.push_back("IndexL");    addvar.push_back("LeptonFlavour");    addvar.push_back("IndexB");
  }
  */
  if(isevent){
    addvar.push_back("weight");
    addvar.push_back("nTightMuons");
    addvar.push_back("nSoftMuons");
    addvar.push_back("nLooseMuons");
    addvar.push_back("nTightElectrons");
    addvar.push_back("nMediumElectrons");
    addvar.push_back("nLooseElectrons");
    addvar.push_back("nVetoElectrons");
    addvar.push_back("nElectronsSF");
    addvar.push_back("mt");
    addvar.push_back("Mt2w");
    addvar.push_back("category");
    addvar.push_back("nMuonsSF");
    addvar.push_back("nCSVTJets");
    addvar.push_back("nCSVMJets");
    addvar.push_back("nCSVLJets");
    addvar.push_back("nTightJets");
    addvar.push_back("nLooseJets");
    addvar.push_back("nType1TopJets");
    addvar.push_back("nType2TopJets");
 
    addvar.push_back("bWeight0CSVT");
    addvar.push_back("bWeight1CSVT");
    addvar.push_back("bWeight2CSVT");

    addvar.push_back("bWeight0CSVM");
    addvar.push_back("bWeight1CSVM");
    addvar.push_back("bWeight2CSVM");

    addvar.push_back("bWeight0CSVL");
    addvar.push_back("bWeight1CSVL");
    addvar.push_back("bWeight2CSVL");

    addvar.push_back("bWeightMisTagDown0CSVT");
    addvar.push_back("bWeightMisTagDown1CSVT");
    addvar.push_back("bWeightMisTagDown2CSVT");

    addvar.push_back("bWeightMisTagDown0CSVM");
    addvar.push_back("bWeightMisTagDown1CSVM");
    addvar.push_back("bWeightMisTagDown2CSVM");

    addvar.push_back("bWeightMisTagDown0CSVL");
    addvar.push_back("bWeightMisTagDown1CSVL");
    addvar.push_back("bWeightMisTagDown2CSVL");

    addvar.push_back("bWeightMisTagUp0CSVT");
    addvar.push_back("bWeightMisTagUp1CSVT");
    addvar.push_back("bWeightMisTagUp2CSVT");

    addvar.push_back("bWeightMisTagUp0CSVM");
    addvar.push_back("bWeightMisTagUp1CSVM");
    addvar.push_back("bWeightMisTagUp2CSVM");

    addvar.push_back("bWeightMisTagUp0CSVL");
    addvar.push_back("bWeightMisTagUp1CSVL");
    addvar.push_back("bWeightMisTagUp2CSVL");

    addvar.push_back("bWeightBTagUp0CSVT");
    addvar.push_back("bWeightBTagUp1CSVT");
    addvar.push_back("bWeightBTagUp2CSVT");

    addvar.push_back("bWeightBTagUp0CSVM");
    addvar.push_back("bWeightBTagUp1CSVM");
    addvar.push_back("bWeightBTagUp2CSVM");

    addvar.push_back("bWeightBTagUp0CSVL");
    addvar.push_back("bWeightBTagUp1CSVL");
    addvar.push_back("bWeightBTagUp2CSVL");

    addvar.push_back("bWeightBTagDown0CSVT");
    addvar.push_back("bWeightBTagDown1CSVT");
    addvar.push_back("bWeightBTagDown2CSVT");

    addvar.push_back("bWeightBTagDown0CSVM");
    addvar.push_back("bWeightBTagDown1CSVM");
    addvar.push_back("bWeightBTagDown2CSVM");

    addvar.push_back("bWeightBTagDown0CSVL");
    addvar.push_back("bWeightBTagDown1CSVL");
    addvar.push_back("bWeightBTagDown2CSVL");

    addvar.push_back("LHEWeightSign");
    addvar.push_back("LHEWeight");
    addvar.push_back("EventNumber");
    addvar.push_back("LumiBlock");
    addvar.push_back("RunNumber");
    
    for (size_t j = 0; j < (size_t)jetScanCuts.size(); ++j){
      stringstream j_n;
      double jetval = jetScanCuts.at(j);
      j_n << "Cut" <<jetval;
      
      addvar.push_back("nJets"+j_n.str());
      addvar.push_back("nCSVTJets"+j_n.str());
      addvar.push_back("nCSVMJets"+j_n.str());
      addvar.push_back("nCSVLJets"+j_n.str());
      addvar.push_back("bWeight1CSVTWeight"+j_n.str());
      addvar.push_back("bWeight2CSVTWeight"+j_n.str());
      addvar.push_back("bWeight1CSVMWeight"+j_n.str());
      addvar.push_back("bWeight2CSVMWeight"+j_n.str());
      addvar.push_back("bWeight1CSVLWeight"+j_n.str());
      addvar.push_back("bWeight2CSVLWeight"+j_n.str());
      addvar.push_back("bWeight0CSVLWeight"+j_n.str());
      addvar.push_back("bWeight0CSVLWeight"+j_n.str());
    }

    if(useLHEWeights){
      for (size_t w = 0; w <= (size_t)maxWeights; ++w)  {
	stringstream w_n;
	w_n << w;
	addvar.push_back("LHEWeight"+w_n.str());
	//	cout << " weight # " << w << " test "<< "LHEWeight"+w_n.str()<< endl; 
	//addvar.push_back(("LHEWeight"+w_n.str())+"ID");
      }
    }
    if(addLHAPDFWeights){
      for (size_t p = 1; p <= (size_t)maxPdf; ++p)  {
	//cout << " pdf # " << pdf_weights[p - 1] << " test "<< endl; 
	stringstream w_n;
	w_n << p;
	addvar.push_back("PDFWeight" + w_n.str());
      }
    }
    if(useMETFilters){
      for (size_t lt = 0; lt < metFilters.size(); ++lt)  {
	string trig = metFilters.at(lt);
	addvar.push_back("passes"+trig);
      }
      addvar.push_back("passesMETFilters");
      addvar.push_back("passesHBHE");
    }
    if(useTriggers){
      for (size_t lt = 0; lt < leptonicTriggers.size(); ++lt)  {
	string trig = leptonicTriggers.at(lt);
	addvar.push_back("passes"+trig);
	addvar.push_back("prescale"+trig);
      }
      for (size_t ht = 0; ht < hadronicTriggers.size(); ++ht)  {
	string trig = hadronicTriggers.at(ht);
	addvar.push_back("passes"+trig);
	addvar.push_back("prescale"+trig);
      }
      addvar.push_back("passesLeptonicTriggers");
      addvar.push_back("passesHadronicTriggers");
    }
  }
      
  return addvar;
}

void DMAnalysisTreeMaker::initializePdf(string central, string varied){

    if(central == "CT") {  LHAPDF::initPDFSet(1, "cteq66.LHgrid"); }
    if(central == "CT10") {  LHAPDF::initPDFSet(1, "CT10.LHgrid"); }
    if(central == "CT10f4") {  LHAPDF::initPDFSet(1, "CT10f4.LHgrid"); }
    if(central == "NNPDF") { LHAPDF::initPDFSet(1, "NNPDF22_100.LHgrid");  }
    if(central == "MSTW") { LHAPDF::initPDFSet(1, "MSTW2008nlo68cl.LHgrid");  }

    if(varied == "CT") {  LHAPDF::initPDFSet(2, "cteq66.LHgrid"); maxPdf = 44; }
    if(varied == "CT10") {  LHAPDF::initPDFSet(2, "CT10.LHgrid"); maxPdf = 52; }
    if(varied == "CT10f4") {  LHAPDF::initPDFSet(2, "CT10f4.LHgrid"); maxPdf = 52; }
    if(varied == "NNPDF") { LHAPDF::initPDFSet(2, "NNPDF22_100.LHgrid");  maxPdf = 100; }
    if(varied == "MSTW") { LHAPDF::initPDFSet(2, "MSTW2008nlo68cl.LHgrid"); maxPdf = 40; }
}


bool DMAnalysisTreeMaker::getMETFilters(){
  bool METFilterAND=true;
  for(size_t mf =0; mf< metFilters.size();++mf){
    string fname = metFilters.at(mf);
    for(size_t bt = 0; bt < metNames->size();++bt){
      std::string tname = metNames->at(bt);
      //      cout << "test tname "<<endl;
      if(tname.find(fname)!=std::string::npos){
	METFilterAND = METFilterAND && (metBits->at(bt)>0);
	float_values["Event_passes"+fname]=metBits->at(bt);
      }
    }
  }
  float_values["Event_passesMETFilters"]=(float)METFilterAND;
  return METFilterAND;
}
bool DMAnalysisTreeMaker::getEventTriggers(){
  bool leptonOR=false,hadronOR=false;
  for(size_t lt =0; lt< leptonicTriggers.size();++lt){
    string lname = leptonicTriggers.at(lt);
    for(size_t bt = 0; bt < triggerNames->size();++bt){
      std::string tname = triggerNames->at(bt);
      if(tname.find(lname)!=std::string::npos){
	leptonOR = leptonOR || (triggerBits->at(bt)>0);
	float_values["Event_passes"+lname]=triggerBits->at(bt);
	float_values["Event_prescale"+lname]=triggerPrescales->at(bt);
      }
    }
  }
  for(size_t ht =0; ht< hadronicTriggers.size();++ht){
    string hname = hadronicTriggers.at(ht);
    for(size_t bt = 0; bt < triggerNames->size();++bt){
      std::string tname = triggerNames->at(bt);
      if(tname.find(hname)!=std::string::npos){
	hadronOR = hadronOR || (triggerBits->at(bt)>0);
	float_values["Event_passes"+hname]=triggerBits->at(bt);
	float_values["Event_prescale"+hname]=triggerPrescales->at(bt);
      }
    }
  }
  float_values["Event_passesLeptonicTriggers"]=(float)leptonOR;
  float_values["Event_passesHadronicTriggers"]=(float)hadronOR;
  return (leptonOR || hadronOR);
}



void DMAnalysisTreeMaker::getEventPdf(){

  if (genprod->pdf()->id.first == 21 || genprod->pdf()->id.second == 21) return;

  std::cout << " getting pdf "<<endl;

  double scalePDF = genprod->pdf()->scalePDF;
  double x1 =  genprod->pdf()->x.first;
  double x2 =  genprod->pdf()->x.second;
  int id1 =  genprod->pdf()->id.first;
  int id2 =  genprod->pdf()->id.second;

  std::cout << " maxpdf "<< maxPdf << " accessing x1 " << x1<< " id1 " << id1<<std::endl;
  std::cout << " maxpdf "<< maxPdf << " accessing x2 " << x2<< " id2 " << id2<<std::endl;
  cout << "scalePDF " << scalePDF << endl;

  LHAPDF::usePDFMember(1, 0);
  double xpdf1 = LHAPDF::xfx(1, x1, scalePDF, id1);
  double xpdf2 = LHAPDF::xfx(1, x2, scalePDF, id2);
  double w0 = xpdf1 * xpdf2;
  int maxPDFCount = maxPdf;

  cout << "xpdf1 " << xpdf1 << endl;
  cout << "xpdf2 " << xpdf2 << endl;

  cout << "weight " << w0 << endl;

  std::cout << "entering pdf loop" <<std::endl;
  for (int p = 1; p <= maxPdf; ++p)
    {
      
      if ( p > maxPDFCount ) continue;
      LHAPDF::usePDFMember(2, p);
      double xpdf1_new = LHAPDF::xfx(2, x1, scalePDF, id1);
      double xpdf2_new = LHAPDF::xfx(2, x2, scalePDF, id2);
      double pweight = xpdf1_new * xpdf2_new / w0;
      stringstream w_n;
      w_n << p;

      cout << "xpdf1 new " << xpdf1_new << endl;
      cout << "xpdf2 new " << xpdf2_new << endl;

      cout << "index " << p << " pweight " << pweight << endl;

      float_values["PDFWeight"+w_n.str()]= pweight;
    }
  
}


void DMAnalysisTreeMaker::getEventLHEWeights(){
  //  std::cout << " in weight "<<endl;
  size_t wgtsize=  lhes->weights().size();
  //  std::cout << "weight size "<< wgtsize<<endl;
  for (size_t i = 0; i <  wgtsize; ++i)  {
    if (i<= (size_t)maxWeights){ 
      stringstream w_n;
      w_n << i;

      float ww = (float)lhes->weights().at(i).wgt;
      
      //      cout << "ww # " << i<< "is "<<ww <<endl;
      //      cout <<" floatval before "<< float_values["Event_LHEWeight"+w_n.str()]<<endl;

      float_values["Event_LHEWeight"+w_n.str()]= ww;

      //      cout <<" floatval after "<< float_values["Event_LHEWeight"+w_n.str()]<<endl;

    }
    else cout << "WARNING! there are " << wgtsize << " weights, and you accomodated for only "<< maxWeights << " weights, check your configuration file/your lhe!!!"<<endl;
  }
  
}


// double DMAnalysisTreeMaker::smearPt(double ptCorr, double genpt, double eta, string syst){
//   double resolScale = resolSF(fabs(eta), syst);
//   double smear =1.0;
//   if(genpt>0) smear = std::max((double)(0.0), (double)(ptCorr + (ptCorr - genpt) * resolScale) / ptCorr);
//   return ptCorr * smear;
// }

double DMAnalysisTreeMaker::smear(double pt, double genpt, double eta, string syst){
  double resolScale = resolSF(fabs(eta), syst);
  double smear =1.0;
  if(genpt>0) smear = std::max((double)(0.0), (double)(pt + (pt - genpt) * resolScale) / pt);
  return  smear;
}


/*
// outdated 7 TeV scale factors
double DMAnalysisTreeMaker::resolSF(double eta, string syst)
{
  double fac = 0.;
  if (syst == "jer__up")fac = 1.;
  if (syst == "jer__down")fac = -1.;
  if (eta <= 0.5) return 0.052 + 0.063 * fac;
  else if ( eta > 0.5 && eta <= 1.1 ) return 0.057 + 0.057 * fac;
  else if ( eta > 1.1 && eta <= 1.7 ) return 0.096 + 0.065 * fac;
  else if ( eta > 1.7 && eta <= 2.3 ) return 0.134 + 0.093 * fac;
  else if ( eta > 2.3 && eta <= 5. ) return 0.288 + 0.200 * fac;
  return 0.1;
}
*/

 // 8 TeV scale factors
double DMAnalysisTreeMaker::resolSF(double eta, string syst)
{
  double fac = 0.;
  if (syst == "jer__up")fac = 1.;
  if (syst == "jer__down")fac = -1.;
  if (eta <= 0.5)                       return 0.079 + (0.026 * fac);
  else if ( eta > 0.5 && eta <= 1.1 )   return 0.099 + (0.028 * fac);
  else if ( eta > 1.1 && eta <= 1.7 )   return 0.121 + (0.029 * fac);
  else if ( eta > 1.7 && eta <= 2.3 )   return 0.208 + (0.046 * fac);
  else if ( eta > 2.3 && eta <= 2.8 )   return 0.254 + (0.062 * fac);
  else if ( eta > 2.8 && eta <= 3.2 )   return 0.395 + (0.063 * fac);
  else if ( eta > 3.2 && eta <= 5.0 )   return 0.056 + (0.191 * fac);
  return 0.1;
}

double DMAnalysisTreeMaker::jetUncertainty(double ptCorr, double eta, string syst)
{
  if(ptCorr<0)return ptCorr;
  if(syst == "jes__up" || syst == "jes__down"){
    double fac = 1.;
    if (syst == "jes__down")fac = -1.;
    jecUnc->setJetEta(eta);
    jecUnc->setJetPt(ptCorr);
    double JetCorrection = jecUnc->getUncertainty(true);
    return JetCorrection*fac;
  }
  return 0.0;
}

double DMAnalysisTreeMaker::getScaleFactor(double ptCorr,double etaCorr,double partonFlavour, string syst){
  return 1.0;
}

bool DMAnalysisTreeMaker::isInVector(std::vector<std::string> v, std::string s){
  for(size_t i = 0;i<v.size();++i){
    //    std::cout << " label is " << s << " vector i-th element  "<< v.at(i)<<" are they equal? "<< (v.at(i)==s) << " is v in s? "<< (s.find(v.at(i))!=std::string::npos)<<endl;
    if(v.at(i)==s)return true;
    //    if(s.find(v.at(i))!=std::string::npos)return true;
  }
  return false;
}

//BTag weighter
bool DMAnalysisTreeMaker::BTagWeight::filter(int t)
{
  return (t >= minTags && t <= maxTags);
}

float DMAnalysisTreeMaker::BTagWeight::weight(vector<JetInfo> jetTags, int tags)
{
  if (!filter(tags))
    {
      //   std::cout << "nThis event should not pass the selection, what is it doing here?" << std::endl;
      return 0;
    }
  int njetTags = jetTags.size();
  //cout<< " njettags "<< njetTags<<endl;
  int comb = 1 << njetTags;
  float pMC = 0;
  float pData = 0;
  for (int i = 0; i < comb; i++)
    {
      float mc = 1.;
      float data = 1.;
      int ntagged = 0;
      for (int j = 0; j < njetTags; j++)
        {
	  bool tagged = ((i >> j) & 0x1) == 1;
	  if (tagged)
            {
	      ntagged++;
	      mc *= jetTags[j].eff;
	      data *= jetTags[j].eff * jetTags[j].sf;
            }
	  else
            {
	      mc *= (1. - jetTags[j].eff);
	      data *= (1. - jetTags[j].eff * jetTags[j].sf);
            }
        }

      if (filter(ntagged))
        {
	  //std::cout << mc << " " << data << endl;
	  pMC += mc;
	  pData += data;
        }
    }

  if (pMC == 0) return 0;
  return pData / pMC;
}

double DMAnalysisTreeMaker::MCTagEfficiency(string algo, int flavor, double pt){
  if (abs(flavor) ==5){
    if(algo=="csvt") return 0.38;
    if(algo=="csvm") return 0.58;
    if(algo=="csvl") return 0.755;
  }

  if (abs(flavor) ==4){
    if(algo=="csvt") return 0.015;
    if(algo=="csvm") return 0.08;
    if(algo=="csvl") return 0.28;
  }

  if (abs(flavor) !=4 && abs(flavor) !=5){
    if(algo=="csvt") return 0.0008;
    if(algo=="csvm") return 0.007;
    if(algo=="csvl") return 0.079;
  }
  return 1.0;
}

double DMAnalysisTreeMaker::TagScaleFactor(string algo, int flavor, string syst, double pt){
  // source (08/24):
  // https://indico.cern.ch/event/439699/contribution/2/attachments/1143400/1638534/BTag_150727_FirstResults13TeVData.pdf
  // http://scodella.web.cern.ch/scodella/Work/CMS/BTagging/PerformanceCombination/Codes/CSVv2_Aug24.csv
  if(algo == "csvt"){
    if(syst ==  "noSyst") {
      if(abs(flavor)==5){
	if (pt >= 30  && pt < 670) return 0.921826;
      }
      if(abs(flavor)==4){
	if (pt >= 30  && pt < 670) return 0.921826;
      }
      if(abs(flavor)!=5 && abs(flavor)!=4){
	return 1.00;
      }
    }
    if(syst ==  "mistag_up") {
      if(abs(flavor)==5){
	return 1.00;
      }
      if(abs(flavor)==4){
	return 1.00;
      }
      if(abs(flavor)!=5 && abs(flavor)!=4){
	return 1.40;
      }
    }
    if(syst ==  "mistag_down") {
      if(abs(flavor)==5){
	return 1.00;
      }
      if(abs(flavor)==4){
	return 1.00;
      }
      if(abs(flavor)!=5 && abs(flavor)!=4){
	return 0.6;
      }
    }

    if(syst ==  "b_tag_up") {
      if(abs(flavor)==5){
	if (pt >= 30  && pt < 50 ) return 0.921826+0.038185462355613708;
	if (pt >= 50  && pt < 70 ) return 0.921826+0.048359211534261703;
	if (pt >= 70  && pt < 100) return 0.921826+0.036345392465591431;
	if (pt >= 100 && pt < 140) return 0.921826+0.090967245399951935;
	if (pt >= 140 && pt < 200) return 0.921826+0.087966755032539368;
	if (pt >= 200 && pt < 300) return 0.921826+0.26064527034759521;
	if (pt >= 300 && pt < 670) return 0.921826+0.20989412069320679;
      }
      if(abs(flavor)==4){
	if (pt >= 30  && pt < 50 ) return 0.921826+0.076370924711227417;
	if (pt >= 50  && pt < 70 ) return 0.921826+0.096718423068523407;
	if (pt >= 70  && pt < 100) return 0.921826+0.072690784931182861;
	if (pt >= 100 && pt < 140) return 0.921826+0.18193449079990387;
	if (pt >= 140 && pt < 200) return 0.921826+0.17593351006507874;
	if (pt >= 200 && pt < 300) return 0.921826+0.52129054069519043;
	if (pt >= 300 && pt < 670) return 0.921826+0.41978824138641357;
      }
      if(abs(flavor)!=5 && abs(flavor)!=4){
	return 1.0;
      }
    }

    if(syst ==  "b_tag_down") {
      if(abs(flavor)==5){
	if (pt >= 30  && pt < 50 ) return 0.921826-0.038185462355613708;
	if (pt >= 50  && pt < 70 ) return 0.921826-0.048359211534261703;
	if (pt >= 70  && pt < 100) return 0.921826-0.036345392465591431;
	if (pt >= 100 && pt < 140) return 0.921826-0.090967245399951935;
	if (pt >= 140 && pt < 200) return 0.921826-0.087966755032539368;
	if (pt >= 200 && pt < 300) return 0.921826-0.26064527034759521;
	if (pt >= 300 && pt < 670) return 0.921826-0.20989412069320679;
      }
      if(abs(flavor)==4){
	if (pt >= 30  && pt < 50 ) return 0.921826-0.076370924711227417;
	if (pt >= 50  && pt < 70 ) return 0.921826-0.096718423068523407;
	if (pt >= 70  && pt < 100) return 0.921826-0.072690784931182861;
	if (pt >= 100 && pt < 140) return 0.921826-0.18193449079990387;
	if (pt >= 140 && pt < 200) return 0.921826-0.17593351006507874;
	if (pt >= 200 && pt < 300) return 0.921826-0.52129054069519043;
	if (pt >= 300 && pt < 670) return 0.921826-0.41978824138641357;
      }
      if(abs(flavor)!=5 && abs(flavor)!=4){
	return 1.0;
      }
    }
  }
  return 1.0;
}

float DMAnalysisTreeMaker::BTagWeight::weightWithVeto(vector<JetInfo> jetsTags, int tags, vector<JetInfo> jetsVetoes, int vetoes)
{//This function takes into account cases where you have n b-tags and m vetoes, but they have different thresholds. 
  if (!filter(tags))
    {
      //   std::cout << "nThis event should not pass the selection, what is it doing here?" << std::endl;
      return 0;
    }
  int njets = jetsTags.size();
  if(njets != (int)(jetsVetoes.size()))return 0;//jets tags and vetoes must have same size!
  int comb = 1 << njets;
  float pMC = 0;
  float pData = 0;
  for (int i = 0; i < comb; i++)
    {
      float mc = 1.;
      float data = 1.;
      int ntagged = 0;
      for (int j = 0; j < njets; j++)
        {
	  bool tagged = ((i >> j) & 0x1) == 1;
	  if (tagged)
            {
	      ntagged++;
	      mc *= jetsTags[j].eff;
	      data *= jetsTags[j].eff * jetsTags[j].sf;
            }
	  else
            {
	      mc *= (1. - jetsVetoes[j].eff);
	      data *= (1. - jetsVetoes[j].eff * jetsVetoes[j].sf);
            }
        }

      if (filter(ntagged))
        {
	  //  std::cout << mc << " " << data << endl;
	  pMC += mc;
	  pData += data;
        }
    }

  if (pMC == 0) return 0;
  return pData / pMC;
}




//DMAnalysisTreeMaker::~DMAnalysisTreeMaker(const edm::ParameterSet& iConfig)
// ------------ method called once each job just after ending the event loop  ------------


#include "FWCore/Framework/interface/MakerMacros.h"


DEFINE_FWK_MODULE(DMAnalysisTreeMaker);
