// compile with:
// g++ `root-config --libs --cflags` LaserAnalysisEB.cpp -o LaserAnalysisEB

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>


#include "TChain.h"
#include "TGraph.h"
#include "TFile.h"
#include "TProfile.h"
#include "TTimeStamp.h"


#include "./LaserDataAnalysis.h"

using namespace std;

int main(int argc, char ** argv)
{
  int selected_fed = atoi(argv[1]);
  
  if ( selected_fed < 610 ||  selected_fed > 645){
    cout << ">>>>>  Fed  " << selected_fed << " is not in EB !!!"  << endl;
    cout << "       Exiting ...  "  << endl;
    return 0;
  }

  bool saveAllChannels = false ;

  int * wl = NULL, nwl = 0;
  TChain * tx = new TChain("x");
  tx->Add(argv[2]);


  init_ttree(tx, &x);
  tx->SetBranchStatus("*",0); //disable all branches
  // only read needed branches
  tx->SetBranchStatus("run",1);
  tx->SetBranchStatus("fed",1);
  tx->SetBranchStatus("seq",1);
  tx->SetBranchStatus("detId",1);
  tx->SetBranchStatus("apdpnAB",1);
  tx->SetBranchStatus("apdpnA",1);
  tx->SetBranchStatus("apdpnB",1);
  tx->SetBranchStatus("time",1);
  tx->SetBranchStatus("eta",1);
  tx->SetBranchStatus("lumi",1);
  tx->SetBranchStatus("field", 1);
  tx->SetBranchStatus("wl",1);
  tx->SetBranchStatus("harness",1);
  tx->SetBranchStatus("elecId",1);
  tx->SetBranchStatus("tmax",1);
  tx->SetBranchStatus("l_ampli",1);
  tx->SetBranchStatus("l_rise_time",1);
  tx->SetBranchStatus("l_fwhm",1);
  tx->SetBranchStatus("l_prepulse",1);

  cout << " Number of entries " << tx->GetEntries() << endl;
  
  // 36 --> number of feds in EE
  // 1700 : max number of channels in the fed
  TGraph* gapdpn1[1700];
  TGraph* gapdpn2[1700];
  TGraph* gratio[1700];
  char gname[300];
  int fed = 0;
    
  for (int ixtal = 0; ixtal < 1700; ixtal++){
      sprintf(gname,"g_apdpn1_fed%d_xtal%d",selected_fed, ixtal);
      gapdpn1[ixtal] = new TGraph();
      gapdpn1[ixtal] -> SetTitle(gname);
      gapdpn1[ixtal] -> SetName(gname);
      gapdpn1[ixtal] -> SetMarkerColor(kBlue);
      gapdpn1[ixtal] -> SetMarkerStyle(20);
      gapdpn1[ixtal] -> SetMarkerSize(0.5);
      
      sprintf(gname,"g_apdpn2_fed%d_xtal%d",selected_fed, ixtal);
      gapdpn2[ixtal] = new TGraph();
      gapdpn2[ixtal] -> SetTitle(gname);
      gapdpn2[ixtal] -> SetName(gname);
      gapdpn2[ixtal] -> SetMarkerColor(kRed+1);
      gapdpn2[ixtal] -> SetMarkerStyle(20);
      gapdpn2[ixtal] -> SetMarkerSize(0.5);
      
      sprintf(gname,"g_ratio_fed%d_xtal%d",selected_fed, ixtal);
      gratio[ixtal] = new TGraph();
      gratio[ixtal] -> SetTitle(gname);
      gratio[ixtal] -> SetName(gname);
      gratio[ixtal] -> SetMarkerColor(kBlack);
      gratio[ixtal] -> SetMarkerStyle(20);
      gratio[ixtal] -> SetMarkerSize(0.5);
    }
  


  // Plot results:
  TTimeStamp dateMin(2011, 1,  01, 0, kTRUE, 0); 
  TTimeStamp dateMax(2011, 10, 31, 0, kTRUE, 0); 
  
  // profile averaging on one harness
  TProfile *p_apdpn1[20];
  TProfile *p_apdpn2[20];
  TProfile *p_ratio[20];

  // other quantities per harness 
  TProfile *p_ratiopn_las1[20];
  TProfile *p_ratiopn_las2[20];

  TProfile *p_tmax_las1[20];
  TProfile *p_tmax_las2[20];
  
  TProfile *p_matacqampli_las1[20];
  TProfile *p_matacqrisetime_las1[20];
  TProfile *p_matacqfwhm_las1[20];
  TProfile *p_matacqprepulse_las1[20];
  TProfile *p_matacqtmax_las1[20];

  TProfile *p_matacqampli_las2[20];
  TProfile *p_matacqrisetime_las2[20];
  TProfile *p_matacqfwhm_las2[20];
  TProfile *p_matacqprepulse_las2[20];
  TProfile *p_matacqtmax_las2[20];


    for (int ih = 0; ih < 20; ih++){
      
      sprintf(gname,"p_apdpn1_fed%d_harness%d", selected_fed, ih+1);
      p_apdpn1[ih] = new TProfile(gname,gname,8000,dateMin, dateMax, 0,100);
      p_apdpn1[ih] ->SetLineColor(kBlue);
      p_apdpn1[ih] ->SetMarkerColor(kBlue);
      p_apdpn1[ih] ->SetMarkerStyle(20);
      p_apdpn1[ih] ->SetMarkerSize(0.5);
      
      sprintf(gname,"p_apdpn2_fed%d_harness%d", selected_fed, ih+1);
      p_apdpn2[ih] = new TProfile(gname,gname,8000,dateMin, dateMax, 0,100);
      p_apdpn2[ih] ->SetLineColor(kRed+1);
      p_apdpn2[ih] ->SetMarkerColor(kRed+1);
      p_apdpn2[ih] ->SetMarkerStyle(20);
      p_apdpn2[ih] ->SetMarkerSize(0.5);

      sprintf(gname,"p_ratio_fed%d_harness%d", selected_fed, ih+1);
      p_ratio[ih] = new TProfile(gname,gname,8000,dateMin, dateMax, 0,100);
      p_ratio[ih] ->SetLineColor(kBlack);
      p_ratio[ih] ->SetMarkerColor(kBlack);
      p_ratio[ih] ->SetMarkerStyle(20);
      p_ratio[ih] ->SetMarkerSize(0.5);

      
      sprintf(gname,"p_ratiopn_las1_fed%d_harness%d", selected_fed, ih+1);
      p_ratiopn_las1[ih] = new TProfile(gname,gname,8000,dateMin, dateMax, 0,100);
      p_ratiopn_las1[ih] ->SetLineColor(kBlue);
      p_ratiopn_las1[ih] ->SetMarkerColor(kBlue);
      p_ratiopn_las1[ih] ->SetMarkerStyle(20);
      p_ratiopn_las1[ih] ->SetMarkerSize(0.5);   
      

      sprintf(gname,"p_ratiopn_las2_fed%d_harness%d", selected_fed, ih+1);
      p_ratiopn_las2[ih] = new TProfile(gname,gname,8000,dateMin, dateMax, 0,100);
      p_ratiopn_las2[ih] ->SetLineColor(kRed+1);
      p_ratiopn_las2[ih] ->SetMarkerColor(kRed+1);
      p_ratiopn_las2[ih] ->SetMarkerStyle(20);
      p_ratiopn_las2[ih] ->SetMarkerSize(0.5);   
      

      sprintf(gname,"p_tmax_las1_fed%d_harness%d", selected_fed, ih+1);
      p_tmax_las1[ih]  = new TProfile(gname,gname,8000,dateMin, dateMax, 0,100);
      p_tmax_las1[ih]  ->SetLineColor(kBlue);
      p_tmax_las1[ih]  ->SetMarkerColor(kBlue);
      p_tmax_las1[ih]  ->SetMarkerStyle(20);
      p_tmax_las1[ih]  ->SetMarkerSize(0.5);   

      sprintf(gname,"p_tmax_las2_fed%d_harness%d", selected_fed, ih+1);
      p_tmax_las2[ih]  = new TProfile(gname,gname,8000,dateMin, dateMax, 0,100);
      p_tmax_las2[ih]  ->SetLineColor(kRed+1);
      p_tmax_las2[ih]  ->SetMarkerColor(kRed+1);
      p_tmax_las2[ih]  ->SetMarkerStyle(20);
      p_tmax_las2[ih]  ->SetMarkerSize(0.5);   
      
      sprintf(gname,"p_matacqampli_las1_fed%d_harness%d", selected_fed, ih+1);
      p_matacqampli_las1[ih]  = new TProfile(gname,gname,8000,dateMin, dateMax, 0,100);
      p_matacqampli_las1[ih]  ->SetLineColor(kBlue);
      p_matacqampli_las1[ih]  ->SetMarkerColor(kBlue);
      p_matacqampli_las1[ih]  ->SetMarkerStyle(20);
      p_matacqampli_las1[ih]  ->SetMarkerSize(0.5);   

      sprintf(gname,"p_matacqrisetime_las1_fed%d_harness%d", selected_fed, ih+1);
      p_matacqrisetime_las1[ih] = new TProfile(gname,gname,8000,dateMin, dateMax, 0,100);
      p_matacqrisetime_las1[ih] ->SetLineColor(kBlue);
      p_matacqrisetime_las1[ih] ->SetMarkerColor(kBlue);
      p_matacqrisetime_las1[ih] ->SetMarkerStyle(20);
      p_matacqrisetime_las1[ih] ->SetMarkerSize(0.5);   

      sprintf(gname,"p_matacqfwhm_las1_fed%d_harness%d", selected_fed, ih+1);
      p_matacqfwhm_las1[ih] = new TProfile(gname,gname,8000,dateMin, dateMax, 0,100);
      p_matacqfwhm_las1[ih] ->SetLineColor(kBlue);
      p_matacqfwhm_las1[ih] ->SetMarkerColor(kBlue);
      p_matacqfwhm_las1[ih] ->SetMarkerStyle(20);
      p_matacqfwhm_las1[ih] ->SetMarkerSize(0.5);   

      sprintf(gname,"p_matacqprepulse_las1_fed%d_harness%d", selected_fed, ih+1);
      p_matacqprepulse_las1[ih] = new TProfile(gname,gname,8000,dateMin, dateMax, 0,100);
      p_matacqprepulse_las1[ih] ->SetLineColor(kBlue);
      p_matacqprepulse_las1[ih] ->SetMarkerColor(kBlue);
      p_matacqprepulse_las1[ih] ->SetMarkerStyle(20);
      p_matacqprepulse_las1[ih] ->SetMarkerSize(0.5);  

      sprintf(gname,"p_matacqampli_las2_fed%d_harness%d", selected_fed, ih+1);
      p_matacqampli_las2[ih] = new TProfile(gname,gname,8000,dateMin, dateMax, 0,100);
      p_matacqampli_las2[ih] ->SetLineColor(kRed+1);
      p_matacqampli_las2[ih] ->SetMarkerColor(kRed+1);
      p_matacqampli_las2[ih] ->SetMarkerStyle(20);
      p_matacqampli_las2[ih] ->SetMarkerSize(0.5);   

      sprintf(gname,"p_matacqrisetime_las2_fed%d_harness%d", selected_fed, ih+1);
      p_matacqrisetime_las2[ih] = new TProfile(gname,gname,8000,dateMin, dateMax, 0,100);
      p_matacqrisetime_las2[ih] ->SetLineColor(kRed+1);
      p_matacqrisetime_las2[ih] ->SetMarkerColor(kRed+1);
      p_matacqrisetime_las2[ih] ->SetMarkerStyle(20);
      p_matacqrisetime_las2[ih] ->SetMarkerSize(0.5);   

      sprintf(gname,"p_matacqfwhm_las2_fed%d_harness%d", selected_fed, ih+1);
      p_matacqfwhm_las2[ih] = new TProfile(gname,gname,8000,dateMin, dateMax, 0,100);
      p_matacqfwhm_las2[ih] ->SetLineColor(kRed+1);
      p_matacqfwhm_las2[ih] ->SetMarkerColor(kRed+1);
      p_matacqfwhm_las2[ih] ->SetMarkerStyle(20);
      p_matacqfwhm_las2[ih] ->SetMarkerSize(0.5);   

      sprintf(gname,"p_matacqprepulse_las2_fed%d_harness%d", selected_fed, ih+1);
      p_matacqprepulse_las2[ih] = new TProfile(gname,gname,8000,dateMin, dateMax, 0,100);
      p_matacqprepulse_las2[ih] ->SetLineColor(kRed+1);
      p_matacqprepulse_las2[ih] ->SetMarkerColor(kRed+1);
      p_matacqprepulse_las2[ih] ->SetMarkerStyle(20);
      p_matacqprepulse_las2[ih] ->SetMarkerSize(0.5);  
      
    }

    
  int evt1[1700] = {0};
  int evt2[1700] = {0};

  int tAve;

  float apdpn1ref[1700], apdpn2ref[1700];
  float las1_tmax0[1700], las2_tmax0[1700] ;

  float las1_ampli0[1700], las1_risetime0[1700], las1_fwhm0[1700] , las1_prepulse0[1700];
  float las2_ampli0[1700], las2_risetime0[1700], las2_fwhm0[1700] , las2_prepulse0[1700];

  
  for (int ientry = 0; ientry < tx->GetEntries(); ientry++){
    tx->GetEntry(ientry);
    
    if (ientry%1000000 == 0 ) cout << "Analyzing entry " << ientry << endl;
    
    // select only EB
    if (!isEB(x.fed)) continue;
        
    int harn     = x.harness;
    int fed      = x.fed;
    int ism      = iSM(fed);
    float apdpn1 = x.apdpnAB[0];
    float apdpn2 = x.apdpnAB[1];
    int t1       = x.time[0];
    int t2       = x.time[1];
    int xtal     = x.elecId;      


    // check only one fed
    if ( fed != selected_fed ) continue;
     
    if (apdpn1<=0) continue;
    if (apdpn2<=0) continue;
   
    // normalize to first point 
    if (evt1[xtal] == 0) {
      apdpn1ref[xtal]      = apdpn1;
      las1_tmax0[xtal]     = x.tmax[0];
      las1_ampli0[xtal]    = x.l_ampli[0];
      las1_risetime0[xtal] = x.l_rise_time[0];
      las1_fwhm0[xtal]     = x.l_fwhm[0];
      las1_prepulse0[xtal] = x.l_prepulse[0];
    }

    if (evt2[xtal] == 0) {
      apdpn2ref[xtal]      = apdpn2;
      las2_tmax0[xtal]     = x.tmax[1];
      las2_ampli0[xtal]    = x.l_ampli[1];
      las2_risetime0[xtal] = x.l_rise_time[1];
      las2_fwhm0[xtal]     = x.l_fwhm[1];
      las2_prepulse0[xtal] = x.l_prepulse[1];
    }

    // average time betweek the two laser measurements
    tAve = t1 + (t2 - t1)/ 2;
    if ( t1 > t2 ) tAve = t2 + (t1 - t2)/ 2;

    // profile plots (i.e. average on one harness)
    
    p_ratio[harn-1] ->Fill(tAve, (apdpn1/apdpn1ref[xtal])/(apdpn2/apdpn2ref[xtal]));
    
    p_apdpn1[harn-1]             -> Fill(t1, apdpn1/apdpn1ref[xtal]);
    p_ratiopn_las1[harn-1]       -> Fill(t1, x.apdpnA[0]/x.apdpnB[0]);
    p_tmax_las1[harn-1]          -> Fill(t1, x.tmax[0]/las1_tmax0[xtal]);
    p_matacqampli_las1[harn-1]   -> Fill(t1, x.l_ampli[0]/las1_ampli0[xtal]);
    p_matacqrisetime_las1[harn-1]-> Fill(t1, x.l_rise_time[0]/las1_risetime0[xtal]);
    p_matacqfwhm_las1[harn-1]    -> Fill(t1, x.l_fwhm[0]/las1_fwhm0[xtal]);
    p_matacqprepulse_las1[harn-1]-> Fill(t1, x.l_prepulse[0]/las1_prepulse0[xtal]);

    p_apdpn2[harn-1]              -> Fill(t2, apdpn2/apdpn2ref[xtal]);
    p_ratiopn_las2[harn-1]        -> Fill(t2, x.apdpnA[1]/x.apdpnB[1]);   
    p_tmax_las2[harn-1]           -> Fill(t2, x.tmax[1]/las2_tmax0[xtal]);
    p_matacqampli_las2[harn-1]    -> Fill(t2, x.l_ampli[1]/las2_ampli0[xtal]);
    p_matacqrisetime_las2[harn-1] -> Fill(t2, x.l_rise_time[1]/las2_risetime0[xtal]);
    p_matacqfwhm_las2[harn-1]     -> Fill(t2, x.l_fwhm[1]/las2_fwhm0[xtal]);
    p_matacqprepulse_las2[harn-1] -> Fill(t2, x.l_prepulse[1]/las2_prepulse0[xtal]);


    // signle channel graphs
    if (saveAllChannels){
      gapdpn1[xtal] -> SetPoint(evt1[xtal], t1 , apdpn1);
      gapdpn2[xtal] -> SetPoint(evt2[xtal], t2 , apdpn2);
      gratio[xtal]  -> SetPoint(evt2[xtal], tAve , apdpn1/apdpn2);
    }
    evt1[xtal]++;    
    evt2[xtal]++;

  } // end loop over entries
  


  //   // ratio plots
  //   for (int ism = 0; ism < 36; ism++){
  //     for (int ixtal = 0; ixtal < 1700; ixtal++){
  //       if ( gapdpn1[ixtal]-> GetN() == 0 ) continue();
  //       if ( gapdpn2[ixtal]-> GetN() == 0 ) continue();
  
  //       for (int ibin = 1; ibin < gapdpn1[ixtal]-> GetNbinsX()+1; ibin++){
  // 	gratio[ixtal]-> Fill();
  //       }
  //     }
  //   }
  
  

  
  char fname[100];
  sprintf(fname,argv[3],selected_fed);
  TFile *fout = new TFile(fname,"recreate");

  if (saveAllChannels){
    TDirectory *mydir      =   fout -> mkdir("by_channel","by_channel");
    mydir->cd();
      for (int ixtal = 0; ixtal < 1700; ixtal++){
	if ( gapdpn1[ixtal]-> GetN() > 0) gapdpn1[ixtal]-> Write();
	if ( gapdpn2[ixtal]-> GetN() > 0) gapdpn2[ixtal]-> Write();
	if ( gratio[ixtal] -> GetN() > 0) gratio[ixtal]-> Write();
    }
  }

  fout->cd();
    for (int ih = 0; ih < 20; ih++){
      if ( p_apdpn1[ih]-> GetEntries() > 0)  	p_apdpn1[ih]->Write();
      if ( p_apdpn2[ih]-> GetEntries() > 0)  	p_apdpn2[ih]->Write();
      if ( p_ratio[ih]-> GetEntries()  > 0)  	p_ratio[ih]->Write();
      
      if ( p_ratiopn_las1[ih]-> GetEntries() > 0) p_ratiopn_las1[ih]->Write();
      if ( p_ratiopn_las2[ih]-> GetEntries() > 0) p_ratiopn_las2[ih]->Write();

      if ( p_tmax_las1[ih] -> GetEntries() > 0) p_tmax_las1[ih] ->Write();
      if ( p_tmax_las2[ih] -> GetEntries() > 0) p_tmax_las2[ih] ->Write();
      
      if ( p_matacqampli_las1[ih] -> GetEntries() > 0) p_matacqampli_las1[ih] ->Write();
      if ( p_matacqrisetime_las1[ih]-> GetEntries() > 0) p_matacqrisetime_las1[ih]->Write();
      if ( p_matacqfwhm_las1[ih]-> GetEntries() > 0) p_matacqfwhm_las1[ih]->Write();
      if ( p_matacqprepulse_las1[ih]-> GetEntries() > 0) p_matacqprepulse_las1[ih]->Write();

      if ( p_matacqampli_las2[ih]-> GetEntries() > 0) p_matacqampli_las2[ih]->Write();
      if ( p_matacqrisetime_las2[ih]-> GetEntries() > 0) p_matacqrisetime_las2[ih]->Write();
      if ( p_matacqfwhm_las2[ih]-> GetEntries() > 0) p_matacqfwhm_las2[ih]->Write();
      if ( p_matacqprepulse_las2[ih]-> GetEntries() > 0) p_matacqprepulse_las2[ih]->Write();
    }
  return 0;
  
}